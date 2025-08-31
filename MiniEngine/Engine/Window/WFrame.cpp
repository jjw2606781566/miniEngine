﻿#include "WFrame.h"

#include "Engine/common/Exception.h"
#include "Engine/common/PC/WFunc.h"


LRESULT DefaultFrameProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WFrame* frame;
    switch (message)
    {
    case WM_NCHITTEST:
        {
            const auto res = DefWindowProc(hwnd, message, wParam, lParam);
            // 禁用缩放
            if (res >= HTLEFT && res <= HTBOTTOMRIGHT) return HTBORDER;
            return res;
        }
    // attach WFrame instance to hwnd
    case WM_NCCREATE:
        frame = static_cast<WFrame*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        if (frame) SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(frame));
        return DefWindowProc(hwnd, message, wParam, lParam);
    case WM_DPICHANGED:
        if ((frame = reinterpret_cast<WFrame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))) 
            frame->OnDpiChanged(HIWORD(wParam));
        return 0;
    case WM_CLOSE:
        if ((frame = reinterpret_cast<WFrame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))))
        {
            frame->Frame::Close();
            DestroyWindow(hwnd);
        }
        else
            DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

Frame* Frame::CreateFrame(const String& title, uint16_t width, uint16_t height, bool isFullScreen = false, WNDPROC pWindowProc = DefaultFrameProc)
{
    return new WFrame(title, width, height, isFullScreen, pWindowProc);
}

WindowClass::WindowClass(const String& name, WNDPROC pWindowProc)
{
    this->mName = name;
    
    WNDCLASS wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = (HINSTANCE)::GetModuleHandle(nullptr);
    wc.lpfnWndProc = pWindowProc ? pWindowProc : DefaultFrameProc;;
    wc.lpszClassName = name.c_str();
    wc.lpszMenuName = nullptr;

    RegisterClass(&wc);
}

WindowClass::~WindowClass()
{
    UnregisterClass(mName.c_str(),
        (HINSTANCE)::GetModuleHandle(nullptr));
}

const String& WindowClass::GetName() const
{
    return mName;
}

HWND WFrame::WindowHandle() const
{
    return mWindowHandle;
}

float WFrame::DPIScalar() const
{
    return static_cast<float>(mDpi) / 96.0f;
}

void WFrame::OnDpiChanged(WORD dpi)
{
    RECT clientRect;
    GetClientRect(mWindowHandle, &clientRect);
    float scalar = static_cast<float>(dpi / mDpi);
    SetWindowPos(mWindowHandle, nullptr,
        0, 0,       // ignored by using flag SWP_NOMOVE 
        static_cast<LONG>((clientRect.right - clientRect.left) * scalar), static_cast<LONG>((clientRect.bottom - clientRect.top) * scalar),   // new size
        SWP_NOMOVE | SWP_NOZORDER);
    mDpi = dpi;
}

void WFrame::GetClientSize(uint16_t& width, uint16_t& height) const
{
    RECT rect;
    GetClientRect(mWindowHandle, &rect);
    width = static_cast<uint16_t>(rect.right - rect.left);
    height = static_cast<uint16_t>(rect.bottom - rect.top);
}

void WFrame::Close()
{
    Frame::Close();
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, 0);
    DestroyWindow(mWindowHandle);
}

void WFrame::setTitle(const std::string& newTitle)
{
    SetWindowText(mWindowHandle, std::wstring(newTitle.begin(), newTitle.end()).c_str());
}

void WFrame::SetFrameSize(uint16_t width, uint16_t height)
{
    Frame::SetFrameSize(width, height);
    SetWindowPos(mWindowHandle, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void WFrame::SetFullScreenMode(bool isFullScreen)
{
    if (isFullScreen == IsFullScreen()) return;
    Frame::SetFullScreenMode(isFullScreen);
    if (!isFullScreen)
    {
        SetWindowLongPtr(mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(mWindowHandle, HWND_NOTOPMOST, 0, 0, Width(), Height(), SWP_FRAMECHANGED);
        return;
    }
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(MonitorFromWindow(mWindowHandle, MONITOR_DEFAULTTOPRIMARY), &mi);
    SetWindowLongPtr(mWindowHandle, GWL_STYLE, WS_OVERLAPPED);
    SetWindowPos(mWindowHandle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED);
}

WFrame::WFrame() = default;

WFrame::WFrame(const String& title, uint16_t width, uint16_t height, bool mode, WNDPROC pWindowProc=DefaultFrameProc) :
    Frame(title, width, height, mode),
    mWindowClassPtr(new WindowClass(title, pWindowProc))
{
    InitializeWinFrame();
}

WFrame::WFrame(WFrame&& other) noexcept :
    Frame(std::move(other)),
    mWindowHandle(other.mWindowHandle),
    mDpi(other.mDpi)
{
    mWindowClassPtr = other.mWindowClassPtr;
    other.mWindowClassPtr = nullptr;
    SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

WFrame::~WFrame()
{
    
    if (!mWindowHandle) return;
    if (!IsClosed()) WFrame::Close();
    delete mWindowClassPtr;
}

WFrame& WFrame::operator=(WFrame&& other) noexcept
{
    if (this != &other)
    {
        Frame::operator=(std::move(other));
        DestroyWindow(mWindowHandle);
        delete mWindowClassPtr;
        
        mWindowClassPtr = other.mWindowClassPtr;
        mWindowHandle = other.mWindowHandle;
        mDpi = other.mDpi;
        other.mWindowClassPtr = nullptr;
        other.mWindowHandle = nullptr;
        SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
    return *this;
}

void WFrame::InitializeWinFrame()
{
    SetProcessDPIAware();
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = Width();
    rect.bottom = Height();
    if (IsFullScreen())
    {
        WFunc::QueryPrimaryScreenRC(&rect);
    }
    DWORD style = IsFullScreen() ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect,
        style,   // style
        FALSE);   // menu)
    mWindowHandle = CreateWindow(mWindowClassPtr->GetName().c_str(), Title().c_str(),
        style,
        0, 0, rect.right-rect.left, rect.bottom-rect.top,
        nullptr, nullptr, (HINSTANCE)::GetModuleHandle(nullptr), this);
    if (!mWindowHandle)
    {
        const wchar_t* errMsg = WFunc::GetHRInfo(GetLastError());
        THROW_EXCEPTION(errMsg);
    }
    mDpi = GetDpiForWindow(mWindowHandle);

    ::ShowWindow(mWindowHandle, SW_SHOWDEFAULT);
    ::UpdateWindow(mWindowHandle);
}