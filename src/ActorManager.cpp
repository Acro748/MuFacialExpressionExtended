#include "ActorManager.h"

namespace Mus {
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

	void ActorManager::SetMorph(RE::Actor* a_actor, std::string morphName, float value) 
	{
		if (!a_actor)
			return;

		bool isApplied = false;
		auto found = find(a_actor->formID);
		if (found != end()) {
			isApplied = found->second->Apply(morphName, value);
		}
		else {
			auto newMorphManager = std::make_shared<MorphManager>(a_actor);
			isApplied = newMorphManager->Apply(morphName, value);
			insert(std::make_pair(a_actor->formID, newMorphManager));
		}

		if (!isApplied)
			logger::error("{:x} {} : Couldn't apply the {}", a_actor->formID, a_actor->GetName(), morphName);
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::string category, std::uint32_t morphNumber, float value)
	{
		if (!a_actor)
			return;

		auto names = morphNameEntry::GetSingleton().GetMorphNames(category);
		if (names.size() < morphNumber)
		{
			SetMorph(a_actor, names.at(morphNumber), value);
		}
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, float value)
	{
		if (!a_actor)
			return;

		auto names = morphNameEntry::GetSingleton().GetMorphNames(categoryNumber);
		if (names.size() < morphNumber)
		{
			SetMorph(a_actor, names.at(morphNumber), value);
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
			for (auto& morphManager : *this) {
				morphManager.second->Revert();
			}
		}
	}

	void ActorManager::Update(RE::Actor* a_actor, std::string category)
	{
		if (a_actor)
		{
			if (auto found = find(a_actor->formID); found != end()) {
				found->second->Update(category);
			}
		}
		else
		{
			for (auto& morphManager : *this) {
				morphManager.second->Revert();
			}
		}
	}

	float ActorManager::GetValue(RE::Actor* a_actor, std::string morphName)
	{
		if (!a_actor)
			return 0.0f;

		auto found = find(a_actor->formID);
		if (found != end()) {
			return found->second->GetValue(morphName);
		}
		return 0.0f;
	}
	float ActorManager::GetValue(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber)
	{
		if (!a_actor)
			return 0.0f;
		auto categories = morphNameEntry::GetSingleton().GetCategories();
		if (categories.size() >= categoryNumber)
			return 0.0f;

		auto found = find(a_actor->formID);
		if (found != end()) {
			auto names = morphNameEntry::GetSingleton().GetMorphNames(categories.at(categoryNumber));
			if (names.size() > morphNumber) {
				return found->second->GetValue(names.at(morphNumber));
			}
		}
		return 0.0f;
	}

	ActorManager::EventResult ActorManager::ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) 
	{
		if (!evn || evn->menuName.empty())
			return EventResult::kContinue;

		if (evn->opening && IsSameString(evn->menuName.c_str(), "RaceSex Menu"))
		{
			if (auto p = RE::PlayerCharacter::GetSingleton(); p)
			{
				if (auto found = find(p->formID); found != end())
					found->second->Reset();
			}
		}
		return EventResult::kContinue;
	};
}
