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
            return 5;
        }

        bool RegisterNewMorphData(RE::StaticFunctionTag*, RE::BSFixedString a_morphBasePath, RE::BSFixedString a_morphPath) {
            return Mus::MorphDataBaseManager::GetSingleton().Register(a_morphBasePath.c_str(), a_morphPath.c_str());
        }
        bool RegisterNewMorphNameOnCategory(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, RE::BSFixedString a_morphName) {
            if (Mus::Config::GetSingleton().GetCustomMode())
                return Mus::morphNameEntry::GetSingleton().Register(a_morphCategory.c_str(), a_morphName.c_str());
            return false;
        }

        std::vector<std::string> GetExpressionCategories(RE::StaticFunctionTag*) {
            return Mus::morphNameEntry::GetSingleton().GetCategories();
        }
        std::size_t GetExpressionCategoriesSize(RE::StaticFunctionTag*) {
            return Mus::morphNameEntry::GetSingleton().GetCategories().size();
        }
        std::string GetExpressionCategoryByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return Mus::morphNameEntry::GetSingleton().GetCategoryByNumber(a_categoryNumber);
        }
        std::string GetExpressionCategoryByMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return Mus::morphNameEntry::GetSingleton().GetCategoryByMorphName(a_morphName.c_str());
        }
        std::int32_t GetExpressionCategoryNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return Mus::morphNameEntry::GetSingleton().GetCategoryNumber(a_morphCategory.c_str());
        }
        bool IsValidExpressionCategory(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return Mus::morphNameEntry::GetSingleton().IsValidCategory(a_morphCategory.c_str());
        }

        std::vector<std::string> GetExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphCategory.c_str());
        }
        std::size_t GetExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_morphCategory.c_str()).size();
        }
        std::vector<std::string> GetExpressionMorphNamesByNumber(RE::StaticFunctionTag*, std::int32_t a_categoryNumber) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNames(a_categoryNumber);
        }
        std::string GetExpressionMorphNameByNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphCategory, std::int32_t a_morphNumber) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_morphCategory.c_str(), a_morphNumber);
        }
        std::string GetExpressionMorphNameByNumbers(RE::StaticFunctionTag*, std::int32_t a_categoryNumber, std::int32_t a_morphNumber) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNameByNumber(a_categoryNumber, a_morphNumber);
        }
        std::int32_t GetExpressionMorphNameNumber(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return Mus::morphNameEntry::GetSingleton().GetMorphNameNumber(a_morphName.c_str());
        }
        bool IsValidExpressionMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName) {
            return Mus::morphNameEntry::GetSingleton().IsValidName(a_morphName.c_str());
        }

        std::int32_t GetExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName) {
            return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphName.c_str());
        }
        std::int32_t GetExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber) {
            return Mus::ActorManager::GetSingleton().GetValue(a_actor, a_morphCategory, a_morphNumber);
        }

        void SetExpressionByNameEx(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, std::int32_t a_value, std::int32_t a_lerpTime) {
            Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphName.c_str(), a_value, a_lerpTime);
        }
        void SetExpressionByCategoryEx(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) {
            Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_morphCategory.c_str(), a_morphNumber, a_value, a_lerpTime);
        }
        void SetExpressionByNumberEx(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_categoryNumber, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) {
            Mus::ActorManager::GetSingleton().SetMorph(a_actor, a_categoryNumber, a_morphNumber, a_value, a_lerpTime);
        }
        void RevertExpression(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphCategory) {
            Mus::ActorManager::GetSingleton().Revert(a_actor, a_morphCategory.c_str());
        }
        void UpdateExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            Mus::ActorManager::GetSingleton().Update(a_actor);
        }
        void InitialMorphData(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            Mus::ActorManager::GetSingleton().Initial(a_actor);
        }

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
            vm->RegisterFunction("PrintConsole", ScriptFileName, PrintConsole);
            vm->RegisterFunction("GetVersion", ScriptFileName, GetVersion);

            vm->RegisterFunction("RegisterNewMorphData", ScriptFileName, RegisterNewMorphData);
            vm->RegisterFunction("RegisterNewMorphNameOnCategory", ScriptFileName, RegisterNewMorphNameOnCategory);

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

            vm->RegisterFunction("SetExpressionByNameEx", ScriptFileName, SetExpressionByNameEx);
            vm->RegisterFunction("SetExpressionByCategoryEx", ScriptFileName, SetExpressionByCategoryEx);
            vm->RegisterFunction("SetExpressionByNumberEx", ScriptFileName, SetExpressionByNumberEx);
            vm->RegisterFunction("RevertExpression", ScriptFileName, RevertExpression);
            vm->RegisterFunction("UpdateExpression", ScriptFileName, UpdateExpression);
            vm->RegisterFunction("InitialMorphData", ScriptFileName, InitialMorphData);
            return true;
        }
    }
}