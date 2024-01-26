#pragma once

namespace MFEE {
	class FacialExpressionExtended :
		public IFacialExpressionExtended
	{
	public:
		virtual std::uint32_t GetVersion() { return 3; };

		virtual bool RegisterNewMorphData(std::string a_morphBasePath, std::string a_morphPath) { 
			return Mus::MorphDataBaseManager::GetSingleton().Register(a_morphBasePath, a_morphPath);
		};
		virtual bool RegisterNewMorphNameOnCategory(std::string a_morphCategory, std::string a_morphName) {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(a_morphCategory, a_morphName);
			return false;
		};

		virtual std::vector<std::string> GetExpressionCategories() {
			return Mus::morphNameEntry::GetSingleton().GetCategories();
		};
		virtual std::string GetExpressionCategoryByNumber(std::int32_t a_categoryNumber) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryByNumber(a_categoryNumber);
		};
		virtual std::string GetExpressionCategoryByMorphName(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryByMorphName(a_morphName);
		};
		virtual std::int32_t GetExpressionCategoryNumber(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().GetCategoryNumber(a_morphCategory);
		};
		virtual bool IsValidExpressionCategory(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().IsValidCategory(a_morphCategory);
		};

		virtual std::vector<std::string> GetExpressionMorphNames(std::string a_morphCategory) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphCategory);
		};
		virtual std::vector<std::string> GetExpressionMorphNamesByNumber(std::int32_t a_categoryNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_categoryNumber);
		};
		virtual std::string GetExpressionMorphNameByNumber(std::string a_morphCategory, std::int32_t a_morphNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_morphCategory, a_morphNumber);
		};
		virtual std::string GetExpressionMorphNameByNumbers(std::int32_t a_categoryNumber, std::int32_t a_morphNumber) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_categoryNumber, a_morphNumber);
		};
		virtual std::int32_t GetExpressionMorphNameNumber(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().GetMorphNameNumber(a_morphName);
		};
		virtual bool IsValidExpressionMorphName(std::string a_morphName) {
			return Mus::morphNameEntry::GetSingleton().IsValidName(a_morphName);
		};

		virtual int32_t GetExpressionValueByName(RE::Actor* a_actor, std::string a_morphName) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphName);
		}
		virtual int32_t GetExpressionValueByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphCategoryNumber, a_morphNumber);
		}

		virtual void SetExpressionByName(RE::Actor* a_actor, std::string a_morphName, int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value); 
		};
		virtual void SetExpressionByCategory(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value);
		};
		virtual void SetExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value);
		};

		virtual void RevertExpression(RE::Actor* a_actor, std::string a_morphCategory = "") {
			Mus::ActorManager::GetSingleton().Revert(a_actor, a_morphCategory);
		};
		virtual void UpdateExpression(RE::Actor* a_actor) {
			Mus::ActorManager::GetSingleton().Update(a_actor);
		};
		virtual void InitialMorphData(RE::Actor* a_actor) {
			Mus::ActorManager::GetSingleton().Initial(a_actor);
		};
	};
	static FacialExpressionExtended FEE;
}