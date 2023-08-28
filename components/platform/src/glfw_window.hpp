#pragma once

#include <core/window.hpp>

struct GLFWwindow;

namespace remus
{

class GlfwWindow final : public Window
{
  public:
	GlfwWindow(const char *title, const Extent2D &extent);
	~GlfwWindow() override;

	Extent2D get_extent() const override;
	void     set_extent(const Extent2D &extent) override;
	void     set_title(const char *title) override;
	void     update() override;

	GLFWwindow *get_native_window() const;

  private:
	GLFWwindow *m_window;
	Extent2D    m_extent;
};
}        // namespace remus
