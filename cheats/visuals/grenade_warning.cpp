#include "grenade_warning.h"

const char* index_to_grenade_name(int index)
{
    switch (index)
    {
    case WEAPON_SMOKEGRENADE: return "smoke"; break;
    case WEAPON_HEGRENADE: return "he grenade"; break;
    case WEAPON_MOLOTOV:return "molotov"; break;
    case WEAPON_INCGRENADE:return "molotov"; break;
    }
}
const char* index_to_grenade_name_icon(int index)
{
    switch (index)
    {
    case WEAPON_SMOKEGRENADE: return "k"; break;
    case WEAPON_HEGRENADE: return "j"; break;
    case WEAPON_MOLOTOV:return "l"; break;
    case WEAPON_INCGRENADE:return "n"; break;
    }
}

void rotate_point(Vector2D& point, Vector2D origin, bool clockwise, float angle) {
    Vector2D delta = point - origin;
    Vector2D rotated;

    if (clockwise) {
        rotated = Vector2D(delta.x * cosf(angle) - delta.y * sinf(angle), delta.x * sinf(angle) + delta.y * cosf(angle));
    }
    else {
        rotated = Vector2D(delta.x * sinf(angle) - delta.y * cosf(angle), delta.x * cosf(angle) + delta.y * sinf(angle));
    }

    point = rotated + origin;
}

void draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, Color color) {
    auto precision = (2 * M_PI) / 30;
    auto step = M_PI / 180;
    auto inner = radius - thickness;
    auto end_angle = (start_angle + percent) * step;
    auto start_angles = (start_angle * M_PI) / 180;

    for (; radius > inner; --radius) {
        for (auto angle = start_angles; angle < end_angle; angle += precision) {
            auto cx = std::round(x + radius * std::cos(angle));
            auto cy = std::round(y + radius * std::sin(angle));

            auto cx2 = std::round(x + radius * std::cos(angle + precision));
            auto cy2 = std::round(y + radius * std::sin(angle + precision));

            render::get().line(cx, cy, cx2, cy2, color);
        }
    }
}

void textured_polygon(int n, std::vector< Vertex_t > vertice, Color color) {
    static int texture_id = m_surface()->CreateNewTextureID(true);
    static unsigned char buf[4] = { 255, 255, 255, 255 };

    m_surface()->DrawSetTextureRGBA(texture_id, buf, 1, 1);
    m_surface()->DrawSetColor(color);
    m_surface()->DrawSetTexture(texture_id);
    m_surface()->DrawTexturedPolygon(n, vertice.data());
}

void filled_circle(int x, int y, int radius, int segments, Color color) {
    std::vector< Vertex_t > vertices;

    float step = M_PI * 2.0f / segments;

    for (float a = 0; a < (M_PI * 2.0f); a += step)
        vertices.emplace_back(Vector2D(radius * cosf(a) + x, radius * sinf(a) + y));

    textured_polygon(vertices.size(), vertices, color);
}

void shoto(int x, int y, float d, int rad)
{
    if (d < 1)
        return;
    for (int i = 0; i < d / 5 + 1; i++)
    {
        static float pulse = 0.f;
        pulse += 1 / 1000.f;
        if (pulse >= 1.f)
            pulse = 0;
        filled_circle(x, y, (rad + d / 2 * pulse), 18, Color(g_cfg.esp.grenade_warning_color.r(), g_cfg.esp.grenade_warning_color.g(), g_cfg.esp.grenade_warning_color.b(), int(25 * pulse)));
    }
}

