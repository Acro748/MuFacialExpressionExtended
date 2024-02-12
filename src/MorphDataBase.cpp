#include "MorphDataBase.h"

namespace Mus {
	void morphNameEntry::Init()
	{
		auto moodNames = magic_enum::enum_names<moodCategory>();
		std::string category = lowLetter(magic_enum::enum_name(morphCategory::Mood).data());
		RegisterCategory(category);
		for (auto& name : moodNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto miscNames = magic_enum::enum_names<miscCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Misc).data());
		RegisterCategory(category);
		for (auto& name : miscNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto earsNames = magic_enum::enum_names<earsCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Ears).data());
		RegisterCategory(category);
		for (auto& name : earsNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto tailNames = magic_enum::enum_names<tailCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Tail).data());
		RegisterCategory(category);
		for (auto& name : tailNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto faceNames = magic_enum::enum_names<faceCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Face).data());
		RegisterCategory(category);
		for (auto& name : faceNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto eyesNames = magic_enum::enum_names<eyesCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Eyes).data()); 
		RegisterCategory(category);
		for (auto& name : eyesNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto browsNames = magic_enum::enum_names<browsCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Brows).data());
		RegisterCategory(category);
		for (auto& name : browsNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto mouthNames = magic_enum::enum_names<mouthCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Mouth).data());
		RegisterCategory(category);
		for (auto& name : mouthNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}

		auto tongueNames = magic_enum::enum_names<tongueCategory>();
		category = lowLetter(magic_enum::enum_name(morphCategory::Tongue).data());
		RegisterCategory(category);
		for (auto& name : tongueNames) {
			std::string name_ = lowLetter(name.data());
			if (IsSameString(name_, "total"))
				continue;
			Register(category, name_);
		}
	}

	bool morphNameEntry::Register(std::string category, std::string morphName)
	{
		if (category.empty())
			return false;
		category = lowLetter(category);
		RegisterCategory(category);

		if (morphName.empty()) //adding category only
			return true;
		morphName = lowLetter(morphName);
		if (IsValidName(morphName))
			return false;

		if (auto foundCategory = std::find_if(names.begin(), names.end(), [category](morphNameEntryData data) {
			return IsSameString(category, data.morphCategory);
			}
		); foundCategory != names.end())
		{
			if (auto foundName = std::find(foundCategory->morphNames.begin(), foundCategory->morphNames.end(), morphName); foundName == foundCategory->morphNames.end())
			{
				foundCategory->morphNames.emplace_back(morphName);
				logger::info("morphNameEntry : Registered {} for {}", morphName, category);
				return true;
			}
		}
		return false;
	}
	bool morphNameEntry::RegisterCategory(std::string category)
	{
		if (IsValidCategory(category))
			return false;

		morphNameEntryData newData;
		newData.morphCategory = lowLetter(category);
		names.emplace_back(newData);
		logger::info("morphNameEntry : Registered {} category", category);
		return true;
	}

	std::vector<std::string> morphNameEntry::GetMorphNames(std::string category)
	{
		category = lowLetter(category);
		if (auto found = std::find_if(names.begin(), names.end(), [category](morphNameEntryData data) {
			return IsSameString(data.morphCategory, category);
			}
		); found != names.end())
			return found->morphNames;
		return std::vector<std::string>();
	}
	std::int32_t morphNameEntry::GetMorphNameNumber(std::string morphName)
	{
		morphName = lowLetter(morphName);
		for (auto name : names) {
			for (std::size_t i = 0; i < name.morphNames.size(); i++) {
				if (IsSameString(name.morphNames.at(i), morphName))
					return i;
			}
		}
		return -1;
	}
	std::string morphNameEntry::GetMorphNameByNumber(std::string category, std::int32_t morphNumber)
	{
		if (morphNumber == -1)
			return "";
		auto morphNames = GetMorphNames(category);
		if (morphNumber < morphNames.size())
			return morphNames.at(morphNumber);
		return "";
	}

	std::vector<std::string> morphNameEntry::GetCategories()
	{
		std::vector<std::string> result;
		for (std::size_t i = 0; i < names.size(); i++) {
			std::string category = names.at(i).morphCategory;
			result.emplace_back(category);
		}
		return result;
	}
	std::int32_t morphNameEntry::GetCategoryNumber(std::string category)
	{
		category = lowLetter(category);
		for (std::int32_t i = 0; i < names.size(); i++) {
			if (IsSameString(names.at(i).morphCategory, category))
				return i;
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
			if (std::find_if(name.morphNames.begin(), name.morphNames.end(), [morphName](std::string str) {
				return IsSameString(str, morphName);
				}) != name.morphNames.end())
				return name.morphCategory;
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
		a_morphBasePath = fixPath(a_morphBasePath);
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
			morph.morphBasePath = fixPath(a_morphBasePath);
			morph.vertexCount = vertexCount;
			morph.multiplier = mult;

			for (std::int32_t v = 0; v < vertexCount; v++)
			{
				MorphDataBase::Morph::Vertex vert;
				Read(&file, &vert, sizeof(vert));
				morph.vertices.emplace_back(vert);
			}

			std::string newMorphName = lowLetter(morphName_.c_str());
			auto found = find(newMorphName);
			if (found != end())
				found->second.Register(morph);
			else /*if (morphNameEntry::GetSingleton().IsValidName(newMorphName))*/ {
				MorphDataBase newMorphDataBase = MorphDataBase(newMorphName);
				newMorphDataBase.Register(morph);
				insert(std::make_pair(newMorphName, newMorphDataBase));
			}
			/*else
				continue;*/
			logger::debug("Registered {} morph", morphName_.c_str());
		}
		logger::info("Registered {} file", a_morphBasePath);
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
