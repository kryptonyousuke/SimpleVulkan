#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <algorithm>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <iostream> 
#include <stdexcept> 
#include <vector> 
#include <array>
#include <cstring> 
#include <cstdint>
#include <optional>
#include <set>
#include <string>

struct ApplicationInfo {
    const char* applicationName;
    const char* title;
    int width;
    int height;
    bool resize = true;
};