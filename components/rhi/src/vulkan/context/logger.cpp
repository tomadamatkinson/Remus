#include "context/logger.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                              VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                              const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                              void                                       *user_data)
{
	if (!user_data)
	{
		return VK_FALSE;
	}

	auto *logger_callbacks = static_cast<remus::vulkan::LoggerCallbacks *>(user_data);

	if (logger_callbacks->simple_callback)
	{
		remus::vulkan::LogLevel log_level = remus::vulkan::LogLevel::Verbose;
		switch (message_severity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Verbose;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Info;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Warning;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Error;
				break;
			default:
				log_level = remus::vulkan::LogLevel::Verbose;
				break;
		}

		if (callback_data && callback_data->pMessage)
		{
			logger_callbacks->simple_callback(log_level, callback_data->pMessage);
		}
	}

	if (logger_callbacks->debug_utils_callback)
	{
		logger_callbacks->debug_utils_callback(
		    static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(message_severity),
		    static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(message_type),
		    callback_data);
	}

	return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT      flags,
                                              VkDebugReportObjectTypeEXT type,
                                              uint64_t                   object,
                                              size_t                     location,
                                              int32_t                    message_code,
                                              const char                *layer_prefix,
                                              const char                *message,
                                              void                      *user_data)
{
	if (!user_data)
	{
		return VK_FALSE;
	}

	auto *logger_callbacks = static_cast<remus::vulkan::LoggerCallbacks *>(user_data);

	if (logger_callbacks->simple_callback)
	{
		remus::vulkan::LogLevel log_level = remus::vulkan::LogLevel::Verbose;
		switch (flags)
		{
			case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Info;
				break;
			case VK_DEBUG_REPORT_WARNING_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Warning;
				break;
			case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Warning;
				break;
			case VK_DEBUG_REPORT_ERROR_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Error;
				break;
			case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
				log_level = remus::vulkan::LogLevel::Debug;
				break;
			default:
				log_level = remus::vulkan::LogLevel::Verbose;
				break;
		}

		logger_callbacks->simple_callback(log_level, message);
	}

	if (logger_callbacks->debug_report_callback)
	{
		logger_callbacks->debug_report_callback(
		    static_cast<vk::DebugReportFlagsEXT>(flags),
		    static_cast<vk::DebugReportObjectTypeEXT>(type),
		    object,
		    location,
		    message_code,
		    layer_prefix,
		    message);
	}

	return VK_FALSE;
}
