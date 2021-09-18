#include "..\..\includes.hpp"

class misc : public singleton <misc> 
{
public:
	void watermark();
	void knifelefthand();
	void spectators_list();

	void lagcompexploit(CUserCmd* m_pcmd);

	void NoDuck(CUserCmd* cmd);
	void freddy(player_t* player);
	void AutoCrouch(CUserCmd* cmd);
	void lsd();
	void rainbow();
	void keylist();
	void flip();
	void pizdec();
	void SlideWalk(CUserCmd* cmd);
	void fast_stop(CUserCmd* m_pcmd);

	void automatic_peek(CUserCmd* cmd, float wish_yaw);
	void ViewModel();
	void PovArrows(player_t* e, Color color);

	void FullBright();
	void NightmodeFix();
		void break_prediction(CUserCmd* cmd);

	void zeus_range();
	void ChatSpammer();

	void desync_arrows();
	void aimbot_hitboxes();
	void ragdolls();

	void AutoAccept();
	void rank_reveal();

	bool double_tap(CUserCmd* m_pcmd);
	void hide_shots(CUserCmd* m_pcmd, bool should_work);

	void ChangeRegion();

	bool recharging_double_tap = false;

	bool jumpbugged = false;

	bool double_tap_enabled = false;
	bool double_tap_key = false;

	bool hide_shots_enabled = false;
	bool hide_shots_key = false;
	void EnableHiddenCVars();
};