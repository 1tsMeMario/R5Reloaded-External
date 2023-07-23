#include "overlay.h"
#include "ImGui\customed.h"
#include <string>

extern Memory m;

int bone = 1;
float VisiblePlayer[16000] = {};
const char* BoxList[] = { "2D Box", "2D Corner Box" };
const char* BoneList[] = { "Head", "Chest", "Belly" };

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent);
bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to);
Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition);

/*  // 既知のバグ：
    * 超至近距離にいる敵にAimが反応しないことがある
    -> チェックするBONEの数を増やす

    * WeaponHandleがうまく取得できない
    -> R5Reloadedのせいなのでどうしようもない
    
    * ダミーのvecAbsVelocityが取得できない
    -> R5Reloadedのせいなのでどうしようもない
*/

void Overlay::m_Info()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)GameSize.right, (float)GameSize.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::Text("R5Reloaded External [%.1f FPS]", ImGui::GetIO().Framerate);

    if (g.AimBot && DrawFov)
        Circle((float)GameSize.right, (float)GameSize.bottom, g.AimFov, WHITE, 1.0f);

    if (g.SpeedHx)
    {
        ImGui::SetCursorPos(ImVec2((float)GameSize.right / 1.35f, (float)GameSize.bottom / 12.f));
        ImGui::BeginChild("Child01", ImVec2(200, 200), (bool*)false, ImGuiWindowFlags_NoBackground);

        ImGui::Text("SpeedHack :"); ImGui::SameLine();
        if (g.DownTime == 0)
            ImGui::TextColored(GREEN, "Ready!");
        else
            ImGui::TextColored(RED, "Locked!");
        ImGui::Text("%d Sec", g.DownTime);

        ImGui::EndChild();
    }

    if (DummyESP)
        g.MaxCount = 16000;
    else
        g.MaxCount = 75;

    ImGui::End();
}

void Overlay::m_Menu()
{
    ImGui::SetNextWindowBgAlpha(0.95f);
    ImGui::Begin("##NULL", (bool*)NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

    // Select Menu
    ImGui::BeginChild("##SelectMenu", ImVec2(240.f, 525.f));
    ImGui::SetCursorPos(ImVec2(8.f, 8.f));
    ImGui::BeginChild("##C1", ImVec2(240.f - 16.f, 300.f), (bool*)false, ImGuiWindowFlags_NoBackground);
    static int menu = 0;

    if (ImGui::Button("Aim", ImVec2(224.f, 45.f)))
        menu = 0;
    else if (ImGui::Button("Visual", ImVec2(224.f, 45.f)))
        menu = 1;
    else if (ImGui::Button("Misc", ImVec2(224.f, 45.f)))
        menu = 2;
    else if (ImGui::Button("Dev", ImVec2(224.f, 45.f)))
        menu = 3;

    ImGui::EndChild();
    ImGui::EndChild();
    // SelectMenu End

    // Main Setting
    ImGui::SameLine();
    ImGui::BeginChild("##MainMenu", ImVec2(315.f, 525.f));
    ImGui::SetCursorPos(ImVec2(8.f, 8.f));
    ImGui::BeginChild("##M1", ImVec2(315.f - 16.f, 525.f - 16.f), (bool*)false, ImGuiWindowFlags_NoBackground);

    ImGui::Text("[+] Main");
    ImGui::Separator();
    ImGui::NewLine();

    // Menu Start
    switch (menu)
    {
    case 0:
        ImGui::Toggle("AimBot", &g.AimBot, g.AimBot);
        ImGui::Toggle("Aim at Team", &g.AimAtTeam, g.AimAtTeam);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Checkbox("Visibility Check", &g.VisCheck);
        ImGui::Checkbox("NoSway", &g.NoSway);
        ImGui::Checkbox("Draw FOV", &DrawFov);
        ImGui::Combo("Bone", &bone, BoneList, IM_ARRAYSIZE(BoneList));

        switch (bone)
        {
        case 0:
            g.AimBone = 8;
            break;
        case 1:
            g.AimBone = 3;
            break;
        case 2:
            g.AimBone = 2;
            break;
        default:
            break;
        }

        break;
    case 1:
        ImGui::Toggle("Player ESP", &ESP, ESP);
        ImGui::Toggle("Dummy ESP", &DummyESP, DummyESP);
        ImGui::Toggle("Team ESP", &TeamESP, TeamESP);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();
        
        ImGui::BulletText("ESP Options");
        ImGui::Checkbox("Box", &ESP_Box);
        ImGui::Checkbox("Line", &ESP_Line);
        ImGui::Checkbox("Distance", &ESP_Distance);
        ImGui::Checkbox("HealthBar (HP + Shield)", &ESP_HealthBar);
        break;
    case 2:
        ImGui::Toggle("SpeedHack", &g.SpeedHx, g.SpeedHx);
        ImGui::SliderFloat("Speed", &g.Speed, 2.f, 6.f);
        break;
    case 3:
        ImGui::Text("BaseAddress : 0x%lx\n", m.BaseAddress);
        ImGui::Text("Process ID  : %d", m.PID);
        ImGui::Text("  pHandle   : 0x%lx\n", m.pHandle);
        ImGui::Text(" GameSize   : %d, %d", GameSize.right, GameSize.bottom);
        break;
    default:
        break;
    }

    ImGui::EndChild();
    ImGui::EndChild();
    // Main Setting End

    // Sub Setting
    if (menu == 0 || menu == 1)
    {
        ImGui::SameLine();
        ImGui::BeginChild("##SubMenu", ImVec2(315.f, 525.f));
        ImGui::SetCursorPos(ImVec2(8.f, 8.f));
        ImGui::BeginChild("##SX1", ImVec2(315.f - 16.f, 525.f - 16.f), (bool*)false, ImGuiWindowFlags_NoBackground);

        ImGui::Text("[+] Advanced");
        ImGui::Separator();
        ImGui::NewLine();

        switch (menu)
        {
        case 0:
            ImGui::BulletText("Aim Config");

            ImGui::SliderFloat("FOV", &g.AimFov, 25.f, 500.f);
            ImGui::SliderFloat("Smooth", &g.Smooth, 1.f, 50.f);
            ImGui::SliderFloat("Distance", &g.AimDist, 15.f, 500.f);
            ImGui::SliderInt("Prediction", &g.PredictVal, 100, 300);
            break;
        case 1:
            ImGui::BulletText("ESP Color");
            ImGui::ColorEdit4("Normal ", &ESP_Normal.Value.x);
            ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);

            ImGui::NewLine();
            ImGui::NewLine();

            ImGui::BulletText("ESP Config");
            ImGui::SliderFloat("Distance (m)", &MaxDistance, 25, 2000);
            ImGui::Combo("Box Style", &BoxType, BoxList, IM_ARRAYSIZE(BoxList));
            break;
        default:
            break;
        }

        ImGui::EndChild();
        ImGui::EndChild();
    }
    // Sub Setting End

    ImGui::End();
}

