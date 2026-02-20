#pragma once

namespace MFEE {
	class FacialExpressionExtended :
		public IFacialExpressionExtended
	{
	public:
        virtual std::uint32_t GetVersion() override { return 7; };

		virtual bool RegisterNewMorphData(const char* a_morphBasePath, const char* a_morphPath) override {
			return Mus::MorphDataBaseManager::GetSingleton().Register(a_morphBasePath, a_morphPath);
		};
		virtual bool RegisterNewMorphNameOnCategory(const char* a_morphCategory, const char* a_morphName) override {
			if (Mus::Config::GetSingleton().GetCustomMode())
				return Mus::morphNameEntry::GetSingleton().Register(a_morphCategory, a_morphName);
			return false;
		};

		virtual std::uint32_t GetExpressionCategorySize() override {
			return Mus::morphNameEntry::GetSingleton().GetCategories().size();
		};
		virtual void GetExpressionCategory(std::int32_t a_categoryNumber, IString& result) override {
			result.String(Mus::morphNameEntry::GetSingleton().GetCategoryByNumber(a_categoryNumber).c_str());
		};

		virtual void GetExpressionCategory(const char* a_morphName, IString& result) override {
			result.String(Mus::morphNameEntry::GetSingleton().GetCategoryByMorphName(a_morphName).c_str());
		};
		virtual std::int32_t GetExpressionCategoryNumber(const char* a_morphCategory) override {
			return Mus::morphNameEntry::GetSingleton().GetCategoryNumber(a_morphCategory);
		};
		virtual bool IsValidExpressionCategory(const char* a_morphCategory) override {
			return Mus::morphNameEntry::GetSingleton().IsValidCategory(a_morphCategory);
		};

		virtual std::uint32_t GetExpressionMorphSize(const char* a_morphCategory) override {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphCategory).size();
		};
		virtual std::uint32_t GetExpressionMorphSize(std::int32_t a_categoryNumber) override {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_categoryNumber).size();
		};
		virtual void GetExpressionMorph(const char* a_morphCategory, std::int32_t a_morphNumber, IString& result) override {
			result.String(Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_morphCategory, a_morphNumber).c_str());
		};
		virtual void GetExpressionMorph(std::int32_t a_categoryNumber, std::int32_t a_morphNumber, IString& result) override {
			result.String(Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_categoryNumber, a_morphNumber).c_str());
		};

		virtual std::int32_t GetExpressionMorphNumber(const char* a_morphName) override {
			return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphName).size();
		};
		virtual bool IsValidExpressionMorph(const char* a_morphName) override {
			return Mus::morphNameEntry::GetSingleton().IsValidName(a_morphName);
		};

		virtual std::int32_t GetExpressionValue(RE::Actor* a_actor, const char* a_morphName) override {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphName);
		};
		virtual std::int32_t GetExpressionValue(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) override {
			return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphCategoryNumber, a_morphNumber);
		};

		virtual void SetExpression(RE::Actor* a_actor, const char* a_morphName, std::int32_t a_value, std::int32_t a_lerpTime) override {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName, a_value, a_lerpTime);
		};
		virtual void SetExpression(RE::Actor* a_actor, const char* a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) override {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, a_lerpTime);
		};
		virtual void SetExpression(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) override {
			Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory, a_morphNumber, a_value, a_lerpTime);
		};
		virtual void RevertExpression(RE::Actor* a_actor, const char* a_morphCategory) override {
			Mus::ActorManager::GetSingleton().Revert(a_actor, a_morphCategory);
		};
		virtual void UpdateExpression(RE::Actor* a_actor) override {
			Mus::ActorManager::GetSingleton().Update(a_actor);
		};
		virtual void InitialMorphData(RE::Actor* a_actor) override {
			Mus::ActorManager::GetSingleton().Initial(a_actor);
		};

		virtual std::uint32_t GetActiveMorphSize(RE::Actor* a_actor) override {
			return Mus::ActorManager::GetSingleton().GetAllActiveMorphs(a_actor).size();
		};
		virtual void GetActiveMorphName(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber, IString& result) override {
			result.String(Mus::ActorManager::GetSingleton().GetAllActiveMorphs(a_actor)[a_activeMorphNumber].morphName.c_str());
		};
		virtual void GetActiveMorphCategory(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber, IString& result) override {
			result.String(Mus::morphNameEntry::GetSingleton().GetCategoryByMorphName(Mus::ActorManager::GetSingleton().GetAllActiveMorphs(a_actor)[a_activeMorphNumber].morphName).c_str());
		};
		virtual std::int32_t GetActiveMorphValue(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber) override {
			return Mus::ActorManager::GetSingleton().GetAllActiveMorphs(a_actor)[a_activeMorphNumber].value;
		};

		virtual void GetAllExpressionList(IContainer& a_container) override {
            const auto categories = Mus::morphNameEntry::GetSingleton().GetCategories();
			for (const auto& category : categories) {
				const auto morphNames = Mus::morphNameEntry::GetSingleton().GetMorphNames(category);
				for (const auto& morphName : morphNames) {
					a_container.Container(category.c_str(), morphName.c_str(), 0);
				}
            }
        };
        virtual void GetAllActiveMorph(RE::Actor* a_actor, IContainer& a_container, bool a_destination) override {
            auto expressions = Mus::ActorManager::GetSingleton().GetAllMorphs(a_actor, a_destination);
            for (const auto& category : expressions) {
                for (const auto& morph : category.second) {
                    if (morph.value != 0)
						a_container.Container(category.first.c_str(), morph.morphName.c_str(), morph.value);
                }
            }
        };
        virtual void GetAllMorph(RE::Actor* a_actor, IContainer& a_container, bool a_destination) override {
            auto expressions = Mus::ActorManager::GetSingleton().GetAllMorphs(a_actor, a_destination);
            for (const auto& category : expressions) {
                for (const auto& morph : category.second) {
                    a_container.Container(category.first.c_str(), morph.morphName.c_str(), morph.value);
                }
            }
        };
	};
	static FacialExpressionExtended FEE;
}