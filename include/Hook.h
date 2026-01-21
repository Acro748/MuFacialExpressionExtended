#pragma once

namespace Mus {
	template <class Event = void>
	class IEventListener
	{
	public:
		virtual void onEvent(const Event&) = 0;
	};

	template <class Event = void>
	class IEventDispatcher
	{
	public:

		virtual ~IEventDispatcher()
		{
		}

		virtual void addListener(IEventListener<Event>*) = 0;
		virtual void removeListener(IEventListener<Event>*) = 0;
		virtual void dispatch(const Event&) = 0;
	};

	template <class Event = void>
	class EventDispatcherImpl : public IEventDispatcher<Event>
	{
	public:
		EventDispatcherImpl() {}
		~EventDispatcherImpl() {}

		void addListener(IEventListener<Event>* listener) override {
			m_listeners.insert(listener);
			m_cacheDirt = true;
		};
		void removeListener(IEventListener<Event>* listener) override {
			m_listeners.erase(listener);
			m_cacheDirt = true;
		};
		void dispatch(const Event& event) override {
			if (m_cacheDirt)
			{
                listenerLock.lock();
				m_caches.clear();
				for (auto& i : m_listeners)
					m_caches.emplace_back(i);
				m_cacheDirt = false;
                listenerLock.unlock();
			}

			for (auto i : m_caches)
				i->onEvent(event);
		};

	private:
		std::unordered_set<IEventListener<Event>*> m_listeners;
		std::vector<IEventListener<Event>*> m_caches;
		bool m_cacheDirt = false;
        std::mutex listenerLock;
	};

	struct FrameEvent
	{
		bool gamePaused;
	};
    
	struct FacegenNiNodeEvent {
        RE::NiNode* root;
        RE::BSFaceGenNiNode* facegenNiNode;
        bool skinned;
    };

	struct FaceUpdateEvent
	{
        RE::TESObjectREFR* ref;
    };

    struct ActorChangeHeadPartEvent {
        RE::Actor* actor;
        RE::BGSHeadPart* oldHeadPart;
        RE::BGSHeadPart* newHeadPart;
    };

	extern EventDispatcherImpl<FrameEvent> g_frameEventDispatcher;
    extern EventDispatcherImpl<FacegenNiNodeEvent> g_facegenNiNodeEventDispatcher;
    extern EventDispatcherImpl<FaceUpdateEvent> g_faceUpdateEventDispatcher;
    extern EventDispatcherImpl<ActorChangeHeadPartEvent> g_actorChangeHeadPartEventDispatcher;

	void hook();
}
