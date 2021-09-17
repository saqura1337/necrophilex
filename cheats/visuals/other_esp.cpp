#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f) //-V550
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat());
}

void otheresp::p2c()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.p2c)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	const auto weapon_info = weapon->get_csweapon_info();
	if (!weapon_info)
		return;

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f)
		return;

	auto color = Color(34, 139, 34, 179);

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color(237, 42, 28, 179);

	float angle_z = math::dot_product(Vector(0, 0, 1), trace.plane.normal);
	float invangle_z = math::dot_product(Vector(0, 0, -1), trace.plane.normal);
	float angle_y = math::dot_product(Vector(0, 1, 0), trace.plane.normal);
	float invangle_y = math::dot_product(Vector(0, -1, 0), trace.plane.normal);
	float angle_x = math::dot_product(Vector(1, 0, 0), trace.plane.normal);
	float invangle_x = math::dot_product(Vector(-1, 0, 0), trace.plane.normal);


	if (angle_z > 0.5 || invangle_z > 0.5)
		render::get().filled_rect_world(trace.endpos, Vector2D(5, 5), color, 0);
	else if (angle_y > 0.5 || invangle_y > 0.5)
		render::get().filled_rect_world(trace.endpos, Vector2D(5, 5), color, 1);
	else if (angle_x > 0.5 || invangle_x > 0.5)
		render::get().filled_rect_world(trace.endpos, Vector2D(5, 5), color, 2);
}

void otheresp::penetration_reticle()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.penetration_reticle)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = Color::Red;

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color::Green;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	if (g_cfg.esp.penetration_reticle && g_cfg.esp.penetration_reticle_mode == 0)
	{
		render::get().rect_filled(width / 2, height / 2 - 1, 1, 3, color);
		render::get().rect_filled(width / 2 - 1, height / 2, 3, 1, color);
	}
	else if (g_cfg.esp.penetration_reticle && g_cfg.esp.penetration_reticle_mode == 1)
	{
		auto x = width / 2, y = height / 2;

		int a = (int)(y / 1 / 60);
		float gamma = atan(a / a);

		static auto rotationdegree = 0.f;

		for (int i = 0; i <= 4; i++) {
			std::vector <int> p;
			p.push_back(a * sin(DEG2RAD(rotationdegree + (i * 90))));
			p.push_back(a * cos(DEG2RAD(rotationdegree + (i * 90))));
			p.push_back((a / cos(gamma)) * sin(DEG2RAD(rotationdegree + (i * 90) + RAD2DEG(gamma))));
			p.push_back((a / cos(gamma)) * cos(DEG2RAD(rotationdegree + (i * 90) + RAD2DEG(gamma))));

			render::get().line(x, y, x + p[0], y - p[1], color);
			render::get().line(x + p[0], y - p[1], x + p[2], y - p[3], color);
		}
		rotationdegree++;
	}
}

