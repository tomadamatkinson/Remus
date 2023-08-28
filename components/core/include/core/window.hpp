#pragma once

#include "common/math.hpp"

namespace remus
{
class Window
{
  public:
	virtual ~Window() = default;

	virtual Extent2D get_extent() const                 = 0;
	virtual void     set_extent(const Extent2D &extent) = 0;
	virtual void     set_title(const char *title)       = 0;
	virtual void     update()                           = 0;
};
}        // namespace remus
