#include "context/context.hpp"

namespace remus
{
namespace vulkan
{
Context::~Context()
{
	// clean up requested resources in reverse order
	for (auto it = cleanup_callbacks.rbegin(); it != cleanup_callbacks.rend(); ++it)
	{
		if ((*it))
		{
			(*it)();
		}
	}

	if (device)
	{
		device.destroy();
	}

	if (debug_utils_messenger)
	{
		instance.destroyDebugUtilsMessengerEXT(debug_utils_messenger);
	}
	if (debug_report_callback)
	{
		instance.destroyDebugReportCallbackEXT(debug_report_callback);
	}

	if (instance)
	{
		instance.destroy();
	}
}

void Context::add_cleanup_callback(CleanupCallback &&callback)
{
	cleanup_callbacks.push_back(callback);
}

vk::Queue Context::get_queue(vk::QueueFlags supported_types) const
{
	for (auto &group : queue_groups)
	{
		if (group.supported_queues & supported_types && group.queues.size() > 0)
		{
			// TODO: make this more complex
			return group.queues[0];
		}
	}

	return vk::Queue{VK_NULL_HANDLE};
}

uint32_t Context::get_queue_family_index(vk::Queue queue) const
{
	for (auto &group : queue_groups)
	{
		for (auto q : group.queues)
		{
			if (q == queue)
			{
				return group.queue_family_index;
			}
		}
	}

	return VK_QUEUE_FAMILY_IGNORED;
}

}        // namespace vulkan
}        // namespace remus