void otheresp::indicators() 
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;
	if (!g_cfg.esp.keybinds)
		return;
	const auto col_background = Color(35, 35, 35, 255); // Watermark background color
	const auto col_accent = Color(90, 120, 240); // Watermark line accent color
	const auto col_text = Color(255, 255, 255); // Watermark text color

	auto gay = g_cfg.esp.key_pos_x;

	auto nigger = g_cfg.esp.key_pos_y;
	int x{ gay };
	int offset = 1;

	// overlays
	render::get().rect_filled(x + 10, nigger + 10, 150, 5 + 16 * 1, col_background);
	// line.
	render::get().gradient(x + 10, nigger + 10, 150, 2, Color(48, 206, 230, 255), Color(220, 57, 218, 255), GRADIENT_HORIZONTAL);
	render::get().gradient(x + 10, nigger + 10, 150, 2, Color(220, 57, 218, 255), Color(220, 232, 47, 255), GRADIENT_HORIZONTAL);
	// text.
	render::get().text(fonts[IND], x + 10 + 55, nigger + 15, col_text, HFONT_CENTERED_NONE, "keybinds");

	// condition dt
	if (misc::get().double_tap_enabled == true && key_binds::get().get_key_bind_state(28) == false)
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Double-tap");
		if (g_cfg.ragebot.double_tap_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else if (g_cfg.ragebot.double_tap_key.mode == TOGGLE)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// hs
	if (misc::get().hide_shots_key)
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Hide shots");
		if (g_cfg.antiaim.hide_shots_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// mindmg
	if (key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Damage override");
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].damage_override_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// baim
	if (key_binds::get().get_key_bind_state(22))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Force body aim");
		if (g_cfg.ragebot.body_aim_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// c_resolver override
	if (key_binds::get().get_key_bind_state(3))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Safe point");
		if (g_cfg.ragebot.safe_point_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// thirdperson
	if (key_binds::get().get_key_bind_state(17))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Thirdperson");
		if (g_cfg.misc.thirdperson_toggle.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// slowwalk
	if (key_binds::get().get_key_bind_state(21))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Slow walk");
		if (g_cfg.misc.slowwalk_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// fakeduck
	if (key_binds::get().get_key_bind_state(20))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Fake duck");
		if (g_cfg.misc.fakeduck_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// edgejump
	if (key_binds::get().get_key_bind_state(19))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Edge jump");
		if (g_cfg.misc.edge_jump.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// auto peek
	if (key_binds::get().get_key_bind_state(18))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Auto peek");
		if (g_cfg.misc.automatic_peek.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	// auto peek
	if (key_binds::get().get_key_bind_state(16))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Desync inverter");
		if (g_cfg.antiaim.flip_desync.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	if (key_binds::get().get_key_bind_state(0))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Legit auto fire");
		if (g_cfg.legitbot.autofire_key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	if (key_binds::get().get_key_bind_state(1))
	{
		render::get().text(fonts[IND], x + 10 + 6, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "Legit aimbot");
		if (g_cfg.legitbot.key.mode == HOLD)
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[holding]");
		}
		else
		{
			render::get().text(fonts[IND], x + 10 + 100, nigger + 16 + 16 * offset, { 255, 255, 255, 255 }, HFONT_CENTERED_NONE, "[toggled]");
		}
		offset = offset + 1;
	}

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static int height, width;
	m_engine()->GetScreenSize(width, height);

	g_ctx.globals.indicator_pos = height / 2;
	if (g_cfg.esp.indicators[INDICATOR_HS] && g_cfg.antiaim.hide_shots && g_cfg.antiaim.hide_shots_key.key > KEY_NONE && g_cfg.antiaim.hide_shots_key.key < KEY_MAX && misc::get().hide_shots_key) {
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "HS") / 2, render::get().text_heigth(fonts[INDICATORFONT], "HS") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "HS") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "HS") / 2, render::get().text_heigth(fonts[INDICATORFONT], "HS") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "HS");
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, Color(240, 240, 240, 255), 0, "HS");
		g_ctx.globals.indicator_pos += 45;
	}
	if (g_cfg.esp.indicators[INDICATOR_DT] && g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key) {
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DT") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DT") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "DT") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DT") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DT") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "DT");
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().double_tap_enabled && !weapon->is_grenade() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && weapon->can_fire(false) ? Color(240, 240, 240, 255) : Color(130, 20, 0, 255), 0, "DT");
		g_ctx.globals.indicator_pos += 45;
	}
	if (g_cfg.esp.indicators[INDICATOR_DAMAGE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon) && !weapon->is_non_aim()) {
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DMG: 00") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DMG: 00") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "DMG: 00") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DMG: 00") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DT") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "DMG: %d", g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage);
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, Color(130, 240, 0, 255), 0, "DMG: %d", g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage);
		g_ctx.globals.indicator_pos += 45;
	}


	if (g_cfg.esp.indicators[INDICATOR_FAKE]) {
		auto colorfake = Color(130, 20 + (int)(((g_ctx.local()->get_max_desync_delta() - 29.f) / 29.f) * 200.0f), 0);
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "FAKE ") / 2, render::get().text_heigth(fonts[INDICATORFONT], "FAKE ") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "FAKE ") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "FAKE ") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DT") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "FAKE");
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, colorfake, 0, "FAKE");
		render::get().draw_arc(10 + render::get().text_width(fonts[INDICATORFONT], "FAKE") + 12, g_ctx.globals.indicator_pos + 13, 7, NULL, 360, 4, Color(0, 0, 0, 200));
		render::get().draw_arc(10 + render::get().text_width(fonts[INDICATORFONT], "FAKE") + 12, g_ctx.globals.indicator_pos + 13, 7, NULL, ((g_ctx.local()->get_max_desync_delta() - 29.f) / 29.f) * 360.f, 4, colorfake);
		g_ctx.globals.indicator_pos += 45;
	}
	if (g_cfg.esp.indicators[INDICATOR_CHOKE]) {
		auto colorfl = Color(130, 20 + (int)((m_clientstate()->iChokedCommands / 15.f) * 200.0f), 0);
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "FL ") / 2, render::get().text_heigth(fonts[INDICATORFONT], "FL ") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "FL ") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "FL ") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DT") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "FL");
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, colorfl, 0, "FL");
		render::get().draw_arc(10 + render::get().text_width(fonts[INDICATORFONT], "FL") + 12, g_ctx.globals.indicator_pos + 13, 7, NULL, 360, 4, Color(0, 0, 0, 200));
		render::get().draw_arc(10 + render::get().text_width(fonts[INDICATORFONT], "FL") + 12, g_ctx.globals.indicator_pos + 13, 7, NULL, (m_clientstate()->iChokedCommands / 15.f) * 360.f, 4, colorfl);
		g_ctx.globals.indicator_pos += 45;
	}


	if (g_cfg.esp.indicators[INDICATORFONT] && key_binds::get().get_key_bind_state(20)) {
		render::get().gradient(10, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DUCK") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DUCK") + 8, Color(0, 0, 0, 0), Color(0, 0, 0, 200), GRADIENT_HORIZONTAL);
		render::get().gradient(10 + render::get().text_width(fonts[INDICATORFONT], "DUCK") / 2, g_ctx.globals.indicator_pos - 4, render::get().text_width(fonts[INDICATORFONT], "DUCK") / 2, render::get().text_heigth(fonts[INDICATORFONT], "DUCK") + 8, Color(0, 0, 0, 200), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);

		render::get().text(fonts[INDICATORFONT], 11, g_ctx.globals.indicator_pos + 1, Color::Black, 0, "DUCK");
		render::get().text(fonts[INDICATORFONT], 10, g_ctx.globals.indicator_pos, Color(240, 240, 240, 255), 0, "DUCK");
		g_ctx.globals.indicator_pos += 45;
	}
}


