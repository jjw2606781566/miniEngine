#pragma once
#include "Engine/Editor/EditorUi.h"

#ifdef WIN32
class EngineMode:public EditorUi
{
public:
    void drawSelf() override;
    static EngineMode* sGetInstance();
private:
    static EngineMode* sInstance;
};
#endif
