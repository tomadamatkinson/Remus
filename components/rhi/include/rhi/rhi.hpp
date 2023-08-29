#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef __APPLE__
#	include <sys/types.h>
#endif        // __APPLE__

namespace remus
{
namespace rhi
{
template <typename T>
struct Handle
{
	size_t handle = 0;
	void  *data   = nullptr;

	operator bool() const
	{
		return handle != 0;
	}

	bool operator==(const Handle<T> &other) const
	{
		return handle == other.handle;
	}

	static Handle<T> null()
	{
		return Handle<T>{};
	}

	static Handle<T> create()
	{
		static std::mutex           mutex;
		static size_t               next_handle = 0;
		std::lock_guard<std::mutex> lock(mutex);
		return Handle<T>{next_handle++, nullptr};
	}
};
}        // namespace rhi
}        // namespace remus

namespace std
{
template <typename T>
struct hash<remus::rhi::Handle<T>>
{
	size_t operator()(const remus::rhi::Handle<T> &handle) const
	{
		return handle.handle;
	}
};
}        // namespace std

namespace remus
{
namespace rhi
{

#define CREATE_RHI_HANDLE(type) \
	struct type                 \
	{                           \
	};                          \
	using type##Handle = Handle<type>;

CREATE_RHI_HANDLE(Buffer)
CREATE_RHI_HANDLE(Image)
CREATE_RHI_HANDLE(ShaderProgram)
CREATE_RHI_HANDLE(Primitive)

enum class BufferUsage
{
	Vertex,
	Index,
	Uniform,
	Storage,
};

struct BufferCreateInfo
{
	BufferUsage usage;
	size_t      size;
};

class Buffers
{
  public:
	virtual ~Buffers() = default;

	virtual void create_buffer(const BufferHandle &handle, const BufferCreateInfo &input) = 0;

	template <typename T>
	void update_buffer(const BufferHandle &handle, size_t offset, const T &data)
	{
		update_buffer(handle, offset, sizeof(T), const_cast<T *>(&data));
	}

	template <typename T>
	void update_buffer(const BufferHandle &handle, const T &data)
	{
		update_buffer(handle, 0, sizeof(T), const_cast<T *>(&data));
	}

	template <typename T>
	void update_buffer(const BufferHandle &handle, size_t offset, const std::vector<T> &data)
	{
		update_buffer(handle, offset, sizeof(T) * data.size(), const_cast<T *>(data.data()));
	}

	template <typename T>
	void update_buffer(const BufferHandle &handle, const std::vector<T> &data)
	{
		update_buffer(handle, 0, sizeof(T) * data.size(), const_cast<T *>(data.data()));
	}

	virtual void update_buffer(const BufferHandle &handle, size_t offset, size_t size, void *data) = 0;

	virtual void destroy_buffer(const BufferHandle &handle) = 0;
};

struct ImageCreateInfo
{};

class Images
{
  public:
	virtual ~Images() = default;

	virtual void create_image(const ImageHandle &handle, const ImageCreateInfo &input) = 0;
	virtual void destroy_image(const ImageHandle &handle)                              = 0;
};

enum class ShaderStage
{
	Vertex,
	Fragment,
	Compute,
};

struct ShaderStageInput
{
	ShaderStage          stage;
	std::string          entrypoint;
	std::vector<uint8_t> data;
};

struct ShaderProgramInput
{
	std::vector<ShaderStageInput> stages;
};

class Shaders
{
  public:
	virtual ~Shaders() = default;

	virtual void create_program(const ShaderProgramHandle &handle, const ShaderProgramInput &input) = 0;
	virtual void destroy_program(const ShaderProgramHandle &handle)                                 = 0;
};

struct VertexAttributeBinding
{
	size_t binding;
	size_t count;
	size_t stride;
	size_t offset;

	BufferHandle buffer_handle;
};

struct PrimitiveCreateInfo
{
	BufferHandle index_buffer_handle;

	std::vector<VertexAttributeBinding> vertex_attribute_bindings;
};

class Primitives
{
  public:
	virtual ~Primitives() = default;

	virtual void create_primitive(const PrimitiveHandle &handle, const PrimitiveCreateInfo &input) = 0;
	virtual void destroy_primitive(const PrimitiveHandle &handle)                                  = 0;
};

enum class CommandType
{
	Draw,
	Unknown,
};

struct Command
{
	virtual ~Command() = default;
	CommandType type{CommandType::Unknown};
};

using CommandList = std::vector<std::unique_ptr<Command>>;

template <CommandType T>
struct TypedCommand : public Command
{
	static constexpr CommandType TYPE = T;

	TypedCommand()
	{
		this->type = TYPE;
	}

	virtual ~TypedCommand() = default;
};

struct DrawCommand : public TypedCommand<CommandType::Draw>
{
	DrawCommand(ShaderProgramHandle program_handle, PrimitiveHandle primitive_handle, size_t count, size_t offset) :
	    program_handle(program_handle), primitive_handle(primitive_handle), count(count), offset(offset)
	{
	}
	virtual ~DrawCommand() = default;

	ShaderProgramHandle program_handle;
	PrimitiveHandle     primitive_handle;

	size_t count;
	size_t offset;
};

class CommandListBuilder
{
  public:
	CommandListBuilder()
	{
	}

	~CommandListBuilder()
	{}

	void add(std::unique_ptr<Command> command)
	{
		m_commands.emplace_back(std::move(command));
	}

	template <typename Command, typename... Args>
	void add(Args &&...args)
	{
		add(std::make_unique<Command>(std::forward<Args>(args)...));
	}

	CommandList release()
	{
		return std::move(m_commands);
	}

  private:
	CommandList m_commands;
};

class RHI : public Buffers, public Images, public Shaders, public Primitives
{
  public:
	RHI()          = default;
	virtual ~RHI() = default;

	RHI(const RHI &)            = delete;
	RHI &operator=(const RHI &) = delete;
	RHI(RHI &&)                 = delete;
	RHI &operator=(RHI &&)      = delete;

	virtual void submit(const CommandList &command_list) = 0;
};
}        // namespace rhi
}        // namespace remus
