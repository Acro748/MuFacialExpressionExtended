#pragma once

namespace Mus {
	typedef std::shared_ptr<MorphManager> MorphManagerPtr;
	class ActorManager :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public IEventListener<FrameEvent>,
		public IEventListener<FacegenNiNodeEvent>,
		public IEventListener<FaceUpdateEvent>,
		public IEventListener<ActorChangeHeadPartEvent> 
	{
        std::unordered_map<RE::FormID, MorphManagerPtr> map;
        mutable std::shared_mutex morphManagerLock;
	public:
		ActorManager() {};
		~ActorManager() {};

		[[nodiscard]] static ActorManager& GetSingleton() {
			static ActorManager instance;
			return instance;
		}

		void Init();

		const std::uint32_t ActorManagerRecord = _byteswap_ulong('FEEA');
		void Save(SKSE::SerializationInterface* serde);
		void Load(SKSE::SerializationInterface* serde, std::uint32_t type);

		void SetMorph(RE::Actor* a_actor, const lString& morphName, std::int32_t value, std::int32_t lerpTime = -1);
        void SetMorph(RE::Actor* a_actor, const lString& category, std::uint32_t morphNumber, std::int32_t value, std::int32_t lerpTime = -1);
        void SetMorph(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, std::int32_t value, std::int32_t lerpTime = -1);
        void Revert(RE::Actor* a_actor = nullptr, const lString& category = "");
		void Update(RE::Actor* a_actor = nullptr);
		void Initial(RE::Actor* a_actor = nullptr);

		std::int32_t GetValue(RE::Actor* a_actor, const lString& morphName, bool destination = false) const;
        std::int32_t GetValue(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, bool destination = false) const;

		std::vector<MorphManager::ActiveMorphSet> GetAllActiveMorphs(RE::Actor* a_actor) const;
        std::vector<std::pair<lString, std::vector<MorphManager::ActiveMorphSet>>> GetAllMorphs(RE::Actor* a_actor, bool destination = false) const;
	protected:
		using EventResult = RE::BSEventNotifyControl;
		EventResult ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;
		void onEvent(const FrameEvent& e) override;
        void onEvent(const FacegenNiNodeEvent& e) override;
        void onEvent(const FaceUpdateEvent& e) override;
        void onEvent(const ActorChangeHeadPartEvent& e) override;
	private:
        std::unordered_map<RE::FormID, MorphManagerPtr> queueUpdate;
		bool isPaused = false;
	};
}
