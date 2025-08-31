#pragma once
#include "ImageTGUI.h"

class TextTGUI:public ImageTGUI
{
public:
    void awake() override;
    void prepareRenderList() override;
    void setText(const TpString& newText);
    TpString getText()const;
    void showSelf() override;
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;

private:
    std::wstring mText=L"新字符 \nnewText";
    Vector2 mFontScale = {1,1};
};
