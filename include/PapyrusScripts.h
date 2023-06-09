#pragma once

namespace Mus {
    namespace Papyrus {
        constexpr std::string_view ScriptFileName = "MuFacialExpressionExtended";
        void PrintConsole(RE::StaticFunctionTag*, RE::BSFixedString strArg);
        std::uint32_t GetVersion(RE::StaticFunctionTag*);

        bool RegisterNewMorphData(RE::StaticFunctionTag*, RE::BSFixedString a_morphBasePath, RE::BSFixedString a_morphPath);
        bool RegisterNewMorphNameOnType(RE::StaticFunctionTag*, RE::BSFixedString a_morphType, RE::BSFixedString a_morphName);
        bool RegisterNewMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName);

        std::vector<std::string> GetFacialExpressionMorphTypes(RE::StaticFunctionTag*);
        std::size_t GetFacialExpressionMorphTypesSize(RE::StaticFunctionTag*);
        std::vector<std::string> GetFacialExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphType);
        std::size_t GetFacialExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphType);
        float GetFacialExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName);
        float GetFacialExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t a_morphNumber);

        void SetFacialExpressionByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, float a_value);
        void SetFacialExpressionByType(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphType, std::uint32_t morphNumber, float a_value);
        void SetFacialExpressionByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t morphNumber, float a_value);
        void RevertFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor);
        void UpdateFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor);

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);
    }
}