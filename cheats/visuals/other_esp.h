#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"
struct m_indicator
{
	std::string m_text;
	Color m_color;

	m_indicator(const char* text, Color color) :
		m_text(text), m_color(color)
	{

	}
	m_indicator(std::string text, Color color) :
		m_text(text), m_color(color)
	{

	}

};

struct m_keybind
{
	std::string m_name;
	std::string m_mode;

	m_keybind(std::string name, std::string mode) : m_name(name), m_mode(mode)
	{
	}
};

class otheresp : public singleton< otheresp >
{
public:

	std::vector<m_keybind> m_keybinds;

	void p2c();
	void penetration_reticle();
	void indicators();
	void circlefake();
	void draw_indicators();
	void dynamic_scopes_lines();
	void custom_scopes_lines();
//	void custom_hud();
	void DrawFOV();
	void hitmarker_paint();
	void damage_marker_paint();
	void draw_fov();
	void spread_crosshair(LPDIRECT3DDEVICE9 device);
	void automatic_peek_indicator();
	void get_keys();
	void keybinds();

	struct Hitmarker
	{
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::White;
		Vector point = ZERO;
	} hitmarker;

	struct Damage_marker
	{
		Vector position = ZERO;
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::Red;
		int damage = -1;
		int hitgroup = -1;

		void reset()
		{
			position.Zero();
			hurt_time = FLT_MIN;
			hurt_color = Color::White;
			damage = -1;
			hitgroup = -1;
		}
	} damage_marker[65];
	std::vector<m_indicator> m_indicators;
};