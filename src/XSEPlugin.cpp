//#include "Hooks.h"
#include <SimpleIni.h>

float minAxisValue;

void loadIni()
{
	CSimpleIniA                        ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\sprintGamepad.ini");

	minAxisValue = (float)ini.GetDoubleValue("settings", "fAxisSprintingMin", 0.1f);

	if (minAxisValue < 0.0f || minAxisValue > 1.0f)
		minAxisValue = 0.1f;
}

struct Hooks
{
	static void Hook()
	{
		REL::Relocation<std::uintptr_t> MovementHandlerVtbl{ RE::VTABLE_MovementHandler[0] };
		_ProcessThumbstick = MovementHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
	}

	static void ProcessThumbstick(RE::MovementHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data)
	{
		_ProcessThumbstick(a_this, a_event, a_data);

		if (RE::PlayerCharacter::GetSingleton()->AsActorState()->IsSprinting()) {

			if (abs(a_data->moveInputVec.x) > minAxisValue)
				a_data->moveInputVec.x = 1.0f * (a_data->moveInputVec.x / abs(a_data->moveInputVec.x));
			else
				a_data->moveInputVec.x = 0.0f;

			if (abs(a_data->moveInputVec.y) > minAxisValue)
				a_data->moveInputVec.y = 1.0f * (a_data->moveInputVec.y / abs(a_data->moveInputVec.y));
			else
				a_data->moveInputVec.y = 0.0f;
		}
	}
	static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;
};


void Init()
{
	loadIni();
	Hooks::Hook();
}

void InitializeLog()
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format("{}.log"sv, Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif

	InitializeLog();

	logger::info("Loaded plugin");

	SKSE::Init(a_skse);

	Init();

	return true;
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
}();

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}
