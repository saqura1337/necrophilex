#pragma once

#include "../../includes.hpp"


#define NUM_OF_TICKS 16

struct StoredData
{
    float simtime;
    Vector hitboxPos;

};

class TimeWarp
{
public:
    void CreateMove(CUserCmd* cmd);
    StoredData TimeWarpData[64][NUM_OF_TICKS];
    int nLatestTick;
    int accuracy_boost = 16;
};

extern TimeWarp g_Backtrack;
