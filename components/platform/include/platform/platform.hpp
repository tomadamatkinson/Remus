#pragma once

#include <memory>

#include <core/window.hpp>

namespace remus
{
class Platform
{
  public:
	virtual ~Platform() = default;

	virtual std::shared_ptr<Window> create_window(const char *title, const Extent2D &extent) = 0;
};
}        // namespace remus
