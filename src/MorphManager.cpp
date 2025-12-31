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
		//PerformaceLog(GetHexStr(id) + data.dynamicShape->name.c_str() + "::MorphManagerRecord::" + __func__, false);

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

        data.dynamicShape->GetDynamicTrishapeRuntimeData().lock.Lock();
        const DirectX::XMVECTOR vmvalue = DirectX::XMVectorReplicate(mvalue);
        const DirectX::XMVECTOR* __restrict morphVert = morphData->vertices.data();
        for (std::size_t i = 0; i < data.vertexCount; i++)
        {
            data.dynamicVertices[i] = DirectX::XMVectorMultiplyAdd(morphVert[i], vmvalue, data.dynamicVertices[i]);
        }
        data.dynamicShape->GetDynamicTrishapeRuntimeData().lock.Unlock();

		//PerformaceLog(GetHexStr(id) + data.dynamicShape->name.c_str() + "::MorphManagerRecord::" + __func__, true, PerformanceCheckAverage, vertexCount);
        logger::debug("{} vertexData applied", data.dynamicShape->name.c_str());
		return true;
	}
    bool MorphManagerRecord::Update(const std::vector<MorphGeoData>& a_morphGeoData)
	{
		if (a_morphGeoData.size() == 0)
			return false;

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
		if (value == lerpTask.endValue)
			return false;
		lerpTask.totalProcessTime += processTime;
		if (lerpTask.endTime <= lerpTask.totalProcessTime)
		{
			value = lerpTask.endValue;
		}
        else
        {
            float t = static_cast<float>(lerpTask.totalProcessTime) / static_cast<float>(lerpTask.endTime);
            value = static_cast<std::int32_t>(lerpTask.startValue + (lerpTask.endValue - lerpTask.startValue) * t);
        }
        fvalue = static_cast<float>(value) * 0.01f;
		return true;
	}

	void MorphManager::Revert(const std::string& category)
	{
		m_lock.lock();
		if (category.empty())
		{
			clear();
		}
		else
		{
            const auto& morphNames = morphNameEntry::GetSingleton().GetMorphNames(lowLetter(category));
			for (const auto& morphName : morphNames)
			{
				auto found = find(morphName);
				if (found != end())
					found->second.SetValue(0);
			}
		}
		m_lock.unlock();
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
		//PerformaceLog(GetHexStr(id) + "::MorphManager::" + __func__, false);

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
                MorphManagerRecord::MorphGeoData data{
                    fixPath(headParts[i]->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].model.c_str()),
                    dynamicTri,
                    dynamicVertices,
                    vertexCount,
					false
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
		for (auto& map : *this)
		{
			map.second.UpdateLerpValue(processTime);
            isUpdated |= map.second.Update(morphGeoDatas);
		}
        if (!isUpdated)
            return;
		for (auto& data : morphGeoDatas)
		{
            lastHash[data.dynamicShape] = GetHash(data);
            //Recalculate(data);
		}
		//PerformaceLog(GetHexStr(id) + "::MorphManager::" + __func__, true, PerformanceCheckAverage, this->size());
	}

	bool MorphManager::SetValue(const std::string& a_morphName, std::int32_t a_value, std::int32_t a_lerpTime)
	{
		auto found = find(a_morphName);
		if (found != end())
		{
			found->second.SetValue(a_value, a_lerpTime);
			return true;
		}
		if (MorphDataBaseManager::GetSingleton().find(a_morphName) == MorphDataBaseManager::GetSingleton().end())
		{
			logger::error("Couldn't get {} morph data", a_morphName);
			return false;
		}
		MorphManagerRecord newMorphManagerRecord = MorphManagerRecord(id, a_morphName);
		newMorphManagerRecord.SetValue(a_value, a_lerpTime);
		insert(std::make_pair(a_morphName, newMorphManagerRecord));
		return true;
	}
    bool MorphManager::SetValue(const std::string& a_morphName, std::int32_t a_value)
	{
		return SetValue(a_morphName, a_value, 0);
	}

	int32_t MorphManager::GetValue(const std::string& a_morphName) const
	{
		auto found = find(a_morphName);
		if (found != end())
			return found->second.GetValue();
		return 0;
	}

    bool MorphManager::Recalculate(const MorphManagerRecord::MorphGeoData& data)
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

        const std::uint32_t triCount = indices.size() / 3;

		struct FaceNormal
        {
            std::uint32_t v0, v1, v2;
            DirectX::XMFLOAT3 normal;
        };
        std::vector<FaceNormal> faceNormals(triCount);
        std::vector<std::vector<std::uint32_t>> vertexToFaceMap(triCount);

        struct FaceTangent
        {
            std::uint32_t v0, v1, v2;
            DirectX::XMFLOAT3 tangent;
            DirectX::XMFLOAT3 bitangent;
        };
        std::vector<FaceTangent> faceTangents(triCount);

        constexpr float floatPrecision = 1e-6f;
        const DirectX::XMVECTOR emptyVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		struct PositionKey
        {
            std::int32_t x, y, z;
            bool operator==(const PositionKey& other) const
            {
                return x == other.x && y == other.y && z == other.z;
            }
        };
        struct PositionKeyHash
        {
            std::size_t operator()(const PositionKey& k) const
            {
                return std::hash<std::int32_t>()(k.x) ^
                       (std::hash<std::int32_t>()(k.y) << 1) ^
                       (std::hash<std::int32_t>()(k.z) << 2);
            }
        };
        auto MakePositionKey = [](const DirectX::XMFLOAT3& pos) -> PositionKey {
            return {
                std::int32_t(std::floor(pos.x * 0.0001f)),
                std::int32_t(std::floor(pos.y * 0.0001f)),
                std::int32_t(std::floor(pos.z * 0.0001f))};
        };
        // without uv seam
        std::unordered_map<PositionKey, std::unordered_map<std::uint32_t, bool>, PositionKeyHash> positionMap; // vertex, indices

        for (std::size_t i = 0; i < triCount; i++)
        {
            const std::size_t offset = i * 3;
            const std::uint32_t i0 = indices[offset + 0];
            const std::uint32_t i1 = indices[offset + 1];
            const std::uint32_t i2 = indices[offset + 2];

            const DirectX::XMFLOAT3& p0 = vertices[i0];
            const DirectX::XMFLOAT3& p1 = vertices[i1];
            const DirectX::XMFLOAT3& p2 = vertices[i2];

            const DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&p0);
            const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);
            const DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);

            const DirectX::XMFLOAT2& uv0 = uvs[i0];
            const DirectX::XMFLOAT2& uv1 = uvs[i1];
            const DirectX::XMFLOAT2& uv2 = uvs[i2];

            // Normal
            const DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(
                DirectX::XMVector3Cross(
                    DirectX::XMVectorSubtract(v1, v0),
                    DirectX::XMVectorSubtract(v2, v0)));
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, normalVec);
            faceNormals[i] = {i0, i1, i2, normal};

            vertexToFaceMap[i0].push_back(i);
            vertexToFaceMap[i1].push_back(i);
            vertexToFaceMap[i2].push_back(i);

            // Tangent / Bitangent
            const DirectX::XMVECTOR dp1 = DirectX::XMVectorSubtract(v1, v0);
            const DirectX::XMVECTOR dp2 = DirectX::XMVectorSubtract(v2, v0);

            const DirectX::XMFLOAT2 duv1 = {uv1.x - uv0.x, uv1.y - uv0.y};
            const DirectX::XMFLOAT2 duv2 = {uv2.x - uv0.x, uv2.y - uv0.y};

            float r = duv1.x * duv2.y - duv2.x * duv1.y;
            r = (fabs(r) < floatPrecision) ? 1.0f : 1.0f / r;

            const DirectX::XMVECTOR tangentVec = DirectX::XMVectorScale(
                DirectX::XMVectorSubtract(
                    DirectX::XMVectorScale(dp1, duv2.y),
                    DirectX::XMVectorScale(dp2, duv1.y)),
                r);

            const DirectX::XMVECTOR bitangentVec = DirectX::XMVectorScale(
                DirectX::XMVectorSubtract(
                    DirectX::XMVectorScale(dp2, duv1.x),
                    DirectX::XMVectorScale(dp1, duv2.x)),
                r);

            DirectX::XMFLOAT3 tangent, bitangent;
            DirectX::XMStoreFloat3(&tangent, tangentVec);
            DirectX::XMStoreFloat3(&bitangent, bitangentVec);
            faceTangents[i] = {i0, i1, i2, tangent, bitangent};

            positionMap[MakePositionKey(p0)][i0];
            positionMap[MakePositionKey(p1)][i1];
            positionMap[MakePositionKey(p2)][i2];
        }

        for (std::size_t i = 0; i < data.vertexCount; i++)
        {
            const DirectX::XMFLOAT3& pos = vertices[i];
            const DirectX::XMFLOAT2& uv = uvs[i];

            const PositionKey pkey = MakePositionKey(pos);
            const auto posIt = positionMap.find(pkey);
            if (posIt == positionMap.end())
                continue;

            DirectX::XMVECTOR nSum = emptyVector;
            DirectX::XMVECTOR tSum = emptyVector;
            DirectX::XMVECTOR bSum = emptyVector;

            DirectX::XMVECTOR nSelf = emptyVector;
            std::unordered_set<std::uint32_t> pastVertices;
            for (const auto& vi : posIt->second)
            {
                pastVertices.insert(vi.first);
                for (const auto& fi : vertexToFaceMap[vi.first])
                {
                    const auto& fn = faceNormals[fi];
                    DirectX::XMVECTOR fnVec = DirectX::XMLoadFloat3(&fn.normal);
                    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(fnVec, nSelf));
                    if (dot < smoothCos)
                        continue;
                    const auto& ft = faceTangents[fi];
                    nSum = DirectX::XMVectorAdd(nSum, fnVec);
                    tSum = DirectX::XMVectorAdd(tSum, DirectX::XMLoadFloat3(&ft.tangent));
                    bSum = DirectX::XMVectorAdd(bSum, DirectX::XMLoadFloat3(&ft.bitangent));
                }
            }
            if (DirectX::XMVector3Equal(nSum, emptyVector))
                continue;

            DirectX::XMVECTOR t = DirectX::XMVectorGetX(DirectX::XMVector3Length(tSum)) > floatPrecision ? DirectX::XMVector3Normalize(tSum) : emptyVector;
            DirectX::XMVECTOR b = DirectX::XMVectorGetX(DirectX::XMVector3Length(bSum)) > floatPrecision ? DirectX::XMVector3Normalize(bSum) : emptyVector;
            const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(nSum);

            if (!DirectX::XMVector3Equal(t, emptyVector))
            {
                t = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(t, DirectX::XMVectorScale(n, DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, t)))));
                b = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(n, t));
            }

            if (!DirectX::XMVector3Equal(t, emptyVector))
                DirectX::XMStoreFloat3(&tangents[i], t);
            if (!DirectX::XMVector3Equal(b, emptyVector))
                DirectX::XMStoreFloat3(&bitangents[i], b);
            DirectX::XMStoreFloat3(&normals[i], n);
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
    }

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
		for (const auto& m : *this)
		{
			if (m.second.GetValue() != 0)
			{
				ActiveMorphSet newActiveMorphSet;
				newActiveMorphSet.morphName = m.first;
				newActiveMorphSet.value = m.second.GetValue();
				result.push_back(newActiveMorphSet);
			}
		}
		return result;
	}
}
