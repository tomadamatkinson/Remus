#pragma once

#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>

#include "context/context.hpp"

#include <common/error.hpp>

REMUS_DISABLE_WARNINGS()
#include <vk_mem_alloc.h>
REMUS_ENABLE_WARNINGS()

namespace remus
{
namespace vulkan
{
// Define a set of memory usages that we want to support and their corresponding Vulkan memory property flags
enum class MemoryUsage : VkMemoryPropertyFlags
{
	GPU_ONLY,          // Prefers VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	CPU_ONLY,          // Guarantees VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	CPU_TO_GPU,        // Guarantees VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT Prefers VK_MEMORY_PROPERTY_HOST_CACHED_BIT
	LAZY_ALLOC,        // Prefers VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT - Lazily allocated by the implementation - useful for transient memory
	AUTO,              // Automatically chooses the best memory type for the usage
};

class MemoryPool;

class BufferAllocation final
{
	friend class MemoryPool;

  public:
	~BufferAllocation();

	vk::Buffer buffer{VK_NULL_HANDLE};

	vk::DeviceSize size() const
	{
		return byte_size;
	}

	bool is_valid() const
	{
		return buffer && allocation != VK_NULL_HANDLE;
	}

	void update(size_t offset, size_t size, const void *data);

  protected:
	BufferAllocation(MemoryPool &pool, vk::Buffer buffer, VmaAllocation allocation, vk::DeviceSize size);

	MemoryPool    &pool;
	VmaAllocation  allocation;
	vk::DeviceSize byte_size;
};

using BufferAllocationPtr = std::shared_ptr<BufferAllocation>;

class ImageAllocation final
{
	friend class MemoryPool;

  public:
	~ImageAllocation();

	vk::Image     image{VK_NULL_HANDLE};
	vk::ImageView view{VK_NULL_HANDLE};

	bool is_valid() const
	{
		return image && allocation != VK_NULL_HANDLE;
	}

  protected:
	ImageAllocation(MemoryPool &pool, vk::Image image, vk::ImageView view, VmaAllocation allocation);

	MemoryPool   &pool;
	VmaAllocation allocation{VK_NULL_HANDLE};
};

using ImageAllocationPtr = std::shared_ptr<ImageAllocation>;

/**
 * @brief A class that manages the allocation and aliasing of Vulkan memory
 */
class MemoryPool
{
	friend class BufferAllocation;
	friend class ImageAllocation;

  public:
	MemoryPool(ContextPtr context);

	virtual ~MemoryPool() = default;

	ImageAllocationPtr allocate_image(const vk::ImageCreateInfo &image_create_info, MemoryUsage usage);

	BufferAllocationPtr allocate_buffer(const vk::BufferCreateInfo &buffer_create_info, MemoryUsage usage);

  private:
	ContextPtr context;

	static VmaAllocator *allocator(ContextPtr &context);

	void update_buffer(BufferAllocation &allocation, size_t offset, size_t size, const void *data);

	void free_buffer(BufferAllocation &allocation);
	void free_image(ImageAllocation &allocation);
};

using MemoryPoolPtr = std::shared_ptr<MemoryPool>;

inline void BufferAllocation::update(size_t offset, size_t size, const void *data)
{
	pool.update_buffer(*this, offset, size, data);
}
}        // namespace vulkan
}        // namespace remus
