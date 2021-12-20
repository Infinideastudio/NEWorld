#pragma once
#include "stdinclude.h"
#include <GLFW/glfw3.h>
#include <array>
#include "Math/Vector2.h"
#include "FunctionsKit.h"
#include "Common/Logger.h"
#include "System/MessageBus.h"

class ControlContext {
public:
	enum class Action { PLACE_BLOCK, PICK_BLOCK };
	struct Frame {
		bool LeftMouse{}, MiddleMouse{}, RightMouse{};
		double MouseScroll{};
		Double2 MousePosition{};
		double Time{};
		std::array<int, GLFW_KEY_LAST> KeyPressed{};
	};

	ControlContext(GLFWwindow* window) :mWindow(window) {
		if(!Listener) Listener = MessageBus::Default().Get<std::pair<int, int>>("KeyEvents")->Listen(
			[this](void*, std::pair<int, int> keyAndAction) {
				auto [key, action] = keyAndAction;
				KeyStates[key] = action != GLFW_RELEASE;
			});
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
		Current.MouseScroll = MouseScroll;
		std::copy(KeyStates.begin(), KeyStates.end(), Current.KeyPressed.begin());
	}

	[[nodiscard]] bool KeyPressed(int key) const noexcept {
		return Current.KeyPressed[key];
	}

	[[nodiscard]] bool KeyJustPressed(int key) const noexcept {
		return Current.KeyPressed[key] && !Last.KeyPressed[key];
	}

	[[nodiscard]] bool ShouldDo(Action action) {
		switch(action)
		{
		case Action::PLACE_BLOCK:
			return (Current.RightMouse && !Last.RightMouse) || KeyPressed(GLFW_KEY_TAB);
		case Action::PICK_BLOCK:
			return Current.LeftMouse || KeyPressed(GLFW_KEY_ENTER);
		}
	}

	static void MouseScrollCallback(GLFWwindow*, double, double yOffset) {
		MouseScroll += yOffset;
	}

private:
	GLFWwindow* mWindow;
	size_t mFrameCounter;

	inline static std::shared_ptr<PmrBase> Listener;
	inline static double MouseScroll = 0;
	inline static std::array<bool, GLFW_KEY_LAST> KeyStates;
};
