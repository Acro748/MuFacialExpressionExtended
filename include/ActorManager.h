#pragma once

namespace Mus {
	typedef std::shared_ptr<MorphManager> MorphManagerPtr;
	class ActorManager :
		public concurrency::concurrent_unordered_map<RE::FormID, MorphManagerPtr>
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
		void SetMorph(RE::Actor* a_actor, std::string category, std::uint32_t morphNumber, float value);
		void SetMorph(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber, float value);
		void Revert(RE::Actor* a_actor, std::string category = "");
		void Update(RE::Actor* a_actor, std::string category = "");

		float GetValue(RE::Actor* a_actor, std::string morphName);
		float GetValue(RE::Actor* a_actor, std::uint32_t categoryNumber, std::uint32_t morphNumber);
	protected:

	private:

	};
}
