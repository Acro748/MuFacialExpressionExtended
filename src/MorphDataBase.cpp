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

        logger::info("Register {} file for {}...", a_morphPath, a_morphBasePath);

		const lString basePath = "Meshes\\" + a_morphBasePath;
        RE::BSResourceNiBinaryStream baseFile(basePath);
        if (!baseFile.good() || !baseFile.stream)
        {
            logger::error("Unable to open the file : {}", basePath);
            return false;
        }

        auto baseErrorCode = baseFile.stream->DoOpen();
        if (baseErrorCode != RE::BSResource::ErrorCode::kNone)
        {
            logger::error("Failed to open the file : {} ({})", basePath, magic_enum::enum_name(baseErrorCode).data());
            return false;
        }

        std::uint64_t baseReadBytes;
		char baseHeader[0x08];
        baseFile.stream->DoRead(baseHeader, 0x08, baseReadBytes);
        if (strncmp(baseHeader, "FRTRI003", 8) != 0)
        {
            logger::error("Invalid tri header : {}", basePath);
            return false;
        }

		std::int32_t baseVertexCount;
        baseFile.stream->DoRead(&baseVertexCount, sizeof(baseVertexCount), baseReadBytes);
		logger::debug("BaseVertexCount : {}", baseVertexCount);

        const lString newPath = "Meshes\\" + a_morphPath;
        RE::BSResourceNiBinaryStream file(newPath);
        if (!file.good() || !file.stream)
        {
            logger::error("Unable to open the file : {}", newPath);
            return false;
        }

        auto errorCode = file.stream->DoOpen();
        if (errorCode != RE::BSResource::ErrorCode::kNone)
        {
            logger::error("Failed to open the file : {} ({})", newPath, magic_enum::enum_name(errorCode).data());
            return false;
        }

        std::uint64_t readBytes;
		char header[0x08];
        file.stream->DoRead(header, 0x08, readBytes);
		if (strncmp(header, "FRTRI003", 8) != 0)
        {
            logger::error("Invalid tri header : {}", newPath);
            return false;
        }

		std::int32_t vertexCount;
        file.stream->DoRead(&vertexCount, sizeof(vertexCount), readBytes);
		logger::debug("vertexCount : {}", vertexCount);
		if (baseVertexCount != vertexCount)
        {
            logger::error("Does not match the vertex count : {} / {}", basePath, newPath);
            return false;
        }

		std::uint32_t polytris = 0, polyquads = 0, lverts = 0, lsurfs = 0,
			uvverts = 0, flags = 0, morphCount = 0, statMorphs = 0,
			statVerts = 0, unk7 = 0, unk8 = 0, unk9 = 0, unk10 = 0;

        file.stream->DoRead(&polytris, sizeof(polytris), readBytes);
        logger::debug("polytris : {}", polytris);
        file.stream->DoRead(&polyquads, sizeof(polyquads), readBytes);
        logger::debug("polyquads : {}", polyquads);
        file.stream->DoRead(&lverts, sizeof(lverts), readBytes);
        logger::debug("lverts : {}", lverts);
        file.stream->DoRead(&lsurfs, sizeof(lsurfs), readBytes);
        logger::debug("lsurfs : {}", lsurfs);
        file.stream->DoRead(&uvverts, sizeof(uvverts), readBytes);
        logger::debug("uvverts : {}", uvverts);
        file.stream->DoRead(&flags, sizeof(flags), readBytes);
        logger::debug("flags : {}", flags);
        file.stream->DoRead(&morphCount, sizeof(morphCount), readBytes);
        logger::debug("morphCount : {}", morphCount);
        file.stream->DoRead(&statMorphs, sizeof(statMorphs), readBytes);
        logger::debug("statMorphs : {}", statMorphs);
        file.stream->DoRead(&statVerts, sizeof(statVerts), readBytes);
        logger::debug("statVerts : {}", statVerts);
        file.stream->DoRead(&unk7, sizeof(unk7), readBytes);
        logger::debug("unk7 : {}", unk7);
        file.stream->DoRead(&unk8, sizeof(unk8), readBytes);
        logger::debug("unk8 : {}", unk8);
        file.stream->DoRead(&unk9, sizeof(unk9), readBytes);
        logger::debug("unk9 : {}", unk9);
        file.stream->DoRead(&unk10, sizeof(unk10), readBytes);
		logger::debug("unk10 : {}", unk10);

		// Skip reference verts
        file.stream->DoSeek(vertexCount * 3 * sizeof(float), RE::BSResource::SeekMode::kCur, readBytes);

		// Skip polytris
        file.stream->DoSeek(polytris * 3 * sizeof(std::uint32_t), RE::BSResource::SeekMode::kCur, readBytes);

		// Skip uvs
        if (uvverts > 0)
            file.stream->DoSeek(uvverts * 2 * sizeof(float), RE::BSResource::SeekMode::kCur, readBytes);

		// Skip texture coords
        file.stream->DoSeek(polytris * 3 * sizeof(std::uint32_t), RE::BSResource::SeekMode::kCur, readBytes);

		for (std::uint32_t i = 0; i < morphCount; i++)
		{
            std::uint32_t strLen = 0;
            file.stream->DoRead(&strLen, sizeof(strLen), readBytes);

			lString morphName = "";
			for (std::uint32_t l = 0; l < strLen; l++)
            {
                char c;
                file.stream->DoRead(&c, sizeof(c), readBytes);
                if (l < strLen - 1)
					morphName += c;
			}

			float mult = 0.0f;
            file.stream->DoRead(&mult, sizeof(mult), readBytes);

			MorphDataBase::Morph morph = { 
				.morphBasePath = fixPath(a_morphBasePath),
				.vertexCount = vertexCount,
				.multiplier = mult
			};

			for (std::int32_t v = 0; v < vertexCount; v++)
			{
                MorphDataBase::Morph::Vertex vert;
                file.stream->DoRead(&vert, sizeof(vert), readBytes);
				morph.vertices.emplace_back(vert.GetXMVector());
			}

            std::lock_guard lg(dbLock);
            auto found = db.find(morphName);
            if (found != db.end())
				found->second->Register(morph);
			else {
                auto newMorphDataBase = std::make_shared<MorphDataBase>(morphName);
				newMorphDataBase->Register(morph);
                db.insert(std::make_pair(morphName, newMorphDataBase));
			}
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
