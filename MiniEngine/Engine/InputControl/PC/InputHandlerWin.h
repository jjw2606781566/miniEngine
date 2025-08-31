#pragma once
#if WIN32

#include "Engine/InputControl/TankinInput.h"
#include "Engine/Utility/MacroUtility.h"

class InputHandlerWIN: public TankinInput
{
    friend void TankinInput::sInit();
public:
    Vector2 getMouseMotion() const override;
    Vector2 getMousePosition() const override;
    int16_t getWheelDelta() const override;

    bool isKeyDown(KeyCode key) override;
    bool isKeyHeld(KeyCode key) override;
    bool isKeyPressed(KeyCode key) override;
    bool isKeyUp(KeyCode key) override;
	Vector2 getInputAxis(AxisType axisType) override;
    void bindAxisMap(AxisKeyCode axisCode, KeyCode keyCode);

    void onMouseMove(int16_t x, int16_t y) ;
    void onMouseAbsoluteMove(int16_t x, int16_t y);
    void onMouseWheel(int16_t delta);
    
    void onKeyDown(WPARAM wParam, LPARAM lParam);
    void onKeyUp(WPARAM wParam, LPARAM lParam);

    void ConstrainCursor(HWND hwnd) const;

    void update() override;

    void clearInput()
    {
        for (auto& keyState : mKeyStates)
        {
            keyState.second.state = KEY_UP;
        }
    }
private:
    InputHandlerWIN();
    struct MouseState
    {
        POINT absolutePosition;
        POINT position;
        POINT motion;
        POINT dragStart;
        int16_t wheelDelta;
        bool isDragging;
        bool leftButton;
    };

    struct KeyState
    {
        int state = 0; // 按键状态
        uint64_t updateFrame = 0; // 按下的帧
    };
    std::unordered_map<uint32_t, KeyState> mKeyStates;
    MouseState mMouseState;
    uint64_t mFrameCounter;
    std::unordered_map<AxisKeyCode, KeyCode> mAxisMap;
};


#endif