#include "MorphDataBase.h"

namespace Mus {
	void morphNameEntry::Init()
	{
		auto moodNames = magic_enum::enum_names<moodCategory>();
		lString category = magic_enum::enum_name(morphCategory::Mood).data();
		RegisterCategory(category);
		for (auto& name : moodNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto miscNames = magic_enum::enum_names<miscCategory>();
		category = magic_enum::enum_name(morphCategory::Misc).data();
		RegisterCategory(category);
		for (auto& name : miscNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto earsNames = magic_enum::enum_names<earsCategory>();
		category = magic_enum::enum_name(morphCategory::Ears).data();
		RegisterCategory(category);
		for (auto& name : earsNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto tailNames = magic_enum::enum_names<tailCategory>();
		category = magic_enum::enum_name(morphCategory::Tail).data();
		RegisterCategory(category);
		for (auto& name : tailNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto faceNames = magic_enum::enum_names<faceCategory>();
		category = magic_enum::enum_name(morphCategory::Face).data();
		RegisterCategory(category);
		for (auto& name : faceNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto eyesNames = magic_enum::enum_names<eyesCategory>();
		category = magic_enum::enum_name(morphCategory::Eyes).data(); 
		RegisterCategory(category);
		for (auto& name : eyesNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto browsNames = magic_enum::enum_names<browsCategory>();
		category = magic_enum::enum_name(morphCategory::Brows).data();
		RegisterCategory(category);
		for (auto& name : browsNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto mouthNames = magic_enum::enum_names<mouthCategory>();
		category = magic_enum::enum_name(morphCategory::Mouth).data();
		RegisterCategory(category);
		for (auto& name : mouthNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}

		auto tongueNames = magic_enum::enum_names<tongueCategory>();
		category = magic_enum::enum_name(morphCategory::Tongue).data();
		RegisterCategory(category);
		for (auto& name : tongueNames) {
			lString name_ = name.data();
			if (name_ == "total")
				continue;
			Register(category, name_);
		}
	}

	bool morphNameEntry::Register(const lString& category, const lString& morphName)
	{
		if (category.empty())
			return false;
		RegisterCategory(category);
		if (morphName.empty()) //adding category only
			return true;
		if (IsValidName(morphName))
			return false;

        std::lock_guard lg(namesLock);
		if (auto foundCategory = std::find_if(names.begin(), names.end(), [category](morphNameEntryData data) {
			return category == data.morphCategory;
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
    bool morphNameEntry::RegisterCategory(const lString& category)
	{
        std::lock_guard lg(namesLock);
		if (IsValidCategory(category))
			return false;
        morphNameEntryData newData = { .morphCategory = category};
		names.emplace_back(newData);
		logger::info("morphNameEntry : Registered {} category", category);
		return true;
	}

	std::vector<lString> morphNameEntry::GetMorphNames(const lString& category)
    {
		if (auto found = std::find_if(names.begin(), names.end(), [category](morphNameEntryData data) {
			return data.morphCategory == category;
			}
		); found != names.end())
			return found->morphNames;
        return std::vector<lString>();
	}
	std::vector<std::string> morphNameEntry::GetMorphNamesBasic(const lString& category)
    {
        std::vector<std::string> result;
		if (auto found = std::find_if(names.begin(), names.end(), [category](morphNameEntryData data) {
			return data.morphCategory == category;
			}
		); found != names.end())
        {
			for (const auto& name : found->morphNames)
			{
                result.push_back(name);
			}
        }
        return result;
	}

    std::int32_t morphNameEntry::GetMorphNameNumber(const lString& morphName)
    {
		for (auto name : names) {
			for (std::size_t i = 0; i < name.morphNames.size(); i++) {
				if (name.morphNames[i] == morphName)
					return i;
			}
		}
		return -1;
	}
    lString morphNameEntry::GetMorphNameByNumber(const lString& category, std::int32_t morphNumber)
    {
		if (morphNumber == -1)
			return "";
		auto morphNames = GetMorphNames(category);
		if (morphNumber < morphNames.size())
			return morphNames[morphNumber];
		return "";
	}

	std::vector<std::string> morphNameEntry::GetCategories()
    {
        std::vector<std::string> result;
		for (std::size_t i = 0; i < names.size(); i++) {
            result.push_back(names[i].morphCategory);
		}
		return result;
	}
    std::int32_t morphNameEntry::GetCategoryNumber(const lString& category)
    {
		for (std::int32_t i = 0; i < names.size(); i++) {
			if (names[i].morphCategory == category)
				return i;
		}
		return -1;
	}
	lString morphNameEntry::GetCategoryByNumber(std::int32_t categoryNumber)
	{
		if (categoryNumber == -1)
			return "";
		auto categories = GetCategories();
		if (categories.size() > categoryNumber)
			return categories[categoryNumber];
		return "";
	}
    lString morphNameEntry::GetCategoryByMorphName(const lString& morphName)
    {
		for (auto& name : names) {
			if (std::find_if(name.morphNames.begin(), name.morphNames.end(), [morphName](lString str) {
				return str == morphName;
				}) != name.morphNames.end())
				return name.morphCategory;
		}
		return "";
	}


	bool MorphDataBase::Register(const Morph& a_morph)
	{
		if (!a_morph.IsValid())
			return false;

		std::lock_guard lg(morphDataLock);
		morphData.insert(std::make_pair(a_morph.morphBasePath, a_morph));
        morphDataAlt.insert(std::make_pair(a_morph.vertexCount, a_morph));
		logger::info("MorphDataBase : Registered {} for {} / {}", morphName, a_morph.morphBasePath, a_morph.vertexCount);
		return true;
	}

	const MorphDataBase::Morph* MorphDataBase::GetMorphData(const lString& a_morphBasePath) const
	{
		auto found = morphData.find(a_morphBasePath);
		if (found != morphData.end())
			return &found->second;
		return nullptr;
	}
	const MorphDataBase::Morph* MorphDataBase::GetMorphData(const std::uint32_t a_vertexCount) const
    {
		auto found = morphDataAlt.find(a_vertexCount);
		if (found != morphDataAlt.end())
			return &found->second;
		return nullptr;
	}

	bool MorphDataBaseManager::Register(const lString& a_morphBasePath, const lString& a_morphPath)
	{
		if (a_morphBasePath.empty() || a_morphPath.empty())
			return false;

        RE::BSFixedString newBasePath = ("Meshes\\" + a_morphBasePath).c_str();
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

        RE::BSFixedString newPath = ("Meshes\\" + a_morphPath).c_str();

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
			lString morphName_(morphName);

			float mult = 0.0f;
			Read(&file, &mult, sizeof(mult));

			MorphDataBase::Morph morph = { 
				.morphBasePath = fixPath(a_morphBasePath),
				.vertexCount = vertexCount,
				.multiplier = mult
			};

			for (std::int32_t v = 0; v < vertexCount; v++)
			{
				MorphDataBase::Morph::Vertex vert;
				Read(&file, &vert, sizeof(vert));
				morph.vertices.emplace_back(vert.GetXMVector());
			}

            std::lock_guard lg(dbLock);
            auto found = db.find(morphName_);
            if (found != db.end())
				found->second->Register(morph);
			else {
                auto newMorphDataBase = std::make_shared<MorphDataBase>(morphName_);
				newMorphDataBase->Register(morph);
                db.insert(std::make_pair(morphName_, newMorphDataBase));
			}
			logger::debug("Registered {} morph", morphName_);
		}
		logger::info("Registered {} file", a_morphBasePath);
		return true;
	}

	const MorphDataBase::Morph* MorphDataBaseManager::GetMorphData(const lString& morphName, const lString& a_morphBasePath) const
    {
        auto found = db.find(morphName);
        if (found != db.end())
			return found->second->GetMorphData(a_morphBasePath);
		return nullptr;
	}
    const MorphDataBase::Morph* MorphDataBaseManager::GetMorphData(const lString& morphName, const std::uint32_t vertexCount) const
    {
        auto found = db.find(morphName);
        if (found != db.end())
			return found->second->GetMorphData(vertexCount);
		return nullptr;
	}

	bool MorphDataBaseManager::IsValidMorphName(const lString& morphName)
    {
        return db.find(morphName) != db.end();
    }
}
