#pragma once
#pragma execution_character_set("utf-8")
#include "Engine/Utility/MacroUtility.h"
#ifdef WIN32
class EditorUi
{
public:
    virtual ~EditorUi() = default;
    virtual void drawSelf() = 0;
    virtual void hideSelf() {isShow = false;}
    virtual void showSelf() {isShow = true;}
    virtual bool getIsShow() const {return isShow;}
    
protected:
    bool isShow = true;
    EditorUi() = default;
};
#endif