void render::draw_arc(int x, int y, int radius, int startangle, int percent, int thickness, Color color) {
	auto precision = (2 * M_PI) / 30;
	auto step = M_PI / 180;
	auto inner = radius - thickness;
	auto end_angle = (startangle + percent) * step;
	auto start_angle = (startangle * M_PI) / 180;

	for (; radius > inner; --radius) {
		for (auto angle = start_angle; angle < end_angle; angle += precision) {
			auto cx = round(x + radius * cos(angle));
			auto cy = round(y + radius * sin(angle));

			auto cx2 = round(x + radius * cos(angle + precision));
			auto cy2 = round(y + radius * sin(angle + precision));

			line(cx, cy, cx2, cy2, color);
		}
	}
}

void draw_circle(int x, int y, int radius, int thickness, Color color)
{
	auto inner = radius - thickness;

	for (; radius > inner; --radius)
	{
		render::get().circle(x, y, 30, radius, color);
	}
}

float adjust_angle(float angle)
{
	if (angle < 0)
	{
		angle = (90 + angle * (-1));
	}
	else if (angle > 0)
	{
		angle = (90 - angle);
	}

	return angle;
}

std::string parseString(const std::string& szBefore, const std::string& szSource)
{
	if (!szBefore.empty() && !szSource.empty() && (szSource.find(szBefore) != std::string::npos))
	{
		std::string t = strstr(szSource.c_str(), szBefore.c_str()); //-V522
		t.erase(0, szBefore.length());
		size_t firstLoc = t.find('\"', 0);
		size_t secondLoc = t.find('\"', firstLoc + 1);
		t = t.substr(firstLoc + 1, secondLoc - 3);
		return t;
	}
	else
		return crypt_str("");
}

