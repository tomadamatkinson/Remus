#include "desktop_platform.hpp"

#include "glfw_window.hpp"

namespace remus
{
DesktopPlatform::DesktopPlatform()
{
}

DesktopPlatform::~DesktopPlatform()
{
}

std::shared_ptr<Window> DesktopPlatform::create_window(const char *title, const Extent2D &extent)
{
	return std::make_shared<GlfwWindow>(title, extent);
}

}        // namespace remus
