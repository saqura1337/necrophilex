#include "animation_system.h"
#include "..\ragebot\aim.h"

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;
	resolve_one = false;
	resolve_two = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}


float __fastcall AngleDiff(float sideone, float sidetwo)
{
	float advalue = fmodf(sideone - sidetwo, 360.0);

	while (advalue < -180.0f)  advalue += 360.0f;
	while (advalue > 180.0f)  advalue -= 360.0f;

	return  advalue;
}



float MaxYawModificator(player_t* enemy)
{
	auto animstate = enemy->get_animation_state();

	if (!animstate)
		return 0.0f;

	auto speedfactor = math::clamp(animstate->m_flFeetSpeedForwardsOrSideWays, 0.0f, 1.0f);
	auto avg_speedfactor = (animstate->m_flStopToFullRunningFraction * -0.3f - 0.2f) * speedfactor + 1.0f;

	auto duck_amount = animstate->m_fDuckAmount;

	if (duck_amount)
	{
		auto max_velocity = math::clamp(animstate->m_flFeetSpeedUnknownForwardOrSideways, 0.0f, 1.0f);
		auto duck_speed = duck_amount * max_velocity;

		avg_speedfactor += duck_speed * (0.5f - avg_speedfactor);
	}

	return animstate->yaw_desync_adjustment() * avg_speedfactor;
}



