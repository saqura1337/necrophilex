// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "prediction_system.h"

void engineprediction::FixNetvarCompression(int time)
{
	auto data = &netvars_data[time % MULTIPLAYER_BACKUP];

	if (data->tickbase != g_ctx.local()->m_nTickBase())
		return;

	data->tickbase = g_ctx.local()->m_nTickBase();
	data->m_vecOrigin = g_ctx.local()->m_vecOrigin();

	data->m_aimPunchAngle = g_ctx.local()->m_aimPunchAngle();
	data->m_aimPunchAngleVel = g_ctx.local()->m_aimPunchAngleVel();

	data->m_viewPunchAngle = g_ctx.local()->m_viewPunchAngle();
	data->m_vecViewOffset = g_ctx.local()->m_vecViewOffset();

	data->m_flDuckAmount = g_ctx.local()->m_flDuckAmount();
	data->m_flDuckSpeed = g_ctx.local()->m_flDuckSpeed();

	data->m_flFallVelocity = g_ctx.local()->m_flFallVelocity();
	data->m_flVelocityModifier = g_ctx.local()->m_flVelocityModifier();

	const auto aim_punch_vel_diff = data->m_aimPunchAngleVel - g_ctx.local()->m_aimPunchAngleVel();
	const auto aim_punch_diff = data->m_aimPunchAngle - g_ctx.local()->m_aimPunchAngle();
	const auto viewpunch_diff = data->m_viewPunchAngle.x - g_ctx.local()->m_viewPunchAngle().x;
	const auto velocity_diff = data->m_vecVelocity - g_ctx.local()->m_vecVelocity();
	const auto origin_diff = data->m_vecOrigin - g_ctx.local()->m_vecOrigin();

	if (std::abs(aim_punch_diff.x) <= 0.03125f && std::abs(aim_punch_diff.y) <= 0.03125f && std::abs(aim_punch_diff.z) <= 0.03125f)
		g_ctx.local()->m_aimPunchAngle() = data->m_aimPunchAngle;

	if (std::abs(aim_punch_vel_diff.x) <= 0.03125f && std::abs(aim_punch_vel_diff.y) <= 0.03125f && std::abs(aim_punch_vel_diff.z) <= 0.03125f)
		g_ctx.local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;

	if (std::abs(g_ctx.local()->m_vecViewOffset().z - data->m_vecViewOffset.z) <= 0.25f)
		g_ctx.local()->m_vecViewOffset().z = data->m_vecViewOffset.z;

	if (std::abs(viewpunch_diff) <= 0.03125f)
		g_ctx.local()->m_viewPunchAngle().x = data->m_viewPunchAngle.x;

	if (abs(g_ctx.local()->m_flDuckAmount() - data->m_flDuckAmount) <= 0.03125f)
		g_ctx.local()->m_flDuckAmount() = data->m_flDuckAmount;

	if (std::abs(velocity_diff.x) <= 0.03125f && std::abs(velocity_diff.y) <= 0.03125f && std::abs(velocity_diff.z) <= 0.03125f)
		g_ctx.local()->m_vecVelocity() = data->m_vecVelocity;

	if (abs(origin_diff.x) <= 0.03125f && abs(origin_diff.y) <= 0.03125f && abs(origin_diff.z) <= 0.03125f) {
		g_ctx.local()->m_vecOrigin() = data->m_vecOrigin;
		g_ctx.local()->set_abs_origin(data->m_vecOrigin);
	}

	if (abs(g_ctx.local()->m_flDuckSpeed() - data->m_flDuckSpeed) <= 0.03125f)
		g_ctx.local()->m_flDuckSpeed() = data->m_flDuckSpeed;

	if (abs(g_ctx.local()->m_flFallVelocity() - data->m_flFallVelocity) <= 0.03125f)
		g_ctx.local()->m_flFallVelocity() = data->m_flFallVelocity;

	if (std::abs(g_ctx.local()->m_flVelocityModifier() - data->m_flVelocityModifier) < 1.f)
		g_ctx.local()->m_flVelocityModifier() = data->m_flVelocityModifier;
}

