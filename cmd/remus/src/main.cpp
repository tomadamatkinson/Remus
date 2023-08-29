#include <common/logging.hpp>
#include <platforms/desktop_platform.hpp>
#include <rhi/rhi.hpp>
#include <rhi/vulkan/rhi.hpp>

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

std::string vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

std::string fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";

struct Vertex
{
	float x;
	float y;
	float z;
};

int main(int, char **)
{
	using namespace remus::rhi;

	remus::logging::init();

	remus::DesktopPlatform platform;

	auto window = platform.create_window("Remus", {800, 600});

	std::shared_ptr<RHI> rhi = std::make_shared<remus::vulkan::RHI>();

	// ShaderProgramHandle program = ShaderProgramHandle::create();

	// ShaderProgramInput input;
	// input.stages.push_back(ShaderStageInput{ShaderStage::Vertex, "main", {vertexShaderSource.begin(), vertexShaderSource.end()}});
	// input.stages.push_back(ShaderStageInput{ShaderStage::Fragment, "main", {fragmentShaderSource.begin(), fragmentShaderSource.end()}});
	// rhi->create_program(program, input);

	BufferHandle vertex_handle = BufferHandle::create();

	std::vector<Vertex> vertices{
	    {0.5f, 0.5f, 0.0f},          // top right
	    {0.5f, -0.5f, 0.0f},         // bottom right
	    {-0.5f, -0.5f, 0.0f},        // bottom left
	    {-0.5f, 0.5f, 0.0f},         // top left
	};

	rhi->create_buffer(vertex_handle, BufferCreateInfo{BufferUsage::Vertex, sizeof(Vertex) * vertices.size()});
	rhi->update_buffer(vertex_handle, vertices);

	// BufferHandle index_handle = BufferHandle::create();

	// std::vector<uint> indices{
	//     0, 1, 3,        // first Triangle
	//     1, 2, 3         // second Triangle
	// };

	// rhi->create_buffer(index_handle, BufferCreateInfo{BufferUsage::Index, sizeof(uint) * indices.size()});
	// rhi->update_buffer(index_handle, indices);

	// PrimitiveHandle primitive = PrimitiveHandle::create();

	// PrimitiveCreateInfo primitive_input;
	// primitive_input.index_buffer_handle = index_handle;
	// primitive_input.vertex_attribute_bindings.emplace_back(VertexAttributeBinding{0, 3, sizeof(Vertex), 0, vertex_handle});

	// rhi->create_primitive(primitive, primitive_input);

	// while (true)
	// {
	// 	auto extent = window->get_extent();

	// 	CommandListBuilder builder;
	// 	builder.add<DrawCommand>(program, primitive, 6, 0);

	// 	rhi->submit(builder.release());

	// 	window->update();

	// 	std::this_thread::sleep_for(std::chrono::milliseconds(16));
	// }

	rhi->destroy_buffer(vertex_handle);

	return 0;
}
