#include "pools/memory_pool.hpp"

#include "common.hpp"

#include <sstream>

#include <common/logging.hpp>

static const char *MEMORY_POOL_BUFFER_ALLOCATIONS_NAME = "Buffer Allocations";
static const char *MEMORY_POOL_IMAGE_ALLOCATIONS_NAME  = "Image Allocations";

namespace remus
{
namespace vulkan
{
BufferAllocation::BufferAllocation(MemoryPool &pool, vk::Buffer buffer, VmaAllocation allocation, vk::DeviceSize size) :
    pool{pool}, buffer{buffer}, allocation{allocation}, byte_size{size}
{
}

BufferAllocation::~BufferAllocation()
{
	if (is_valid())
	{
		pool.free_buffer(*this);
	}
}

ImageAllocation::~ImageAllocation()
{
	if (is_valid())
	{
		pool.free_image(*this);
	}
}

ImageAllocation::ImageAllocation(MemoryPool &pool, vk::Image image, vk::ImageView view, VmaAllocation allocation) :
    pool{pool},
    image{image},
    view{view},
    allocation{allocation}
{}

VmaAllocator *MemoryPool::allocator(ContextPtr &context)
{
	static VmaAllocator      allocator = VK_NULL_HANDLE;
	static vk::DynamicLoader dl;

	if (allocator == VK_NULL_HANDLE)
	{
		VmaVulkanFunctions vulkan_functions    = {};
		vulkan_functions.vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		vulkan_functions.vkGetDeviceProcAddr   = dl.getProcAddress<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");

		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.physicalDevice         = context->physical_device;
		allocator_info.device                 = context->device;
		allocator_info.instance               = context->instance;
		allocator_info.vulkanApiVersion       = VK_API_VERSION_1_2;
		allocator_info.pVulkanFunctions       = &vulkan_functions;

		if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create allocator");
		}

		context->add_cleanup_callback([&]() { vmaDestroyAllocator(allocator); });
	}

	return &allocator;
}

MemoryPool::MemoryPool(ContextPtr context) :
    context{context}
{
}

VmaMemoryUsage to_vma_memory_usage(MemoryUsage usage)
{
	switch (usage)
	{
		case MemoryUsage::CPU_TO_GPU:
			return VMA_MEMORY_USAGE_CPU_TO_GPU;
		case MemoryUsage::GPU_ONLY:
			return VMA_MEMORY_USAGE_GPU_ONLY;
		case MemoryUsage::CPU_ONLY:
			return VMA_MEMORY_USAGE_CPU_ONLY;
		case MemoryUsage::LAZY_ALLOC:
			return VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
		case MemoryUsage::AUTO:
			return VMA_MEMORY_USAGE_AUTO;
		default:
			throw std::runtime_error("Invalid memory usage");
	}
}

ImageAllocationPtr MemoryPool::allocate_image(const vk::ImageCreateInfo &image_create_info, MemoryUsage usage)
{
	auto *alloc = allocator(context);

	VkImageCreateInfo create_info = image_create_info;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage                   = to_vma_memory_usage(usage);

	VkImage       image;
	VmaAllocation allocation;
	vk::Result    result = static_cast<vk::Result>(vmaCreateImage(*alloc, &create_info, &alloc_info, &image, &allocation, nullptr));
	if (result != vk::Result::eSuccess)
	{
		LOGE(Vulkan, "Failed to allocate image: {}", vk::to_string(result));
		throw std::runtime_error("Failed to allocate image");
	}

	vk::ImageViewCreateInfo view_create_info;
	view_create_info.image                           = image;
	view_create_info.viewType                        = vk::ImageViewType::e2D;
	view_create_info.format                          = image_create_info.format;
	view_create_info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	view_create_info.subresourceRange.baseMipLevel   = 0;
	view_create_info.subresourceRange.levelCount     = image_create_info.mipLevels;
	view_create_info.subresourceRange.baseArrayLayer = 0;
	view_create_info.subresourceRange.layerCount     = image_create_info.arrayLayers;

	auto image_view = context->device.createImageView(view_create_info);

	return std::shared_ptr<ImageAllocation>(new ImageAllocation{*this, image, image_view, allocation});
}

BufferAllocationPtr MemoryPool::allocate_buffer(const vk::BufferCreateInfo &buffer_create_info, MemoryUsage usage)
{
	auto *alloc = allocator(context);

	VkBufferCreateInfo create_info = buffer_create_info;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage                   = to_vma_memory_usage(usage);

	VkBuffer          buffer;
	VmaAllocation     allocation;
	VmaAllocationInfo allocation_info;
	vk::Result        result = static_cast<vk::Result>(vmaCreateBuffer(*alloc, &create_info, &alloc_info, &buffer, &allocation, &allocation_info));
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to allocate buffer");
	}

	return std::shared_ptr<BufferAllocation>(new BufferAllocation{*this, buffer, allocation, create_info.size});
}

void MemoryPool::free_buffer(BufferAllocation &allocation)
{
	auto *alloc = allocator(context);

	if (!allocation.is_valid())
	{
		return;
	}

	vmaDestroyBuffer(*alloc, allocation.buffer, allocation.allocation);

	// Reset the allocation
	allocation.buffer     = VK_NULL_HANDLE;
	allocation.allocation = VK_NULL_HANDLE;
}

void MemoryPool::free_image(ImageAllocation &allocation)
{
	auto *alloc = allocator(context);

	if (!allocation.is_valid())
	{
		return;
	}

	// Destroy the image view
	if (allocation.view)
	{
		context->device.destroyImageView(allocation.view);
	}

	vmaDestroyImage(*alloc, allocation.image, allocation.allocation);

	// Reset the allocation
	allocation.image      = VK_NULL_HANDLE;
	allocation.allocation = VK_NULL_HANDLE;
}

void MemoryPool::update_buffer(BufferAllocation &allocation, size_t offset, size_t size, const void *data)
{
	auto *alloc = allocator(context);

	if (!allocation.is_valid())
	{
		return;
	}

	LOG_CHECK(Vulkan, offset + size > allocation.byte_size, "Buffer update out of bounds");
	LOG_CHECK(Vulkan, offset > 0, "Offsets not supported yet");

	void *mapped_data;
	vmaMapMemory(*alloc, allocation.allocation, &mapped_data);
	memcpy(mapped_data, data, size);
	vmaUnmapMemory(*alloc, allocation.allocation);
}

}        // namespace vulkan
}        // namespace remus
