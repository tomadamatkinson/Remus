#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace remus
{
namespace vulkan
{
enum class LogLevel
{
	Verbose,
	Debug,
	Info,
	Warning,
	Error,
};

using SimpleLogCallback      = std::function<void(LogLevel, const char *)>;
using DebugUtilsLogCallback  = std::function<void(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *)>;
using DebugReportLogCallback = std::function<void(vk::DebugReportFlagsEXT, vk::DebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char *, const char *)>;

/**
 * @brief A set of callbacks for logging Vulkan messages
 * @note It is not guaranteed that all callbacks will be called for every message
 *       as extension specific callbacks may or may not be available.
 */
struct LoggerCallbacks
{
	SimpleLogCallback      simple_callback{nullptr};
	DebugUtilsLogCallback  debug_utils_callback{nullptr};
	DebugReportLogCallback debug_report_callback{nullptr};
};

class ContextBuilder;

// How layers and extensions are handled
enum class InclusionMode
{
	Optional,        // Can be enabled, but not required
	Required,        // Must be enabled
};

/**
 * @brief A context contains the core Vulkan objects required for most operations
 */
class Context final
{
	friend class ContextBuilder;
	friend class InstanceBuilder;
	friend class DeviceBuilder;

  public:
	using CleanupCallback = std::function<void()>;

	~Context();

	vk::Instance       instance;
	vk::PhysicalDevice physical_device;
	vk::Device         device;

	void add_cleanup_callback(CleanupCallback &&callback);

	vk::Queue get_queue(vk::QueueFlags supported_types) const;
	uint32_t  get_queue_family_index(vk::Queue queue) const;

  private:
	// Only allow context to be created by the context builder
	// Prevents misuse of the Context class
	Context() = default;

	std::vector<CleanupCallback> cleanup_callbacks;

	struct QueueGroup
	{
		uint32_t                    queue_family_index;
		vk::QueueFlags              supported_queues;
		std::vector<vk::Queue>      queues;
		std::vector<vk::SurfaceKHR> supported_presentation_surfaces;
	};

	std::vector<QueueGroup> queue_groups;

	// keep the loggers alive for the lifetime of the context
	// void* user_data == LoggerCallbacks*
	std::shared_ptr<LoggerCallbacks> logging_callbacks;
	vk::DebugUtilsMessengerEXT       debug_utils_messenger;
	vk::DebugReportCallbackEXT       debug_report_callback;
};

using ContextPtr     = std::shared_ptr<Context>;
using WeakContextPtr = std::weak_ptr<Context>;
}        // namespace vulkan
}        // namespace remus
