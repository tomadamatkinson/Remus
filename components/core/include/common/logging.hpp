#pragma once

#include <spdlog/spdlog.h>

#define LOGI(...) spdlog::info(__VA_ARGS__)
#define LOGW(...) spdlog::warn(__VA_ARGS__)
#define LOGE(...) spdlog::error(__VA_ARGS__)
#define LOGD(...) spdlog::debug(__VA_ARGS__)

#define LOG_ASSERT(x, ...)           \
	if (x)                           \
	{                                \
		LOGE(__VA_ARGS__);           \
		throw "Unrecoverable Error"; \
	}
