#include "MorphManager.h"

namespace Mus {

	void ErrorLogger::FlushMorphDataErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName, std::string morphBasePath)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} couldn't get morph data for {} / {}", actorID, geometryName, morphName, morphBasePath);
		AssignErrorLog(actorID, geometryName, morphName);
	}
	void ErrorLogger::FlushDynamicTriErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} failed to get dynamic shape", actorID, geometryName);
		AssignErrorLog(actorID, geometryName, morphName);
	}
	void ErrorLogger::FlushDynamicVerticesErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} failed to get dynamic vertices", actorID, geometryName);
		AssignErrorLog(actorID, geometryName, morphName);
	}
	void ErrorLogger::FlushVertexCountErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName, std::uint32_t geoVertexCount, std::size_t triVertexCount)
	{
		if (FindErrorLog(actorID, geometryName, morphName))
			return;
		logger::error("{:x} : {} / {} <=> {} vertex count is invalid", actorID, geometryName, geoVertexCount, triVertexCount);
		AssignErrorLog(actorID, geometryName, morphName);
	}

	bool ErrorLogger::FindErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName)
	{
		return errorMap[actorID][geometryName].find(morphName) != errorMap[actorID][geometryName].end();
	}
	void ErrorLogger::AssignErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName)
	{
		errorMap[actorID][geometryName].insert(morphName);
	}

	bool MorphManagerRecord::Recalculate(RE::BSGeometry* a_geometry)
	{
		if (!a_geometry)
			return false;

		auto vertexDesc = a_geometry->GetGeometryRuntimeData().vertexDesc;
		if (!vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_NORMAL) && !vertexDesc.HasFlag(RE::BSGraphics::Vertex::VF_TANGENT))
			return false;

		auto& skinInstance = a_geometry->GetGeometryRuntimeData().skinInstance;
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

		auto& newPartition = newSkinPartition->partitions[0];
		std::uint32_t vertexCount = newSkinPartition->vertexCount;
		std::uint32_t vertexSize = newPartition.vertexDesc.GetSize();

		RE::BSDynamicTriShape* dynamicTriShape = a_geometry->AsDynamicTriShape();
		RE::BSTriShape* triShape = a_geometry->AsTriShape();

		if (dynamicTriShape)
			dynamicTriShape->GetDynamicTrishapeRuntimeData().lock.Lock();

		std::unordered_set<std::uint16_t> lockVertices;
		RE::NiIntegersExtraData* extraData = static_cast<RE::NiIntegersExtraData*>(a_geometry->GetExtraData("LOCKEDNORM"));
		if (extraData)
		{
			for (std::uint32_t i = 0; i < extraData->size; ++i)
			{
				lockVertices.insert(static_cast<std::uint16_t>(extraData->value[i]));
			}
		}

		std::uint32_t numVertices = vertexCount;
		if (triShape)
			numVertices = triShape->GetTrishapeRuntimeData().vertexCount;

		if (!skinPartition->partitions[0].buffData || !skinPartition->partitions[0].buffData->rawVertexData)
			return false;
		std::uint8_t* vertexData = skinPartition->partitions[0].buffData->rawVertexData;

		std::vector<DirectX::XMFLOAT3*> vertices(numVertices);
		std::vector<DirectX::PackedVector::XMHALF2> uvs(numVertices);
		for (std::uint16_t i = 0; i < numVertices; i++)
		{
			if (dynamicTriShape)
				vertices[i] = reinterpret_cast<DirectX::XMFLOAT3*>(&((std::uint8_t*)dynamicTriShape->GetDynamicTrishapeRuntimeData().dynamicData)[i]);
			else
				vertices[i] = reinterpret_cast<DirectX::XMFLOAT3*>(&skinPartition->partitions[0].buffData->rawVertexData[i * vertexSize]);

			auto uv = reinterpret_cast<DirectX::XMFLOAT2*>(&skinPartition->partitions[0].buffData->rawVertexData[i * vertexSize + 4 + 4 + 4 + 4]);
			uvs[i] = DirectX::PackedVector::XMHALF2(uv->x, uv->y);
		}

		if (dynamicTriShape) dynamicTriShape->GetDynamicTrishapeRuntimeData().lock.Unlock();

		for (std::uint32_t i = 1; i < newSkinPartition->numPartitions; ++i)
		{
			auto& newPartitionAlt = newSkinPartition->partitions[i];
			memcpy(newPartitionAlt.buffData->rawVertexData, newPartition.buffData->rawVertexData, vertexCount * vertexSize);
		}

		newSkinPartition->DecRefCount();

		return true;
	}
	bool MorphManagerRecord::Update(std::string a_morphBasePath, RE::BSGeometry* a_geometry)
	{
		if (a_morphBasePath.empty() || !a_geometry)
			return false;

		//PerformaceLog(std::to_string(id) + a_geometry->name.c_str() + "::MorphManagerRecord::" + __func__, false);

		const MorphDataBase::Morph* morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, a_morphBasePath);
		if (!morphData)
		{
			ErrorLogger::GetSingleton().FlushMorphDataErrorLog(id, a_geometry->name.c_str(), morphName, a_morphBasePath);
			return false;
		}
		auto dynamicShape = a_geometry->AsDynamicTriShape();
		if (!dynamicShape)
		{
			ErrorLogger::GetSingleton().FlushDynamicTriErrorLog(id, a_geometry->name.c_str(), morphName);
			return false;
		}

		DirectX::XMFLOAT4* dynamicVertices = reinterpret_cast<DirectX::XMFLOAT4*>(dynamicShape->GetDynamicTrishapeRuntimeData().dynamicData);
		if (!dynamicVertices)
		{
			ErrorLogger::GetSingleton().FlushDynamicVerticesErrorLog(id, a_geometry->name.c_str(), morphName);
			return false;
		}
		std::uint32_t vertexCount = dynamicShape->GetDynamicTrishapeRuntimeData().dataSize / 16;
		if (vertexCount == 0 || morphData->vertices.size() != vertexCount)
		{
			ErrorLogger::GetSingleton().FlushVertexCountErrorLog(id, a_geometry->name.c_str(), morphName, vertexCount, morphData->vertices.size());
			return false;
		}

		float mvalue = morphData->multiplier * fvalue;

		/*if (Config::GetSingleton().GetEnableGPUMode())
		{
			MorphProcess newMorphProcess;
			newMorphProcess.Initial(dynamicVertices, morphData->vertices, mvalue);
			newMorphProcess.Run();
		}
		else
		{*/
			for (std::size_t i = 0; i < vertexCount; i++)
			{
				auto& vert = morphData->vertices[i];
				dynamicVertices[i].x += vert.x * mvalue;
				dynamicVertices[i].y += vert.y * mvalue;
				dynamicVertices[i].z += vert.z * mvalue;
			}
		//}

		//PerformaceLog(std::to_string(id) + a_geometry->name.c_str() + "::MorphManagerRecord::" + __func__, true, true, vertexCount);
		logger::debug("{} vertexData applied", a_geometry->name.c_str());
		return true;
	}
	bool MorphManagerRecord::Update(std::vector<MorphGeoData>& a_morphGeoData)
	{
		if (a_morphGeoData.size() == 0)
			return false;

		if (value == 0)
			return true;

		for (auto& data : a_morphGeoData) {
			Update(data.morphBasePath, data.geometry);
		}
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
			fvalue = (double)value * 0.01f;
			return true;
		}
		float t = static_cast<float>(lerpTask.totalProcessTime) / static_cast<float>(lerpTask.endTime);
		value = static_cast<std::int32_t>(lerpTask.startValue + (lerpTask.endValue - lerpTask.startValue) * t);
		fvalue = (double)value * 0.01f;
		return true;
	}

	void MorphManager::Revert(std::string category)
	{
		m_lock.lock();
		if (category.empty())
		{
			clear();
		}
		else
		{
			category = lowLetter(category);
			auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
			for (auto morphName : morphNames)
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
			return;
		//PerformaceLog(std::to_string(id) + "::MorphManager::" + __func__, false);

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;

		RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
		std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

		auto faceNode = actor->GetFaceNode();
		if (!faceNode)
			return;

		std::vector<MorphManagerRecord::MorphGeoData> morphGeoDatas;
		std::uint32_t j = 0;
		for (std::uint32_t i = 0; i < numHeadParts; i++) {
			if (!headParts[i] || headParts[i]->formEditorID.empty() || headParts[i]->model.empty() || headParts[i]->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].model.empty())
				continue;
			for (; j < faceNode->children.size(); j++)
			{
				RE::NiAVObject* obj = faceNode->children[j].get();
				if (!obj || obj->name != headParts[i]->formEditorID)
					continue;
				morphGeoDatas.emplace_back(
					MorphManagerRecord::MorphGeoData{
					fixPath(headParts[i]->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].model.c_str()),
					skyrim_cast<RE::BSGeometry*>(obj)
					}
				);
				break;
			}
		}

		for (auto& map : *this)
		{
			map.second.UpdateLerpValue(processTime);
			map.second.Update(morphGeoDatas);
		}
		//PerformaceLog(std::to_string(id) + "::MorphManager::" + __func__, true, true, this->size());
	}

	bool MorphManager::SetValue(std::string a_morphName, std::int32_t a_value, std::int32_t a_lerpTime)
	{
		a_morphName = lowLetter(a_morphName);
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
	bool MorphManager::SetValue(std::string a_morphName, std::int32_t a_value)
	{
		return SetValue(a_morphName, a_value, 0);
	}

	int32_t MorphManager::GetValue(std::string a_morphName) const
	{
		a_morphName = lowLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end())
			return found->second.GetValue();
		return 0;
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
