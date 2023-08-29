# required to ignore clang-tidy warnings in external code
add_library(remus_vma STATIC src/vma/vma.cpp)
target_link_libraries(remus_vma INTERFACE GPUOpen::VulkanMemoryAllocator)

add_library(remus__rhi_vulkan
    STATIC
        src/vulkan/rhi.cpp
        src/vulkan/common.cpp
        src/vulkan/context/context_builder.cpp
        src/vulkan/context/context.cpp
        src/vulkan/context/logger.cpp
        src/vulkan/pools/memory_pool.cpp)

target_include_directories(remus__rhi_vulkan
    PUBLIC
        include)

target_include_directories(remus__rhi_vulkan
    PRIVATE
        src/vulkan)

target_link_libraries(remus__rhi_vulkan
    PUBLIC
        remus__core
        Vulkan::Headers
        remus_vma
        )

configure_remus_library(remus__rhi_vulkan)

# use dynamic dispatch loader
target_compile_definitions(remus__rhi_vulkan PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

add_library(remus__rhi INTERFACE)

target_link_libraries(remus__rhi INTERFACE remus__rhi_vulkan)
