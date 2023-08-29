#include "rhi/vulkan/rhi.hpp"

#include "context/context_builder.hpp"
#include "pools/memory_pool.hpp"

namespace remus
{
namespace vulkan
{

RHI::RHI()
{
	ContextBuilder builder;

	builder.configure_instance()
	    .set_application_info(vk::ApplicationInfo{
	        "Remus",
	        VK_MAKE_API_VERSION(1, 0, 0, 0),
	        "Remus",
	        VK_MAKE_API_VERSION(1, 0, 0, 0),
	        VK_API_VERSION_1_3,
	    });

	InstanceBuilder::enable_default_logger(builder);
	InstanceBuilder::enable_validation_layers(builder);

	m_context     = builder.build();
	m_memory_pool = std::make_shared<MemoryPool>(m_context);
}

RHI::~RHI()
{
	m_memory_pool.reset();
	m_context.reset();
}

inline vk::BufferUsageFlagBits get_buffer_usage(rhi::BufferUsage usage)
{
	switch (usage)
	{
		case rhi::BufferUsage::Vertex:
			return vk::BufferUsageFlagBits::eVertexBuffer;
		case rhi::BufferUsage::Index:
			return vk::BufferUsageFlagBits::eIndexBuffer;
		case rhi::BufferUsage::Uniform:
			return vk::BufferUsageFlagBits::eUniformBuffer;
		case rhi::BufferUsage::Storage:
			return vk::BufferUsageFlagBits::eStorageBuffer;
		default:
			return vk::BufferUsageFlagBits::eVertexBuffer;
	}
}

void RHI::create_buffer(const rhi::BufferHandle &handle, const rhi::BufferCreateInfo &info)
{
	vk::BufferCreateInfo create_info{};
	create_info.usage                 = get_buffer_usage(info.usage);
	create_info.size                  = info.size;
	create_info.pQueueFamilyIndices   = 0;
	create_info.queueFamilyIndexCount = 0;

	m_buffer_allocations[handle] = m_memory_pool->allocate_buffer(create_info, MemoryUsage::CPU_ONLY);
}

void RHI::update_buffer(const rhi::BufferHandle &handle, size_t offset, size_t size, void *data)
{
	auto it = m_buffer_allocations.find(handle);
	if (it != m_buffer_allocations.end())
	{
		it->second->update(offset, size, data);
	}
}

void RHI::destroy_buffer(const rhi::BufferHandle &handle)
{
	// pool auto cleans up buffer
	auto it = m_buffer_allocations.find(handle);
	if (it != m_buffer_allocations.end())
	{
		m_buffer_allocations.erase(it);
	}
}

void RHI::create_image(const rhi::ImageHandle &handle, const rhi::ImageCreateInfo &info)
{
}

void RHI::destroy_image(const rhi::ImageHandle &handle)
{
}

void RHI::create_program(const rhi::ShaderProgramHandle &handle, const rhi::ShaderProgramInput &input)
{
}

void RHI::destroy_program(const rhi::ShaderProgramHandle &handle)
{
}

void RHI::create_primitive(const rhi::PrimitiveHandle &handle, const rhi::PrimitiveCreateInfo &input)
{
}

void RHI::destroy_primitive(const rhi::PrimitiveHandle &handle)
{
}

void RHI::submit(const rhi::CommandList &command_list)
{
}
}        // namespace vulkan
}        // namespace remus
