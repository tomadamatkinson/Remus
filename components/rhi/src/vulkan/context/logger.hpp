#pragma once

#include "context/context.hpp"

// Default implementation of the debug utils messenger callback
VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                              VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                              const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                              void                                       *user_data);

// Default implementation of the debug report callback
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT      flags,
                                              VkDebugReportObjectTypeEXT type,
                                              uint64_t                   object,
                                              size_t                     location,
                                              int32_t                    message_code,
                                              const char                *layer_prefix,
                                              const char                *message,
                                              void                      *user_data);
