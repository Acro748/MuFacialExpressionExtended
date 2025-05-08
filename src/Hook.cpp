#include "hook.h"

namespace Mus {
	constexpr REL::VariantID GameLoopFunction(35565, 36564, 0x005BAB10);

	EventDispatcherImpl<FrameEvent>  g_frameEventDispatcher;

	typedef void (*_NullSub)();
	REL::Relocation<_NullSub> NullSubOrig;
	void onNullSub()
	{
		NullSubOrig();

		FrameEvent e;
		const auto main = RE::Main::GetSingleton();
		e.gamePaused = (main && main->freezeTime);
		const auto menu = RE::UI::GetSingleton();
		IsGamePaused.store(e.gamePaused || (menu && menu->numPausesGame > 0));
		g_frameEventDispatcher.dispatch(e);
	}
	void hookEngineTrampoline(SKSE::Trampoline& trampoline)
	{
		//NullSub_594, NullSub_471, NullSub_611
		constexpr auto GameLoopFunctionOffset = REL::VariantOffset(0x748, 0xC26, 0x7EE);
		NullSubOrig = trampoline.write_call<5>(GameLoopFunction.address() + GameLoopFunctionOffset.offset(), onNullSub);
	}

	void hook()
	{
		logger::info("Skyrim Hooking...");

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.create(16);
		hookEngineTrampoline(trampoline);
	}
}