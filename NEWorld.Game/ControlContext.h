#pragma once

#include <GLFW/glfw3.h>

#include "Math/Vector2.h"

class ControlContext {
public:
	struct Frame {
		bool LeftMouse, MiddleMouse, RightMouse;
		double MouseScroll;
		Double2 MousePosition;
		double Time;
	private:
		int KeyPressed[GLFW_KEY_LAST];
		friend class ControlContext;
	};
	ControlContext(GLFWwindow* window) {
		
	}
	ControlContext(ControlContext&) = delete;
	ControlContext operator=(ControlContext&) = delete;

	Frame Current, Last;

	void Update() {
		Last = Current;
		mFrameCounter++;

		glfwGetCursorPos(mWindow, &Current.MousePosition.X, &Current.MousePosition.Y);
		Current.LeftMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		Current.RightMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		Current.MiddleMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
		Current.Time = timer();
		// MouseScroll will be updated by callback
		// KeyPressed will be updated lazily
		memset(Current.KeyPressed, 0, sizeof Current.KeyPressed );
	}

	bool KeyPressed(int key) const noexcept {
		return glfwGetKey(mWindow, key); // TODO: update frame
	}
private:
	GLFWwindow* mWindow;
	size_t mFrameCounter;
};
