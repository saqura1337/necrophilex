#include "legit_bt.h"
#define TICK_INTERVAL            (m_globals()->m_intervalpertick)
#define TIME_TO_TICKS( dt )        ( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
TimeWarp g_Backtrack;

void TimeWarp::CreateMove(CUserCmd* cmd)
{

    int bestTargetIndex = -1;
    float bestFov = FLT_MAX;

    if (!g_ctx.local()->is_alive())
        return;

    if (!g_cfg.legitbot.enabled)
        return;

    for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
    {
        auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

        if (!e || !g_ctx.local())
            continue;

        if (!e->is_player())
            continue;

        if (e == g_ctx.local())
            continue;

        if (e->IsDormant())
            continue;

        if (e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
            continue;

        auto pEntity = e;

        float simtime = pEntity->m_flSimulationTime();
        Vector hitboxPos = pEntity->hitbox_position(0); 
        accuracy_boost = 16;
        TimeWarpData[i][cmd->m_command_number % (accuracy_boost + 1)] = StoredData{ simtime, hitboxPos };
        Vector ViewDir;
        math::angle_vectors(cmd->m_viewangles + (g_ctx.local()->m_aimPunchAngle() * 2.f), ViewDir);
        float FOVDistance = math::DistancePointToLine(hitboxPos, g_ctx.local()->GetEyePos(), ViewDir);

        if (bestFov > FOVDistance)
        {
            bestFov = FOVDistance;
            bestTargetIndex = i;
        }
    }

    float bestTargetSimTime = -1;
    if (bestTargetIndex != -1)
    {
        float tempFloat = FLT_MAX;
        Vector ViewDir;
        math::angle_vectors(cmd->m_viewangles + (g_ctx.local()->m_aimPunchAngle() * 2.f), ViewDir);
        for (int t = 0; t < accuracy_boost; ++t)
        {
            float tempFOVDistance = math::DistancePointToLine(TimeWarpData[bestTargetIndex][t].hitboxPos, g_ctx.local()->GetEyePos(), ViewDir);
            if (tempFloat > tempFOVDistance && TimeWarpData[bestTargetIndex][t].simtime > g_ctx.local()->m_flSimulationTime() - 1)
            {
                if (g_ctx.local()->CanSeePlayer(static_cast<player_t*>(m_entitylist()->GetClientEntity(bestTargetIndex)), TimeWarpData[bestTargetIndex][t].hitboxPos))
                {
                    tempFloat = tempFOVDistance;
                    bestTargetSimTime = TimeWarpData[bestTargetIndex][t].simtime;
                }
            }
        }

        if (bestTargetSimTime >= 0 && cmd->m_buttons & IN_ATTACK)
            cmd->m_tickcount = TIME_TO_TICKS(bestTargetSimTime);
    }

}