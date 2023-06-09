#pragma once

namespace Mus {
	class ActorManager :
		public concurrency::concurrent_unordered_map<RE::FormID, MorphManager>
	{
	public:
		ActorManager() {};
		~ActorManager() {};

		[[nodiscard]] static ActorManager& GetSingleton() {
			static ActorManager instance;
			return instance;
		}

		const std::uint32_t ActorManagerRecord = _byteswap_ulong('FEEA');
		void Save(SKSE::SerializationInterface* serde);
		void Load(SKSE::SerializationInterface* serde, std::uint32_t type);

		void SetMorph(RE::Actor* a_actor, std::string morphName, float value);
		void SetMorph(RE::Actor* a_actor, morphNameEntry::morphType type, std::uint32_t morphNumber, float value);
		void Revert(RE::Actor* a_actor);
		void Update(RE::Actor* a_actor);
	protected:

	private:

	};
}
