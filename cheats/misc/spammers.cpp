#include "spammers.h"

void spammers::clan_tag()
{
    auto apply = [](const char* tag) -> void
    {
        using Fn = int(__fastcall*)(const char*, const char*);
        static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

        fn(tag, tag);
    };

    static auto removed = false;

    if (!g_cfg.misc.clantag_spammer && !removed)
    {
        removed = true;
        apply(crypt_str(""));
        return;
    }

    if (g_cfg.misc.clantag_spammer)
    { 
        static std::string tag = "Necrophilex.ru ";

        static float lastTime{ };
        float curtime = m_globals()->m_curtime;
        if (curtime > lastTime)
        {
            {
                std::rotate(std::begin(tag), std::next(std::begin(tag)), std::end(tag));
                apply(crypt_str(tag.c_str()));

                lastTime = curtime + 0.9f;

                if (fabs(lastTime - curtime) > 10.1f)
                    lastTime = curtime;
            }
        }
    }
}