void otheresp::draw_indicators()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto h = height / 2 + 50;

	for (auto& indicator : m_indicators)
	{
		render::get().gradient(5, h - 15, 30, 30, Color(0, 0, 0, 0), Color(0, 0, 0, 150), GRADIENT_HORIZONTAL);
		render::get().gradient(35, h - 15, 30, 30, Color(0, 0, 0, 150), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 10, h, indicator.m_color, HFONT_CENTERED_Y, indicator.m_text.c_str());
		h += 35;
	}

	m_indicators.clear();
}

#define HIDEHUD_HEALTH 1 << 3

void otheresp::custom_hud()
{
	if (!g_cfg.esp.custom_hud)
		return;

	g_ctx.local()->m_iHideHUD() |= HIDEHUD_HEALTH;

	if (!g_ctx.local()->is_alive())
		return;

	auto hp = g_ctx.local()->m_iHealth();
	auto armor = g_ctx.local()->m_ArmorValue();

	int red = 255 - (hp * 2.55);
	int green = hp * 2.55;

	Color hp_color = Color(red, green, 0);
	Color armor_color = Color(7, 169, 232);

	int screen_w, screen_h;
	m_engine()->GetScreenSize(screen_w, screen_h);
	int x = 25, y = screen_h - 40;

	////////////////////////////////	////////////////////////////////	////////////////////////////////	////////////////////////////////

	render::get().text(fonts[HUD], x, y, Color(hp_color), HFONT_CENTERED_Y, std::to_string(hp).c_str());
	render::get().text(fonts[HUD], x + 55, y, Color(255, 255, 255), HFONT_CENTERED_Y, "HP");

	////////////////////////////////	////////////////////////////////	////////////////////////////////	////////////////////////////////

	render::get().rect(x - 1, y + 27, x - 1 + 92, y + 27 + 5, Color(0, 0, 0, 200));
	render::get().rect_filled(x, y + 28, x + (int)(hp / 1.111111111), y + 28 + 3, hp_color);

	////////////////////////////////	////////////////////////////////	////////////////////////////////	////////////////////////////////

	render::get().text(fonts[HUD], x + 130, y, Color(armor_color), HFONT_CENTERED_Y, std::to_string(armor).c_str());
	render::get().text(fonts[HUD], x + 185, y, Color(255, 255, 255), HFONT_CENTERED_Y, "ARMOR");

	////////////////////////////////	////////////////////////////////	////////////////////////////////	////////////////////////////////

	render::get().rect(x + 129, y + 27, x + 129 + 152, y + 27 + 5, Color(0, 0, 0, 200)); 
	render::get().rect_filled(x + 130, y + 28, x + 130 + (int)(armor / 0.6666666666666), y + 28 + 3, armor_color); 
}

void otheresp::dynamic_scopes_lines()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (!g_cfg.esp.removals[REMOVALS_SCOPE])
		return;

	if (!g_cfg.esp.dynamic_scopes_line)
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto is_scoped = g_ctx.globals.scoped && weapon->is_scopable() && weapon->m_zoomLevel();

	if (!is_scoped)
		return;

	static int w, h;
	m_engine()->GetScreenSize(w, h);
	auto cone = g_ctx.globals.weapon->get_spread() + g_ctx.globals.weapon->get_inaccuracy();

	if (cone <= 0.f) return;

	auto size = cone * h * 0.7f;

	render::get().line(0, h / 2, w / 2 - size, h / 2, Color::Black);
	render::get().line(w, h / 2, w / 2 + size, h / 2, Color::Black);
	render::get().line(w / 2, 0, w / 2, h / 2 - size, Color::Black);
	render::get().line(w / 2, h, w / 2, h / 2 + size, Color::Black);
}