void Overlay::m_ESP()
{
    // ImGui Window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)GameSize.right, (float)GameSize.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // LocalPlayer
    CPlayer LPlayer, *pLocal = &LPlayer;
    uint64_t LocalPLayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
    LPlayer = m.Read<CPlayer>(LocalPLayer + 0x140);

    if (DummyESP)
    {
        for (int x = 0; x < 75; x++)
        {
            uint64_t Ent = GetEntityById(m.BaseAddress, x);

            if (Ent == NULL)
                continue;

            // Vis check
            bool visible = false;
            float LastVisibleTime = m.Read<float>(Ent + 0x1754);
            visible = LastVisibleTime > 0.f && LastVisibleTime > VisiblePlayer[x];
            VisiblePlayer[x] = LastVisibleTime;
        }
    }

    for (int i = 0; i < g.MaxCount; i++)
    {
        // Local Check
        if (pLocal->m_iHealth <= 0)
            continue;
        else if (pLocal->m_localOrigin == Vector3(0.f, 0.f, 0.f))
            continue;

        CPlayer BaseEntity, *pEntity = &BaseEntity;

        uint64_t Entity = GetEntityById(m.BaseAddress, i);
        
        if (Entity == NULL || Entity == LocalPLayer)
            continue;

        BaseEntity = m.Read<CPlayer>(Entity + 0x140);

        if (!TeamESP && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;
        else if (pEntity->m_iTeamNum == 0)
            continue;
        else if (pEntity->m_iHealth <= 0 || pEntity->m_iHealth == 0)
            continue;

        uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
        uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
        Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

        // W2S
        Vector2 ScreenPos = {};
        WorldToScreen(pEntity->m_localOrigin, &ViewMatrix._11, (float)GameSize.right, (float)GameSize.bottom, ScreenPos);

        // ターゲットがレンダリング可能な範囲内にいたら
        if (ScreenPos.x != 0.f && ScreenPos.y != 0.f)
        {   
            // Distance
            float pDist = ((pEntity->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);

            if (pDist <= MaxDistance)
            {
                // Head ScreenPosision
                Vector3 HeadPos = GetEntityBonePosition(Entity, 8, pEntity->m_localOrigin);
                Vector2 ScreenHeadPos = {};
                WorldToScreen(HeadPos, &ViewMatrix._11, (float)GameSize.right, (float)GameSize.bottom, ScreenHeadPos);

                // Box
                float Height = abs(abs(ScreenHeadPos.y) - abs(ScreenPos.y));
                float Width = Height / 1.85f;
                float BoxMiddle = ScreenPos.x - (Width / 2.f);

                // Vis check
                bool visible = false;
                float LastVisibleTime = m.Read<float>(Entity + 0x1754);
                visible = LastVisibleTime > 0.f && LastVisibleTime > VisiblePlayer[i];
                VisiblePlayer[i] = LastVisibleTime;

                ImVec4 ESP_Color;

                if (visible)
                    ESP_Color = ESP_Visible;
                else if (!visible)
                    ESP_Color = ESP_Normal;
                else if (TeamESP && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
                    ESP_Color = ESP_Team;

                // Line
                if (ESP_Line)
                    DrawLine(ImVec2((float)GameSize.right / 2, (float)GameSize.bottom), ImVec2(ScreenPos.x, ScreenPos.y), ESP_Color, 1);

                // ESP Box
                if (ESP_Box)
                {
                    switch (BoxType)
                    {
                    case 0:
                        // Normal 2D Box
                        DrawBox(ESP_Color, BoxMiddle, ScreenHeadPos.y, Width, Height);
                        break;
                    case 1:
                        // Corner ESP Box
                        DrawLine(ImVec2((ScreenHeadPos.x + Width / 2), ScreenHeadPos.y), ImVec2((ScreenHeadPos.x + Width / 3), ScreenHeadPos.y), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x - Width / 2), ScreenHeadPos.y), ImVec2((ScreenHeadPos.x - Width / 3), ScreenHeadPos.y), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x + Width / 2), ScreenHeadPos.y), ImVec2((ScreenHeadPos.x + Width / 2), ScreenHeadPos.y + Height / 4), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x - Width / 2), ScreenHeadPos.y), ImVec2((ScreenHeadPos.x - Width / 2), ScreenHeadPos.y + Height / 4), ESP_Color, 1);

                        DrawLine(ImVec2((ScreenHeadPos.x + Width / 2), ScreenPos.y), ImVec2((ScreenHeadPos.x + Width / 2), ScreenPos.y - (Height / 4)), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x - Width / 2), ScreenPos.y), ImVec2((ScreenHeadPos.x - Width / 2), ScreenPos.y - (Height / 4)), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x + Width / 2), ScreenPos.y), ImVec2((ScreenHeadPos.x + Width / 3), ScreenPos.y), ESP_Color, 1);
                        DrawLine(ImVec2((ScreenHeadPos.x - Width / 2), ScreenPos.y), ImVec2((ScreenHeadPos.x - Width / 3), ScreenPos.y), ESP_Color, 1);
                        break;
                    default:
                        break;
                    }
                }

                // Health Bar
                if (ESP_HealthBar)
                {
                    BaseBar(ScreenHeadPos.x - (Width / 2.f) - 7, (ScreenHeadPos.y + Height) + 1, 4, -Height - 1);
                    ProgressBar((ScreenHeadPos.x - (Width / 2.f) - 6), (ScreenHeadPos.y + Height), 2, -Height, pEntity->m_iHealth + pEntity->m_shieldHealth, pEntity->m_iMaxHealth + pEntity->m_shieldHealthMax);
                }

                // Distance
                if (ESP_Distance)
                {
                    std::string distance = std::to_string((int)pDist) + "m";

                    // Get text center position
                    ImVec2 textSize = ImGui::CalcTextSize(distance.c_str());
                    float TextCentor = textSize.x / 2.f;

                    String(ImVec2(ScreenPos.x - TextCentor, ScreenPos.y), WHITE, distance.c_str());
                }
            }
        }
    }

    if (!DummyESP)
        std::this_thread::sleep_for(std::chrono::microseconds(1));

    ImGui::End();
}

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent)
{
    uint64_t EntityList = GameBaseAddr + offset::dwEntityList;
    uint64_t BaseEntity = m.Read<uint64_t>(EntityList + (Ent << 5));
    if (!BaseEntity)
        return 0;
    return BaseEntity;
}

bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to)
{
    float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

    if (w < 0.01f)
        return false;

    to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
    to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

    float invw = 1.0f / w;
    to.x *= invw;
    to.y *= invw;

    float x = targetWidth / 2;
    float y = targetHeight / 2;

    x += 0.5 * to.x * targetWidth + 0.5;
    y -= 0.5 * to.y * targetHeight + 0.5;

    to.x = x;
    to.y = y;

    return true;
}

Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition)
{
    uint64_t pBoneArray = m.Read<uint64_t>(ent + offset::s_BoneMatrix);

    Vector3 EntityHead = Vector3();

    EntityHead.x = m.Read<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
    EntityHead.y = m.Read<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
    EntityHead.z = m.Read<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

    return EntityHead;
}