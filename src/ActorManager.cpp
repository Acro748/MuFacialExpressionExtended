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

	void ActorManager::SetMorph(RE::Actor* a_actor, const lString& morphName, std::int32_t value, std::int32_t lerpTime)
    {
        if (!a_actor || morphName.empty())
            return;

        if (value < Config::GetSingleton().GetMin() || Config::GetSingleton().GetMax() < value)
            return;

        if (lerpTime <= -1)
            lerpTime = Config::GetSingleton().GetDefaultLerpTime();

        MorphManagerPtr mm = nullptr;
        {
            std::shared_lock sl(morphManagerLock);
            if (auto found = map.find(a_actor->formID); found != map.end())
                mm = found->second;
        }
        if (!mm)
        {
            std::lock_guard lg(morphManagerLock);
            mm = std::make_shared<MorphManager>(a_actor);
            map.insert(std::make_pair(a_actor->formID, mm));
        }
        mm->SetValue(morphName, value, lerpTime);
        UIManager::GetSingleton().UpdateData();
    }
	void ActorManager::SetMorph(RE::Actor* a_actor, const lString& category, std::uint32_t morphNumber, std::int32_t value, std::int32_t lerpTime)
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

	void ActorManager::Revert(RE::Actor* a_actor, const lString& category)
    {
        std::shared_lock sl(morphManagerLock);
		if (a_actor)
		{
            if (auto found = map.find(a_actor->formID); found != map.end())
            {
				found->second->Revert(category);
            }
		}
		else
		{
            concurrency::parallel_for_each(map.begin(), map.end(), [&](auto& morphManager) {
				morphManager.second->Revert(category);
            });
        }
        UIManager::GetSingleton().UpdateData();
	}

	void ActorManager::Update(RE::Actor* a_actor)
	{
		PerformaceLog(std::string("ActorManager::") + __func__, false);
        std::clock_t processTime = RE::GetSecondsSinceLastFrame() * 1000;
		if (a_actor)
        {
            MorphManagerPtr mm = nullptr;
            {
                std::shared_lock sl(morphManagerLock);
                if (auto found = map.find(a_actor->formID); found != map.end())
                    mm = found->second;
            }
            if (mm)
                mm->Update(processTime);
		}
        else
        {
            std::unordered_map<RE::FormID, MorphManagerPtr> queueUpdate_;
            {
				std::lock_guard sl(morphManagerLock);
                queueUpdate_ = std::move(queueUpdate);
            }
            concurrency::parallel_for_each(queueUpdate_.begin(), queueUpdate_.end(), [&](auto& morphManager) {
                morphManager.second->Update(processTime);
            });
        }
        PerformaceLog(std::string("ActorManager::") + __func__, true, PerformanceCheckAverage, a_actor ? 1 : map.size());
	}

	void ActorManager::Initial(RE::Actor* a_actor)
    {
        std::shared_lock sl(morphManagerLock);
		if (a_actor)
		{
            if (auto found = map.find(a_actor->formID); found != map.end())
            {
				found->second->Revert();
			}
		}
		else
		{
            concurrency::parallel_for_each(map.begin(), map.end(), [&](auto& morphManager) {
				morphManager.second->Revert();
			});
		}
	}

	std::int32_t ActorManager::GetValue(RE::Actor* a_actor, const lString& morphName, bool destination) const
	{
		if (!a_actor)
			return 0;
        std::shared_lock sl(morphManagerLock);
        auto found = map.find(a_actor->formID);
        if (found != map.end())
        {
            return found->second->GetValue(morphName, destination);
		}
		return 0;
	}
    std::int32_t ActorManager::GetValue(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, bool destination) const
	{
		if (!a_actor)
			return 0;
		auto categories = morphNameEntry::GetSingleton().GetCategories();
		if (categories.size() <= categoryNumber)
			return 0;
        std::shared_lock sl(morphManagerLock);
        auto found = map.find(a_actor->formID);
        if (found != map.end())
        {
			auto names = morphNameEntry::GetSingleton().GetMorphNames(categories[categoryNumber]);
			if (names.size() > morphNumber) {
                return found->second->GetValue(names[morphNumber], destination);
			}
		}
		return 0;
	}

	std::vector<MorphManager::ActiveMorphSet> ActorManager::GetAllActiveMorphs(RE::Actor* a_actor) const
	{
		if (!a_actor)
			return std::vector<MorphManager::ActiveMorphSet>();

        std::shared_lock sl(morphManagerLock);
        if (const auto found = map.find(a_actor->formID); found != map.end())
		{
			return found->second->GetAllActiveMorphs();
		}
		return std::vector<MorphManager::ActiveMorphSet>();
	}
    std::vector<std::pair<lString, std::vector<MorphManager::ActiveMorphSet>>> ActorManager::GetAllMorphs(RE::Actor* a_actor, bool destination) const
    {
        std::vector<std::pair<lString, std::vector<MorphManager::ActiveMorphSet>>> result;
        if (!a_actor)
            return result;

		auto categories = morphNameEntry::GetSingleton().GetCategories();
        for (const auto& category : categories)
        {
            std::vector<MorphManager::ActiveMorphSet> morphValues;
            auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
            for (const auto& morphName : morphNames)
            {
                MorphManager::ActiveMorphSet set{
                    .morphName = morphName,
                    .value = GetValue(a_actor, morphName, destination)
                };
                morphValues.push_back(set);
            }
            result.push_back(std::make_pair(category, morphValues));
		}
        return result;
    }

	ActorManager::EventResult ActorManager::ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) 
	{
		if (!evn || evn->menuName.empty())
			return EventResult::kContinue;

		if (evn->opening)
		{
            if (IsSameString(evn->menuName.c_str(), "RaceSex Menu"))
				IsShowracemenuLoaded.store(true);
		}
		else if (!evn->opening)
		{
            if (IsSameString(evn->menuName.c_str(), "RaceSex Menu"))
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
        std::shared_lock sl(morphManagerLock);
        auto found = map.find(a_actor->formID);
        if (found == map.end())
            return;
        found->second->SetNeedUpdateFacegenMeshes();
	}
    void ActorManager::onEvent(const FaceUpdateEvent& e)
	{
        if (!e.ref)
            return;

		MorphManagerPtr mm = nullptr;
        {
            std::shared_lock sl(morphManagerLock);
            auto found = map.find(e.ref->formID);
            if (found == map.end())
                return;
            mm = found->second;
        }
        if (!mm)
            return;
        std::lock_guard lg(morphManagerLock);
        queueUpdate[e.ref->formID] = mm;
    }
    void ActorManager::onEvent(const ActorChangeHeadPartEvent& e)
    {
        if (!e.actor)
            return;
        std::shared_lock sl(morphManagerLock);
        auto found = map.find(e.actor->formID);
        if (found == map.end())
            return;
        found->second->SetNeedUpdateFacegenMeshes();
    }
}
