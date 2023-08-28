#include "rhi/opengl/rhi.hpp"

#include <common/logging.hpp>

namespace remus
{
namespace gl
{
void RHI::create_primitive(const rhi::PrimitiveHandle &handle, const rhi::PrimitiveCreateInfo &input)
{
	auto it = m_buffers.find(input.index_buffer_handle);
	if (it == m_buffers.end())
	{
		LOGE("Buffer not found");
		return;
	}
	auto &index_buffer = it->second;

	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.handle);

	for (auto &binding : input.vertex_attribute_bindings)
	{
		auto it = m_buffers.find(binding.buffer_handle);
		if (it == m_buffers.end())
		{
			LOGE("Buffer not found");
			return;
		}
		auto &buffer = it->second;

		glVertexAttribPointer(binding.binding, binding.count, GL_FLOAT, GL_FALSE, binding.stride, (void *) binding.offset);
		glEnableVertexAttribArray(binding.binding);

		glBindBuffer(buffer.usage, buffer.handle);
	}

	glBindVertexArray(0);

	m_primitives.emplace(
	    handle,
	    Primitive{
	        vao,
	        index_buffer.handle,
	    });
}

void RHI::destroy_primitive(const rhi::PrimitiveHandle &handle)
{
	auto it = m_primitives.find(handle);
	if (it == m_primitives.end())
	{
		LOGE("Primitive not found");
		return;
	}
	auto &primitive = it->second;
	glDeleteVertexArrays(1, &primitive.vao);
	m_primitives.erase(it);
}
}        // namespace gl
}        // namespace remus
