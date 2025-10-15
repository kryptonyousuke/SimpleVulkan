#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <vulkan/vulkan_handles.hpp>
#include <stdexcept>
const typedef struct {
    const char* applicationName;
    char* title;
    int width;
    int height;

} ApplicationInfo;