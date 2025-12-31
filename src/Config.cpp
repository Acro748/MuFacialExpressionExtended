#include "Config.h"

#include <direct.h>

namespace Mus {
    bool Config::LoadConfig() {
        std::string configPath = GetRuntimeSKSEDirectory();
        configPath += "MuFacialExpressionExtended.ini";

        std::ifstream file(configPath);

        if (!file.is_open())
        {
            std::transform(configPath.begin(), configPath.end(), configPath.begin(), ::tolower);
            file.open(configPath);
        }

        if (!file.is_open())
        {
            logger::critical("Unable to MuSkeletonEditor.ini file.");
            return false;
        }

        std::string line;
        std::string currentSetting;
        while (std::getline(file, line))
        {
            //trim(line);
            skipComments(line);
            trim(line);
            if (line.length() == 0)
                continue;

            if (line.substr(0, 1) == "[")
            {
                currentSetting = line;
                continue;
            }
            std::string variableName;
            std::string variableValue = GetConfigSettingsStringValue(line, variableName);
            if (currentSetting == "[Debug]")
            {
                if (variableName == "logLevel")
                {
                    logLevel = spdlog::level::from_str(variableValue);
                }
                else if (variableName == "flushLevel")
                {
                    flushLevel = spdlog::level::from_str(variableValue);
                }
            }
            else if (currentSetting == "[General]")
            {
                if (variableName == "CustomMode")
                {
                    CustomMode = GetConfigSettingsBoolValue(variableValue);
                }
                else if (variableName == "Min")
                {
                    Min = GetConfigSettingsIntValue(variableValue);
                }
                else if (variableName == "Max")
                {
                    Max = GetConfigSettingsIntValue(variableValue);
                }
                else if (variableName == "DefaultLerpTime")
                {
                    DefaultLerpTime = GetConfigSettingsIntValue(variableValue);
                }
                else if (variableName == "MorphByVertexCount")
                {
                    MorphByVertexCount = GetConfigSettingsBoolValue(variableValue);
                }
                else if (variableName == "DisableDistance")
                {
                    DisableDistance = GetConfigSettingsBoolValue(variableValue);
                }
                else if (variableName == "Recalculate")
                {
                    Recalculate = GetConfigSettingsBoolValue(variableValue);
                }
            }
        }
        logger::info("Config loaded done");
        return true;
    }

    bool MultipleConfig::LoadMorphConfig()
    {
        std::string configPath = GetRuntimeSKSEDirectory();
        configPath += "MuFacialExpressionExtended\\";

        auto files = GetAllFiles(configPath);
        for (auto& file : files)
        {
            std::u8string filename_utf8 = file.filename().u8string();
            std::string filename(filename_utf8.begin(), filename_utf8.end());
            if (filename == "." || filename == "..")
                continue;
            if (!stringEndsWith(filename, ".ini"))
                continue;
            logger::info("File found: {}", filename);
            std::ifstream ifile(file);
            if (!ifile.is_open())
            {
                logger::info("Unable to open the file: {}", filename);
                continue;
            }

            std::string line;
            std::string extensionfiles;
            bool isReading = false;
            while (std::getline(ifile, line))
            {
                //trim(line);
                skipComments(line);
                trim(line);
                if (line.length() == 0)
                    continue;

                std::string variableName;
                std::string variableValue = GetConfigSettingsStringValue(line, variableName);

                if (variableName == "ExtensionFile")
                {
                    if (isReading)
                    {
                        auto extensionfile = split(extensionfiles, ",");
                        if (extensionfile.size() > 1)
                        {
                            for (std::size_t j = 1; j < extensionfile.size(); j++)
                            {
                                MorphDataBaseManager::GetSingleton().Register(extensionfile[0], extensionfile[j]);
                            }
                        }
                    }
                    extensionfiles = variableValue;
                    isReading = true;
                }
                else if (isReading)
                {
                    extensionfiles += variableValue;
                }
            }

            if (isReading)
            {
                auto extensionfile = split(extensionfiles, ",");
                if (extensionfile.size() > 1)
                {
                    for (std::size_t j = 1; j < extensionfile.size(); j++)
                    {
                        MorphDataBaseManager::GetSingleton().Register(extensionfile[0], extensionfile[j]);
                    }
                }
            }
        }
        return true;
    }

    bool MultipleConfig::LoadMorphNameConfig()
    {
        if (!GetCustomMode())
            return false;

        std::string configPath = GetRuntimeSKSEDirectory();
        configPath += "MuFacialExpressionExtended\\Morphs\\";

        auto files = GetAllFiles(configPath);
        for (auto& file : files)
        {
            std::u8string filename_utf8 = file.filename().u8string();
            std::string filename(filename_utf8.begin(), filename_utf8.end());
            if (filename == "." || filename == "..")
                continue;
            if (!stringEndsWith(filename, ".ini"))
                continue;
            logger::info("File found: {}", filename);
            std::ifstream ifile(file);
            if (!ifile.is_open())
            {
                logger::info("Unable to open the file: {}", filename);
                continue;
            }

            std::string line;
            while (std::getline(ifile, line))
            {
                //trim(line);
                skipComments(line);
                trim(line);
                if (line.length() == 0)
                    continue;

                auto newMorph = split(line, "|");
                if (newMorph.size() != 2)
                    continue;
                if (newMorph[0].empty() || newMorph[1].empty())
                    continue;
                morphNameEntry::GetSingleton().Register(newMorph[0], newMorph[1]);
            }
        }
        return true;
    }
}

