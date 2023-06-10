#include "PapyrusScripts.h"

namespace Mus {
    namespace Papyrus {
        void PrintConsole(RE::StaticFunctionTag*, RE::BSFixedString strArg)
        {
            if (strArg.empty())
                return;
            ConsolePrint("%s", strArg.c_str());
        }
        
        std::uint32_t GetVersion(RE::StaticFunctionTag*)
        {
            return MFEE::FEE.GetVersion();
        }

        bool RegisterNewMorphData(RE::StaticFunctionTag*, RE::BSFixedString a_morphBasePath, RE::BSFixedString a_morphPath)
        {
            return MFEE::FEE.RegisterNewMorphData(a_morphBasePath.c_str(), a_morphPath.c_str());
        }
        bool RegisterNewMorphNameOnType(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, RE::BSFixedString a_morphName)
        {
            return MFEE::FEE.RegisterNewMorphNameOnType(a_morphCategory.c_str(), a_morphName.c_str());
        }
        bool RegisterNewMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName)
        {
            return MFEE::FEE.RegisterNewMorphName(a_morphName.c_str());
        }

        std::vector<std::string> GetFacialExpressionCategories(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetFacialExpressionCategories();
        }
        std::size_t GetFacialExpressionCategoriesSize(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetFacialExpressionCategories().size();
        }
        std::string GetFacialExpressionCategoryByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return MFEE::FEE.GetFacialExpressionCategoryByNumber(a_categoryNumber);
        }
        std::string GetFacialExpressionCategoryByMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetFacialExpressionCategoryByMorphName(a_morphName.c_str());
        }
        std::int32_t GetFacialExpressionCategoryNumber(RE::StaticFunctionTag*, RE::BSFixedString a_category) {
            return MFEE::FEE.GetFacialExpressionCategoryNumber(a_category.c_str());
        }
        bool IsValidFacialExpressionCategory(RE::StaticFunctionTag*, RE::BSFixedString a_category) {
            return MFEE::FEE.IsValidFacialExpressionCategory(a_category.c_str());
        }

        std::vector<std::string> GetFacialExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return MFEE::FEE.GetFacialExpressionMorphNames(a_morphCategory.c_str());
        }
        std::size_t GetFacialExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return MFEE::FEE.GetFacialExpressionMorphNames(a_morphCategory.c_str()).size();
        }
        std::vector<std::string> GetFacialExpressionMorphNamesByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return MFEE::FEE.GetFacialExpressionMorphNamesByNumber(a_categoryNumber);
        }
        std::string GetFacialExpressionMorphNameByNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, std::int32_t a_morphNumber) {
            return MFEE::FEE.GetFacialExpressionMorphNameByNumber(a_morphCategory.c_str(), a_morphNumber);
        }
        std::string GetFacialExpressionMorphNameByNumbers(RE::StaticFunctionTag*, std::int32_t a_categoryNumber, std::int32_t a_morphNumber) {
            return MFEE::FEE.GetFacialExpressionMorphNameByNumbers(a_categoryNumber, a_morphNumber);
        }
        std::int32_t GetFacialExpressionMorphNameNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetFacialExpressionMorphNameNumber(a_morphName.c_str());
        }
        bool IsValidFacialExpressionMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.IsValidFacialExpressionMorphName(a_morphName.c_str());
        }

        float GetFacialExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetFacialExpressionValueByName(a_actor, a_morphName.c_str());
        }
        float GetFacialExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber) {
            return MFEE::FEE.GetFacialExpressionValueByNumber(a_actor, a_morphCategory, a_morphNumber);
        }

        void SetFacialExpressionByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, float a_value) {
           MFEE::FEE.SetFacialExpressionByName(a_actor, a_morphName.c_str(), a_value);
        }
        void SetFacialExpressionByType(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphCategory, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetFacialExpressionByType(a_actor, a_morphCategory.c_str(), morphNumber, a_value);
        }
        void SetFacialExpressionByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetFacialExpressionByNumber(a_actor, a_morphCategory, morphNumber, a_value);
        }
        void RevertFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.RevertFacialExpression(a_actor);
        }
        void UpdateFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.UpdateFacialExpression(a_actor);
        }

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
            vm->RegisterFunction("PrintConsole", ScriptFileName, PrintConsole);
            vm->RegisterFunction("GetVersion", ScriptFileName, GetVersion);

            vm->RegisterFunction("RegisterNewMorphData", ScriptFileName, RegisterNewMorphData);
            vm->RegisterFunction("RegisterNewMorphNameOnType", ScriptFileName, RegisterNewMorphNameOnType);
            vm->RegisterFunction("RegisterNewMorphName", ScriptFileName, RegisterNewMorphName);

            vm->RegisterFunction("GetFacialExpressionCategories", ScriptFileName, GetFacialExpressionCategories);
            vm->RegisterFunction("GetFacialExpressionCategoriesSize", ScriptFileName, GetFacialExpressionCategoriesSize);
            vm->RegisterFunction("GetFacialExpressionMorphNamesByNumber", ScriptFileName, GetFacialExpressionMorphNamesByNumber);
            vm->RegisterFunction("GetFacialExpressionCategoryByMorphName", ScriptFileName, GetFacialExpressionCategoryByMorphName);
            vm->RegisterFunction("GetFacialExpressionCategoryNumber", ScriptFileName, GetFacialExpressionCategoryNumber);
            vm->RegisterFunction("IsValidFacialExpressionCategory", ScriptFileName, IsValidFacialExpressionCategory);

            vm->RegisterFunction("GetFacialExpressionMorphNames", ScriptFileName, GetFacialExpressionMorphNames);
            vm->RegisterFunction("GetFacialExpressionMorphNamesSize", ScriptFileName, GetFacialExpressionMorphNamesSize);
            vm->RegisterFunction("GetFacialExpressionMorphNamesByNumber", ScriptFileName, GetFacialExpressionMorphNamesByNumber);
            vm->RegisterFunction("GetFacialExpressionMorphNameByNumber", ScriptFileName, GetFacialExpressionMorphNameByNumber);
            vm->RegisterFunction("GetFacialExpressionMorphNameByNumbers", ScriptFileName, GetFacialExpressionMorphNameByNumbers);
            vm->RegisterFunction("GetFacialExpressionMorphNameNumber", ScriptFileName, GetFacialExpressionMorphNameNumber);
            vm->RegisterFunction("IsValidFacialExpressionMorphName", ScriptFileName, IsValidFacialExpressionMorphName);

            vm->RegisterFunction("GetFacialExpressionValueByName", ScriptFileName, GetFacialExpressionValueByName);
            vm->RegisterFunction("GetFacialExpressionValueByNumber", ScriptFileName, GetFacialExpressionValueByNumber);

            vm->RegisterFunction("SetFacialExpressionByName", ScriptFileName, SetFacialExpressionByName);
            vm->RegisterFunction("SetFacialExpressionByType", ScriptFileName, SetFacialExpressionByType);
            vm->RegisterFunction("SetFacialExpressionByNumber", ScriptFileName, SetFacialExpressionByNumber);
            vm->RegisterFunction("RevertFacialExpression", ScriptFileName, RevertFacialExpression);
            vm->RegisterFunction("UpdateFacialExpression", ScriptFileName, UpdateFacialExpression);
            return true;
        }
    }
}