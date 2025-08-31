#include "RankList.h"

#include <codecvt>

#include "Engine/Application.h"
#include "Engine/Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Engine/Dependencies/rapidxml/rapidxml_ext.h"

RankList* RankList::sInstance = nullptr;

void RankList::save()
{
    auto doc = new rapidxml::xml_document<>();
    auto root = doc->allocate_node(rapidxml::node_element, "RankList");
    doc->append_node(root);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    for (auto& rank:mRankSet)
    {
        std::wstring Wscore = std::to_wstring(rank.mScore);
        std::wstring Wtime = std::to_wstring(rank.mTime);
        TpString resultScore = "";
        TpString resultTime = "";
        resultScore = converter.to_bytes(Wscore);
        resultTime = converter.to_bytes(Wtime);
        auto rankNode = doc->allocate_node(rapidxml::node_element, "Rank");
        root->append_node(rankNode);
        rankNode->append_attribute(doc->allocate_attribute("Score", doc->allocate_string(resultScore.c_str())));
        rankNode->append_attribute(doc->allocate_attribute("Time", doc->allocate_string(resultTime.c_str())));
    }
    std::ofstream file(Application::sGetDataPath() + "Save/Save.xml");
    ASSERT(file.is_open(), TEXT("can not open file!"))
    file << (*doc);

    file.close();
    SAFE_DELETE_POINTER(doc);
}

void RankList::load()
{
    mRankSet.clear();
    rapidxml::file<>* xmlFile = new rapidxml::file<>((Application::sGetDataPath() + "Save/Save.xml").c_str());
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

    doc->parse<0>(xmlFile->data());
    rapidxml::xml_node<>* rootnode = doc->first_node("RankList");

    auto rankNode = rootnode->first_node("Rank");
    while (rankNode != nullptr)
    {
        Rank rank;
        rank.mScore = std::stoi(rankNode->first_attribute("Score")->value());
        rank.mTime = std::stod(rankNode->first_attribute("Time")->value());
        mRankSet.insert(rank);
        rankNode = rankNode->next_sibling("Rank");
    }

    delete xmlFile;
    delete doc;
}

TpString RankList::getRankListTimeString() const
{
    TpString result = "";
    int i=0;
    for (auto& rank:mRankSet)
    {
        result += getTimeString(rank.mTime);
        result += "\n\n";
        
        ++i;
        if (i==10)
            break;
    }
    for (;i<10; ++i)
    {
        result += "00: 00: 00\n\n";
    }
    return result;
}

TpString RankList::getRankListScoreString() const
{
    TpString result = "";
    int i=0;
    for (auto& rank:mRankSet)
    {
        result += std::to_string(rank.mScore);
        result += "\n\n";
        
        ++i;
        if (i==10)
            break;
    }
    for (;i<10; ++i)
    {
        result += "0\n\n";
    }
    return result;
}

TpString RankList::getTimeString(float time) const
{
    TpString finalResult;
    int minutes = static_cast<int>(time)/60;
    int second = static_cast<int>(time) - minutes*60;
    int millisecond = static_cast<int>(time*100 - static_cast<int>(time)*100);
    return std::to_string(minutes) + ": " + std::to_string(second) + ": " + std::to_string(millisecond);
}
