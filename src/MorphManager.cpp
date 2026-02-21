#include "MorphManager.h"
#include "xxhash.h"

namespace Mus {

	void ErrorLogger::FlushMorphDataErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName, const std::string& morphBasePath, const std::uint32_t vertexCount)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} couldn't get morph data for {} / {} / {}", actorID, geometryName, morphName, morphBasePath, vertexCount);
		AssignErrorLog(actorID, geometryName, morphName);
	}
    void ErrorLogger::FlushDynamicTriErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} failed to get dynamic shape", actorID, geometryName);
		AssignErrorLog(actorID, geometryName, morphName);
	}
    void ErrorLogger::FlushDynamicVerticesErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} failed to get dynamic vertices", actorID, geometryName);
		AssignErrorLog(actorID, geometryName, morphName);
	}
    void ErrorLogger::FlushVertexCountErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName, const std::uint32_t geoVertexCount, const std::size_t triVertexCount)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} / {} <=> {} vertex count is invalid", actorID, geometryName, geoVertexCount, triVertexCount);
		AssignErrorLog(actorID, geometryName, morphName);
	}

	bool ErrorLogger::FindErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName)
	{
		return errorMap[actorID][geometryName].find(morphName) != errorMap[actorID][geometryName].end();
	}
    void ErrorLogger::AssignErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName)
	{
		errorMap[actorID][geometryName].insert(morphName);
	}

    bool MorphManagerRecord::Update(const MorphGeoData& data)
	{
		const MorphDataBase::Morph* morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, data.morphBasePath);
		if (!morphData)
		{
			if (Config::GetSingleton().GetMorphByVertexCount())
				morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, data.vertexCount);
			if (!morphData)
			{
                ErrorLogger::GetSingleton().FlushMorphDataErrorLog(id, data.dynamicShape->name.c_str(), morphName, data.morphBasePath, data.vertexCount);
				return false;
			}
		}

		if (morphData->vertices.size() != data.vertexCount)
		{
            ErrorLogger::GetSingleton().FlushVertexCountErrorLog(id, data.dynamicShape->name.c_str(), morphName, data.vertexCount, morphData->vertices.size());
			return false;
		}

		const float mvalue = morphData->multiplier * (fvalue - pfvalue * data.isSameHash);
        if (mvalue == 0.0f)
            return true;

        const DirectX::XMVECTOR vmvalue = DirectX::XMVectorReplicate(mvalue);
        const DirectX::XMVECTOR* __restrict morphVert = morphData->vertices.data();
        {
            RE::BSSpinLockGuard slg(data.dynamicShape->GetDynamicTrishapeRuntimeData().lock);
            for (std::size_t i = 0; i < data.vertexCount; i++)
            {
                data.dynamicVertices[i] = DirectX::XMVectorMultiplyAdd(morphVert[i], vmvalue, data.dynamicVertices[i]);
            }
        }
        logger::debug("{} vertexData applied", data.dynamicShape->name.c_str());
		return true;
	}
    bool MorphManagerRecord::Update(const std::vector<MorphGeoData>& a_morphGeoData)
	{
		if (a_morphGeoData.size() == 0)
			return false;

        std::lock_guard lg(valueLock);
		if (value == 0)
            return false;
		for (auto& data : a_morphGeoData) {
			Update(data);
		}
        pvalue = value;
        pfvalue = static_cast<float>(pvalue) * 0.01f;
		return true;
	}

	void MorphManagerRecord::SetValue(std::int32_t a_value, std::int32_t a_lerpTime)
	{
        std::lock_guard lg(valueLock);
		lerpTask.totalProcessTime = 0;
		lerpTask.endTime = a_lerpTime;
		lerpTask.startValue = value;
		lerpTask.endValue = a_value;
	}
	void MorphManagerRecord::SetValue(std::int32_t a_value)
	{
		SetValue(a_value, 0);
	}
	bool MorphManagerRecord::UpdateLerpValue(std::clock_t processTime)
    {
        std::lock_guard lg(valueLock);
		if (value == lerpTask.endValue)
			return false;
		lerpTask.totalProcessTime += processTime;
		if (lerpTask.endTime <= lerpTask.totalProcessTime)
		{
			value = lerpTask.endValue;
		}
        else
        {
            const float t = static_cast<float>(lerpTask.totalProcessTime) / static_cast<float>(lerpTask.endTime);
            value = static_cast<std::int32_t>(lerpTask.startValue + (lerpTask.endValue - lerpTask.startValue) * t);
        }
        fvalue = static_cast<float>(value) * 0.01f;
		return true;
	}

	void MorphManager::Revert(const lString& category)
	{
		if (category.empty())
        {
            std::lock_guard lg(recordLock);
            record.clear();
		}
		else
		{
            const auto& morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
            std::lock_guard lg(recordLock);
			for (const auto& morphName : morphNames)
			{
                auto found = record.find(morphName);
                if (found != record.end())
					found->second->SetValue(0);
			}
		}
	}
	void MorphManager::Update(std::clock_t processTime)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
        if (!actor || !actor->loadedData || !actor->loadedData->data3D)
        {
            faceGenMeshes.clear();
            morphGeoDatas.clear();
            return;
        }
		PerformaceLog(GetHexStr(id) + "::MorphManager::" + __func__, false);

		if (isNeedUpdateFacegen)
		{
            auto facegen = actor->GetFaceNode();
            if (!facegen)
                return;
            RE::TESNPC* actorBase = actor->GetActorBase();
            if (!actorBase)
                return;

            RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
            std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

            faceGenMeshes.clear();
            for (std::uint32_t j = 0; j < facegen->children.size(); j++)
            {
                RE::NiAVObject* obj = facegen->children[j].get();
                if (!obj || obj->name.empty())
                    continue;
                auto geo = obj->AsGeometry();
                if (!geo)
                    continue;
                auto dynamicTri = geo->AsDynamicTriShape();
                if (!dynamicTri)
                    continue;
                faceGenMeshes[obj->name.c_str()] = dynamicTri;
            }

            morphGeoDatas.clear();
            for (std::uint32_t i = 0; i < numHeadParts; i++)
            {
                if (!headParts[i] || headParts[i]->formEditorID.empty() || headParts[i]->model.empty() || headParts[i]->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].model.empty())
                    continue;
                auto dynamicTri = faceGenMeshes[headParts[i]->GetFormEditorID()];
                if (!dynamicTri)
                    continue;
                DirectX::XMVECTOR* __restrict dynamicVertices = reinterpret_cast<DirectX::XMVECTOR*>(dynamicTri->GetDynamicTrishapeRuntimeData().dynamicData);
                if (!dynamicVertices)
                    continue;
                const std::uint32_t vertexCount = dynamicTri->GetDynamicTrishapeRuntimeData().dataSize / 16;
                if (vertexCount == 0)
                    continue;
                MorphManagerRecord::MorphGeoData data = {
                    .morphBasePath = fixPath(headParts[i]->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].model.c_str()),
                    .dynamicShape = dynamicTri,
                    .dynamicVertices = dynamicVertices,
                    .vertexCount = vertexCount,
					.isSameHash = false
				};
                morphGeoDatas.push_back(std::move(data));
            }
            isNeedUpdateFacegen = false;
		}

        for (auto& data : morphGeoDatas)
        {
            auto newHash = GetHash(data);
            data.isSameHash = (newHash == lastHash[data.dynamicShape]);
        }
        bool isUpdated = false;
        for (const auto& map : record)
		{
			map.second->UpdateLerpValue(processTime);
            isUpdated |= map.second->Update(morphGeoDatas);
		}
        if (!isUpdated)
            return;
		for (const auto& data : morphGeoDatas)
		{
            lastHash[data.dynamicShape] = GetHash(data);
            //Recalculate(data);
		}
		PerformaceLog(GetHexStr(id) + "::MorphManager::" + __func__, true, PerformanceCheckAverage, morphGeoDatas.size());
	}

	bool MorphManager::SetValue(const lString& a_morphName, std::int32_t a_value, std::int32_t a_lerpTime)
	{
        std::lock_guard lg(recordLock);
        auto found = record.find(a_morphName);
        if (found != record.end())
		{
			found->second->SetValue(a_value, a_lerpTime);
			return true;
		}
        if (!MorphDataBaseManager::GetSingleton().IsValidMorphName(a_morphName))
		{
			logger::error("Couldn't get {} morph data", a_morphName);
			return false;
		}
        MorphManagerRecordPtr newMorphManagerRecord = std::make_shared<MorphManagerRecord>(id, a_morphName);
		newMorphManagerRecord->SetValue(a_value, a_lerpTime);
        record.insert(std::make_pair(a_morphName, newMorphManagerRecord));
		return true;
	}
    bool MorphManager::SetValue(const lString& a_morphName, std::int32_t a_value)
	{
		return SetValue(a_morphName, a_value, 0);
	}

	std::int32_t MorphManager::GetValue(const lString& a_morphName, bool destination) const
    {
        std::lock_guard lg(recordLock);
        auto found = record.find(a_morphName);
        if (found != record.end())
            return found->second->GetValue(destination);
		return 0;
	}

    /*bool MorphManager::Recalculate(const MorphManagerRecord::MorphGeoData& data)
    {
        if (!Config::GetSingleton().GetRecalculate() || data.isSameHash)
            return false;

        auto vertexDesc = data.dynamicShape->GetGeometryRuntimeData().vertexDesc;
        const bool hasNormal = vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_NORMAL);
        const bool hasTangent = vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_TANGENT);
        if (!hasNormal && !hasTangent)
            return false;

        auto& skinInstance = data.dynamicShape->GetGeometryRuntimeData().skinInstance;
        if (!skinInstance)
            return false;

        auto& skinPartition = skinInstance->skinPartition;
        if (!skinPartition)
            return false;

        RE::NiPointer<RE::NiObject> newNiObject;
        skinPartition->CreateDeepCopy(newNiObject);
        if (!newNiObject)
            return false;
        RE::NiSkinPartition* newSkinPartition = netimmerse_cast<RE::NiSkinPartition*>(newNiObject.get());
        if (!newSkinPartition)
            return false;

        if (!skinPartition->partitions[0].buffData || !skinPartition->partitions[0].buffData->rawVertexData)
            return false;

        data.dynamicShape->GetDynamicTrishapeRuntimeData().lock.Lock();

        const std::uint32_t vertexSize = skinPartition->partitions[0].vertexDesc.GetSize();

        std::unordered_set<std::uint32_t> lockVertices;
        RE::NiIntegersExtraData* extraData = static_cast<RE::NiIntegersExtraData*>(data.dynamicShape->GetExtraData("LOCKEDNORM"));
        if (extraData)
        {
            for (std::uint32_t i = 0; i < extraData->size; ++i)
            {
                lockVertices.insert(static_cast<std::uint32_t>(extraData->value[i]));
            }
        }

        std::vector<std::uint16_t> indices;
		for (auto& partition : skinPartition->partitions)
        {
            const std::size_t indicesCount = (std::size_t)partition.triangles * 3;
            const std::size_t size = indices.size();
            indices.resize(size + indicesCount);
            std::memcpy(indices.data() + indices.size(), partition.triList, sizeof(std::uint16_t) * indicesCount);
        }

        const float smoothCos = std::cosf(DirectX::XMConvertToRadians(60.0f));

        std::vector<DirectX::XMFLOAT3> vertices(data.vertexCount);
        std::vector<DirectX::XMFLOAT2> uvs(data.vertexCount);
        std::vector<DirectX::XMFLOAT3> normals(data.vertexCount);
        std::vector<DirectX::XMFLOAT3> tangents(data.vertexCount);
        std::vector<DirectX::XMFLOAT3> bitangents(data.vertexCount);

        const float convert = 1.0f / 255.0f;
        std::uint8_t* vertexData = skinPartition->partitions[0].buffData->rawVertexData;
        const bool hasVertex = vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_VERTEX);
        const bool hasUV = vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_VERTEX);
        for (std::uint32_t i = 0; i < data.vertexCount; i++)
        {
            std::uint8_t* block = &vertexData[i * vertexSize];
            DirectX::XMStoreFloat3(&vertices[i], data.dynamicVertices[i]);
            if (hasVertex)
                block += 12;
            if (hasUV)
            {
                uvs[i].x = DirectX::PackedVector::XMConvertHalfToFloat(*reinterpret_cast<std::uint16_t*>(block));
                uvs[i].y = DirectX::PackedVector::XMConvertHalfToFloat(*reinterpret_cast<std::uint16_t*>(block + 2));
                block += 4;
            }
        }

        for (std::uint32_t i = 0; i < data.vertexCount; i++)
        {
            if (lockVertices.find(i) != lockVertices.end())
                continue;
            std::uint8_t* block = &vertexData[i * vertexSize];
            DirectX::XMVectorSetW(data.dynamicVertices[i], bitangents[i].x);
            if (hasVertex)
                block += 12;
            if (hasUV)
                block += 4;
            if (hasNormal)
            {
                *block = static_cast<std::uint8_t>((normals[i].x + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((normals[i].y + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((normals[i].z + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((bitangents[i].y + 1.0f) * 0.5f * 255.0f);
                block += 1;
            }
            if (hasTangent)
            {
                *block = static_cast<std::uint8_t>((tangents[i].x + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((tangents[i].y + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((tangents[i].z + 1.0f) * 0.5f * 255.0f);
                block += 1;
                *block = static_cast<std::uint8_t>((bitangents[i].z + 1.0f) * 0.5f * 255.0f);
            }
        }
        data.dynamicShape->GetDynamicTrishapeRuntimeData().lock.Unlock();
        return true;
    }*/

	std::uint64_t MorphManager::GetHash(const MorphManagerRecord::MorphGeoData& data)
    {
        return XXH3_64bits(data.dynamicVertices, data.vertexCount * sizeof(DirectX::XMVECTOR));
	}

	std::uint32_t MorphManager::UpdateModelFace(RE::NiAVObject* obj) {
		using func_t = decltype(&MorphManager::UpdateModelFace);
		REL::VariantID offset(26458, 27044, 0x003EB710);
		REL::Relocation<func_t> func{ offset };
		return func(obj);
	}

	std::vector<MorphManager::ActiveMorphSet> MorphManager::GetAllActiveMorphs()
	{
        std::vector<ActiveMorphSet> result;
        std::lock_guard lg(recordLock);
        for (const auto& m : record)
		{
			if (m.second->GetValue() != 0)
			{
                ActiveMorphSet newActiveMorphSet = {
                    .morphName = m.second->GetMorphName(), 
                    .value = m.second->GetValue()
                };
				result.push_back(newActiveMorphSet);
			}
		}
		return result;
	}
}
