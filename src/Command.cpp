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

		std::string param1 = buffer1; //morphType
		std::string param2 = buffer2; //morphName
		std::string param3 = buffer3; //value

		auto morphTypes = magic_enum::enum_entries<morphNameEntry::morphType>();
		if (!param1.empty()) //is morphType entered
		{
			std::int32_t numMorphType = GetNum(param1);
			if (auto found = std::find_if(morphTypes.begin(), morphTypes.end(), [param1](std::pair<morphNameEntry::morphType, std::string_view> pair) { return IsSameString(pair.second.data(), param1); });
				found != morphTypes.end()
				|| (numMorphType != -1 && numMorphType < morphTypes.size() - 1))
			{
				morphNameEntry::morphType morphType;
				if (numMorphType != -1)
					morphType = morphNameEntry::morphType(numMorphType);
				else
					morphType = found->first;

				auto morphNames = morphNameEntry::GetSingleton().GetNames(morphType);
				if (!param2.empty()) //is morphName entered
				{
					std::int32_t numMorphName = GetNum(param2);
					if (auto found = std::find_if(morphNames.begin(), morphNames.end(), [param2](std::string str) { return IsSameString(str.data(), param2); });
						found != morphNames.end()
						|| (numMorphName != -1 && numMorphName < morphNames.size() - 1))
					{
						std::string morphName;
						if (numMorphName != -1)
							morphName = morphNames.at(numMorphName);
						else
							morphName = found->c_str();

						if (!param3.empty())
						{
							float value = std::stof(param3);
							if (value <= 100 && 0 <= value || !Config::GetSingleton().GetRestriction())
							{
								ActorManager::GetSingleton().SetMorph(a_actor, morphName, value);
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
				Console->Print("Morph name/number is incorrect!");
				std::string print;
				for (std::size_t i = 0; i < morphNames.size(); i++) {
					if (i != 0)
						print += " / ";
					print += std::to_string(i);
					print += " : ";
					print += morphNames.at(i);
				}
				return false;
			}
		}
		// failed to get morphType
		std::string print;
		for (auto& type : morphTypes) {
			if (type.first == morphNameEntry::morphType::total)
				continue;
			else if (type.first != morphNameEntry::morphType::Mood)
				print += " / ";
			print += std::to_string(std::to_underlying(type.first));
			print += " : ";
			print += type.second.data();
		}
		Console->Print("Please enter the following form");
		Console->Print("mfee <morphTypeNumber> <morphNameNumber> <value>");
		Console->Print("Morph Type numbers = %s", print);

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
		cmd.helpString = "mfee <morphTypeName or morphTypeNumber> <morphName or morphNumber> <value>";
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