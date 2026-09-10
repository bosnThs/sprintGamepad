#include <SimpleIni.h>

float minAxisValue;
bool  disableMounted;

static void loadIni()
{
	CSimpleIniA                        ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\sprintGamepad.ini");

	minAxisValue = (float)ini.GetDoubleValue("settings", "fAxisSprintingMin", 0.1f);
	disableMounted = (bool)ini.GetBoolValue("settings", "bDisableWhileMounted", false);

	if (minAxisValue < 0.0f)
		minAxisValue = 0.1f;
	if (minAxisValue > 1.0f)
		minAxisValue = 1.0f;
}

struct Hooks
{
	static void ProcessThumbstick(RE::MovementHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data)
	{
		_ProcessThumbstick(a_this, a_event, a_data);

		auto player = RE::PlayerCharacter::GetSingleton();
		if (player->IsOnMount() && disableMounted)
			return;

		if (player->AsActorState()->IsSprinting()) {
			if (player->IsOnMount()) {
				if (abs(a_data->moveInputVec.x) > minAxisValue)
					a_data->moveInputVec.x = 1.0f * (a_data->moveInputVec.x / abs(a_data->moveInputVec.x));
				else
					a_data->moveInputVec.x = 0.0f;
			}

			if (abs(a_data->moveInputVec.y) > minAxisValue)
				a_data->moveInputVec.y = 1.0f * (a_data->moveInputVec.y / abs(a_data->moveInputVec.y));
			else
				a_data->moveInputVec.y = 0.0f;

			//auto playerCamera = RE::PlayerCamera::GetSingleton();
			//if (playerCamera->IsInFirstPerson())
			//	a_data->moveInputVec.x = 0.0f;

		}
	}
	static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;

	static void Install()
	{
		REL::Relocation<std::uintptr_t> MovementHandlerVtbl{ RE::VTABLE_MovementHandler[0] };
		auto vtblShift = REL::Module::IsAtLeast(SKSE::RUNTIME_SSE_1_7_99) ? 2 : 0;
		_ProcessThumbstick = MovementHandlerVtbl.write_vfunc(0x2 + vtblShift, ProcessThumbstick);
		//_ProcessThumbstick = MovementHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
	}
};

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	loadIni();
	Hooks::Install();

	return true;
}