void otheresp::custom_scopes_lines()
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (!g_cfg.esp.removals[REMOVALS_SCOPE])
		return;

	if (!g_cfg.esp.custom_scopes_line)
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto is_scoped = g_ctx.globals.scoped && weapon->is_scopable() && weapon->m_zoomLevel();

	if (!is_scoped)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto offset = g_cfg.esp.scopes_line_offset;
	auto leng = g_cfg.esp.scopes_line_width;
	auto accent = g_cfg.esp.scopes_line_color;
	auto accent2 = Color(g_cfg.esp.scopes_line_color.r(), g_cfg.esp.scopes_line_color.g(), g_cfg.esp.scopes_line_color.b(), 0);

	render::get().gradient(width / 2 + offset, height / 2, leng, 1, accent, accent2, GradientType::GRADIENT_HORIZONTAL);
	render::get().gradient(width / 2 - leng - offset, height / 2, leng, 1, accent2, accent, GradientType::GRADIENT_HORIZONTAL);
	render::get().gradient(width / 2, height / 2 + offset, 1, leng, accent, accent2, GradientType::GRADIENT_VERTICAL);
	render::get().gradient(width / 2, height / 2 - leng - offset, 1, leng, accent2, accent, GradientType::GRADIENT_VERTICAL);
}

void otheresp::hitmarker_paint()
{
	if (!g_cfg.esp.hitmarker[0] && !g_cfg.esp.hitmarker[1])
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (g_cfg.esp.hitmarker[0])
		{
			static int width, height;
			m_engine()->GetScreenSize(width, height);

			auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
			hitmarker.hurt_color.SetAlpha(alpha);

			auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

			render::get().line(width / 3 + 6 + offset, height / 3 - 6 - offset, width / 3 + 6 + offset, height / 3 - 6 - offset, Color(255, 255, 255, 255));
			render::get().line(width / 3 + 6 + offset, height / 3 + 6 + offset, width / 3 + 6 + offset, height / 3 + 6 + offset, Color(255, 255, 255, 255));
			render::get().line(width / 3 - 6 - offset, height / 3 + 6 + offset, width / 3 - 6 - offset, height / 3 + 6 + offset, Color(255, 255, 255, 255));
			render::get().line(width / 3 - 6 - offset, height / 3 - 6 - offset, width / 3 - 6 - offset, height / 3 - 6 - offset, Color(255, 255, 255, 255));
		}

		if (g_cfg.esp.hitmarker[1])
		{
			Vector world;

			if (math::world_to_screen(hitmarker.point, world))
			{
				auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
				hitmarker.hurt_color.SetAlpha(alpha);

				auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

				render::get().line(world.x + 3 + offset, world.y - 3 - offset, world.x + 6 + offset, world.y - 6 - offset, Color(255, 255, 255, 255));
				render::get().line(world.x + 3 + offset, world.y + 3 + offset, world.x + 6 + offset, world.y + 6 + offset, Color(255, 255, 255, 255));
				render::get().line(world.x - 3 - offset, world.y + 3 + offset, world.x - 6 - offset, world.y + 6 + offset, Color(255, 255, 255, 255));
				render::get().line(world.x - 3 - offset, world.y - 3 - offset, world.x - 6 - offset, world.y - 6 - offset, Color(255, 255, 255, 255));
			}
		}
	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y, damage_marker[i].hurt_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);
		}
	}
}

