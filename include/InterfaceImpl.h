#pragma once

namespace MFEE {
	class FacialExpressionExtended :
		public IFacialExpressionExtended
	{
	public:
		virtual std::uint32_t GetVersion() { return 1; };

		virtual bool RegisterNewMorphData(std::string a_morphBasePath, std::string a_morphPath) { 
			return Mus::MorphDataBaseManager::GetSingleton().Register(a_morphBasePath, a_morphPath);
		};
		virtual bool RegisterNewMorphNameOnType(std::string a_morphCategory, std::string a_morphName) {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(a_morphCategory, a_morphName);
			return false;
		};
		virtual bool RegisterNewMorphName(std::string a_morphName) {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(a_morphName);
			return false;
		};

		virtual std::vector<std::string> GetFacialExpressionCategories() {
			return Mus::morphNameEntry::GetSingleton().GetCategories();
		};
		virtual std::string GetFacialExpressionCategoryByNumber(std::int32_t a_categoryNumber) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryByNumber(a_categoryNumber);
		};
		virtual std::string GetFacialExpressionCategoryByMorphName(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryByMorphName(a_morphName);
		};
		virtual std::int32_t GetFacialExpressionCategoryNumber(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryNumber(a_morphCategory);
		};
		virtual bool IsValidFacialExpressionCategory(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().IsValidCategory(a_morphCategory);
		};

		virtual std::vector<std::string> GetFacialExpressionMorphNames(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphCategory);
		};
		virtual std::vector<std::string> GetFacialExpressionMorphNamesByNumber(std::int32_t a_categoryNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_categoryNumber);
		};
		virtual std::string GetFacialExpressionMorphNameByNumber(std::string a_morphCategory, std::int32_t a_morphNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_morphCategory, a_morphNumber);
		};
		virtual std::string GetFacialExpressionMorphNameByNumbers(std::int32_t a_categoryNumber, std::int32_t a_morphNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_categoryNumber, a_morphNumber);
		};
		virtual std::int32_t GetFacialExpressionMorphNameNumber(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameNumber(a_morphName);
		};
		virtual bool IsValidFacialExpressionMorphName(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().IsValidName(a_morphName);
		};

		virtual float GetFacialExpressionValueByName(RE::Actor* a_actor, std::string a_morphName) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphName);
		}
		virtual float GetFacialExpressionValueByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphCategoryNumber, a_morphNumber);
		}

		virtual void SetFacialExpressionByName(RE::Actor* a_actor, std::string a_morphName, float a_value) { 
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value); 
		};
		virtual void SetFacialExpressionByType(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, float a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value);
		};
		virtual void SetFacialExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, float a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value);
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