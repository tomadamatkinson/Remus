#pragma once

#include <spdlog/spdlog.h>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define LOGGER_PATTERN "%^[%H:%M:%S.%e] [%n] [%l]%$ %v"

#define DEFINE_LOGGER_EXTERN(name) \
	std::shared_ptr<spdlog::logger> get_logger_##name();

#define DEFINE_LOGGER_IMPL(name)                        \
	std::shared_ptr<spdlog::logger> get_logger_##name() \
	{                                                   \
		static std::shared_ptr<spdlog::logger> logger;  \
		if (logger)                                     \
		{                                               \
			return logger;                              \
		}                                               \
		logger = spdlog::stdout_color_mt(#name);        \
		logger->set_pattern(LOGGER_PATTERN);            \
		return logger;                                  \
	}

#define LOGI(name, ...) get_logger_##name()->info(__VA_ARGS__)
#define LOGW(name, ...) get_logger_##name()->warn(__VA_ARGS__)
#define LOGE(name, ...) get_logger_##name()->error(__VA_ARGS__)
#define LOGD(name, ...) get_logger_##name()->debug(__VA_ARGS__)

#define LOG_CHECK(name, x, ...)      \
	if (x)                           \
	{                                \
		LOGE(name, __VA_ARGS__);     \
		throw "Unrecoverable Error"; \
	}

namespace remus
{
namespace logging
{
inline void init()
{
	auto env_val = spdlog::details::os::getenv("REMUS_LEVEL");
	if (!env_val.empty())
	{
		spdlog::cfg::helpers::load_levels(env_val);
	}

	auto logger = spdlog::stdout_color_mt("remus");
	spdlog::set_default_logger(logger);
}
}        // namespace logging
}        // namespace remus
