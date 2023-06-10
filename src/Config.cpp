#include "Config.h"

#include <articuno/archives/ryml/ryml.h>
#include <direct.h>

using namespace articuno::ryml;

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
                    //CustomMode = GetConfigSettingsBoolValue(variableValue);
                }
                else if (variableName == "Restriction")
                {
                    Restriction = GetConfigSettingsBoolValue(variableValue);
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

        auto configList = get_all_files_names_within_folder(configPath.c_str());
        concurrency::parallel_for(std::size_t(0), configList.size(), [&](std::size_t i) {
            std::string filename = configList.at(i);
            
            if (filename != "." && filename != "..")
            {
                if (stringEndsWith(filename, ".ini"))
                {
                    logger::info("File found: {}", filename);

                    std::string filepath = configPath;
                    filepath.append(filename);
                    std::ifstream file(filepath);

                    if (!file.is_open())
                    {
                        transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
                        file.open(filepath);
                    }

                    if (file.is_open())
                    {
                        std::string line;
                        std::string currentSetting;

                        std::string extensionfiles;
                        bool isReading = false;
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

                            if (variableName == "ExtensionFile")
                            {
                                if (isReading)
                                {
                                    auto extensionfile = split(extensionfiles, ",");
                                    if (extensionfile.size() > 1)
                                    {
                                        for (std::size_t i = 1; i < extensionfile.size(); i++)
                                        {
                                            MorphDataBaseManager::GetSingleton().Register(extensionfile.at(0), extensionfile.at(i));
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
                                for (std::size_t i = 1; i < extensionfile.size(); i++)
                                {
                                    MorphDataBaseManager::GetSingleton().Register(extensionfile.at(0), extensionfile.at(i));
                                }
                            }
                        }
                    }
                }
            }
        });
        return true;
    }

    bool MultipleConfig::LoadMorphNameConfig()
    {
        if (!GetCustomMode())
            return false;

        std::string configPath = GetRuntimeSKSEDirectory();
        configPath += "MuFacialExpressionExtended\\MorphNames";

        auto configList = get_all_files_names_within_folder(configPath.c_str());
        for(std::size_t i = 0; i < configList.size(); i++) 
        {
            std::string filename = configList.at(i);

            if (filename != "." && filename != "..")
            {
                if (stringEndsWith(filename, ".ini"))
                {
                    logger::info("File found: {}", filename);

                    std::string filepath = configPath;
                    filepath.append(filename);
                    std::ifstream file(filepath);

                    if (!file.is_open())
                    {
                        transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
                        file.open(filepath);
                    }

                    if (file.is_open())
                    {
                        std::string line;
                        std::string currentSetting;

                        std::string morphCategory;
                        std::string morphNames;
                        bool isReading = false;
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

                            if (variableName == "morphCategory")
                            {
                                if (isReading)
                                {
                                    auto morphName = split(morphNames, ",");
                                    if (morphName.size() > 1)
                                    {
                                        for (auto& name : morphName) {
                                            morphNameEntry::GetSingleton().Register(morphCategory, name);
                                        }
                                    }
                                }
                                morphNameEntry::GetSingleton().RegisterCategory(variableValue);
                                isReading = false;
                            }
                            else if (variableName == "MorphName")
                            {
                                if (isReading)
                                {
                                    auto morphName = split(morphNames, ",");
                                    if (morphName.size() > 1)
                                    {
                                        for (auto& name : morphName) {
                                            morphNameEntry::GetSingleton().Register(morphCategory, name);
                                        }
                                    }
                                }
                                morphNames = variableValue;
                                isReading = true;
                            }
                            else if (isReading)
                            {
                                morphNames += variableValue;
                            }
                        }

                        if (isReading)
                        {
                            auto morphName = split(morphNames, ",");
                            if (morphName.size() > 1)
                            {
                                for (auto& name : morphName) {
                                    morphNameEntry::GetSingleton().Register(morphCategory, name);
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
}

