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

    inline std::wstring string2wstring(const std::string& str)
    {
        if (str.empty())
            return L"";
        int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
        std::wstring wstr(wlen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], wlen);
        return wstr;
    }
    inline std::string wstring2string(const std::wstring& wstr)
    {
        if (wstr.empty())
            return "";
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
        std::string str(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &str[0], len, NULL, NULL);
        str[len] = '\0';
        return str;
    }

    inline RE::FormID GetIDByModAndID(RE::FormID id, std::string_view modname = "")
    {
        if (modname.empty())
            return 0;
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return 0;
        return DataHandler->LookupFormID(id, modname);
    }
    inline RE::TESForm* GetFormByID(RE::FormID id, std::string_view modname = "")
    {
        if (!modname.empty())
        {
            id = GetIDByModAndID(id, modname);
        }
        RE::TESForm* result = RE::TESForm::LookupByID(id);
        return result ? result : nullptr;
    }

    template <typename T>
    inline T GetFormByID(RE::FormID id, std::string_view modname = "")
    {
        return skyrim_cast<T>(GetFormByID(id, modname));
    }

    inline bool IsActivePlugin(std::string_view modname, bool loaded = false)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return false;
        if (!loaded)
            return DataHandler->LookupModByName(modname) ? true : false;

        if (DataHandler->LookupLoadedModByName(modname))
            return true;
        else
            return DataHandler->LookupLoadedLightModByName(modname) ? true : false;
    }

    inline RE::FormID GetIndexByPluginName(std::string_view modname)
    {
        RE::TESDataHandler* DataHandler = RE::TESDataHandler::GetSingleton();
        if (!DataHandler)
            return 0;
        return DataHandler->LookupFormID(0, modname);
    }

    inline std::uint32_t GetActorBaseFormID(RE::Actor* actor)
    {
        if (!actor)
            return 0;
        RE::TESNPC* actorBase = actor->GetActorBase();
        return actorBase ? actorBase->formID : 0;
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
        if ((id >> 24) == 0xFF)
            return "Template";
        RE::TESForm* form = GetFormByID(id);
        if (!form)
            return "NULL";
        auto file = form->GetFile(0);
        if (!file)
            return "NULL";
        return file->GetFilename();
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

    inline RE::SEX GetSex(RE::Actor* actor)
    {
        RE::SEX sex = RE::SEX::kNone;
        if (!actor)
            return sex;
        auto actorBase = actor->GetActorBase();
        if (!actorBase)
            return sex;
        sex = actorBase->GetSex();
        return sex;
    }
    inline RE::SEX GetPlayerSex() {
        return GetSex(RE::PlayerCharacter::GetSingleton());
    }

    inline std::string lowLetter(std::string Letter)
    {
        for (auto& str : Letter) {
            str = tolower(str);
        }
        return Letter;
    }

    inline std::string highLetter(std::string Letter)
    {
        for (auto& str : Letter) {
            str = toupper(str);
        }
        return Letter;
    }

    inline bool IsContainString(std::string base, std::string find)
    {
        base = lowLetter(base);
        find = lowLetter(find);

        return base.contains(find);
    }

    inline bool IsSameString(std::string str1, std::string str2)
    {
        str1 = lowLetter(str1);
        str2 = lowLetter(str2);

        return (str1 == str2);
    }

    inline std::string fixPath(std::string Letter)
    {
        Letter = lowLetter(Letter);
        Letter = std::regex_replace(Letter, std::regex("/"), "\\");
        return Letter;
    }

    inline bool stringStartsWith(std::string str, std::string prefix)
    {
        str = lowLetter(str);
        prefix = lowLetter(prefix);
        return str.starts_with(prefix);
    }

    inline bool stringEndsWith(std::string str, std::string suffix)
    {
        str = lowLetter(str);
        suffix = lowLetter(suffix);
        return str.ends_with(suffix);
    }

    inline std::string stringRemoveStarts(std::string str, std::string prefix)
    {
        if (!stringStartsWith(str, prefix))
            return str;
        return std::string(str.begin() + prefix.size(), str.end());
    }

    inline std::string stringRemoveEnds(std::string str, std::string suffix)
    {
        if (!stringEndsWith(str, suffix))
            return str;
        return std::string(str.begin(), str.end() - suffix.size());
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

    inline bool HasKeywordWithContainsString(RE::BGSKeywordForm* keywords, std::string find)
    {
        bool result = false;
        keywords->ForEachKeyword([&](const RE::BGSKeyword* a_keyword) {
            if (IsContainString(a_keyword->GetFormEditorID(), find))
            {
                result = true;
                return RE::BSContainer::ForEachResult::kStop;
            }
            return RE::BSContainer::ForEachResult::kContinue;
                                 });
        return result;
    }
    inline bool HasKeywordWithString(RE::BGSKeywordForm* keywords, std::string find)
    {
        bool result = false;
        keywords->ForEachKeyword([&](const RE::BGSKeyword* a_keyword) {
            if (IsSameString(a_keyword->GetFormEditorID(), find))
            {
                result = true;
                return RE::BSContainer::ForEachResult::kStop;
            }
            return RE::BSContainer::ForEachResult::kContinue;
                                 });
        return result;
    }
    inline RE::BGSKeyword* GetkeywordByContainsString(RE::BGSKeywordForm* keywords, std::string find)
    {
        RE::BGSKeyword* result = nullptr;
        keywords->ForEachKeyword([&](RE::BGSKeyword* a_keyword) {
            if (IsContainString(a_keyword->GetFormEditorID(), find))
            {
                result = a_keyword;
                return RE::BSContainer::ForEachResult::kStop;
            }
            return RE::BSContainer::ForEachResult::kContinue;
                                 });
        return result ? result : nullptr;
    }
    inline RE::BGSKeyword* GetKeywordByString(RE::BGSKeywordForm* keywords, std::string find)
    {
        if (!keywords)
            return nullptr;
        RE::BGSKeyword* result = nullptr;
        keywords->ForEachKeyword([&](RE::BGSKeyword* a_keyword) {
            if (IsSameString(a_keyword->GetFormEditorID(), find))
            {
                result = a_keyword;
                return RE::BSContainer::ForEachResult::kStop;
            }
            return RE::BSContainer::ForEachResult::kContinue;
                                 });
        return result ? result : nullptr;
    }

    inline std::string GetHexStr(std::uint32_t a_value)
    {
        std::stringstream hex;
        hex << std::hex << a_value;
        return std::string(hex.str());
    }

    inline std::string Replace(std::string str, std::string search, std::string replace)
    {
        auto pos = std::search(str.begin(), str.end(), search.begin(), search.end(),
                               [](const char c1, const char c2) { return (std::tolower(c1)) == (std::tolower(c2)); });
        if (pos == str.end())
            return str;
        auto pos2 = pos;
        std::advance(pos2, search.size());
        str.replace(pos, pos2, replace);
        return str;
    }

    inline std::string FixPath(std::string path)
    {
        path = lowLetter(path);
        std::filesystem::path filePath = string2wstring(path);
        return Replace(wstring2string(filePath.make_preferred().wstring()), "\\\\", "\\");
    }

    inline RE::NiStream* NiStreamCtor(RE::NiStream* stream)
    {
        using func_t = decltype(&NiStreamCtor);
        REL::VariantID offset(68971, 70324, 0x00C9EC40);
        REL::Relocation<func_t> func{ offset };
        return func(stream);
    }
    inline void NiStreamDtor(RE::NiStream* stream)
    {
        using func_t = decltype(&NiStreamDtor);
        REL::VariantID offset(68972, 70325, 0x00C9EEA0);
        REL::Relocation<func_t> func{ offset };
        return func(stream);
    }

    enum ExistType
    {
        meshes,
        textures,
        none
    };
    inline bool IsExistFileInFileSystem(std::string path)
    {
        return _access_s((GetRuntimeDataDirectory() + path).c_str(), 0) == 0;
    }
    inline bool IsExistFileInStream_Impl(std::string path)
    {
        RE::BSResourceNiBinaryStream binaryStream(path.c_str());
        return binaryStream.good();
    }
    inline bool IsExistFileInStream(std::string path, ExistType type = ExistType::none)
    {
        path = stringRemoveStarts(path, "Data\\");
        if (stringStartsWith(path, "meshes") || stringStartsWith(path, "textures"))
            return IsExistFileInStream_Impl(path);
        else if (type == ExistType::meshes)
            return IsExistFileInStream_Impl("meshes\\" + path);
        else if (type == ExistType::textures)
            return IsExistFileInStream_Impl("textures\\" + path);
        return IsExistFileInStream_Impl(path);
    }
    inline bool IsExistFile(std::string path, ExistType type = ExistType::none, bool inFileSystem = false)
    {
        path = stringRemoveStarts(path, "Data\\");
        if (!inFileSystem)
        {
            if (IsContainString(path, "meshes") || IsContainString(path, "textures") || type != ExistType::none)
                return IsExistFileInStream(path, type);
        }
        else
        {
            if (IsContainString(path, "meshes") || IsContainString(path, "textures") || type != ExistType::none)
            {
                if (stringStartsWith(path, "meshes") || stringStartsWith(path, "textures"))
                    return IsExistFileInFileSystem(path);
                else if (type == ExistType::meshes)
                    return IsExistFileInFileSystem("meshes\\" + path);
                else if (type == ExistType::textures)
                    return IsExistFileInFileSystem("textures\\" + path);
            }
        }
        return _access_s((GetRuntimeDataDirectory() + path).c_str(), 0) == 0;
    }

    inline bool DuplicateFileInStream(std::string src_file, std::string dst_file)
    {
        if (!IsExistFileInStream_Impl(src_file))
            return false;

        RE::BSResourceNiBinaryStream binaryStream(src_file.c_str());
        if (!binaryStream.good()) {
            return false;
        }

        std::uint8_t niStreamMemory[sizeof(RE::NiStream)];
        RE::NiStream* niStream = (RE::NiStream*)niStreamMemory;
        NiStreamCtor(niStream);
        if (!niStream->Load1(&binaryStream))
        {
            if (!stringStartsWith(src_file, "Data"))
                src_file = GetRuntimeDataDirectory() + src_file;
            if (!niStream->Load3((src_file).c_str()))
            {
                NiStreamDtor(niStream);
                return false;
            }
        }

        if (!stringStartsWith(dst_file, "Data"))
            dst_file = GetRuntimeDataDirectory() + dst_file;
        bool result = niStream->Save3(dst_file.c_str());
        NiStreamDtor(niStream);
        return result;
    }

    inline std::int8_t VersionCheck(std::uint16_t major, std::uint16_t minor, std::uint16_t patch) //-1 smaller, 0 equel, 1 bigger
    {
        auto version = REL::Module::get().version();
        if (major == version.major())
        {
            if (minor == version.minor())
            {
                if (patch == version.patch())
                    return 0;
                else if (patch > version.patch())
                    return -1;
                else
                    return 1;
            }
            else if (minor > version.minor())
                return -1;
            else
                return 1;
        }
        else
        {
            if (major > version.major())
                return -1;
            else
                return 1;
        }
    }

    inline std::uint32_t GetDLLVersion(std::string fileName)
    {
        LPDWORD handle = 0;
        DWORD datalen = GetFileVersionInfoSize(string2wstring(fileName).c_str(), handle);
        if (datalen == 0)
            return 0;
        BYTE* data = new BYTE[datalen];
        DWORD handleinfo = 0;
        if (!GetFileVersionInfoW(string2wstring(fileName).c_str(), handleinfo, datalen, data))
            return 0;
        std::uint32_t verlen = 0;
        VS_FIXEDFILEINFO* verinfo = nullptr;
        if (!VerQueryValue(data, L"\\", (void**)&verinfo, &verlen))
            return 0;
        std::uint32_t version0 = HIWORD(verinfo->dwFileVersionMS);
        std::uint32_t version1 = LOWORD(verinfo->dwFileVersionMS);
        std::uint32_t version2 = HIWORD(verinfo->dwFileVersionLS);
        std::uint32_t version3 = LOWORD(verinfo->dwFileVersionLS);
        return version0 * 1000 + version1 * 100 + version2 * 10 + version3;
    }

    inline std::string removeFileNameExtention(std::string s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](char c) {return c == '.'; }).base() - 1, s.end());
        return s;
    }

    inline void ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        [](int c) {return !std::isspace(c); }));
    }

    inline void rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int c) {return !std::isspace(c); }).base(), s.end());
    }

    inline void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }

    inline void ltrim(std::string& s, char e)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        [e](char c) {return c != e; }));
    }

    inline void rtrim(std::string& s, char e)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [e](char c) {return c != e; }).base(), s.end());
    }

    inline std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }

    inline std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::string str = trim_copy(s);

        std::vector<std::string> tokens;
        if (!str.empty())
        {
            std::string token;
            std::istringstream tokenStream(str);
            while (std::getline(tokenStream, token, delimiter))
            {
                trim(token);
                tokens.emplace_back(token);
            }
        }
        return tokens;
    }

    inline bool IsNumber(std::string str)
    {
        for (auto s : str)
        {
            if (!std::isdigit(s))
                return false;
        }
        return true;
    }
    inline std::int32_t GetNum(std::string str)
    {
        return IsNumber(str) ? std::stoi(str) : -1;
    }

    inline unsigned long getHex(std::string hexstr)
    {
        unsigned long result = 0;
        try {
            result = strtoul(hexstr.c_str(), 0, 16);
        }
        catch (...) {
            result = 0;
        }
        return result;
    }

    inline std::string SetHex(std::uint32_t number, bool addPrefix)
    {
        std::stringstream hex;
        hex << std::hex << number;
        return (addPrefix ? "0x" : "") + hex.str();
    }

    inline std::string GetTexturePath(RE::BSTextureSet* textureSet, RE::BSTextureSet::Texture type)
    {
        const char* path = textureSet->GetTexturePath(type);
        return path ? path : "";
    }

    inline std::uint64_t GetVideoMemotrySize()
    {
        static bool IsDetected = false;
        static std::uint64_t VideoMemoryMB = 0;
        if (IsDetected)
            return VideoMemoryMB;
        IsDetected = true;

        Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory = nullptr;
        if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory))) {
            logger::error("Failed to create DXGI Factory to get GPU info");
            return VideoMemoryMB;
        }
        Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter = nullptr;
        DXGI_ADAPTER_DESC1 adapterDesc;
        int adapterIndex = 0;
        while (pFactory->EnumAdapters1(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
        {
            if (SUCCEEDED(pAdapter->GetDesc1(&adapterDesc))) { //first usable gpu
                VideoMemoryMB = adapterDesc.DedicatedVideoMemory / (1024 * 1024);
                if (VideoMemoryMB == 0)
                    VideoMemoryMB == 1; //internal gpu
                return VideoMemoryMB;
            }
            adapterIndex++;
        }
        return VideoMemoryMB;
    }

    inline std::string ReplaceNewlineEscapes(const std::string& str)
    {
        std::string result = str;
        std::string::size_type pos = 0;
        while ((pos = result.find("\\n", pos)) != std::string::npos) {
            result.replace(pos, 2, "\n");
            pos += 1;
        }
        return result;
    }

    inline std::string GetFileName(const std::string& str)
    {
        std::filesystem::path file(str);
        return lowLetter(file.stem().string());
    }

    inline void InsertOnFront(RE::BSTArray<RE::TESForm*>& formList, RE::TESForm* form)
    {
        if (!form)
            return;
        if (formList.size() == 0)
        {
            formList.emplace_back(form);
            return;
        }
        std::vector<RE::TESForm*> tempFormList;
        for (auto tempForm : formList)
        {
            tempFormList.emplace_back(tempForm);
        }
        formList.clear();
        formList.emplace_back(form);
        for (auto tempForm : tempFormList)
        {
            formList.emplace_back(tempForm);
        }
        return;
    }

    inline void PerformaceLog(std::string funcStr, bool isEnd, bool isAverage = true, std::uint32_t args = 0)
    {
        if (!PerformanceCheck)
            return;

        using namespace std::chrono;
        static concurrency::concurrent_unordered_map<std::string, high_resolution_clock::time_point> funcTime;
        static concurrency::concurrent_unordered_map<std::string, long long> funcAverageTime;
        static concurrency::concurrent_unordered_map<std::string, unsigned long> funcAverageCount;
        static concurrency::concurrent_unordered_map<std::string, std::uint32_t> funcAverageArgs;

        if (!isEnd)
        {
            funcTime[funcStr] = high_resolution_clock::now();
        }
        else
        {
            auto end = high_resolution_clock::now();
            auto duration_ns = duration_cast<nanoseconds>(end - funcTime[funcStr]).count();
            if (isAverage)
            {
                funcAverageTime[funcStr] += duration_ns;
                funcAverageCount[funcStr]++;
                funcAverageArgs[funcStr] += args;
                if (funcAverageCount[funcStr] >= 100)
                {
                    auto average = funcAverageTime[funcStr] / funcAverageCount[funcStr];
                    logger::info("{} average time: {}ns{}=> {:.6f}%", funcStr, average,
                                 funcAverageArgs[funcStr] > 0 ? (std::string(" with average count ") + std::to_string(funcAverageArgs[funcStr] / funcAverageCount[funcStr]) + " ") : " ",
                                 (double)average / (double)(RE::GetSecondsSinceLastFrame() * 1000000000) * 100
                    );
                    funcAverageTime[funcStr] = 0;
                    funcAverageCount[funcStr] = 0;
                    funcAverageArgs[funcStr] = 0;
                }
            }
            else
            {
                logger::info("{} time: {}ns{}=> {:.6f}%", funcStr, duration_ns,
                             args > 0 ? (std::string(" with count ") + std::to_string(args) + " ") : " ",
                             (double)duration_ns / double(RE::GetSecondsSinceLastFrame() * 1000000000) * 100
                );
            }
        }
    }
}