void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;


	if (!g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
		return;


	auto animstate = player->get_animation_state();
	float new_body_yaw_pose = 0.0f;
	auto m_flCurrentFeetYaw = player->get_animation_state()->m_flCurrentFeetYaw;
	auto m_flGoalFeetYaw = player->get_animation_state()->m_flGoalFeetYaw;
	auto m_flEyeYaw = player->get_animation_state()->m_flEyeYaw;
	float flMaxYawModifier = MaxYawModificator(player);
	float flMinYawModifier = player->get_animation_state()->pad10[512];
	auto delta = AngleDiff(m_flGoalFeetYaw, m_flEyeYaw);

	auto valid_lby = true;

	auto speed = player->m_vecVelocity().Length2D();

	float m_lby = player->m_flLowerBodyYawTarget();
	auto matfirst = player_record->matrixes_data.first;
	auto matsecond = player_record->matrixes_data.second;
	auto matzero = player_record->matrixes_data.zero;


	if (fabs(original_pitch) > 85.0f)
		fake = true;
	else if (!fake)
	{
		player_record->angles.y = original_goal_feet_yaw;
		return;
	}

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;

	auto absangles = player_record->abs_angles.y + player_record->abs_angles.x;

	auto fire_first = autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player);
	auto fire_second = autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player);
	auto fire_third = autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.zero), player);
	auto sidefirst = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
	auto sidesecond = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));
	auto sidezero = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.zero));

	auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
	auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());
	auto third_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.zero), player, g_ctx.local());


	auto matrix_detect_first = math::random_float(sidefirst - sidesecond, sidezero);
	auto matrix_detect_second = math::random_float(sidesecond - sidefirst, sidezero);

	auto v58 = *(float*)((uintptr_t)animstate + 0x334) * ((((*(float*)((uintptr_t)animstate + 0x11C)) * -0.30000001) - 0.19999999) * animstate->m_flFeetSpeedForwardsOrSideWays) + 1.0f;
	auto v59 = *(float*)((uintptr_t)animstate + 0x330) * ((((*(float*)((uintptr_t)animstate + 0x11C)) * -0.30000001) - 0.19999999) * animstate->m_flFeetSpeedForwardsOrSideWays) + 1.0f;

	auto angrec = player_record->angles.y;
	if (player_record->flags & MOVETYPE_NOCLIP && player->m_fFlags() & MOVETYPE_NOCLIP)
	{
		if (delta > 30.f)
		{
			int i = player->EntIndex();
			if (g_ctx.globals.missed_shots[i] < 1)
			{
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta + 30.f);
			}
			else
			{
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta - 30.f);
			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 4)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta + 30.f);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta - 30.f);

			}

		}
		else
		{
			int i = player->EntIndex();
			if (g_ctx.globals.missed_shots[i] < 1)
			{
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta - 30.f);
			}
			else
			{
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta + 30.f);
			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 4)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta - 30.f);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(delta + 30.f);

			}
		}
	}



	auto choked = abs(TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime()) - 1);

	float m_flLastClientSideAnimationUpdateTimeDelta = 0.0f;

	auto recordangle = AngleDiff(angrec - m_flGoalFeetYaw, 360.f);

	AnimationLayer moveLayers[3][15];
	AnimationLayer layers[15];
	memcpy(moveLayers, player->get_animlayers(), sizeof(AnimationLayer) * 15);
	memcpy(layers, player->get_animlayers(), sizeof(AnimationLayer) * 15);
	int m_side;
	int updateanim;
	m_lby = (delta > 0.f) ? 1 : -1;
	bool first_detected = false;
	bool second_detected = false;

	auto diff_matrix_first = AngleDiff(fire_first.visible - fire_second.visible, fire_third.visible);
	auto diff_matrix_second = AngleDiff(fire_second.visible - fire_first.visible, fire_third.visible);


	if (player_record->flags & FL_ONGROUND && player->m_fFlags() & FL_ONGROUND) {
		if (speed < 0.1f)
		{
			auto result = player->sequence_activity(player_record->layers[3].m_nSequence);

			m_side = (delta > 0.f) ? 1 : -1;

			if (result == 979) {
				if (int(layers[3].m_flCycle != int(layers[3].m_flCycle))) {
					if (int(layers[3].m_flWeight == 0.0f && int(layers[3].m_flCycle == 0.0f)))
					{

						g_ctx.globals.updating_animation = true;

						animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 60.f) * m_side;
					}
				}
			}
		}
		else if (int(layers[12].m_flWeight < 0.01f || abs(int(layers[12].m_flWeight - (int(layers[12].m_flWeight) < 0.01f) && (int(layers[6].m_nSequence == (int(layers[6].m_nSequence))))))))
		{
			if (std::abs(layers[6].m_flWeight - layers[6].m_flWeight) < 0.01f)
			{
				float delta1 = abs(layers[6].m_flPlaybackRate - moveLayers[0][6].m_flPlaybackRate);
				float delta2 = abs(layers[6].m_flPlaybackRate - moveLayers[2][6].m_flPlaybackRate);
				float delta3 = abs(layers[6].m_flPlaybackRate - moveLayers[1][6].m_flPlaybackRate);

				if (int(delta1 * 1000.0f) < int(delta2 * 1000.0f) || int(delta3 * 1000.0f) <= int(delta2 * 1000.0f) || int(delta2 * 1000.0f))
				{
					if (int(delta1 * 1000.0f) >= int(delta3 * 1000.0f) && int(delta2 * 1000.0f) > int(delta3 * 1000.0f) && !int(delta3 * 1000.0f))
					{
						g_ctx.globals.updating_animation = true;
						m_side = 1;
						updateanim = 1;
					}
				}
				else
				{
					g_ctx.globals.updating_animation = true;
					m_side = -1;
					updateanim = -1;
				}
			}
		}


	}
	int positives = 0;
	int negatives = 0;

	m_side = (delta > 0.f) ? 1 : -1;
	delta = -delta;
	if (delta > 30.f && matrix_detect_first != matrix_detect_second)
	{
		if (m_side <= 1)
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
				break;

			}
		}
		else if (m_side >= -1)
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
				break;

			}
		}

	}
	else if (delta < -30.f && matrix_detect_second != matrix_detect_first)
	{
		if (m_side <= 1)
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
				break;

			}
		}
		else if (m_side >= -1)
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * m_side);
				break;
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f * -m_side);
				break;
			}
		}

	}
	if (choked < 1 && player_record->flags & IN_WALK)
	{
		if (delta < 15.f)
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 11);
		}
		else
		{
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 11);
		}
		switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
		{
		case 0:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 11.f);
			break;
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 11.f);
			break;
		}

	}
	if (player_record->bot || choked < 1)
	{
		m_lby = false;
		animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + original_goal_feet_yaw);
	}
}

bool resolver::ent_use_jitter(player_t* player, int* new_side) {

	if (!player->is_alive())
		return false;

	if (!player->valid(false, false))
		return false;

	if (player->IsDormant())
		return false;

	static float LastAngle[64];
	static int LastBrute[64];
	static bool Switch[64];
	static float LastUpdateTime[64];

	int i = player->EntIndex();

	float CurrentAngle = player->m_angEyeAngles().y;
	if (CurrentAngle, LastAngle[i], 50.f) {
		Switch[i] = !Switch[i];
		LastAngle[i] = CurrentAngle;
		*new_side = Switch[i] ? 1 : -1;
		LastBrute[i] = *new_side;
		LastUpdateTime[i] = m_globals()->m_curtime;
		return true;
	}
	else {
		if (fabsf(LastUpdateTime[i] - m_globals()->m_curtime >= TICKS_TO_TIME(17))
			|| player->m_flSimulationTime() != player->m_flOldSimulationTime()) {
			LastAngle[i] = CurrentAngle;
		}
		*new_side = LastBrute[i];
	}
	return false;
}

