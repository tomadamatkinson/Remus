#include "rhi/opengl/rhi.hpp"

#include <glad/glad.h>

#include <common/logging.hpp>

namespace remus
{
namespace gl
{
inline GLenum get_shader_stage(rhi::ShaderStage stage)
{
	switch (stage)
	{
		case rhi::ShaderStage::Vertex:
			return GL_VERTEX_SHADER;
		case rhi::ShaderStage::Fragment:
			return GL_FRAGMENT_SHADER;
		case rhi::ShaderStage::Compute:
			return GL_COMPUTE_SHADER;
		default:
			LOG_ASSERT(false, "Invalid shader stage");
			return GL_VERTEX_SHADER;
	}
}

void RHI::create_program(const rhi::ShaderProgramHandle &handle, const rhi::ShaderProgramInput &input)
{
	std::unordered_map<rhi::ShaderStage, uint> shader_stages;

	int  success;
	char info_log[512];

	for (auto &stage : input.stages)
	{
		GLenum      gl_stage      = get_shader_stage(stage.stage);
		uint        shader        = glCreateShader(gl_stage);
		const char *shader_source = reinterpret_cast<const char *>(stage.data.data());
		GLint       length        = stage.data.size();
		glShaderSource(shader, 1, &shader_source, &length);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			LOGE("Shader Compilation Failed: {}", info_log);
		}

		shader_stages.emplace(stage.stage, shader);
	}

	uint program = glCreateProgram();
	for (auto &[stage, shader] : shader_stages)
	{
		glAttachShader(program, shader);
	}
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, info_log);
		LOGE("Shader Program Linking Failed: {}", info_log);
	}

	for (auto &[stage, shader] : shader_stages)
	{
		glDeleteShader(shader);
	}

	m_shaders.emplace(handle, program);
}

void RHI::destroy_program(const rhi::ShaderProgramHandle &handle)
{
	auto it = m_shaders.find(handle);
	if (it != m_shaders.end())
	{
		glDeleteProgram(it->second);
		m_shaders.erase(it);
	}
}

}        // namespace gl
}        // namespace remus