void otheresp::draw_fov()
{
	if (!g_cfg.esp.draw_fov)
		return;

	if (g_ctx.globals.weapon->is_knife() || g_ctx.globals.weapon->is_grenade())
		return;

	int x, y;
	m_engine()->GetScreenSize(x, y);
	float radius = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov / 90.f * x / 2;

	render::get().circle(x / 2, y / 2, 90, radius, Color(g_cfg.esp.draw_fov_clr.r(), g_cfg.esp.draw_fov_clr.g(), g_cfg.esp.draw_fov_clr.b(), 100));
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	draw_circe((float)w * 0.5f, (float)h * 0.5f, g_ctx.globals.inaccuracy * 500.0f, 50, D3DCOLOR_RGBA(g_cfg.esp.show_spread_color.r(), g_cfg.esp.show_spread_color.g(), g_cfg.esp.show_spread_color.b(), g_cfg.esp.show_spread_color.a()), D3DCOLOR_RGBA(0, 0, 0, 0), device);
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr); //-V107

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0); //-V107
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 4.3f * m_globals()->m_frametime; //-V807
		else
			alpha -= 4.3f * m_globals()->m_frametime;
		auto color_main = Color(197, 208, 230, (int)((alpha * 197) / 3));
		auto color_fired = Color(255, 255, 230, (int)(255));
		auto color_notfired = Color(124, 252, 0, (int)(255));
		alpha = math::clamp(alpha, 0.0f, 1.0f);
		render::get().Draw3DCircle(position, alpha * 15.f, !g_ctx.globals.fired_shot ? Color(color_fired) : Color(color_notfired));

		Vector screen;
	}
}

void render::filled_rect_world(Vector center, Vector2D size, Color color, int angle) {
	Vector top_left, top_right, bot_left, bot_right;

	switch (angle) {
	case 0: // Z
		top_left = Vector(-size.x, -size.y, 0);
		top_right = Vector(size.x, -size.y, 0);

		bot_left = Vector(-size.x, size.y, 0);
		bot_right = Vector(size.x, size.y, 0);

		break;
	case 1: // Y
		top_left = Vector(-size.x, 0, -size.y);
		top_right = Vector(size.x, 0, -size.y);

		bot_left = Vector(-size.x, 0, size.y);
		bot_right = Vector(size.x, 0, size.y);

		break;
	case 2: // X
		top_left = Vector(0, -size.y, -size.x);
		top_right = Vector(0, -size.y, size.x);

		bot_left = Vector(0, size.y, -size.x);
		bot_right = Vector(0, size.y, size.x);

		break;
	}

	//top line
//    Vector c_top_left = center + add_top_left;
	Vector c_top_left = center + top_left;
	Vector c_top_right = center + top_right;

	//bottom line
	Vector c_bot_left = center + bot_left;
	Vector c_bot_right = center + bot_right;

	Vector m_flTopleft, m_flTopRight, m_flBotLeft, m_flBotRight;
	//your standard world to screen if u need one just grab from a past
	if (math::world_to_screen(c_top_left, m_flTopleft) && math::world_to_screen(c_top_right, m_flTopRight) &&
		math::world_to_screen(c_bot_left, m_flBotLeft) && math::world_to_screen(c_bot_right, m_flBotRight)) {

		Vertex_t vertices[4];
		//static int m_flTexID = g_pSurface->CreateNewTextureID(true);
		static int m_flTexID = m_surface()->CreateNewTextureID(true);
		m_surface()->DrawSetTexture(m_flTexID);
		m_surface()->DrawSetColor(color);

		vertices[0].Init(Vector2D(m_flTopleft.x, m_flTopleft.y));
		vertices[1].Init(Vector2D(m_flTopRight.x, m_flTopRight.y));
		vertices[2].Init(Vector2D(m_flBotRight.x, m_flBotRight.y));
		vertices[3].Init(Vector2D(m_flBotLeft.x, m_flBotLeft.y));

		m_surface()->DrawTexturedPolygon(4, vertices, true);
	}
}