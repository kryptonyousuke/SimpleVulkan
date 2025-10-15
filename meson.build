
# Project definition

project(
    'SimpleVulkan',
    'cpp',
    version: '1.0.0',
    default_options: ['cpp_std=c++23']
)


# Dependencies

sdl3 = dependency('sdl3')
vulkan = dependency('vulkan')



# Source
app_sources = [
    'main.cpp'
]


# Compile and Linking
executable(
    'SimpleVulkan', 
    app_sources,
    dependencies: [sdl3, vulkan]
)