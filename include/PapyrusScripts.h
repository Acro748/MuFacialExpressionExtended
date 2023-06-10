#pragma once

namespace Mus {
    namespace Papyrus {
        constexpr std::string_view ScriptFileName = "MuFacialExpressionExtended";
        void PrintConsole(RE::StaticFunctionTag*, RE::BSFixedString strArg);
        std::uint32_t GetVersion(RE::StaticFunctionTag*);

        bool RegisterNewMorphData(RE::StaticFunctionTag*, RE::BSFixedString a_morphBasePath, RE::BSFixedString a_morphPath);
        bool RegisterNewMorphNameOnType(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, RE::BSFixedString a_morphName);
        bool RegisterNewMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName);

        std::vector<std::string> GetFacialExpressionCategories(RE::StaticFunctionTag*);
        std::size_t GetFacialExpressionCategoriesSize(RE::StaticFunctionTag*);
        std::string GetFacialExpressionCategoryByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber);
        std::string GetFacialExpressionCategoryByMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName);
        std::int32_t GetFacialExpressionCategoryNumber(RE::StaticFunctionTag*, RE::BSFixedString a_category);
        bool IsValidFacialExpressionCategory(RE::StaticFunctionTag*, RE::BSFixedString a_category);

        std::vector<std::string> GetFacialExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory);
        std::size_t GetFacialExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory);
        std::vector<std::string> GetFacialExpressionMorphNamesByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber);
        std::string GetFacialExpressionMorphNameByNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, std::int32_t a_morphNumber);
        std::string GetFacialExpressionMorphNameByNumbers(RE::StaticFunctionTag*, std::int32_t a_categoryNumber, std::int32_t a_morphNumber);
        std::int32_t GetFacialExpressionMorphNameNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphName);
        bool IsValidFacialExpressionMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName);

        float GetFacialExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName);
        float GetFacialExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber);

        void SetFacialExpressionByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, float a_value);
        void SetFacialExpressionByType(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphCategory, std::uint32_t morphNumber, float a_value);
        void SetFacialExpressionByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t morphNumber, float a_value);
        void RevertFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor);
        void UpdateFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor);

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);
    }
}
