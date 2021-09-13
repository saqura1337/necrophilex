#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum Prediction_stage
{
	SETUP,
	PREDICT,
	FINISH
};

struct PlayerData
{
	PlayerData()
	{
		reset();
	};
	~PlayerData()
	{
		reset();
	};

	void reset()
	{
		m_aimPunchAngle.Zero();
		m_aimPunchAngleVel.Zero();
		m_viewPunchAngle.Zero();

		m_vecViewOffset.Zero();
		m_vecBaseVelocity.Zero();
		m_vecVelocity.Zero();
		m_vecOrigin.Zero();

		m_flFallVelocity = 0.0f;
		m_flVelocityModifier = 0.0f;
		m_flDuckAmount = 0.0f;
		m_flDuckSpeed = 0.0f;
		m_fAccuracyPenalty = 0.0f;
		m_flThirdpersonRecoil = 0.0f;

		m_hGroundEntity = 0;
		m_nMoveType = 0;
		m_nFlags = 0;
		m_nTickBase = 0;
		m_flRecoilIndex = 0;
		tick_count = 0;
		command_number = INT_MAX;
		is_filled = false;
	}

	Vector m_aimPunchAngle = {};
	Vector m_aimPunchAngleVel = {};
	Vector m_viewPunchAngle = {};

	Vector m_vecViewOffset = {};
	Vector m_vecBaseVelocity = {};
	Vector m_vecVelocity = {};
	Vector m_vecOrigin = {};

	float m_flFallVelocity = 0.0f;
	float m_flVelocityModifier = 0.0f;
	float m_flThirdpersonRecoil = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_fAccuracyPenalty = 0.0f;

	int m_hGroundEntity = 0;
	int m_nMoveType = 0;
	int m_nFlags = 0;
	int m_nTickBase = 0;
	int m_flRecoilIndex = 0;

	int tick_count = 0;
	int command_number = INT_MAX;

	bool is_filled = false;
};

class engineprediction : public singleton <engineprediction>
{
	player_t* player = nullptr;

	struct Netvars_data
	{
		int tickbase = INT_MIN;
		Vector m_vecVelocity = {};
		Vector m_vecOrigin = {};
		int get_move_type = 0;
		float m_flFallVelocity = 0.0f;
		float m_flVelocityModifier = 0.0f;
		float m_flThirdpersonRecoil = 0.0f;
		float m_flDuckAmount = 0.0f;
		float m_flDuckSpeed = 0.0f;
		float m_fAccuracyPenalty = 0.0f;

		Vector m_aimPunchAngle = ZERO;
		Vector m_aimPunchAngleVel = ZERO;
		Vector m_viewPunchAngle = ZERO;
		Vector m_vecViewOffset = ZERO;
	};

	struct Backup_data
	{
		int flags = 0;
		Vector velocity = ZERO;
	};

	struct Prediction_data
	{
		void reset()
		{
			prediction_stage = SETUP;
			old_curtime = 0.0f;
			old_frametime = 0.0f;
		}

		Prediction_stage prediction_stage = SETUP;
		float old_curtime = 0.0f;
		float old_frametime = 0.0f;
		int* prediction_random_seed = nullptr;
		int* prediction_player = nullptr;
	};

	float m_spread, m_inaccuracy;

	struct Viewmodel_data
	{
		weapon_t* weapon = nullptr;

		int viewmodel_index = 0;
		int sequence = 0;
		int animation_parity = 0;

		float cycle = 0.0f;
		float animation_time = 0.0f;
	};
public:
	Netvars_data netvars_data[MULTIPLAYER_BACKUP];
	Backup_data backup_data;
	Prediction_data prediction_data;
	Viewmodel_data viewmodel_data;

	PlayerData m_Data[150] = { };

	void store_netvars();
	void FixNetvarCompression(int time);
	void restore_netvars();
	void setup();
	void predict(CUserCmd* m_pcmd);
	void finish();
};