#pragma once
#include "Engine/Component/Transform.h"

class Canvas;
enum class PivotType:int //for imgui convenience
{
    PIVOT_TOP_LEFT,
    PIVOT_TOP_CENTER,
    PIVOT_TOP_RIGHT,
    PIVOT_CENTER_LEFT,
    PIVOT_CENTER,
    PIVOT_CENTER_RIGHT,
    PIVOT_BOTTOM_LEFT,
    PIVOT_BOTTOM_CENTER,
    PIVOT_BOTTOM_RIGHT,
    PIVOT_TYPE_NUM
};

class RectTransform final:public Transform
{
    friend class GameObject;
public:
    Vector2 getPivot() const {return mPivot;}
    Vector2 getPivotPos() const {return mPivotPos;}
    void setPivot(const Vector2& pivot);
    void setPivotPos(const Vector2& pivot) {mPivotPos = pivot;}
    void setPivotType(PivotType type);

    //debug
    Vector3 getWorldPosition(const Vector2& offset = {0,0}) const override;
    Quaternion getWorldRotation() const override;
    Vector3 getWorldScale() const override;
    
    Matrix4x4 getModelMatrix()const override;

    float getWidth()const {return mWidth;}
    float getHeight()const {return mHeight;}
    void setWidth(float width) {mWidth = width;}
    void setHeight(float height) {mHeight = height;}
    void setSize(const Vector2& size){mWidth = size.v.x; mHeight = size.v.y;}

    Canvas* getCanvas()const {return mCanvas;}

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    
    void showSelf() override;
    
private:
    RectTransform(GameObject* go, Transform* parent, Canvas* canvas):Transform(go, parent), mCanvas(canvas){}
    
    float mWidth = 100;
    float mHeight = 100;
    Vector2 mPivot{0,0}; //default is PIVOT_CENTER_CENTER (0,0)
    Vector2 mPivotPos{0,0};
    PivotType mPivotType = PivotType::PIVOT_CENTER;
    Canvas* mCanvas = nullptr;
};
