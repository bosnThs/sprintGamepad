#pragma once
#include <SimpleIni.h>
#include "SKSEMenuFramework.h"
#include "Configuration.h"
namespace UI {
    void Register();
	void LoadSettings();
	void SaveSettings();

    namespace Settings {
        void __stdcall Render();
        void __stdcall OnEvent(SKSEMenuFramework::Model::EventType eventType);
	}
};