#include "MorphDataBase.h"

namespace Mus {
	void morphNameEntry::Init()
	{
		auto moodNames = magic_enum::enum_names<moodCategory>();
		auto miscNames = magic_enum::enum_names<miscCategory>();
		auto earsNames = magic_enum::enum_names<earsCategory>();
		auto tailNames = magic_enum::enum_names<tailCategory>();
		auto faceNames = magic_enum::enum_names<faceCategory>();
		auto eyesNames = magic_enum::enum_names<eyesCategory>();
		auto browsNames = magic_enum::enum_names<browsCategory>();
		auto mouthNames = magic_enum::enum_names<mouthCategory>();
		auto tongueNames = magic_enum::enum_names<tongueCategory>();

		std::string typeName = lowLetter(magic_enum::enum_name(morphCategory::Mood).data());
		for (auto& name : moodNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Misc).data());
		for (auto& name : miscNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Ears).data());
		for (auto& name : earsNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Tail).data());
		for (auto& name : tailNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Face).data());
		for (auto& name : faceNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Eyes).data());
		for (auto& name : eyesNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Brows).data());
		for (auto& name : browsNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Mouth).data());
		for (auto& name : mouthNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}

		typeName = lowLetter(magic_enum::enum_name(morphCategory::Tongue).data());
		for (auto& name : tongueNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			names[typeName].emplace_back(name_);
		}
	}

	bool morphNameEntry::Register(std::string category, std::string morphName)
	{
		category = lowLetter(category);
		morphName = lowLetter(morphName);
		if (!IsValidCategory(category))
			return false;
		if (IsValidName(morphName))
			return false;
		if (std::find(names[category].begin(), names[category].end(), morphName) == names[category].end())
		{
			names[category].emplace_back(morphName);
			logger::debug("morphNameEntry : Registered {} for {}", morphName, category);
			return true;
		}
		return false;
	}
	bool morphNameEntry::Register(std::string morphName)
	{
		morphName = lowLetter(morphName);
		if (IsValidName(morphName))
			return false;

		std::string category;
		if (auto found = std::find_if(names.begin(), names.end(), [morphName](std::pair<std::string, std::vector<std::string>> pair) {
			return IsContainString(morphName, pair.first);
			}); found != names.end())
		{
			category = found->first;
		}
		else
		{
			category = lowLetter(magic_enum::enum_name(morphCategory::Misc).data());
		}
		names[category].emplace_back(morphName);
		logger::debug("morphNameEntry : Registered {} for {}", morphName, category);
		return true;
	}
	bool morphNameEntry::RegisterCategory(std::string category)
	{
		if (GetCategoryNumber(category) != -1)
			return false;
		category = lowLetter(category);
		std::vector<std::string> namelist = names[category];
		names[category] = namelist;
		return true;
	}
	std::vector<std::string> morphNameEntry::GetMorphNames(std::string category)
	{
		if (GetCategoryNumber(category) == -1)
			return std::vector<std::string>();
		category = lowLetter(category);
		return names[category];
	}
	std::int32_t morphNameEntry::GetMorphNameNumber(std::string morphName)
	{
		morphName = lowLetter(morphName);
		for (auto name : names) {
			for (std::size_t i = 0; i < name.second.size(); i++) {
				if (IsSameString(name.second.at(i), morphName))
					return i;
			}
		}
		return -1;
	}
	std::string morphNameEntry::GetMorphNameByNumber(std::string category, std::int32_t morphNumber)
	{
		if (morphNumber == -1)
			return "";
		auto names = GetMorphNames(category);
		if (morphNumber < names.size())
			return names.at(morphNumber);
		return "";
	}

	std::vector<std::string> morphNameEntry::GetCategories()
	{
		std::vector<std::string> result;
		for (auto name : names) {
			result.emplace_back(name.first);
		}
		return result;
	}
	std::int32_t morphNameEntry::GetCategoryNumber(std::string category)
	{
		category = lowLetter(category);
		std::int32_t i = 0;
		for (auto name : names) {
			if (IsSameString(name.first, category))
				return i;
			i++;
		}
		return -1;
	}
	std::string morphNameEntry::GetCategoryByNumber(std::int32_t categoryNumber)
	{
		if (categoryNumber == -1)
			return "";
		auto categories = GetCategories();
		if (categories.size() > categoryNumber)
			return categories.at(categoryNumber);
		return "";
	}
	std::string morphNameEntry::GetCategoryByMorphName(std::string morphName)
	{
		morphName = lowLetter(morphName);
		for (auto name : names) {
			if (std::find_if(name.second.begin(), name.second.end(), [morphName](std::string str) {
				return IsSameString(str, morphName);
				}) != name.second.end())
				return name.first;
		}
		return "";
	}


	bool MorphDataBase::Register(Morph a_morph)
	{
		if (!a_morph.IsValid())
			return false;

		morphData[a_morph.morphBasePath] = a_morph;
		logger::info("MorphDataBase : Registered {} for {}", morphName, a_morph.morphBasePath);
		return true;
	}

	const MorphDataBase::Morph* MorphDataBase::GetMorphData(std::string a_morphBasePath) const
	{
		a_morphBasePath = lowLetter(a_morphBasePath);
		auto found = morphData.find(a_morphBasePath);
		if (found != morphData.end())
			return &found->second;
		return nullptr;
	}

	bool MorphDataBaseManager::Register(std::string a_morphBasePath, std::string a_morphPath)
	{
		if (a_morphBasePath.empty() || a_morphPath.empty())
			return false;

		char basefilePath[MAX_PATH];
		memset(basefilePath, 0, MAX_PATH);
		sprintf_s(basefilePath, MAX_PATH, "Meshes\\%s", a_morphBasePath.c_str());
		RE::BSFixedString newBasePath(basefilePath);

		RE::BSResourceNiBinaryStream basefile(newBasePath.data());
		if (!basefile.good()) {
			return false;
		}

		char baseheader[0x08];
		Read(&basefile, baseheader, 0x08);
		if (strncmp(baseheader, "FRTRI003", 8) != 0)
			return false;

		std::int32_t baseVertexCount;
		Read(&basefile, &baseVertexCount, sizeof(baseVertexCount));
		logger::debug("BaseVertexCount : {}", baseVertexCount);


		char filePath[MAX_PATH];
		memset(filePath, 0, MAX_PATH);
		sprintf_s(filePath, MAX_PATH, "Meshes\\%s", a_morphPath.c_str());
		RE::BSFixedString newPath(filePath);

		RE::BSResourceNiBinaryStream file(newPath.data());
		if (!file.good()) {
			return false;
		}

		logger::debug("Register {} file for {}...", a_morphPath, a_morphBasePath);

		char header[0x08];
		Read(&file, header, 0x08);
		if (strncmp(header, "FRTRI003", 8) != 0)
			return false;

		std::int32_t vertexCount;
		Read(&file, &vertexCount, sizeof(vertexCount));
		logger::debug("vertexCount : {}", vertexCount);
		if (baseVertexCount != vertexCount)
			return false;

		std::uint32_t polytris = 0, polyquads = 0, lverts = 0, lsurfs = 0,
			uvverts = 0, flags = 0, morphCount = 0, statMorphs = 0,
			statVerts = 0, unk7 = 0, unk8 = 0, unk9 = 0, unk10 = 0;

		Read(&file, &polytris, sizeof(polytris));
		logger::debug("polytris : {}", polytris);
		Read(&file, &polyquads, sizeof(polyquads));
		logger::debug("polyquads : {}", polyquads);
		Read(&file, &lverts, sizeof(lverts));
		logger::debug("lverts : {}", lverts);
		Read(&file, &lsurfs, sizeof(lsurfs));
		logger::debug("lsurfs : {}", lsurfs);
		Read(&file, &uvverts, sizeof(uvverts));
		logger::debug("uvverts : {}", uvverts);
		Read(&file, &flags, sizeof(flags));
		logger::debug("flags : {}", flags);
		Read(&file, &morphCount, sizeof(morphCount));
		logger::debug("morphCount : {}", morphCount);
		Read(&file, &statMorphs, sizeof(statMorphs));
		logger::debug("statMorphs : {}", statMorphs);
		Read(&file, &statVerts, sizeof(statVerts));
		logger::debug("statVerts : {}", statVerts);
		Read(&file, &unk7, sizeof(unk7));
		logger::debug("unk7 : {}", unk7);
		Read(&file, &unk8, sizeof(unk8));
		logger::debug("unk8 : {}", unk8);
		Read(&file, &unk9, sizeof(unk9));
		logger::debug("unk9 : {}", unk9);
		Read(&file, &unk10, sizeof(unk10));
		logger::debug("unk10 : {}", unk10);

		// Skip reference verts
		Seek(&file, vertexCount * 3 * sizeof(float));

		// Skip polytris
		Seek(&file, polytris * 3 * sizeof(std::uint32_t));

		// Skip UV
		if (uvverts > 0)
			Seek(&file, uvverts * 2 * sizeof(float));

		// Skip text coords
		Seek(&file, polytris * 3 * sizeof(std::uint32_t));

		for (std::uint32_t i = 0; i < morphCount; i++)
		{
			std::uint32_t strLen = 0;
			Read(&file, &strLen, sizeof(strLen));

			char* morphName = new char[strLen + 1];
			for (std::uint32_t l = 0; l < strLen; l++)
			{
				Read(&file, &morphName[l], sizeof(char));
			}
			morphName[strLen] = 0;
			RE::BSFixedString morphName_(morphName);

			float mult = 0.0f;
			Read(&file, &mult, sizeof(mult));

			MorphDataBase::Morph morph;
			morph.morphBasePath = lowLetter(a_morphBasePath);
			morph.vertexCount = vertexCount;
			morph.multiplier = mult;

			for (std::uint32_t v = 0; v < vertexCount; v++)
			{
				MorphDataBase::Morph::Vertex vert;
				Read(&file, &vert, sizeof(vert));
				morph.vertices.emplace_back(vert);
			}

			std::string newMorphName = lowLetter(morphName_.c_str());
			auto found = find(newMorphName);
			if (found != end())
				found->second.Register(morph);
			else if (Config::GetSingleton().GetCustomMode() || morphNameEntry::GetSingleton().IsValidName(newMorphName)) {
				morphNameEntry::GetSingleton().Register(newMorphName);

				MorphDataBase newMorphDataBase = MorphDataBase(newMorphName);
				newMorphDataBase.Register(morph);
				insert(std::make_pair(newMorphName, newMorphDataBase));
			}
			else
				continue;
			logger::debug("Registered {} morph", morphName_.c_str());
		}
		logger::info("Registered {} file", a_morphPath);
		return true;
	}

	const MorphDataBase::Morph* MorphDataBaseManager::GetMorphData(std::string morphName, std::string a_morphBasePath) const
	{
		morphName = lowLetter(morphName);
		auto found = find(morphName);
		if (found != end())
			return found->second.GetMorphData(a_morphBasePath);
		return nullptr;
	}
}
