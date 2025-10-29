#include <cstdint>
#include <stdexcept>
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_CPP_VERSION 23
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_raii.hpp>

#include "SimpleVulkan.hpp"



VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
    

namespace SimpleVulkan {

    const bool enableValidationLayers = true;
    SDL_Window* CreateSDL3Window(ApplicationInfo appInfo){
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Window* sdlWindow = SDL_CreateWindow(appInfo.title, appInfo.width, appInfo.height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        return sdlWindow;
    }

    const char** getRequiredExtensions() {
        static std::vector<const char*> extensions;
        uint32_t sdlExtensionCount = 0;
        const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
        extensions.resize(sdlExtensionCount + 1);
        if (sdlExtensions) {
            for (uint32_t i = 0; i < sdlExtensionCount; ++i) {
                extensions[i + 1] = sdlExtensions[i];
            }
        }
        extensions[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        const char** extensionsData = extensions.data();
        return extensionsData;
    }
    
    unsigned int getRequiredExtensionsCount() {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        return static_cast<unsigned int>(extensionCount) + 1;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        
        (void)messageSeverity;
        (void)messageType;
        (void)pUserData;
        
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() {
        return vk::DebugUtilsMessengerCreateInfoEXT{
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            &debugCallback
        };
    }



    vk::raii::Instance CreateVulkanInstance(ValidationLayers validationLayers){
        vk::raii::Context context{}; 
        
        // APP INFO
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Vulkan VBO Square";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4;
        vk::InstanceCreateInfo createInfo{{}, &appInfo};

        // EXTENSIONS INFO
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(getRequiredExtensionsCount());
        std::cout << "----------------- AVAILABLE VULKAN INSTANCE'S EXTENSIONS ---------------------" << std::endl;
        for (uint32_t i = 0; i != createInfo.enabledExtensionCount; i++){
            std::cout << " - " << extensions[i] << "\n";
        }
        std::cout << "-----------------------------------------------------------------------------" << std::endl;
        createInfo.ppEnabledExtensionNames = extensions;
        

        // LAYERS
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            debugCreateInfo = populateDebugMessengerCreateInfo();
            createInfo.pNext = &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        // Cria a instância RAII
        vk::raii::Instance instance(context, createInfo);
        try {
            VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance, vkGetInstanceProcAddr);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Falha na inicialização do Vulkan Loader: ") + e.what() + "\n");
        }

        return instance;
    }



    QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface, bool debug = false) {
        QueueFamilyIndices indices;

        const std::vector<vk::QueueFamilyProperties>& queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (debug){
                std::cout << "  Família [" << i << "]:\n";
                std::cout << "    Contagem de Filas: " << queueFamily.queueCount << "\n";
                std::cout << "    Capacidades (Flags): ";
                
                
                if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                    std::cout << "[Gráficos] ";
                }
                if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
                    std::cout << "[Computação] ";
                }
                if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) {
                    std::cout << "[Transferência] ";
                }
                if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding) {
                    std::cout << "[Sparse Binding] ";
                }

                if (device.getSurfaceSupportKHR(i, *surface)) {
                    std::cout << "[Presente (Surface)] ";
                }
                std::cout << "\n";
            }
            i++;
        }
        i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
            }

            // 2. Checa o suporte à apresentação na Surface
            if (device.getSurfaceSupportKHR(i, *surface)) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }
            i++;
        }

        return indices;
    }
    inline vk::raii::SurfaceKHR CreateSurface(const vk::raii::Instance& instance, SDL_Window* window, VkSurfaceKHR& rawSurface) {
        if (SDL_Vulkan_CreateSurface(window, *instance, nullptr, &rawSurface) != true) {
            throw std::runtime_error("Falha ao criar Surface SDL Vulkan!");
        }
        return vk::raii::SurfaceKHR{instance, rawSurface};
    }

    bool isDeviceSuitable(const vk::raii::PhysicalDevice& device, vk::raii::SurfaceKHR& surface) {
        QueueFamilyIndices indices = findQueueFamilies(device, surface);
        return indices.isComplete();
    }



    vk::raii::PhysicalDevice PickPhysicalDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface) {
        std::cout << "Verifying devices..." << "\n";
        vk::raii::PhysicalDevices devices{instance};
        if (devices.empty()) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        auto it = std::ranges::find_if(devices, [&surface] (auto& device) {
            return isDeviceSuitable(device, surface);
        });

        it == devices.end() ? throw std::runtime_error("failed to find a suitable GPU. Trying to select the first") : void();
        std::cout << "GPU selecionada: " << it->getProperties().deviceName << std::endl; return *it;
    }



    vk::raii::Device CreateLogicalDevice(vk::raii::PhysicalDevice& PhysicalDevice, vk::raii::SurfaceKHR& surface, const std::optional<std::vector<const char*>>& deviceExtensions = std::nullopt){
        QueueFamilyIndices indices = findQueueFamilies(PhysicalDevice, surface, true);
        float queuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo(
                {},
                queueFamily,
                1,             // número de filas para essa família
                &queuePriority // prioridade da fila
            );
            queueCreateInfos.push_back(queueCreateInfo);
        }
        vk::PhysicalDeviceFeatures deviceFeatures{}; // Initialize every feature as disabled

        const std::vector<const char*> emptyExtensions;
        const std::vector<const char*> extensions = deviceExtensions.has_value() ? *deviceExtensions : emptyExtensions;


        vk::DeviceCreateInfo createInfo(
            {},
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data(),
            0, nullptr,
            static_cast<uint32_t>(extensions.size()), extensions.data(),  // extensões do device
            &deviceFeatures
        );

        std::cout << "Logical Device successfully created!" << "\n";

        
        return vk::raii::Device{PhysicalDevice, createInfo};
    }



    
    std::array<vk::raii::Queue, 2> getQueues(vk::raii::PhysicalDevice& pdev, vk::raii::Device& ldev, vk::raii::SurfaceKHR& surface){
        QueueFamilyIndices indices = findQueueFamilies(pdev, surface);
        std::array<vk::raii::Queue, 2> queues{
            vk::raii::Queue{ ldev, indices.graphicsFamily.value(), 0 },
            vk::raii::Queue{ ldev, indices.presentFamily.value(), 0 }
        };
        return queues;
    }

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }


    vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode; // eMailBox selected (more consistent "vsync")
            }
        }
        return vk::PresentModeKHR::eFifo; // eFifo selected (globally supported)
    }

    vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){ // checks if the frame's width is'n higher than a 32 bits unsigned int
            return capabilities.currentExtent;
        } else {
            vk::Extent2D actualExtent = {width, height};
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }


    SwapchainBundle CreateSwapchain(
        vk::raii::Device& dev,
        vk::raii::PhysicalDevice& pdev,
        vk::raii::SurfaceKHR& surface,
        const QueueFamilyIndices indices,
        uint32_t width,
        uint32_t height,
        uint32_t imageCount = 2
    ){
        uint32_t imgsCount;
        vk::SurfaceCapabilitiesKHR capabilities = pdev.getSurfaceCapabilitiesKHR(*surface);
        std::vector<vk::SurfaceFormatKHR> formats = pdev.getSurfaceFormatsKHR(*surface);
        std::vector<vk::PresentModeKHR> presentModes = pdev.getSurfacePresentModesKHR(*surface);
        
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
        vk::PresentModeKHR presentMode = choosePresentMode(presentModes);
        vk::Extent2D extent = chooseExtent(capabilities, width, height);
        if (capabilities.maxImageCount > 0){
            imgsCount = std::clamp(imageCount, capabilities.minImageCount, capabilities.maxImageCount);
        }
        else {
            imgsCount = std::max(imageCount, capabilities.minImageCount);
        }

        vk::SwapchainCreateInfoKHR swapchainInfo {};
        swapchainInfo.surface = *surface;
        swapchainInfo.minImageCount = imgsCount;
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchainInfo.preTransform = capabilities.currentTransform;
        swapchainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = true;
        swapchainInfo.oldSwapchain = nullptr;
        

        if (indices.graphicsFamily != indices.presentFamily) {
            swapchainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices = &indices.graphicsFamily.value();
        }
        else {
            swapchainInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }


        vk::raii::SwapchainKHR swapchain{dev, swapchainInfo}; // swapchain itself
        std::vector<vk::Image> images = swapchain.getImages();
        std::vector<vk::raii::ImageView> imageViews;
        imageViews.reserve(images.size());
        for (const auto& image : images) {
            vk::ImageViewCreateInfo viewInfo{
                {},
                image,
                vk::ImageViewType::e2D,
                surfaceFormat.format,
                { vk::ComponentSwizzle::eIdentity,
                  vk::ComponentSwizzle::eIdentity,
                  vk::ComponentSwizzle::eIdentity,
                  vk::ComponentSwizzle::eIdentity },
                { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
            };
            imageViews.emplace_back(dev, viewInfo);

        }
        std::cout << "SwapImage successfully created." << std::endl;
        return SwapchainBundle{
            std::move(swapchain),
            surfaceFormat.format,
            extent,
            std::move(images),
            std::move(imageViews)
        };

        
    }

} // namespace SimpleVulkan

int main(){
    SimpleVulkan::ValidationLayers validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    SimpleVulkan::ApplicationInfo appInfo {
        "Vulkan Square",
        "My window title",
        800,
        600,
    };
    SDL_Window* window = SimpleVulkan::CreateSDL3Window(appInfo);
    vk::raii::Instance instance = SimpleVulkan::CreateVulkanInstance(validationLayers);
    VkSurfaceKHR rawSurface;
    vk::raii::SurfaceKHR surface = SimpleVulkan::CreateSurface(instance, window, rawSurface);
    vk::raii::PhysicalDevice device = SimpleVulkan::PickPhysicalDevice(instance, surface);
    std::vector<const char*> vulkanDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    vk::raii::Device logicalDevice = SimpleVulkan::CreateLogicalDevice(device, surface, vulkanDeviceExtensions);
    SimpleVulkan::QueueFamilyIndices queueIndices = SimpleVulkan::findQueueFamilies(device, surface);
    auto [graphicsQueue, presentQueue] = SimpleVulkan::getQueues(device, logicalDevice, surface);
    SimpleVulkan::CreateSwapchain(logicalDevice, device, surface, queueIndices, 1920, 1080); // makes a swapchain with 1920x1080 as default resolution


    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }
    }

}