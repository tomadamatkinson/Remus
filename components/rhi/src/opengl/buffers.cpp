#include "rhi/opengl/rhi.hpp"

#include <common/logging.hpp>

#include <glad/glad.h>

namespace remus
{
namespace gl
{
inline GLenum get_buffer_usage(rhi::BufferUsage usage)
{
	switch (usage)
	{
		case rhi::BufferUsage::Vertex:
			return GL_ARRAY_BUFFER;
		case rhi::BufferUsage::Index:
			return GL_ELEMENT_ARRAY_BUFFER;
		case rhi::BufferUsage::Uniform:
			return GL_UNIFORM_BUFFER;
		case rhi::BufferUsage::Storage:
			return GL_SHADER_STORAGE_BUFFER;
		default:
			LOG_ASSERT(false, "Invalid buffer usage");
			return GL_ARRAY_BUFFER;
	}
}

void RHI::create_buffer(const rhi::BufferHandle &handle, const rhi::BufferCreateInfo &info)
{
	auto usage = get_buffer_usage(info.usage);

	uint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(usage, buffer);
	glBufferData(usage, info.size, nullptr, GL_STATIC_DRAW);

	m_buffers.emplace(
	    handle,
	    Buffer{
	        usage,
	        buffer,
	        info.size,
	    });
}

void RHI::update_buffer(const rhi::BufferHandle &handle, size_t offset, size_t size, void *data)
{
	auto it = m_buffers.find(handle);
	if (it == m_buffers.end())
	{
		LOGE("Buffer not found");
		return;
	}
	auto &buffer = it->second;

	if (offset + size > buffer.size)
	{
		LOGE("Buffer overflow");
		return;
	}

	glNamedBufferSubData(buffer.handle, offset, size, data);
}

void RHI::destroy_buffer(const rhi::BufferHandle &handle)
{
	auto it = m_buffers.find(handle);
	if (it == m_buffers.end())
	{
		LOGE("Buffer not found");
		return;
	}
	auto &buffer = it->second;
	glDeleteBuffers(1, &buffer.handle);
	m_buffers.erase(it);
}
}        // namespace gl
}        // namespace remus
