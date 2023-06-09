#pragma once

namespace MFEE {
	class FacialExpressionExtended :
		public IFacialExpressionExtended
	{
	public:
		virtual std::uint32_t GetVersion() { return 0; };

		virtual bool RegisterNewMorphData(std::string a_morphBasePath, std::string a_morphPath) { 
			return Mus::MorphDataBaseManager::GetSingleton().Register(a_morphBasePath, a_morphPath);
		};
		virtual bool RegisterNewMorphNameOnType(std::string a_morphType, std::string a_morphName) {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(Mus::morphNameEntry::GetSingleton().GetMorphTypeByString(a_morphType), a_morphName);
			return false;
		};
		virtual bool RegisterNewMorphName(std::string a_morphName) {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(a_morphName);
			return false;
		};

		virtual std::vector<std::string> GetFacialExpressionMorphTypes() {
			return Mus::morphNameEntry::GetSingleton().GetMorphTypes();
		};
		virtual std::vector<std::string> GetFacialExpressionMorphNames(std::string a_morphType) {
			return Mus::morphNameEntry::GetSingleton().GetNames(Mus::morphNameEntry::GetSingleton().GetMorphTypeByString(a_morphType));
		};
		virtual float GetFacialExpressionValueByName(RE::Actor* a_actor, std::string a_morphName) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphName);
		}
		virtual float GetFacialExpressionValueByNumber(RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t a_morphNumber) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, Mus::morphNameEntry::morphType(a_morphType), a_morphNumber);
		}

		virtual void SetFacialExpressionByName(RE::Actor* a_actor, std::string a_morphName, float a_value) { 
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value); 
		};
		virtual void SetFacialExpressionByType(RE::Actor* a_actor, std::string a_morphType, std::uint32_t a_morphNumber, float a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, Mus::morphNameEntry::GetSingleton().GetMorphTypeByString(a_morphType), a_morphNumber, a_value);
		};
		virtual void SetFacialExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t a_morphNumber, float a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, Mus::morphNameEntry::morphType(a_morphType), a_morphNumber, a_value);
		};

		virtual void RevertFacialExpression(RE::Actor* a_actor) {
			Mus::ActorManager::GetSingleton().Revert(a_actor);
		};
		virtual void UpdateFacialExpression(RE::Actor* a_actor) {
			Mus::ActorManager::GetSingleton().Update(a_actor);
		};
	};
	static FacialExpressionExtended FEE;
}