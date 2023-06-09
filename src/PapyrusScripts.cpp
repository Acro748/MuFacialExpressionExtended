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
        bool RegisterNewMorphNameOnType(RE::StaticFunctionTag*, RE::BSFixedString a_morphType, RE::BSFixedString a_morphName)
        {
            return MFEE::FEE.RegisterNewMorphNameOnType(a_morphType.c_str(), a_morphName.c_str());
        }
        bool RegisterNewMorphName(RE::StaticFunctionTag*, RE::BSFixedString a_morphName)
        {
            return MFEE::FEE.RegisterNewMorphName(a_morphName.c_str());
        }

        std::vector<std::string> GetFacialExpressionMorphTypes(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetFacialExpressionMorphTypes();
        };
        std::size_t GetFacialExpressionMorphTypesSize(RE::StaticFunctionTag*) {
            return MFEE::FEE.GetFacialExpressionMorphTypes().size();
        };
        std::vector<std::string> GetFacialExpressionMorphNames(RE::StaticFunctionTag*, RE::BSFixedString a_morphType) {
            return MFEE::FEE.GetFacialExpressionMorphNames(a_morphType.c_str());
        };
        std::size_t GetFacialExpressionMorphNamesSize(RE::StaticFunctionTag*, RE::BSFixedString a_morphType) {
            return MFEE::FEE.GetFacialExpressionMorphNames(a_morphType.c_str()).size();
        };
        float GetFacialExpressionValueByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName) {
            return MFEE::FEE.GetFacialExpressionValueByName(a_actor, a_morphName.c_str());
        };
        float GetFacialExpressionValueByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t a_morphNumber) {
            return MFEE::FEE.GetFacialExpressionValueByNumber(a_actor, a_morphType, a_morphNumber);
        };

        void SetFacialExpressionByName(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphName, float a_value) {
           MFEE::FEE.SetFacialExpressionByName(a_actor, a_morphName.c_str(), a_value);
        };
        void SetFacialExpressionByType(RE::StaticFunctionTag*, RE::Actor* a_actor, RE::BSFixedString a_morphType, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetFacialExpressionByType(a_actor, a_morphType.c_str(), morphNumber, a_value);
        };
        void SetFacialExpressionByNumber(RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_morphType, std::uint32_t morphNumber, float a_value) {
            MFEE::FEE.SetFacialExpressionByNumber(a_actor, a_morphType, morphNumber, a_value);
        };
        void RevertFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.RevertFacialExpression(a_actor);
        };
        void UpdateFacialExpression(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            MFEE::FEE.UpdateFacialExpression(a_actor);
        };

        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
            vm->RegisterFunction("PrintConsole", ScriptFileName, PrintConsole);
            vm->RegisterFunction("GetVersion", ScriptFileName, GetVersion);
            vm->RegisterFunction("RegisterNewMorphData", ScriptFileName, RegisterNewMorphData);
            vm->RegisterFunction("RegisterNewMorphNameOnType", ScriptFileName, RegisterNewMorphNameOnType);
            vm->RegisterFunction("RegisterNewMorphName", ScriptFileName, RegisterNewMorphName);
            vm->RegisterFunction("GetFacialExpressionMorphTypes", ScriptFileName, GetFacialExpressionMorphTypes);
            vm->RegisterFunction("GetFacialExpressionMorphTypesSize", ScriptFileName, GetFacialExpressionMorphTypesSize);
            vm->RegisterFunction("GetFacialExpressionMorphNames", ScriptFileName, GetFacialExpressionMorphNames);
            vm->RegisterFunction("GetFacialExpressionMorphNamesSize", ScriptFileName, GetFacialExpressionMorphNamesSize);
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