#include "ActorManager.h"

namespace Mus {
	void ActorManager::Init()
	{
		if (const auto Menu = RE::UI::GetSingleton(); Menu) {
			Menu->AddEventSink<RE::MenuOpenCloseEvent>(this);
		}
	}

	void ActorManager::Save(SKSE::SerializationInterface* serde)
	{
		if (!serde->OpenRecord(ActorManagerRecord, 0)) {
			logger::error("Unable to open record to write cosave data");
			return;
		}
		logger::info("Saving on cosave...");
	}

	void ActorManager::Load(SKSE::SerializationInterface* serde, const std::uint32_t type)
	{
		if (type == ActorManagerRecord) {
			logger::info("Loding on cosave ActorManagerRecord...");
		}
	}

	void ActorManager::SetMorph(RE::Actor* a_actor, std::string morphName, std::int32_t value, std::int32_t lerpTime)
	{
		if (!a_actor || morphName.empty())
			return;

		if (Config::GetSingleton().GetMin() > value || value > Config::GetSingleton().GetMax())
			return;

		if (lerpTime <= -1)
			lerpTime = Config::GetSingleton().GetDefaultLerpTime();

		morphName = lowLetter(morphName);
		bool isApplied = false;
		auto found = find(a_actor->formID);
		if (found != end()) {
			isApplied = found->second->SetValue(morphName, value, lerpTime);
		}
		else {
			auto newMorphManager = std::make_shared<MorphManager>(a_actor);
			isApplied = newMorphManager->SetValue(morphName, value, lerpTime);
			insert(std::make_pair(a_actor->formID, newMorphManager));
		}

		if (!isApplied)
			logger::error("{:x} {} : Couldn't apply the {}", a_actor->formID, a_actor->GetName(), morphName);
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::string category, std::uint32_t morphNumber, std::int32_t value, std::int32_t lerpTime)
	{
		if (!a_actor)
			return;

		auto names = morphNameEntry::GetSingleton().GetMorphNames(category);
		if (names.size() > morphNumber)
		{
			SetMorph(a_actor, names[morphNumber], value, lerpTime);
		}
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, std::int32_t value, std::int32_t lerpTime)
	{
		auto names = morphNameEntry::GetSingleton().GetMorphNames(categoryNumber);
		if (names.size() > morphNumber)
		{
			SetMorph(a_actor, names[morphNumber], value, lerpTime);
		}
	}

	void ActorManager::Revert(RE::Actor* a_actor, std::string category)
	{
		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Revert(category);
			}
		}
		else
		{
			concurrency::parallel_for_each(this->begin(), this->end(), [&](auto& morphManager) {
				morphManager.second->Revert(category);
			});
		}
	}

	void ActorManager::Update(RE::Actor* a_actor)
	{
		PerformaceLog(std::string("ActorManager::") + __func__, false);
		std::clock_t processTime = RE::GetSecondsSinceLastFrame() * 1000;
		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Update(processTime);
			}
		}
		else
		{
            concurrency::parallel_for_each(queueUpdate.begin(), queueUpdate.end(), [&](auto& morphManager) {
				morphManager.second->Update(processTime);
			});
            queueUpdate.clear();
		}
		PerformaceLog(std::string("ActorManager::") + __func__, true, PerformanceCheckAverage, a_actor ? 1 : size());
	}

	void ActorManager::Initial(RE::Actor* a_actor)
	{
		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Revert();
			}
		}
		else
		{
			concurrency::parallel_for_each(this->begin(), this->end(), [&](auto& morphManager) {
				morphManager.second->Revert();
			});
		}
	}

	std::int32_t ActorManager::GetValue(RE::Actor* a_actor, std::string morphName)
	{
		if (!a_actor)
			return 0;

		if (isPlayer(a_actor->formID))
			return 0;

		auto found = find(a_actor->formID);
		if (found != end()) {
			return found->second->GetValue(morphName);
		}
		return 0;
	}
	std::int32_t ActorManager::GetValue(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber)
	{
		if (!a_actor)
			return 0;

		if (isPlayer(a_actor->formID))
			return 0;

		auto categories = morphNameEntry::GetSingleton().GetCategories();
		if (categories.size() <= categoryNumber)
			return 0;

		auto found = find(a_actor->formID);
		if (found != end()) {
			auto names = morphNameEntry::GetSingleton().GetMorphNames(categories[categoryNumber]);
			if (names.size() > morphNumber) {
				return found->second->GetValue(names[morphNumber]);
			}
		}
		return 0;
	}

	std::vector<MorphManager::ActiveMorphSet> ActorManager::GetAllActiveMorphs(RE::Actor* a_actor)
	{
		if (!a_actor)
			return std::vector<MorphManager::ActiveMorphSet>();

		if (const auto found = find(a_actor->formID); found != end())
		{
			return found->second->GetAllActiveMorphs();
		}
		return std::vector<MorphManager::ActiveMorphSet>();
	}

	ActorManager::EventResult ActorManager::ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) 
	{
		if (!evn || evn->menuName.empty())
			return EventResult::kContinue;

		if (evn->opening && IsSameString(evn->menuName.c_str(), "RaceSex Menu"))
		{
			IsShowracemenuLoaded.store(true);
		}
		else if (!evn->opening && IsSameString(evn->menuName.c_str(), "RaceSex Menu"))
		{
			IsShowracemenuLoaded.store(false);
		}
		return EventResult::kContinue;
	}; 
	
	void ActorManager::onEvent(const FrameEvent& e)
	{
		if (isPaused || IsShowracemenuLoaded.load())
		{
			isPaused = IsGamePaused.load();
			return;
		}
		Update();
		isPaused = IsGamePaused.load(); // one frame delay
    }
	void ActorManager::onEvent(const FacegenNiNodeEvent& e)
    {
        if (!e.root || !e.root->userData)
            return;
        RE::Actor* a_actor = skyrim_cast<RE::Actor*>(e.root->userData);
        if (!a_actor)
            return;
        auto found = find(a_actor->formID);
        if (found == end())
            return;
        found->second->SetNeedUpdateFacegenMeshes();
	}
    void ActorManager::onEvent(const FaceUpdateEvent& e)
	{
        auto found = find(e.ref->formID);
        if (found == end())
			return;
        queueUpdate[e.ref->formID] = found->second;
	}
}
