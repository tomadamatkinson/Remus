#include "rhi/opengl/rhi.hpp"

#include <common/logging.hpp>

namespace remus
{
namespace gl
{

RHI::RHI()
{}

void RHI::create_image(const rhi::ImageHandle &handle, const rhi::ImageCreateInfo &info)
{
}

void RHI::destroy_image(const rhi::ImageHandle &handle)
{
}

void RHI::submit(const rhi::CommandList &command_list)
{
	for (auto &command : command_list)
	{
		switch (command->type)
		{
			case rhi::CommandType::Draw:
			{
				auto draw_command = static_cast<rhi::DrawCommand *>(command.get());
				auto primitive_it = m_primitives.find(draw_command->primitive_handle);
				if (primitive_it == m_primitives.end())
				{
					LOGE("Primitive not found");
					return;
				}

				auto shader_it = m_shaders.find(draw_command->program_handle);
				if (shader_it == m_shaders.end())
				{
					LOGE("Shader not found");
					return;
				}

				auto &primitive = primitive_it->second;
				auto &shader    = shader_it->second;

				glUseProgram(shader);
				glBindVertexArray(primitive.vao);
				glDrawElements(GL_TRIANGLES, draw_command->count, GL_UNSIGNED_INT, (const void *) nullptr);
				glBindVertexArray(0);
				break;
			}
			case rhi::CommandType::Unknown:
				break;
			default:
				continue;
		}
	}
}
}        // namespace gl
}        // namespace remus
