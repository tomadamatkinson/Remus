#pragma once

#include "rhi/rhi.hpp"

#include <glad/glad.h>

#include <unordered_map>

namespace remus
{
namespace gl
{
class RHI : public rhi::RHI
{
  public:
	RHI();
	virtual ~RHI() = default;

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
	std::unordered_map<rhi::ShaderProgramHandle, uint32_t> m_shaders;

	struct Buffer
	{
		GLenum usage;
		uint   handle;
		size_t size;
	};

	std::unordered_map<rhi::BufferHandle, Buffer> m_buffers;

	struct Primitive
	{
		uint vao;
		uint index_buffer;
	};

	std::unordered_map<rhi::PrimitiveHandle, Primitive> m_primitives;
};
}        // namespace gl
}        // namespace remus
