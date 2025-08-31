#pragma once
#include "Engine/Memory/TankinMemory.h"
#include "GamePlay/Script/Player/TankPlayer.h"

//单例类实现排行榜和局外存储
class RankList
{
public:
    struct Rank
    {
        float mTime = 0;
        int mScore = 0;
    };
    class rankCompare
    {
    public:
        bool operator() (const Rank& left, const Rank& right) const
        {
            if (left.mScore == right.mScore)
                return left.mTime < right.mTime;
            else
            {
                return left.mScore > right.mScore;
            }
        }
    };
    static RankList* sGetInstance()
    {
        if (sInstance == nullptr)
            sInstance = new RankList();
        return sInstance;
    }
    void save();
    void load();
    void addRank(float time, int score)
    {
        mRankSet.insert({time, score});
        if (mRankSet.size() > 10)
        {
            mRankSet.erase(mRankSet.cbegin()); // 去掉最后一名
        }
    }
    TpMultiSet<Rank, rankCompare>& getRankSet(){return mRankSet;}
    TpString getRankListTimeString() const;
    TpString getRankListScoreString() const;
    TpString getTimeString(float time) const;
private:
    RankList(){}
    static RankList* sInstance;
    TpMultiSet<Rank, rankCompare> mRankSet;
};
