#include <thread>
#include "api.h"
#include "Sounds.h"
using namespace std::chrono_literals;

static std::pair<std::string, char> channels[] = {
	__(" "),
	__("http://pool.anison.fm:9000/AniSonFM(320)?nocache=0.752104723294601"), //Anime
	__("http://air2.radiorecord.ru:805/rr_320"), // Radio record
	__("http://uk5.internet-radio.com:8270/"), // Hard style
	__("http://streams.bigfm.de/bigfm-deutschland-128-mp3"), // Big fm 
	__("https://streams.bigfm.de/bigfm-deutschrap-128-mp3"), // big fm deutschrap
	__("http://air.radiorecord.ru:805/dub_320"), // Record dubstep
	__("http://air.radiorecord.ru:805/dc_320") // Record Dancecore
};

void playback_loop()
{
	//auto& var = variable::get();

	static bool once = false;

	if (!once)
	{
		BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));

		if (BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL))
		{
			BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
			BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
			once = true;
		}
	}

	static auto bass_needs_reinit = false;

	const auto desired_channel = g_cfg.misc.radiochannel;
	static auto current_channel = 0;

	if (g_cfg.misc.radiochannel == 0)
	{
		current_channel = 0;
		BASS_Stop();
		BASS_STOP_STREAM();
		BASS_StreamFree(BASS::stream_handle);
	}
	else if (once && g_cfg.misc.radiochannel > 0)
	{

		if (current_channel != desired_channel || bass_needs_reinit)
		{
			bass_needs_reinit = false;
			BASS_Start();
			_rt(channel, channels[desired_channel]);
			BASS_OPEN_STREAM(channel);
			current_channel = desired_channel;
		}

		BASS_SET_VOLUME(BASS::stream_handle, radio_muted ? 0 : g_cfg.misc.radiovolume / 100);
		BASS_PLAY_STREAM();
	}
	else if (BASS::bass_init)
	{
		bass_needs_reinit = true;
		BASS_StreamFree(BASS::stream_handle);
	}
}