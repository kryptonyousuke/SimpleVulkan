
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
inc = include_directories('include')


# Source
app_sources = [
    'src/main.cpp'
]


# Compile and Linking
executable(
    'SimpleVulkan', 
    app_sources,
    dependencies: [sdl3, vulkan],
    include_directories: inc
)