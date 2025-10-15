# SimpleVulkan
<strong>SimpleVulkan is a vulkan framework that allows you to easily implement vulkan in your projects.
You can see it's usage below.</strong>

```cpp
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
        auto [graphicsQueue, presentQueue] = SimpleVulkan::getQueues(device, logicalDevice, surface);



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
```