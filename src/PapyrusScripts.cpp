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

        std::vector<std::string> GetExpressionCategories(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetExpressionCategories();
        }
        std::size_t GetExpressionCategoriesSize(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetExpressionCategories().size();
        }
        std::string GetExpressionCategoryByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return MFEE::FEE.GetExpressionCategoryByNumber(a_categoryNumber);
        }
        std::string GetExpressionCategoryByMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetExpressionCategoryByMorphName(a_morphName.c_str());
        }
        std::int32_t GetExpressionCategoryNumber(RE::StaticFunctionTag*, RE::BSFixedString a_category) {
            return MFEE::FEE.GetExpressionCategoryNumber(a_category.c_str());
        }
        bool IsValidExpressionCategory(RE::StaticFunctionTag*, RE::BSFixedString a_category) {
            return MFEE::FEE.IsValidExpressionCategory(a_category.c_str());
        }

        std::vector<std::string> GetExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return MFEE::FEE.GetExpressionMorphNames(a_morphCategory.c_str());
        }
        std::size_t GetExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return MFEE::FEE.GetExpressionMorphNames(a_morphCategory.c_str()).size();
        }
        std::vector<std::string> GetExpressionMorphNamesByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return MFEE::FEE.GetExpressionMorphNamesByNumber(a_categoryNumber);
        }
        std::string GetExpressionMorphNameByNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, std::int32_t a_morphNumber) {
            return MFEE::FEE.GetExpressionMorphNameByNumber(a_morphCategory.c_str(), a_morphNumber);
        }
        std::string GetExpressionMorphNameByNumbers(RE::StaticFunctionTag*, std::int32_t a_categoryNumber, std::int32_t a_morphNumber) {
            return MFEE::FEE.GetExpressionMorphNameByNumbers(a_categoryNumber, a_morphNumber);
        }
        std::int32_t GetExpressionMorphNameNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetExpressionMorphNameNumber(a_morphName.c_str());
        }
        bool IsValidExpressionMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return MFEE::FEE.IsValidExpressionMorphName(a_morphName.c_str());
        }

        float GetExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetExpressionValueByName(a_actor, a_morphName.c_str());
        }
        float GetExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber) {
            return MFEE::FEE.GetExpressionValueByNumber(a_actor, a_morphCategory, a_morphNumber);
        }

        void SetExpressionByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, float a_value) {
           MFEE::FEE.SetExpressionByName(a_actor, a_morphName.c_str(), a_value);
        }
        void SetExpressionByType(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphCategory, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetExpressionByType(a_actor, a_morphCategory.c_str(), morphNumber, a_value);
        }
        void SetExpressionByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetExpressionByNumber(a_actor, a_morphCategory, morphNumber, a_value);
        }
        void RevertExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.RevertExpression(a_actor);
        }
        void UpdateExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.UpdateExpression(a_actor);
        }

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
            vm->RegisterFunction("PrintConsole", ScriptFileName, PrintConsole);
            vm->RegisterFunction("GetVersion", ScriptFileName, GetVersion);

            vm->RegisterFunction("RegisterNewMorphData", ScriptFileName, RegisterNewMorphData);
            vm->RegisterFunction("RegisterNewMorphNameOnType", ScriptFileName, RegisterNewMorphNameOnType);
            vm->RegisterFunction("RegisterNewMorphName", ScriptFileName, RegisterNewMorphName);

            vm->RegisterFunction("GetExpressionCategories", ScriptFileName, GetExpressionCategories);
            vm->RegisterFunction("GetExpressionCategoriesSize", ScriptFileName, GetExpressionCategoriesSize);
            vm->RegisterFunction("GetExpressionMorphNamesByNumber", ScriptFileName, GetExpressionMorphNamesByNumber);
            vm->RegisterFunction("GetExpressionCategoryByMorphName", ScriptFileName, GetExpressionCategoryByMorphName);
            vm->RegisterFunction("GetExpressionCategoryNumber", ScriptFileName, GetExpressionCategoryNumber);
            vm->RegisterFunction("IsValidExpressionCategory", ScriptFileName, IsValidExpressionCategory);

            vm->RegisterFunction("GetExpressionMorphNames", ScriptFileName, GetExpressionMorphNames);
            vm->RegisterFunction("GetExpressionMorphNamesSize", ScriptFileName, GetExpressionMorphNamesSize);
            vm->RegisterFunction("GetExpressionMorphNamesByNumber", ScriptFileName, GetExpressionMorphNamesByNumber);
            vm->RegisterFunction("GetExpressionMorphNameByNumber", ScriptFileName, GetExpressionMorphNameByNumber);
            vm->RegisterFunction("GetExpressionMorphNameByNumbers", ScriptFileName, GetExpressionMorphNameByNumbers);
            vm->RegisterFunction("GetExpressionMorphNameNumber", ScriptFileName, GetExpressionMorphNameNumber);
            vm->RegisterFunction("IsValidExpressionMorphName", ScriptFileName, IsValidExpressionMorphName);

            vm->RegisterFunction("GetExpressionValueByName", ScriptFileName, GetExpressionValueByName);
            vm->RegisterFunction("GetExpressionValueByNumber", ScriptFileName, GetExpressionValueByNumber);

            vm->RegisterFunction("SetExpressionByName", ScriptFileName, SetExpressionByName);
            vm->RegisterFunction("SetExpressionByType", ScriptFileName, SetExpressionByType);
            vm->RegisterFunction("SetExpressionByNumber", ScriptFileName, SetExpressionByNumber);
            vm->RegisterFunction("RevertExpression", ScriptFileName, RevertExpression);
            vm->RegisterFunction("UpdateExpression", ScriptFileName, UpdateExpression);
            return true;
        }
    }
}