// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <ShlObj.h>
#include <ShlObj_core.h>
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
#include "nSkinz\SkinChanger.h"

#include "../necrophilex/mutiny merge/merged.h"
#include "../necrophilex/mutiny merge/console.hpp"
#include "../necrophilex/mutiny merge/NetworkedVariables.h"

enum error_type
{
	ERROR_NONE,
	ERROR_DEBUG,
	ERROR_OPEN_KEY,
	ERROR_QUERY_DATA,
	ERROR_CONNECT,
	ERROR_1,
	ERROR_2,
	ERROR_3,
	ERROR_4,
	ERROR_5,
	ERROR_6,
	ERROR_7,
	ERROR_8,
	ERROR_9,
	ERROR_CHECK_HASH
};

PVOID base_address = nullptr;
volatile error_type error = ERROR_NONE;

LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);

__forceinline void setup_render();
__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_hooks();

DWORD WINAPI main(PVOID base)
{

	g_ctx.signatures =
	{
		crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
		crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
		crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
		crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
		crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
		crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
		crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
		crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
		crypt_str("55 8B EC 83 E4 F0 B8 D8"),
		crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
		crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
		crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
		crypt_str("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83"),
		crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
		crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
		crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"),
		crypt_str("53 0F B7 1D ? ? ? ? 56"),
		crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")
	};

	g_ctx.indexes =
	{
		5,
		33,
		339,
		218,
		219,
		34,
		157,
		75,
		460,
		482,
		452,
		483,
		284,
		223,
		246,
		27,
		17,
		123
	};

	while (!IFH(GetModuleHandle)(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	base_address = base;

	static TCHAR path[MAX_PATH];
	std::string folder;
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path);
	folder = std::string(path) + crypt_str("\\necrophilex\\");
	CreateDirectory(folder.c_str(), 0);

	setup_sounds();
	setup_skins();
	setup_netvars();
	setup_render();
	cfg_manager->setup();
	c_lua::get().initialize();
	key_binds::get().initialize_key_binds();
	setup_hooks();
	Netvars::Netvars();
	return EXIT_SUCCESS;
}

HANDLE FindHandle(std::string name)
{
	return GetModuleHandle(name.c_str());
}

DWORD WINAPI mutiny_merge(LPVOID lpThreadParameter)
{
	if (!ClientHandle)
		ClientHandle = FindHandle("client.dll");

	if (!MatchmakingHandle)
		MatchmakingHandle = FindHandle("matchmaking.dll");

	if (!VPhysicsHandle)
		VPhysicsHandle = FindHandle("vphysics.dll");

	if (!VSTDLIBHandle)
		VSTDLIBHandle = FindHandle("vstdlib.dll");

	if (!DatacacheHandle)
		DatacacheHandle = FindHandle("datacache.dll");

	if (!Tier0Handle)
		Tier0Handle = FindHandle("tier0.dll");

	if (!MaterialSystemHandle)
		MaterialSystemHandle = FindHandle("materialsystem.dll");

	if (!StudioRenderHandle)

		StudioRenderHandle = FindHandle(("studiorender.dll"));

	if (!FileSystemStdioHandle)
		FileSystemStdioHandle = FindHandle(("filesystem_stdio.dll"));
	if (!ServerHandle)

		ServerHandle = FindHandle(("server.dll"));
	if (!EngineHandle)
		EngineHandle = FindHandle(("engine.dll"));

	if
		(!VGUIMatSurfaceHandle)
		VGUIMatSurfaceHandle = FindHandle(("vguimatsurface.dll"));

	shonax_mutiny.run();

	return true;

}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		IFH(DisableThreadLibraryCalls)(hModule);
		CreateThread(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513
	}

	return TRUE;
}

typedef bool(__thiscall* tDispatchUserMessage)(void*, int type, unsigned int a3, unsigned int length, const void* msg_data);
tDispatchUserMessage otDispatchUserMessage;

bool __fastcall hkDispatchUserMessage(void* ecx, void* edx, int type, unsigned int a3, unsigned int length, const void* msg_data)
{

	if (!m_gamerules()->m_bIsValveDS()) {
		if (g_cfg.misc.block_adv && (type == 7 || type == 8 || type == 5)) {
			return true;
		}
	}

	return otDispatchUserMessage(ecx, type, a3, length, msg_data);
}

void __fastcall hkPerformScreenOverlay(void* _this, void* edx, int x, int y, int w, int h);
decltype(&hkPerformScreenOverlay) ohkPerformScreenOverlay;
void __fastcall hkPerformScreenOverlay(void* _this, void* edx, int x, int y, int w, int h) {
	if (g_cfg.misc.block_adv)
		return;
	return ohkPerformScreenOverlay(_this, edx, x, y, w, h);
}

