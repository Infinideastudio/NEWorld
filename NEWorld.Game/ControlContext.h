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
	struct KeyState {
		bool Pressed : 1;
		uint16_t LastPressedFrame : 15; // 0 represents never pressed
	};

	enum class Action { PLACE_BLOCK, PICK_BLOCK };
	struct Frame {
		bool LeftMouse{}, MiddleMouse{}, RightMouse{};
		double MouseScroll{};
		Double2 MousePosition{};
		double Time{};
		std::array<KeyState, GLFW_KEY_LAST> KeyState{};
	};

	ControlContext(GLFWwindow* window) :mWindow(window) {
		if (!Listener) Listener = MessageBus::Default().Get<std::pair<int, int>>("KeyEvents")->Listen(
			[this](void*, std::pair<int, int> keyAndAction) {
				auto [key, action] = keyAndAction;
				KeyStates[key].Pressed = action != GLFW_RELEASE;
				if (KeyStates[key].Pressed)
					KeyStates[key].LastPressedFrame = mFrameCounter;
			});
	}
	ControlContext(ControlContext&) = delete;
	ControlContext operator=(ControlContext&) = delete;

	Frame Current, Last;

	void Update() {
		Last = Current;
		mFrameCounter++;
		if (mFrameCounter > 1<<15) mFrameCounter = 1; // loop back to squeeze into 7 bits

		glfwGetCursorPos(mWindow, &Current.MousePosition.X, &Current.MousePosition.Y);
		Current.LeftMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		Current.RightMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		Current.MiddleMouse = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
		Current.Time = timer();
		Current.MouseScroll = MouseScroll;
		std::copy(KeyStates.begin(), KeyStates.end(), Current.KeyState.begin());
	}

	[[nodiscard]] bool KeyPressed(int key) const noexcept {
		return Current.KeyState[key].Pressed;
	}

	[[nodiscard]] bool KeyJustDoublePressed(int key, int intervalInFrames = 8) const noexcept {
		assert(intervalInFrames <= 1<<15);

		if (!Current.KeyState[key].Pressed || Last.KeyState[key].Pressed) return false;

		auto lastPressed = Last.KeyState[key].LastPressedFrame;
		if (lastPressed == 0) return false;

		auto currentFrame = mFrameCounter;
		if (currentFrame < lastPressed) currentFrame += 1<<15;
		return currentFrame - lastPressed < intervalInFrames;
	}

	[[nodiscard]] bool KeyJustPressed(int key) const noexcept {
		return Current.KeyState[key].Pressed && !Last.KeyState[key].Pressed;
	}

	[[nodiscard]] bool ShouldDo(Action action) {
		switch (action) {
		case Action::PLACE_BLOCK:
			return (Current.RightMouse && !Last.RightMouse) || KeyPressed(GLFW_KEY_TAB);
		case Action::PICK_BLOCK:
			return Current.LeftMouse || KeyPressed(GLFW_KEY_ENTER);
		default:
			return false;
		}
	}

	static void MouseScrollCallback(GLFWwindow*, double, double yOffset) {
		MouseScroll += yOffset;
	}

private:
	GLFWwindow* mWindow;
	uint16_t mFrameCounter = 1;

	inline static std::shared_ptr<PmrBase> Listener;
	inline static double MouseScroll = 0;
	inline static std::array<KeyState, GLFW_KEY_LAST> KeyStates;
};
