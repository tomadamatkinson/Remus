#pragma once

#include "rhi/rhi.hpp"

#include <memory>

namespace remus
{
namespace vulkan
{
class RHI : public rhi::RHI
{
  public:
	RHI();
	virtual ~RHI();

	virtual void create_buffer(const rhi::BufferHandle &handle, const rhi::BufferCreateInfo &info) override;
	virtual void update_buffer(const rhi::BufferHandle &handle, size_t offset, size_t size, void *data) override;
	virtual void destroy_buffer(const rhi::BufferHandle &handle) override;
	virtual void create_image(const rhi::ImageHandle &handle, const rhi::ImageCreateInfo &info) override;
	virtual void destroy_image(const rhi::ImageHandle &handle) override;
	virtual void create_program(const rhi::ShaderProgramHandle &handle, const rhi::ShaderProgramInput &input) override;
	virtual void destroy_program(const rhi::ShaderProgramHandle &handle) override;
	virtual void create_primitive(const rhi::PrimitiveHandle &handle, const rhi::PrimitiveCreateInfo &input) override;
	virtual void destroy_primitive(const rhi::PrimitiveHandle &handle) override;
	virtual void submit(const rhi::CommandList &command_list) override;

  private:
	std::shared_ptr<class Context>    m_context;
	std::shared_ptr<class MemoryPool> m_memory_pool;

	std::unordered_map<rhi::BufferHandle, std::shared_ptr<class BufferAllocation>> m_buffer_allocations;
};
}        // namespace vulkan
}        // namespace remus
