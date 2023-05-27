#include "pch.h"
#include "Window.h"
#pragma warning (push)
#pragma warning (disable:26812)
namespace BE
{
	BeWindow::BeWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	BeWindow::~BeWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void BeWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

#ifndef USE_EDITOR
		window = glfwCreateWindow(width, height, windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
#else
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
#endif
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mousebutton_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetWindowFocusCallback(window, windowFocused_callback);
		glfwSetWindowSizeCallback(window, window_size_callback);
	}

	void BeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to craete window surface");
		}
	}

	void BeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto beWindow = reinterpret_cast<BeWindow*>(glfwGetWindowUserPointer(window));
		beWindow->framebufferResized = true;
		beWindow->width = width;
		beWindow->height = height;
	}

	void BeWindow::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		UNUSED(window);
		//auto beWindow = reinterpret_cast<BeWindow*>(glfwGetWindowUserPointer(window));
		KeyButtonEvent kb{ key, action };
		EventMgr->Dispatch(kb);
	}

	void BeWindow::mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
	{
		UNUSED(window);
		//auto beWindow = reinterpret_cast<BeWindow*>(glfwGetWindowUserPointer(window));
		MouseButtonEvent mb{ button, action };
		EventMgr->Dispatch(mb);
	}

	void BeWindow::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		UNUSED(window);
		//auto beWindow = reinterpret_cast<BeWindow*>(glfwGetWindowUserPointer(window));
		MouseMoveEvent mm{ (float)xpos, (float)ypos };
		EventMgr->Dispatch(mm);
	}

	void BeWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		UNUSED(window);
		//auto beWindow = reinterpret_cast<BeWindow*>(glfwGetWindowUserPointer(window));
		MouseScrollEvent ms{ (float)xoffset, (float)yoffset };
		EventMgr->Dispatch(ms);
	}

	void BeWindow::windowFocused_callback(GLFWwindow* window, int isFocused)
	{
		UNUSED(window);

		WindowInterruptEvent wi((bool)isFocused);
		EventMgr->Dispatch(wi);
	}

	void BeWindow::window_size_callback(GLFWwindow* window, int width, int height)
	{
		WindowResizeEvent wr(width, height);
		EventMgr->Dispatch(wr);
	}


}
#pragma warning (pop)