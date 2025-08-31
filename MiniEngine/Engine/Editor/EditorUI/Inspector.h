#pragma once
#include "Engine/Component/GameObject.h"
#include "Engine/Editor/EditorUi.h"

#ifdef WIN32
class Inspector : public EditorUi
{
public:
    void drawSelf() override;
    void setGameObject(GameObject* gameObject){mSelectedGameObject = gameObject;}
    static Inspector* sGetInstance();
    
private:
    static Inspector* sInstance;
    GameObject* mSelectedGameObject = nullptr;
};
#endif
