#pragma once

#include "platform/platform.hpp"

namespace remus
{
class DesktopPlatform : public Platform
{
  public:
	DesktopPlatform();
	~DesktopPlatform() override;

	std::shared_ptr<Window> create_window(const char *title, const Extent2D &extent) override;
};
}        // namespace remus
