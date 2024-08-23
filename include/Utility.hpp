#pragma once

namespace Mus {
    inline std::string GetRuntimeDataDirectory()
    {
        return "Data\\";
    }

    inline std::string GetRuntimeMeshesDirectory()
    {
        return "Data\\Meshes\\";
    }

    inline std::string GetRuntimeTexturesDirectory()
    {
        return "Data\\Textures\\";
    }

    inline std::string GetRuntimeSKSEDirectory()
    {
        return "Data\\SKSE\\Plugins\\";
    }

    inline RE::TESForm* GetFormByID(RE::FormID id, std::string_view modname = "")
    {
        if (!modname.empty())
        {
            RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
            if (!DataHandler)
                return nullptr;

            id = DataHandler->LookupFormID(id, modname);
        }
        RE::TESForm* result = RE::TESForm::LookupByID(id);
        return result ? result : nullptr;
    }

    template <typename T>
    inline T GetFormByID(RE::FormID id, std::string_view modname = "")
    {
        return skyrim_cast<T>(GetFormByID(id, modname));
    }

    inline bool IsThereTheESP(std::string_view modname)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return false;
        auto esp = DataHandler->LookupLoadedModByName(modname);
        return esp ? true : false;
    }

    inline RE::FormID GetIndexByPluginName(std::string_view modname)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return 0;
        auto esp = DataHandler->LookupLoadedModByName(modname);
        if (!esp)
            return 0;
        std::uint32_t index = esp->GetPartialIndex();
        return !esp->IsLight() ? index << 24 : index << 16;
    }

    inline std::uint32_t GetActorBaseFormID(RE::Actor* actor)
    {
        if (!actor)
            return 0;
        RE::TESNPC* actorBase = actor->GetActorBase();
        return actorBase ? actorBase->formID : 0;
    }

    inline std::uint8_t GetModIndex(RE::FormID id)
    {
        return (id >> 24);
    }

    inline std::uint16_t GetLightModIndex(RE::FormID id)
    {
        return ((id - (0xFE << 24)) >> 16);
    }

    inline bool IsLightMod(RE::FormID id)
    {
        return REL::Module::IsVR() && !RE::TESDataHandler::GetSingleton()->VRcompiledFileCollection ? false : ((id >> 24) == 0xFE);
    }

    inline std::string_view GetModNameByIndex(std::uint8_t index)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return "NULL";
        const RE::TESFile* esp = DataHandler->LookupLoadedModByIndex(index);
        if (!esp)
            return "NULL";
        return DataHandler->LookupLoadedModByIndex(index)->GetFilename();
    }

    inline std::string_view GetLightModNameByIndex(std::uint16_t index)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return "NULL";
        const RE::TESFile* esp = DataHandler->LookupLoadedLightModByIndex(index);
        if (!esp)
            return "NULL";
        return DataHandler->LookupLoadedLightModByIndex(index)->GetFilename();
    }

    inline std::string_view GetModNameByID(RE::FormID id) //if modindex is 0xFF then it'll return "Template"
    {
        //if (REL::Module::IsVR()) //why doesn't work on VR?
        //    return "VR";

        if ((id >> 24) == 0xFF)
            return "Template";

        try {
            if (!IsLightMod(id))
                return GetModNameByIndex(GetModIndex(id));
            else
                return GetLightModNameByIndex(GetLightModIndex(id));
        }
        catch (...) {
            return "ERROR";
        }
    }

    inline std::string_view GetModNameByForm(RE::TESForm* form)
    {
        if (!form)
            return "NULL";
        RE::TESFile* file = form->GetFile();
        return file ? file->GetFilename() : "Template";
    }

    inline std::string spaces(int n) {
        auto s = std::string(n, ' ');
        return s;
    }

    inline bool isPlayer(RE::FormID id) { return (id == 0x14 || id == 0x7); };

    inline RE::SEX GetPlayerSex() {
        RE::SEX sex = RE::SEX::kNone;
        auto Player = RE::PlayerCharacter::GetSingleton();
        if (!Player)
            return sex;
        auto PlayerBase = Player->GetActorBase();
        if (!PlayerBase)
            return sex;
        sex = PlayerBase->GetSex();
        return sex;
    }

    inline std::string lowLetter(std::string Letter)
    {
        for (size_t i = 0; i < Letter.size(); i++) {
            Letter[i] = tolower(Letter[i]);
        }
        return Letter;
    }

    inline std::string fixPath(std::string Letter)
    {
        Letter = lowLetter(Letter);
        Letter = std::regex_replace(Letter, std::regex("/"), "\\");
        return Letter;
    }

    inline bool IsContainString(std::string base, std::string find)
    {
        base = lowLetter(base);
        find = lowLetter(find);

        return (base.find(find) != std::string::npos);
    }

    inline bool IsSameString(std::string str1, std::string str2)
    {
        str1 = lowLetter(str1);
        str2 = lowLetter(str2);

        return (str1 == str2);
    }

    inline bool IsEqual(float a_value, float b_value)
    {
        return (a_value - b_value) >= -0.0001f && (a_value - b_value) <= 0.0001f;
    }
    inline bool IsEqual(RE::NiPoint3 a_value, RE::NiPoint3 b_value)
    {
        return IsEqual(a_value.x, b_value.x) && IsEqual(a_value.y, b_value.y) && IsEqual(a_value.z, b_value.z);
    }
    inline bool IsEqual(RE::NiMatrix3 a_value, RE::NiMatrix3 b_value)
    {
        return IsEqual(nif::GetEulerAngles(a_value), nif::GetEulerAngles(b_value));
    }

    inline bool IsVirtual(RE::TESFile* file)
    {
        return file ? true : false;
    }

    inline float Round(float a_value, std::int8_t num = 4)
    {
        return round(a_value * pow(10, num - 1)) * pow(0.1, num - 1);
    }
    inline double Round(double a_value, std::int8_t num = 4)
    {
        return round(a_value * pow(10, num - 1)) * pow(0.1, num - 1);
    }
    inline RE::NiPoint3 Round(RE::NiPoint3 a_value, std::int8_t num = 4)
    {
        return RE::NiPoint3(Round(a_value.x, num), Round(a_value.y, num), Round(a_value.z, num));
    }

    inline bool IsArmorSlot(RE::TESObjectARMO* thisArmor, RE::BIPED_MODEL::BipedObjectSlot slotMask)
    {
        return thisArmor->bipedModelData.bipedObjectSlots.underlying() & std::to_underlying(slotMask);
    }
    inline std::int32_t GetNum(std::string str)
    {
        for (auto s : str)
        {
            if (!std::isdigit(s))
                return -1;
        }
        return std::stoi(str);
    }

    inline std::string GetHexStr(std::uint32_t a_value)
    {
        std::stringstream hex;
        hex << std::hex << a_value;
        return std::string(hex.str());
    }
}