__forceinline void setup_render()
{
	static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[LOGS] = create_font(crypt_str("Raleway"), 15, FW_DONTCARE, FONTFLAG_ANTIALIAS);
	fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[NAME] = create_font(crypt_str("Square Sans Serif 7"), 11, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_DONTCARE, FONTFLAG_ANTIALIAS);
	fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_DONTCARE, FONTFLAG_ANTIALIAS);
	fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[INDICATORFONT] = create_font(crypt_str("Verdana"), 15, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[DAMAGE_MARKER] = create_font(crypt_str("Raleway"), 17, FW_MEDIUM, FONTFLAG_ANTIALIAS);
	fonts[WATERMARK] = create_font(crypt_str("Verdana"), 13, FW_MEDIUM, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	fonts[IND] = create_font(crypt_str("Raleway"), 15, FW_MEDIUM, FONTFLAG_ANTIALIAS);
	fonts[WARNING_icon] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[WARNING_text] = create_font(crypt_str("Verdana"), 12, FW_SEMIBOLD, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	g_ctx.last_font_name.clear();
}

__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}

__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } + '='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } + '=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } + ')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}

__forceinline void setup_hooks()
{
	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname); //-V206

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones); //-V206

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing); //-V206

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules); //-V206

	static auto updateclientsideanimation = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(13).c_str()));
	hooks::original_updateclientsideanimation = (DWORD)DetourFunction((PBYTE)updateclientsideanimation, (PBYTE)hooks::hooked_updateclientsideanimation); //-V206

	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((byte*)processinterpolatedlist, (byte*)hooks::processinterpolatedlist); //-V206

	static auto cl_move = (DWORD)(util::FindSignature(crypt_str("engine.dll"), crypt_str("55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A")));
	hooks::original_clmove = (DWORD)DetourFunction((PBYTE)cl_move, (PBYTE)hooks::hooked_clmove);

	hooks::client_hook = new vmthook(reinterpret_cast<DWORD**>(m_client()));
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_fsn), 37); //-V107 //-V221
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_writeusercmddeltatobuffer), 24); //-V107 //-V221

	hooks::clientstate_hook = new vmthook(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)));
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetstart), 5); //-V107 //-V221
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetend), 6); //-V107 //-V221

	hooks::panel_hook = new vmthook(reinterpret_cast<DWORD**>(m_panel())); //-V1032
	hooks::panel_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_painttraverse), 41); //-V107 //-V221

	hooks::clientmode_hook = new vmthook(reinterpret_cast<DWORD**>(m_clientmode()));
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_createmove), 24); //-V107 //-V221
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_postscreeneffects), 44); //-V107 //-V221
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_overrideview), 18); //-V107 //-V221
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_drawfog), 17); //-V107 //-V221

	hooks::inputinternal_hook = new vmthook(reinterpret_cast<DWORD**>(m_inputinternal())); //-V114
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setkeycodestate), 91); //-V107 //-V221
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setmousecodestate), 92); //-V107 //-V221

	hooks::engine_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()));
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_isconnected), 27); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getscreenaspectratio), 101); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_ishltv), 93); //-V107 //-V221

	hooks::renderview_hook = new vmthook(reinterpret_cast<DWORD**>(m_renderview()));
	hooks::renderview_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_sceneend), 9); //-V107 //-V221

	hooks::materialsys_hook = new vmthook(reinterpret_cast<DWORD**>(m_materialsystem())); //-V1032
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_beginframe), 42); //-V107 //-V221
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getmaterial), 84); //-V107 //-V221

	hooks::modelrender_hook = new vmthook(reinterpret_cast<DWORD**>(m_modelrender()));
	hooks::modelrender_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_dme), 21); //-V107 //-V221

	hooks::surface_hook = new vmthook(reinterpret_cast<DWORD**>(m_surface()));
	hooks::surface_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_lockcursor), 67); //-V107 //-V221

	hooks::bspquery_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()->GetBSPTreeQuery()));
	hooks::bspquery_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_listleavesinbox), 6); //-V107 //-V221



	hooks::prediction_hook = new vmthook(reinterpret_cast<DWORD**>(m_prediction())); //-V1032
	hooks::prediction_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_runcommand), 19); //-V107 //-V221

	hooks::trace_hook = new vmthook(reinterpret_cast<DWORD**>(m_trace()));
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_clip_ray_collideable), 4); //-V107 //-V221
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_trace_ray), 5); //-V107 //-V221

	hooks::filesystem_hook = new vmthook(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2));
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_loosefileallowed), 128); //-V107 //-V221

	while (!(INIT::Window = IFH(FindWindow)(crypt_str("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)IFH(SetWindowLongPtr)(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

	hooks::directx_hook = new vmthook(reinterpret_cast<DWORD**>(m_device()));
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene_Reset), 16); //-V107 //-V221
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_present), 17); //-V107 //-V221
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene), 42); //-V107 //-V221

	hooks::hooked_events.RegisterSelf();
}

