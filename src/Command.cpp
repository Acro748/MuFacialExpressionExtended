#include "Command.h"

namespace Mus {
	bool Execute(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj,
		RE::Script* a_scriptObj, RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr)
	{
		char buffer1[MAX_PATH];
		memset(buffer1, 0, MAX_PATH);
		char buffer2[MAX_PATH];
		memset(buffer2, 0, MAX_PATH);
		char buffer3[MAX_PATH];
		memset(buffer3, 0, MAX_PATH);

		if (!RE::Script::ParseParameters(a_paramInfo, a_scriptData, a_opcodeOffsetPtr, a_thisObj, a_containingObj, a_scriptObj, a_locals,
			buffer1, buffer2, buffer3))
		{
			return false;
		}

		auto Console = RE::ConsoleLog::GetSingleton();
		if (!Console)
			return false;

		RE::Actor* a_actor = RE::PlayerCharacter::GetSingleton();
		if (a_thisObj && a_thisObj->Is(RE::FormType::ActorCharacter))
			a_actor = skyrim_cast<RE::Actor*>(a_thisObj);
		if (!a_actor)
			return false;

		std::string category = RE::BSFixedString(buffer1).c_str(); //morphCategory
		std::string name = RE::BSFixedString(buffer2).c_str(); //morphName
		std::string value = RE::BSFixedString(buffer3).c_str(); //value

		if (!category.empty()) //is category entered
		{
			std::int32_t numMorphCategory = GetNum(category);
			category = lowLetter(category);
			auto categories = morphNameEntry::GetSingleton().GetCategories();
			if ((numMorphCategory >= 0 && numMorphCategory < categories.size()) || morphNameEntry::GetSingleton().IsValidCategory(category))
			{
				if (numMorphCategory != -1)
				{
					category = morphNameEntry::GetSingleton().GetCategories().at(numMorphCategory);
				}

				if (!name.empty()) //is morphName entered
				{
					std::int32_t numMorphName = GetNum(name);
					if (numMorphName >= 0 || morphNameEntry::GetSingleton().IsValidName(name))
					{
						if (numMorphName != -1)
							name = morphNameEntry::GetSingleton().GetMorphNameByNumber(category, numMorphName);

						if (!value.empty())
						{
							float a_value = std::stof(value);
							if (a_value <= 100 && 0 <= a_value || !Config::GetSingleton().GetRestriction())
							{
								ActorManager::GetSingleton().SetMorph(a_actor, name, a_value);
							}
							else
							{
								Console->Print("Value range restrict exceeded!");
								Console->Print("Please enter 0 ~ 100 range");
							}
							return false;
						}
						//failed to get value
						Console->Print("Value is incorrect!");
						return false;
					}
				}
				// failed to get morphName
				Console->Print("Morph num/name is incorrect!");
				auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
				std::string print;
				for (std::size_t i = 0; i < morphNames.size(); i++) {
					if (i != 0)
						print += " / ";
					print += std::to_string(i);
					print += ":";
					print += morphNames.at(i);
				}
				Console->Print("%s", print.c_str());
				return false;
			}
		}
		// failed to get morphCategory
		auto morphCategories = morphNameEntry::GetSingleton().GetCategories();
		std::string print;
		for (std::size_t i = 0; i < morphCategories.size(); i++) {
			if (i != 0)
				print += " / ";
			print += std::to_string(i);
			print += ":";
			print += morphCategories.at(i);
		}
		Console->Print("Please enter the following form");
		Console->Print("mfee <morphCategoryNumber> <morphNameNumber> <value>");
		Console->Print("%s", print.c_str());

		return false;
	}

	void Commandhook()
	{
	logger::info("Command Console Hooking...");
	RE::SCRIPT_FUNCTION* command = nullptr;
	for (RE::SCRIPT_FUNCTION* iter = RE::SCRIPT_FUNCTION::GetFirstConsoleCommand();
		std::to_underlying(iter->output) < RE::SCRIPT_FUNCTION::Commands::kConsoleCommandsEnd + RE::SCRIPT_FUNCTION::Commands::kConsoleOpBase;
		++iter)
	{
		if (IsSameString(iter->functionName, "OutputAllocations"))
		{
			command = iter;
			break;
		}
	}
	if (command)
	{
		static RE::SCRIPT_PARAMETER params[3];
		params[0].paramType = RE::SCRIPT_PARAM_TYPE::kChar;
		params[0].paramName = "String (optional)";
		params[0].optional = true;
		params[1].paramType = RE::SCRIPT_PARAM_TYPE::kChar;
		params[1].paramName = "String (optional)";
		params[1].optional = true;
		params[2].paramType = RE::SCRIPT_PARAM_TYPE::kChar;
		params[2].paramName = "String (optional)";
		params[2].optional = true;

		RE::SCRIPT_FUNCTION cmd = *command;
		cmd.functionName = "mfee";
		cmd.shortName = "mfee";
		cmd.helpString = "mfee <morphCategoryName or morphCategoryNumber> <morphName or morphNumber> <value>";
		cmd.referenceFunction = false;
		cmd.numParams = 3;
		cmd.params = params;
		cmd.executeFunction = Execute;
		cmd.editorFilter = 0;
		REL::safe_write(reinterpret_cast<uintptr_t>(command), &cmd, sizeof(cmd));
	}
	else
		logger::error("Failed to Get Console Command");
	}
}