inline float CSGO_Armores(float flDamage, int ArmorValue) {
    float flArmorRatio = 0.5f;
    float flArmorBonus = 0.5f;
    if (ArmorValue > 0) {
        float flNew = flDamage * flArmorRatio;
        float flArmor = (flDamage - flNew) * flArmorBonus;

        if (flArmor > static_cast<float>(ArmorValue)) {
            flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
            flNew = flDamage - flArmor;
        }

        flDamage = flNew;
    }
    return flDamage;
}
bool c_grenade_prediction::data_t::draw() const
{
    if (m_path.size() <= 1u
        || m_globals()->m_curtime >= m_expire_time)
        return false;

    bool is_he = m_index == 44;
    float distance = g_ctx.local()->m_vecOrigin().DistTo(m_origin) / 3.28;

    if (distance > 999.f)
        return false;

    static int iScreenWidth, iScreenHeight;
    m_engine()->GetScreenSize(iScreenWidth, iScreenHeight);

    auto isOnScreen = [](Vector origin, Vector& screen) -> bool
    {
        if (!math::world_to_screen(origin, screen))
            return false;

        static int iScreenWidth, iScreenHeight;
        m_engine()->GetScreenSize(iScreenWidth, iScreenHeight);

        auto xOk = iScreenWidth > screen.x;
        auto yOk = iScreenHeight > screen.y;

        return xOk && yOk;
    };

    auto prev_screen = ZERO;
    auto prev_on_screen = math::world_to_screen(std::get< Vector >(m_path.front()), prev_screen);
    /*Draw tracer*/ {

        for (auto i = 1u; i < m_path.size(); ++i) {
            auto cur_screen = ZERO, last_cur_screen = ZERO;
            const auto cur_on_screen = math::world_to_screen(std::get< Vector >(m_path.at(i)), cur_screen);
           
               
            if (prev_on_screen && cur_on_screen)
            {
                if (g_cfg.warning.trace.enable) {
                    if (!g_cfg.warning.trace.type)
                    //{
                    //    Vector sstart, eend;
                    // //   DrawBeamPaw(std::get< Vector >(m_path.at(i - 1)), std::get< Vector >(m_path.at(i)), Color(g_cfg.warning.trace.color));
                    //    if (!g_cfg.warning.trace.visible_only)
                    // //       DrawBeamPawWall(std::get< Vector >(m_path.at(i - 1)), std::get< Vector >(m_path.at(i)), Color(g_cfg.warning.trace.color));
                    //}
                    if (g_cfg.warning.trace.type == 1)
                    //{
                    //    Vector sstart, eend;

                    //    auto rainbow_col = Color::FromHSB((360 / m_path.size() * i) / 360.f, 0.9f, 0.8f);
                    ////    DrawBeamPaw(std::get< Vector >(m_path.at(i - 1)), std::get< Vector >(m_path.at(i)), Color(rainbow_col));
                    //    if (!g_cfg.warning.trace.visible_only)
                    ////        DrawBeamPawWall(std::get< Vector >(m_path.at(i - 1)), std::get< Vector >(m_path.at(i)), Color(rainbow_col));
                    //}
                    if (g_cfg.warning.trace.type == 2)
                    {

                        auto color = g_cfg.esp.grenade_warning_color;
                        Vector sstart, eend;
                        if (math::world_to_screen(std::get< Vector >(m_path.at(i - 1)), sstart) && math::world_to_screen(std::get< Vector >(m_path.at(i)), eend))
                            render::get().line(sstart.x, sstart.y, eend.x, eend.y, Color(g_cfg.warning.trace.color));


                    }
                }
            }

            prev_screen = cur_screen;
            prev_on_screen = cur_on_screen;
        }
    }
    if (m_index == 45)
        return true;

    if (!g_cfg.warning.main.enable)
        return true;

    float percent = ((m_expire_time - m_globals()->m_curtime) / TICKS_TO_TIME(m_tick));

    int end_damage = 0;
    static auto size = Vector2D(35.0f, 5.0f);
    CTraceFilter filter;
    std::pair <float, player_t*> target{ 0.f, nullptr };
    for (int i{ 0 }; i < m_globals()->m_maxclients; ++i) {
        player_t* player = (player_t*)m_entitylist()->GetClientEntity(i);
        if (!player) //-V704
            continue;

        if (!player->is_player())
            continue;

        if (!player->is_alive())
            continue;

        if (player->IsDormant())
            continue;

        // get center of mass for player.
        auto origin = player->m_vecOrigin();
        auto collideable = player->GetCollideable();

        auto min = collideable->OBBMins() + origin;
        auto max = collideable->OBBMaxs() + origin;

        auto center = min + (max - min) * 0.5f;

        // get delta between center of mass and final nade pos.
        auto delta = center - m_origin;

        if (m_index == 44) {

            // is within damage radius?
            if (delta.Length() > 350.f)
                continue;

            Ray_t ray;
            Vector NadeScreen;
            math::world_to_screen(m_origin, NadeScreen);

            // main hitbox, that takes damage
            Vector vPelvis = player->hitbox_position(HITBOX_PELVIS);
            ray.Init(m_origin, vPelvis);
            trace_t ptr;
            m_trace()->TraceRay(ray, MASK_SHOT, &filter, &ptr);
            //trace to it

            if (ptr.hit_entity == player) {
                Vector PelvisScreen;

                math::world_to_screen(vPelvis, PelvisScreen);

                // some magic values by VaLvO
                static float a = 105.0f;
                static float b = 25.0f;
                static float c = 140.0f;

                float d = ((delta.Length() - b) / c);
                float flDamage = a * exp(-d * d);

                auto dmg = max(static_cast<int>(ceilf(CSGO_Armores(flDamage, player->m_ArmorValue()))), 0);

                dmg = min(dmg, (player->m_ArmorValue() > 0) ? 57 : 100);
                if (dmg > target.first) {
                    target.first = dmg;
                    target.second = player;
                }
                end_damage = dmg;
            }
        }

    }
    int local_damage = 0;
    if (target.second == g_ctx.local())
        local_damage = end_damage;
   {
        Vector screenPos;
        int red_adjust = 0;
        auto dist = g_ctx.local()->GetAbsOrigin().DistTo(m_origin);
        if (g_cfg.warning.main.d_warn_type == 1) {
            if (dist <= (!is_he ? 150 : 350))
                red_adjust = int(195 * (1.f - (dist / (!is_he ? 150 : 350))));
        }
        /*Visible*/
        if (isOnScreen(m_origin, screenPos))
        {
            bool O = g_cfg.warning.main.color_by_time;
            Color g_col = Color(min(510 * int(percent * 100) / 100, 255), min(510 * (102 - int(percent * 100)) / 110, 255), 5);
            Vector vOutStart, vOutEnd;
            if (math::world_to_screen(m_origin, vOutStart))
            {
                if (end_damage > 0 && is_he) {
                    if (g_cfg.warning.main.show_bg) {
                        filled_circle(vOutStart.x, vOutStart.y, 19, 26, Color(g_cfg.warning.main.bg_col));
                        if (g_cfg.warning.main.d_warn_type == 1)
                            filled_circle(vOutStart.x, vOutStart.y, 19, 26, Color(25 + red_adjust, 25, 25, red_adjust));
                    }
                    if (g_cfg.warning.main.show_icon)
                        render::get().text(fonts[GRENADES], vOutStart.x, vOutStart.y - 10, O ? g_col : g_cfg.warning.main.icon_col, HFONT_CENTERED_X, !is_he ? "l" : "j");
                    std::string dmg = "-"; dmg += std::to_string(int(end_damage));
                   if (g_cfg.warning.main.show_timer)
                        render::get().CircularProgressBar(vOutStart.x, vOutStart.y, 18, 19, 90, 360 * percent, O ? g_col : g_cfg.warning.main.timer_col, false);
                }
                else {
                    if (g_cfg.warning.main.show_bg) {
                        filled_circle(vOutStart.x, vOutStart.y, 19, 26, Color(g_cfg.warning.main.bg_col));
                        if (g_cfg.warning.main.d_warn_type == 1)
                            filled_circle(vOutStart.x, vOutStart.y, 19, 26, Color(25 + red_adjust, 25, 25, red_adjust));
                    }
                    if (g_cfg.warning.main.show_icon)
                        render::get().text(fonts[GRENADES], vOutStart.x, vOutStart.y - 10, O ? g_col : g_cfg.warning.main.icon_col, HFONT_CENTERED_X, !is_he ? "l" : "j");
                    if (g_cfg.warning.main.show_timer)
                        render::get().CircularProgressBar(vOutStart.x, vOutStart.y, 18, 19, 90, 360 * percent, O ? g_col : g_cfg.warning.main.timer_col, false);
                }

            }
        }
        /*Out of Fov*/
        if (!g_cfg.warning.main.visible_only && !isOnScreen(m_origin, screenPos))
        {
            bool O = g_cfg.warning.main.color_by_time;
            Color g_col = Color(min(510 * int(percent * 100) / 100, 255), min(510 * (102 - int(percent * 100)) / 110, 255), 5);
            Vector viewAngles;
            m_engine()->GetViewAngles(viewAngles);

            static int width, height;
            m_engine()->GetScreenSize(width, height);
            auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
            auto angleYawRad = DEG2RAD(viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, m_origin).y - 90.0f);

            auto radius = 80;
            auto size = 6;

            auto newPointX = screenCenter.x + ((((width - (size * 3)) * 0.5f) * (radius / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
            auto newPointY = screenCenter.y + ((((height - (size * 3)) * 0.5f) * (radius / 100.0f)) * sin(angleYawRad));
            auto newPointX2 = screenCenter.x + ((((width - (size * 11)) * 0.5f) * (radius / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
            auto newPointY2 = screenCenter.y + ((((height - (size * 11)) * 0.5f) * (radius / 100.0f)) * sin(angleYawRad));
            std::array <Vector2D, 3> points
            {
                Vector2D(newPointX - size, newPointY - size),
                Vector2D(newPointX + size, newPointY),
                Vector2D(newPointX - size, newPointY + size)
            };
            int alp;
            if (dist <= (!is_he ? 200 : 400))
                alp = int(100 * (1.f - (dist / (!is_he ? 200 : 400))));
            auto warn = Vector2D(newPointX2, newPointY2);
            math::rotate_triangle(points, viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, m_origin).y - 90.0f);
            if (end_damage > 0 && is_he) {
                if (g_cfg.warning.main.show_bg) {
                    filled_circle(warn.x, warn.y, 19, 26, Color(g_cfg.warning.main.bg_col));
                    if (g_cfg.warning.main.d_warn_type == 1)
                        filled_circle(warn.x, warn.y, 19, 26, Color(25 + red_adjust, 25, 25, alp));
                }
                if (g_cfg.warning.main.show_icon)
                    render::get().text(fonts[GRENADES], warn.x, warn.y - 16, O ? g_col : g_cfg.warning.main.icon_col, HFONT_CENTERED_X, !is_he ? "l" : "j");
                std::string dmg = "-"; dmg += std::to_string(int(end_damage));
               /* if (g_cfg.warning.main.show_damage_dist)
                    render::get().text(fonts[NAME], warn.x, warn.y + 1, Color(255, 255, 255), HFONT_CENTERED_X, dmg.c_str());*/
                if (g_cfg.warning.main.show_timer)
                    render::get().CircularProgressBar(warn.x, warn.y, 18, 19, 90, 360 * percent, O ? g_col : g_cfg.warning.main.timer_col, false);
            }
            else {
                if (g_cfg.warning.main.show_bg) {
                    filled_circle(warn.x, warn.y, 19, 26, Color(g_cfg.warning.main.bg_col));
                    if (g_cfg.warning.main.d_warn_type == 1)
                        filled_circle(warn.x, warn.y, 19, 26, Color(25 + red_adjust, 25, 25, alp));
                }
                if (g_cfg.warning.main.show_icon)
                    render::get().text(fonts[GRENADES], warn.x, warn.y - 16, O ? g_col : g_cfg.warning.main.icon_col, HFONT_CENTERED_X, !is_he ? "l" : "j");
               /* std::string dmg = ""; dmg += std::to_string(int(max((g_ctx.local()->GetAbsOrigin().DistTo(m_origin)) / 3.28, 0))); dmg += "m";
                if (g_cfg.warning.main.show_damage_dist)
                    render::get().text(fonts[NAME], warn.x, warn.y + 1, Color(255, 255, 255), HFONT_CENTERED_X, dmg.c_str());*/
                if (g_cfg.warning.main.show_timer)
                    render::get().CircularProgressBar(warn.x, warn.y, 18, 19, 90, 360 * percent, O ? g_col : g_cfg.warning.main.timer_col, false);
            }
        }
    }





    return true;
}

void c_grenade_prediction::grenade_warning(projectile_t* entity)
{
    auto& predicted_nades = c_grenade_prediction::get().get_list();

    static auto last_server_tick = m_globals()->m_curtime;
    if (last_server_tick != m_globals()->m_curtime) {
        predicted_nades.clear();

        last_server_tick = m_globals()->m_curtime;
    }

    if (entity->IsDormant() || !g_cfg.esp.grenade_warning)
        return;

    const auto client_class = entity->GetClientClass();
    if (!client_class
        || client_class->m_ClassID != CMolotovProjectile && client_class->m_ClassID != CBaseCSGrenadeProjectile && client_class->m_ClassID != CSmokeGrenadeProjectile)
        return;

    if (client_class->m_ClassID == CBaseCSGrenadeProjectile) {
        const auto model = entity->GetModel();
        if (!model)
            return;

        const auto studio_model = m_modelinfo()->GetStudioModel(model);
        if (!studio_model
            || std::string_view(studio_model->szName).find("fraggrenade") == std::string::npos)
            return;
    }

    const auto handle = entity->GetRefEHandle().ToLong();

    if (entity->m_nExplodeEffectTickBegin() || !entity->m_hThrower().IsValid() || (entity->m_hThrower().Get()->m_iTeamNum() == g_ctx.local()->m_iTeamNum() && entity->m_hThrower().Get() != g_ctx.local())) {
        predicted_nades.erase(handle);

        return;
    }

    if (predicted_nades.find(handle) == predicted_nades.end()) {
        predicted_nades.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(handle),
            std::forward_as_tuple(
                entity->m_hThrower().Get(),
                client_class->m_ClassID == CMolotovProjectile ? WEAPON_MOLOTOV : WEAPON_HEGRENADE,
                entity->m_vecOrigin(), reinterpret_cast<player_t*>(entity)->m_vecVelocity(),
                entity->m_flSpawnTime(), TIME_TO_TICKS(reinterpret_cast<player_t*>(entity)->m_flSimulationTime() - entity->m_flSpawnTime())
            )
        );
    }

    if (predicted_nades.at(handle).draw())
        return;

    predicted_nades.erase(handle);
}