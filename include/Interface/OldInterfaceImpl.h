#pragma once

namespace MFEEOLD {
	class FacialExpressionExtended :
		public IFacialExpressionExtended
	{
	public:
		virtual std::uint32_t GetVersion() { return 5; };

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
			return Mus::morphNameEntry::GetSingleton().GetMorphNamesBasic(a_morphCategory);
		};
		virtual std::vector<std::string> GetExpressionMorphNamesByNumber(std::int32_t a_categoryNumber) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNamesBasic(a_categoryNumber);
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

		virtual void SetExpressionByName(RE::Actor* a_actor, std::string a_morphName, std::int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value, 0);
		};
		virtual void SetExpressionByCategory(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, 0);
		};
		virtual void SetExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, 0);
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

		virtual std::vector<IActiveMorphSet> GetAllActiveMorphs(RE::Actor* a_actor) {
			auto allActiveMorphs = Mus::ActorManager::GetSingleton().GetAllActiveMorphs(a_actor);
			std::vector<IActiveMorphSet> result;
			for (auto& m : allActiveMorphs)
			{
				IActiveMorphSet newIActiveMorphSet = { m.morphName, m.value };
				result.push_back(newIActiveMorphSet);
			}
			return result;
		};

		virtual void SetExpressionByNameEx(RE::Actor* a_actor, std::string a_morphName, std::int32_t a_value, std::int32_t a_lerpTime) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value, a_lerpTime);
		};
		virtual void SetExpressionByCategoryEx(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, a_lerpTime);
		};
		virtual void SetExpressionByNumberEx(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, a_lerpTime);
		};
	};
	static FacialExpressionExtended FEE;
}