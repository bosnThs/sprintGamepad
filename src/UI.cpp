#include "UI.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("SprintGamepad");
    SKSEMenuFramework::AddSectionItem("Settings", Settings::Render);
    SKSEMenuFramework::AddEvent(UI::Settings::OnEvent, 0);
}

void UI::LoadSettings() {
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile("Data/SKSE/Plugins/SprintGamepad.ini");
    Configuration::Settings::fAxisSprintingMin = (float)ini.GetDoubleValue("Settings", "fAxisSprintingMin", 0.1);
    Configuration::Settings::bDisableWhileMounted = ini.GetBoolValue("Settings", "bDisableWhileMounted", false);
}

void UI::SaveSettings() {
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile("Data/SKSE/Plugins/SprintGamepad.ini");
    ini.SetDoubleValue("Settings", "fAxisSprintingMin", Configuration::Settings::fAxisSprintingMin);
    ini.SetBoolValue("Settings", "bDisableWhileMounted", Configuration::Settings::bDisableWhileMounted);
    ini.SaveFile("Data/SKSE/Plugins/SprintGamepad.ini");
}

void UI::Settings::Render() {
    ImGuiMCP::Text("Thumbstick Axis Min Value:");
    ImGuiMCP::SliderFloat("", &Configuration::Settings::fAxisSprintingMin, 0.0, 1.0);
	ImGuiMCP::Checkbox("Disable fix on mount", &Configuration::Settings::bDisableWhileMounted);
}

void __stdcall UI::Settings::OnEvent(SKSEMenuFramework::Model::EventType eventType) {
    if (SKSEMenuFramework::Model::EventType::kCloseMenu == eventType) {
        UI::SaveSettings();
    }
}
