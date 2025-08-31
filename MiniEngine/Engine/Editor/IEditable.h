#pragma once

#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
#endif

class IEditable
{
public:
    virtual void showSelf() = 0;
private:
};
