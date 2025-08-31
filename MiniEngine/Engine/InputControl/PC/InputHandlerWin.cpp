#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
#include "Engine/Window/WFrame.h"
#include "InputHandlerWin.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT InputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	InputHandlerWIN* input = dynamic_cast<InputHandlerWIN*>(TankinInput::sInstance);
	
	//imGui, capture message
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;
	
	static bool needClearInput = true;
	if (ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow))
	{
		if (needClearInput)
		{
			input->clearInput();
			needClearInput = false;
		}
		return DefaultFrameProc(hwnd, message, wParam, lParam);
	}
	else
	{
		needClearInput = true;
	}

	static HKL hkl = LoadKeyboardLayout(TEXT("00000409"), KLF_ACTIVATE);

	switch (message)
	{
	case WM_ACTIVATE:
		if (wParam != WA_INACTIVE)
		{
			input->ConstrainCursor(hwnd);
		
			ActivateKeyboardLayout(hkl, KLF_SETFORPROCESS);
		}
	case WM_MOUSEMOVE:
		RECT rect;
		GetClientRect(hwnd, &rect);
		input->onMouseAbsoluteMove(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		input->onKeyDown(wParam, lParam);
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		input->onKeyUp(wParam, lParam);
		return 0;
	case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				//RY_INFO(std::format(L"Mouse Position:({}, {})\n", xPosRelative, yPosRelative));
				input->onMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			}
			break;
		}
	case WM_MOUSEWHEEL:
		input->onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		return 0;
	default:
		return DefaultFrameProc(hwnd, message, wParam, lParam);
	}
	return DefaultFrameProc(hwnd, message, wParam, lParam); 
}

void InputHandlerWIN::onMouseMove(int16_t x, int16_t y)
{
	// 更新位置
	//RY_INFO(std::format(L"Mouse Motion:({}, {})\n", x, y));
	mMouseState.motion.x = x;
	mMouseState.motion.y = y;
	mMouseState.position.x += x;
	mMouseState.position.y += y;
}

void InputHandlerWIN::onMouseAbsoluteMove(int16_t x, int16_t y)
{
	mMouseState.absolutePosition.x = x;
	mMouseState.absolutePosition.y = y;
}

void InputHandlerWIN::onMouseWheel(const int16_t delta)
{
	mMouseState.wheelDelta = delta;
}

Vector2 InputHandlerWIN::getMousePosition() const 
{
	Vector2 position{static_cast<float>(mMouseState.absolutePosition.x)
		, static_cast<float>(mMouseState.absolutePosition.y)};
	return position;
}

int16_t InputHandlerWIN::getWheelDelta() const 
{
	return mMouseState.wheelDelta;
}

Vector2 InputHandlerWIN::getMouseMotion() const 
{
	Vector2 result{static_cast<float>(mMouseState.motion.x),
		static_cast<float>(mMouseState.motion.y)};
	return result;
}

bool InputHandlerWIN::isKeyDown(const KeyCode key) 
{
	return mKeyStates[key].state == KEY_DOWN && mKeyStates[key].updateFrame == mFrameCounter;
}

bool InputHandlerWIN::isKeyHeld(const KeyCode key) 
{
	return mKeyStates[key].state == KEY_HELD;
}

bool InputHandlerWIN::isKeyPressed(const KeyCode key) 
{
	const auto& state = mKeyStates[key].state;
	return state == KEY_HELD || state == KEY_DOWN;
}

bool InputHandlerWIN::isKeyUp(const KeyCode key) 
{
	const auto& state = mKeyStates[key];
	return state.state == KEY_UP && state.updateFrame == mFrameCounter;
}

Vector2 InputHandlerWIN::getInputAxis(AxisType axisType)
{
	Vector2 result{};
	if (axisType == AxisType::LEFT_STICK)
	{
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::LEFT_STICK_UP)))
		{
			result.v.y = 1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::LEFT_STICK_DOWN)))
		{
			result.v.y = -1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::LEFT_STICK_RIGHT)))
		{
			result.v.x = 1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::LEFT_STICK_LEFT)))
		{
			result.v.x = -1;
		}
	}
	else
	{
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::RIGHT_STICK_UP)))
		{
			result.v.y = 1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::RIGHT_STICK_DOWN)))
		{
			result.v.y = -1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::RIGHT_STICK_RIGHT)))
		{
			result.v.x = 1;
		}
		if (isKeyPressed(mAxisMap.at(AxisKeyCode::RIGHT_STICK_LEFT)))
		{
			result.v.x = -1;
		}
	}
	return result;
}

void InputHandlerWIN::bindAxisMap(AxisKeyCode axisCode, KeyCode keyCode)
{
	mAxisMap[axisCode] = keyCode;
}

void InputHandlerWIN::onKeyDown(WPARAM wParam, LPARAM lParam)
{
	UINT key = static_cast<UINT>(wParam);
	auto& keyState = mKeyStates[key];
	keyState.updateFrame = mFrameCounter;

	if (keyState.state == KEY_HELD) return;
	if (keyState.state == KEY_DOWN)
	{
		keyState.state = KEY_HELD;
		return;
	}
	keyState.state = KEY_DOWN;
}

void InputHandlerWIN::onKeyUp(WPARAM wParam, LPARAM lParam)
{
	UINT key = static_cast<UINT>(wParam);
	auto& keyState = mKeyStates[key];
	keyState.updateFrame = mFrameCounter;
	keyState.state = KEY_UP;
}

void InputHandlerWIN::update()
{
	mFrameCounter++;
	memset(&mMouseState.motion, 0, sizeof(POINT));
}

void InputHandlerWIN::ConstrainCursor(HWND hwnd) const
{
	RECT windowRect;
	RECT clientRect;
	GetWindowRect(hwnd, &windowRect);
	GetClientRect(hwnd, &clientRect);
	windowRect.top = clientRect.top;
	windowRect.bottom = clientRect.bottom;
	windowRect.left = clientRect.left;
	windowRect.right = clientRect.right;
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&windowRect));
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&windowRect) + 1);
    
	//解锁的话就这里传一个空指针
	ClipCursor(&windowRect);
}

InputHandlerWIN::InputHandlerWIN():mKeyStates(),mMouseState(),mFrameCounter()
{
	mKeyStates.max_load_factor(0.5);
}

#endif
