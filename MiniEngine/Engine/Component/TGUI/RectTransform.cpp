#include "RectTransform.h"

#include "Canvas.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/RenderComponent/Camera.h"

void RectTransform::setPivot(const Vector2& pivot)
{
    ASSERT((abs(pivot.v.x) < 1 && abs(pivot.v.y) < 1)
        ,TEXT("pivot range is -1 to 1!"));
    mPivot = pivot;
}

void RectTransform::setPivotType(PivotType type)
{
    mPivotType = type;
    switch (type)
    {
    case PivotType::PIVOT_TOP_LEFT:
        mPivot = Vector2(-0.5f, -0.5f);
        break;
    case PivotType::PIVOT_TOP_CENTER:
        mPivot = Vector2(0, -0.5f);
        break;
    case PivotType::PIVOT_TOP_RIGHT:
        mPivot = Vector2(0.5f, -0.5f);
        break;
    case PivotType::PIVOT_CENTER_LEFT:
        mPivot = Vector2(-0.5f, 0);
        break;
    case PivotType::PIVOT_CENTER:
        mPivot = Vector2(0, 0);
        break;
    case PivotType::PIVOT_CENTER_RIGHT:
        mPivot = Vector2(0.5f, 0);
        break;
    case PivotType::PIVOT_BOTTOM_LEFT:
        mPivot = Vector2(-0.5f, 0.5f);
        break;
    case PivotType::PIVOT_BOTTOM_CENTER:
        mPivot = Vector2(0, 0.5f);
        break;
    case PivotType::PIVOT_BOTTOM_RIGHT:
        mPivot = Vector2(0.5f, 0.5f);
        break;
    default:
        std::cerr << "Unknown pivot type!" << std::endl;
        break;
    }
}

Vector3 RectTransform::getWorldPosition(const Vector2& offset) const
{
    ASSERT(mCanvas != nullptr, TEXT("no canvas! can not render!"));
    Vector3 canvasWorldPos = mParent->getWorldPosition();
    Vector3 canvasRight = mCanvas->getTransform()->getRight();
    Vector3 canvasUp = mCanvas->getTransform()->getUp();
    
    //pivot screenPos, need to reverse
    Vector3 resultWorldPos = canvasWorldPos;
    resultWorldPos -= (canvasRight * (mPivotPos.v.x -mPivot.v.x * mWidth + offset.v.x));
    resultWorldPos -= (canvasUp * (mPivotPos.v.y -mPivot.v.y * mHeight + offset.v.y));
    resultWorldPos.v.z = canvasWorldPos.v.z;

    return resultWorldPos;
}

Quaternion RectTransform::getWorldRotation() const
{
    return Transform::getWorldRotation();
}

Vector3 RectTransform::getWorldScale() const
{
    return {mWidth, mHeight, 1};
}

Matrix4x4 RectTransform::getModelMatrix() const
{
    Matrix4x4 model;
    model.setModelMatrixQuaternion(getWorldPosition(), getWorldRotation(), getWorldScale());
    return model;
}

rapidxml::xml_node<>* RectTransform::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    Transform::serialize(doc, father, value);
    //mXmlNode already created in Transform::serialize
    mXmlNode->first_attribute("name")->value(doc->allocate_string("RectTransform"));

    mXmlNode->append_attribute(doc->allocate_attribute("Width", doc->allocate_string(std::to_string(mWidth).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Height", doc->allocate_string(std::to_string(mHeight).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("PivotType", doc->allocate_string(std::to_string(static_cast<int>(mPivotType)).c_str())));
    mPivotPos.serialize(doc, mXmlNode, "Pivot");

    return mXmlNode;
}

void RectTransform::deSerialize(const rapidxml::xml_node<>* node)
{
    Transform::deSerialize(node);

    mWidth = std::stof(node->first_attribute("Width")->value());
    mHeight = std::stof(node->first_attribute("Height")->value());
    mPivotType = static_cast<PivotType>(std::stoi(node->first_attribute("PivotType")->value()));
    setPivotType(mPivotType);
    
    auto currentNode = node->first_node("Vector2");
    mPivotPos.deSerialize(currentNode);
}

void RectTransform::showSelf()
{
#ifdef WIN32
    static const char* PivotTypeNames[] = {
        "PIVOT_TOP_LEFT",
        "PIVOT_TOP_CENTER",
        "PIVOT_TOP_RIGHT",
        "PIVOT_CENTER_LEFT",
        "PIVOT_CENTER",
        "PIVOT_CENTER_RIGHT",
        "PIVOT_BOTTOM_LEFT",
        "PIVOT_BOTTOM_CENTER",
        "PIVOT_BOTTOM_RIGHT"};
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        const static int inputWidth = 50;
        const static float step = 1;
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Width", &mWidth);
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mWidth += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Height", &mHeight);
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mHeight += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::Text("Size");
        
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("X##xx", &mPivotPos.v.x);
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mPivotPos.v.x += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Y##xx", &mPivotPos.v.y);
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mPivotPos.v.y += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::Text("PivotPosition");
        
        if (ImGui::Combo("Layer", reinterpret_cast<int*>(&mPivotType), PivotTypeNames, static_cast<int>(PivotType::PIVOT_TYPE_NUM)))
        {
            setPivotType(mPivotType);
        }
    
        ImGui::TreePop();
    }
#endif
}