void engineprediction::setup()
{
	if (prediction_data.prediction_stage != SETUP)
		return;

	backup_data.flags = g_ctx.local()->m_fFlags();
	backup_data.velocity = g_ctx.local()->m_vecVelocity();

	prediction_data.old_curtime = m_globals()->m_curtime;
	prediction_data.old_frametime = m_globals()->m_frametime;

	m_globals()->m_curtime = TICKS_TO_TIME(g_ctx.globals.fixed_tickbase);
	m_globals()->m_frametime = m_prediction()->EnginePaused ? 0.f : m_globals()->m_intervalpertick;

	prediction_data.prediction_stage = PREDICT;
}

void engineprediction::predict(CUserCmd* m_pcmd)
{
    if (prediction_data.prediction_stage != PREDICT)
        return;

    const auto backup_footsteps = m_cvar()->FindVar(crypt_str("sv_footsteps"))->GetInt();
    m_cvar()->FindVar(crypt_str("sv_footsteps"))->m_nFlags &= ~(1 << 14);
    m_cvar()->FindVar(crypt_str("sv_footsteps"))->m_nFlags &= ~(1 << 8);
    m_cvar()->FindVar(crypt_str("sv_footsteps"))->SetValue(0);

    if (m_clientstate()->iDeltaTick > 0)
        m_prediction()->Update(m_clientstate()->iDeltaTick, true, m_clientstate()->nLastCommandAck, m_clientstate()->nLastOutgoingCommand + m_clientstate()->iChokedCommands);

    if (!prediction_data.prediction_random_seed)
        prediction_data.prediction_random_seed = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);

    *prediction_data.prediction_random_seed = MD5_PseudoRandom(m_pcmd->m_command_number % 150);

    if (!prediction_data.prediction_player)
        prediction_data.prediction_player = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2);

    *prediction_data.prediction_player = reinterpret_cast <int> (g_ctx.local());

    m_movehelper()->set_host(g_ctx.local());
    m_gamemovement()->StartTrackPredictionErrors(g_ctx.local()); //-V807

    static auto m_nImpulse = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_nImpulse"));
    static auto m_nButtons = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_nButtons"));
    static auto m_afButtonLast = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonLast"));
    static auto m_afButtonPressed = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonPressed"));
    static auto m_afButtonReleased = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonReleased"));

    if (m_pcmd->m_impulse)
        *reinterpret_cast<uint32_t*>(uint32_t(g_ctx.local()) + m_nImpulse) = m_pcmd->m_impulse;

    CMoveData move_data;
    memset(&move_data, 0, sizeof(CMoveData));

    m_prediction()->SetupMove(g_ctx.local(), m_pcmd, m_movehelper(), &move_data);
    m_gamemovement()->ProcessMovement(g_ctx.local(), &move_data);
    m_prediction()->FinishMove(g_ctx.local(), m_pcmd, &move_data);

    m_gamemovement()->FinishTrackPredictionErrors(g_ctx.local());
    m_movehelper()->set_host(nullptr);

	auto viewmodel = g_ctx.local()->m_hViewModel().Get();

	if (viewmodel)
	{
		viewmodel_data.weapon = viewmodel->m_hWeapon().Get();

		viewmodel_data.viewmodel_index = viewmodel->m_nViewModelIndex();
		viewmodel_data.sequence = viewmodel->m_nSequence();
		viewmodel_data.animation_parity = viewmodel->m_nAnimationParity();

		viewmodel_data.cycle = viewmodel->m_flCycle();
		viewmodel_data.animation_time = viewmodel->m_flAnimTime();
	}

	const auto weapon = g_ctx.local()->m_hActiveWeapon().Get();
	if (!weapon) {
		m_spread = m_inaccuracy = 0.f;
		return;
	}

	weapon->update_accuracy_penality();

	m_spread = weapon->get_spread_virtual();

	m_inaccuracy = weapon->get_inaccuracy_virtual();

	m_cvar()->FindVar(crypt_str("sv_footsteps"))->SetValue(backup_footsteps);

    prediction_data.prediction_stage = FINISH;
}

void engineprediction::finish()
{
	if (prediction_data.prediction_stage != FINISH)
		return;

	m_gamemovement()->FinishTrackPredictionErrors(g_ctx.local());
	m_movehelper()->set_host(nullptr);

	*prediction_data.prediction_random_seed = -1;
	*prediction_data.prediction_player = 0;

	m_globals()->m_curtime = prediction_data.old_curtime;
	m_globals()->m_frametime = prediction_data.old_frametime;
}