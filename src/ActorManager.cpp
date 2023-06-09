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

		auto found = find(a_actor->formID);
		if (found != end()) {
			found->second.Apply(morphName, value);
		}
		else {
			MorphManager newMorphManager = MorphManager(a_actor);
			newMorphManager.Apply(morphName, value);
			insert(std::make_pair(a_actor->formID, newMorphManager));
		}
	}
	void ActorManager::SetMorph(RE::Actor* a_actor, morphNameEntry::morphType type, std::uint32_t morphNumber, float value)
	{
		if (!a_actor)
			return;

		auto names = morphNameEntry::GetSingleton().GetNames(type);
		if (names.size() < morphNumber)
		{
			SetMorph(a_actor, names.at(morphNumber), value);
		}
	}

	void ActorManager::Revert(RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto found = find(a_actor->formID);
		if (found != end()) {
			found->second.Revert();
		}
	}

	void ActorManager::Update(RE::Actor* a_actor)
	{
		if (!a_actor)
			return;

		auto found = find(a_actor->formID);
		if (found != end()) {
			found->second.Update();
		}
	}

	float ActorManager::GetValue(RE::Actor* a_actor, std::string morphName)
	{
		if (!a_actor)
			return 0.0f;

		auto found = find(a_actor->formID);
		if (found != end()) {
			return found->second.GetValue(morphName);
		}
		return 0.0f;
	}
	float ActorManager::GetValue(RE::Actor* a_actor, morphNameEntry::morphType type, std::uint32_t morphNumber)
	{
		if (!a_actor)
			return 0.0f;
		auto found = find(a_actor->formID);
		if (found != end()) {
			auto names = morphNameEntry::GetSingleton().GetNames(type);
			if (names.size() > morphNumber) {
				return found->second.GetValue(names.at(morphNumber));
			}
		}
		return 0.0f;
	}
}
