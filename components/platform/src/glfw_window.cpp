#include "glfw_window.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <common/logging.hpp>

namespace remus
{
GlfwWindow::GlfwWindow(const char *title, const Extent2D &extent)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_window = glfwCreateWindow(extent.width, extent.height, title, nullptr, nullptr);
	LOG_ASSERT(m_window == nullptr, "Failed to create GLFW window");

	glfwMakeContextCurrent(m_window);

	LOG_ASSERT(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "Failed to initialize GLAD");
}

GlfwWindow::~GlfwWindow()
{
	glfwDestroyWindow(m_window);
}

Extent2D GlfwWindow::get_extent() const
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void GlfwWindow::set_extent(const Extent2D &extent)
{
	glfwSetWindowSize(m_window, extent.width, extent.height);
}

void GlfwWindow::set_title(const char *title)
{
	glfwSetWindowTitle(m_window, title);
}

void GlfwWindow::update()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

GLFWwindow *GlfwWindow::get_native_window() const
{
	return m_window;
}
}        // namespace remus
