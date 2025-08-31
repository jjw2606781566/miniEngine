#pragma once
#include "Engine/Dependencies/rapidxml/rapidxml.hpp"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/render/RenderItem.h"

class TankinFont
{
public:
    struct FontChar
    {
        int x;
        int y;
        int width;
        int height;
        int xoffset;
        int yoffset;
        int xadvance;
        int page;
        std::unique_ptr<MaterialInstance> mMaterialGpu = nullptr;
        void deserialize(rapidxml::xml_node<>* node);
    };
    struct FontPage
    {
        TpString textureName;
    };
    static TankinFont* sGetInstance()
    {
        if (sInstance == nullptr)
        {
            sInstance = new TankinFont();
            sInstance->DeserializeFontFromFile();
        }
        return sInstance;
    }
    FontPage* getFontPage(int pageNumber)
    {
        auto itor = mPages.find(pageNumber);
        if (itor == mPages.end())
            return nullptr;
        return &(itor->second);
    }
    FontChar* getFontChar(wchar_t charId)
    {
        const std::wstring defaultChar = L"烫";
        auto itor = mChars.find(charId);
        if (itor == mChars.end())
            return &(mChars[defaultChar[0]]);
        return &(itor->second);
    }
    float getKerningAmount(wchar_t lastCh, wchar_t currentCh)
    {
        auto itor = mKernings.find(lastCh);
        if (itor == mKernings.end())
            return 0;
        auto itor2 = itor->second.find(currentCh);
        if (itor2 == itor->second.end())
            return 0;
        return itor2->second;
    }
    float getLineHeight() const { return lineHeight; }
    float getTextureWidth() const { return textureWidth; }
    float getTextureHeight() const { return textureHeight; }
private:
    TankinFont() = default;
    void DeserializeFontFromFile();
    static TankinFont* sInstance;
    TpUnorderedMap<int, FontPage> mPages;
    TpUnorderedMap<wchar_t, FontChar> mChars;
    TpUnorderedMap<wchar_t, TpUnorderedMap<wchar_t, int>> mKernings;
    float lineHeight;
    float textureWidth;
    float textureHeight;
};
