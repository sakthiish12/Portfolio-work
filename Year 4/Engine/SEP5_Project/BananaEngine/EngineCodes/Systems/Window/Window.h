#pragma once
#ifndef	WINDOW_H
#define WINDOW_H
#include <Precompiled/pch.h>
#include <EventDispatcher/EventDispatcher.h>
namespace BE 
{
	class BeWindow {
	public:
		BeWindow(int w, int h, std::string name);
		~BeWindow();

		BeWindow(const BeWindow&) = delete;
		BeWindow& operator=(const BeWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }
		void CloseWindow() { glfwSetWindowShouldClose(window, 1); }
		
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
		static void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void windowFocused_callback(GLFWwindow* window, int isFocused);
		static void window_size_callback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;

		//Debug stuff
		VkDebugReportCallbackEXT debugReportCallBack{};
		VkDebugUtilsMessengerEXT debugUtilsMessenger{}; // Vulkan debug output handle

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{};
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{};

		PFN_vkDebugReportCallbackEXT debugReportCallBackFunction{};
		PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsCallBackFunction{};
	public:
		inline GLFWwindow* GetGLFWWindow() const { return window; }
	};
}
#endif