bool ent_use_lowdelta(player_t* player)
{

	if (!player->is_alive())
		return false;

	if (!player->valid(false, false))
		return false;

	if (player->IsDormant())
		return false;

	int m_side;
	float m_lby = player->m_flLowerBodyYawTarget();
	auto delta = math::normalize_yaw(player->m_angEyeAngles().y - 0.f);
	m_side = (delta > 0.f) ? -1 : 1;
	auto speed = player->m_vecVelocity().Length2D();
	auto choked = abs(TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime()) - 1);
	m_lby = (delta > 0.f) ? -1 : 1;
	auto m_flGoalFeetYaw = player->get_animation_state()->m_flGoalFeetYaw;
	auto m_flEyeYaw = player->get_animation_state()->m_flEyeYaw;
	auto animstate = player->get_animation_state();

	if (speed < 0.1f && !choked && m_lby < 30.f && fabs(delta < 15.f)) // low delta resolve > 10.f delta
	{
		int i = player->EntIndex();
		if (m_side <= 1 && !choked) // if mside <= 1 && player->not_choke
		{
			int i = player->EntIndex();
			if (fabs(delta < 15.f) && !choked || !choked && g_ctx.globals.missed_shots[i] < 1)
			{
				AngleDiff(m_flEyeYaw - m_flGoalFeetYaw, 360.f);
				delta = math::normalize_yaw(delta + 11.f * m_side);  // not m_flgoalfeetyaw cuz lowdelta not choked and not desync'ing
			}
			else
			{
				AngleDiff(m_flEyeYaw - m_flGoalFeetYaw, 360.f);
				delta = math::normalize_yaw(delta - 11.f * -m_side);

			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2) //missedshots case
			{
			case 0:
				delta = math::normalize_yaw(player->m_angEyeAngles().y + 11.f);
				break;
			case 1:
				delta = math::normalize_yaw(player->m_angEyeAngles().y - 11.f);
				break;

			}
		}
		else if (m_side >= -1 && !choked)
		{
			if (fabs(delta > -15.f) && !choked || !choked && g_ctx.globals.missed_shots[i] < 1)
			{
				AngleDiff(m_flEyeYaw - m_flGoalFeetYaw, 360.f);
				delta = math::normalize_yaw(delta - 11.f * m_side);
			}
			else
			{
				AngleDiff(m_flEyeYaw - m_flGoalFeetYaw, 360.f);
				delta = math::normalize_yaw(delta + 11.f * -m_side);
			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				delta = math::normalize_yaw(player->m_angEyeAngles().y - 11.f);
				break;
			case 1:
				delta = math::normalize_yaw(player->m_angEyeAngles().y + 11.f);
				break;

			}


		}
	}
	else if (speed < 0.1f && !choked && m_lby > -30.f && fabs(delta > -15.f)) // low delta resolve < -10 delta
	{
		int i = player->EntIndex();
		if (m_side >= -1 && !choked)
		{
			if (fabs(delta > -15.f) && !choked || !choked && g_ctx.globals.missed_shots[i] < 1)
			{

				delta = math::normalize_yaw(delta + 11.f * m_side);
			}
			else
			{

				delta = math::normalize_yaw(delta - 11.f * -m_side);
			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				delta = math::normalize_yaw(player->m_angEyeAngles().y - 11.f);
				break;
			case 1:
				delta = math::normalize_yaw(player->m_angEyeAngles().y + 11.f);
				break;

			}


		}
		else if (m_side <= 1 && !choked)
		{
			int i = player->EntIndex();
			if (fabs(delta < 15.f) && !choked || !choked && g_ctx.globals.missed_shots[i] < 1)
			{

				delta = math::normalize_yaw(delta - 11.f * m_side);
			}
			else
			{

				delta = math::normalize_yaw(delta + 11.f * -m_side);
			}
			switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
			{
			case 0:
				delta = math::normalize_yaw(player->m_angEyeAngles().y + 11.f);
				break;
			case 1:
				delta = math::normalize_yaw(player->m_angEyeAngles().y - 11.f);
				break;

			}
		}

	}


}

float resolver::resolve_pitch()
{
	return original_pitch;
}