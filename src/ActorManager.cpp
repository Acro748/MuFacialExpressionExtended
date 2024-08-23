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

	void ActorManager::SetMorph(RE::Actor* a_actor, std::string morphName, int32_t value)
	{
		if (!a_actor || IsShowracemenuLoaded.load())
			return;

		if (Config::GetSingleton().GetMin() > value || value > Config::GetSingleton().GetMax())
			return;

		bool isApplied = false;
		auto found = find(a_actor->formID);
		if (found != end()) {
			isApplied = found->second->Apply(morphName, value);
		}
		else {
			auto newMorphManager = std::make_shared<MorphManager>(a_actor);
			newMorphManager->Initial();
			isApplied = newMorphManager->Apply(morphName, value);
			insert(std::make_pair(a_actor->formID, newMorphManager));
		}
		Update(a_actor);

		if (!isApplied)
			logger::error("{:x} {} : Couldn't apply the {}", a_actor->formID, a_actor->GetName(), morphName);
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::string category, std::uint32_t morphNumber, int32_t value)
	{
		if (!a_actor || IsShowracemenuLoaded.load())
			return;

		auto names = morphNameEntry::GetSingleton().GetMorphNames(category);
		if (names.size() > morphNumber)
		{
			SetMorph(a_actor, names.at(morphNumber), value);
		}
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, int32_t value)
	{
		if (!a_actor || IsShowracemenuLoaded.load())
			return;

		auto names = morphNameEntry::GetSingleton().GetMorphNames(categoryNumber);
		if (names.size() > morphNumber)
		{
			SetMorph(a_actor, names.at(morphNumber), value);
		}
	}

	void ActorManager::Revert(RE::Actor* a_actor, std::string category)
	{
		if (IsShowracemenuLoaded.load())
			return;

		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Revert(category);
			}
		}
		else
		{
			for (auto& morphManager : *this) {
				morphManager.second->Revert(category);
			}
		}
		Update(a_actor);
	}

	void ActorManager::Update(RE::Actor* a_actor)
	{
		if (IsShowracemenuLoaded.load())
			return;

		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Update();
			}
		}
		else
		{
			for (auto& morphManager : *this) {
				morphManager.second->Update();
			}
		}
	}

	void ActorManager::Initial(RE::Actor* a_actor, std::int32_t a_slot)
	{
		if (IsShowracemenuLoaded.load())
			return;

		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Revert();
				found->second->Update();
				found->second->Initial(a_slot);
			}
		}
	}

	std::int32_t ActorManager::GetValue(RE::Actor* a_actor, std::string morphName)
	{
		if (!a_actor)
			return 0;

		if (isPlayer(a_actor->formID) && IsShowracemenuLoaded.load())
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

		if (isPlayer(a_actor->formID) && IsShowracemenuLoaded.load())
			return 0;

		auto categories = morphNameEntry::GetSingleton().GetCategories();
		if (categories.size() <= categoryNumber)
			return 0;

		auto found = find(a_actor->formID);
		if (found != end()) {
			auto names = morphNameEntry::GetSingleton().GetMorphNames(categories.at(categoryNumber));
			if (names.size() > morphNumber) {
				return found->second->GetValue(names.at(morphNumber));
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
			if (auto p = RE::PlayerCharacter::GetSingleton(); p)
			{
				if (auto found = find(p->formID); found != end())
					found->second->Initial();
			}
		}
		return EventResult::kContinue;
	};

	//ActorManager::EventResult ActorManager::ProcessEvent(const RE::TESResetEvent* evn, RE::BSTEventSource<RE::TESResetEvent>*)
	//{
	//	if (!evn || !evn->object)
	//		return EventResult::kContinue;

	//	auto found = find(evn->object->formID);
	//	if (found == end())
	//		return EventResult::kContinue;

	//	found->second->Initial();
	//	return EventResult::kContinue;
	//};
}
