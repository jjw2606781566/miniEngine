#pragma once
#include "Engine/Component/Component.h"
#include "Engine/Component/Transform.h"

class Canvas:public Component
{
public:
    friend class ComponentFactory;
    friend class ComponentRegister;
    void awake() override;
    void setCanvas(int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }
    Transform* getTransform()const {return mTransform;}
    int getWidth() const{return mWidth;}
    int getHeight() const{return mHeight;}

    static Canvas* sGetCurrentCanvas(){return sCurrentCanvas;}
    
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    
private:
    Canvas()
    {
        sCurrentCanvas = this;
    }
    int mWidth = 0;
    int mHeight = 0;
    Transform* mTransform = nullptr;
    static Canvas* sCurrentCanvas;
};
