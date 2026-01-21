#include "hook.h"
#include "detours/detours.h"

namespace Mus {
	EventDispatcherImpl<FrameEvent>  g_frameEventDispatcher;
    EventDispatcherImpl<FacegenNiNodeEvent> g_facegenNiNodeEventDispatcher;
    EventDispatcherImpl<FaceUpdateEvent> g_faceUpdateEventDispatcher;
    EventDispatcherImpl<ActorChangeHeadPartEvent> g_actorChangeHeadPartEventDispatcher;

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
        constexpr REL::VariantID GameLoopFunction(35565, 36564, 0x005BAB10);
		constexpr auto GameLoopFunctionOffset = REL::VariantOffset(0x748, 0xC26, 0x7EE);
        NullSubOrig = trampoline.write_call<5>(GameLoopFunction.address() + GameLoopFunctionOffset.offset(), onNullSub);
	}

    constexpr REL::VariantID BSFaceGenNiNodeFunction(26405, 26986, 0x003E8120);
	#ifndef ENABLE_SKYRIM_VR
    typedef void (*_onFaceGen)(RE::BSFaceGenNiNode*, RE::NiNode*, RE::BSGeometry*, std::uint8_t);
#else
    typedef void (*_onFaceGen)(RE::BSFaceGenNiNode*, RE::NiNode*, RE::BSGeometry*);
#endif
    REL::Relocation<_onFaceGen> onFaceGen_Orig(BSFaceGenNiNodeFunction);

#ifndef ENABLE_SKYRIM_VR
    void __fastcall onFaceGen(RE::BSFaceGenNiNode* facegen, RE::NiNode* root, RE::BSGeometry* geometry, std::uint8_t unk4)
#else
    void __fastcall onFaceGen(RE::BSFaceGenNiNode* facegen, RE::NiNode* root, RE::BSGeometry* geometry)
#endif
    {
#ifndef ENABLE_SKYRIM_VR
        onFaceGen_Orig(facegen, root, geometry, unk4);
#else
        onFaceGen_Orig(facegen, root, geometry);
#endif
        FacegenNiNodeEvent e;
        e.root = root;
        e.facegenNiNode = facegen;
        g_facegenNiNodeEventDispatcher.dispatch(e);
    }

	enum FaceGenNiNodeFlags {
		kUpdate = 1 << 0,
		kIsInDistance = 1 << 6,
		kIsValid = 1 << 8
	};
	typedef void (*_FaceMorphingFunction)(RE::BSFaceGenNiNode*, bool);
    REL::Relocation<_FaceMorphingFunction> FaceMorphingFunctionOrig;
    void onFaceMorphingFunction(RE::BSFaceGenNiNode* facegen, bool unk1)
	{
        if (!facegen)
            return FaceMorphingFunctionOrig(facegen, unk1);
        auto ref = facegen->GetUserData();
        if (!ref)
            return FaceMorphingFunctionOrig(facegen, unk1);
        //logger::info("{} {:x} : {}", ref->GetName(), ref->formID, facegen->GetRuntimeData().flags);
        if (!(facegen->GetRuntimeData().flags & FaceGenNiNodeFlags::kUpdate) && !unk1)
            return FaceMorphingFunctionOrig(facegen, unk1);
        if (Config::GetSingleton().GetDisableDistance())
			facegen->GetRuntimeData().flags |= FaceGenNiNodeFlags::kIsInDistance;
        FaceUpdateEvent e;
        e.ref = ref;
        g_faceUpdateEventDispatcher.dispatch(e);
        return FaceMorphingFunctionOrig(facegen, unk1);
	}
    void hookFaceMorphingFunction(SKSE::Trampoline& trampoline)
    {
        constexpr REL::VariantID FaceMorphingFunction(26418, 26999, 0x003E91D0);
        constexpr auto FaceMorphingFunctionOffset = REL::VariantOffset(0x13, 0x13, 0x13);
        FaceMorphingFunctionOrig = trampoline.write_call<5>(FaceMorphingFunction.address() + FaceMorphingFunctionOffset.offset(), onFaceMorphingFunction);
    }

    constexpr REL::VariantID ActorChangeHeadPartFunction(26468, 27063, 0x003EBD30);
    typedef void* (*_ActorChangeHeadPart)(RE::Actor*, RE::BGSHeadPart*, RE::BGSHeadPart*);
    REL::Relocation<_ActorChangeHeadPart> onActorChangeHeadPart_Orig(ActorChangeHeadPartFunction);
    void* __fastcall onActorChangeHeadPart(RE::Actor* actor, RE::BGSHeadPart* oldPart, RE::BGSHeadPart* newPart)
    {
        void* result = onActorChangeHeadPart_Orig(actor, oldPart, newPart);
        ActorChangeHeadPartEvent e;
        e.actor = actor;
        e.oldHeadPart = oldPart;
        e.newHeadPart = newPart;
        g_actorChangeHeadPartEventDispatcher.dispatch(e);
        return result;
    }
	
	void hook()
	{
		logger::info("Skyrim Hooking...");

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.create(32);
		hookEngineTrampoline(trampoline);
        hookFaceMorphingFunction(trampoline);

        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)onFaceGen_Orig, onFaceGen);
        DetourAttach(&(PVOID&)onActorChangeHeadPart_Orig, onActorChangeHeadPart);
        DetourTransactionCommit();
	}
}