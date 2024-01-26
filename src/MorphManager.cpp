#include "MorphManager.h"

namespace Mus {
	bool MorphManagerRecord::Apply(Vertices& vertices, RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry, float a_value)
	{
		if (!a_headpart || !a_geometry)
			return false;

		std::string morphBasePath = a_headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
		if (morphBasePath.empty())
			return false;
		morphBasePath = fixPath(morphBasePath);

		const MorphDataBase::Morph* morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, morphBasePath);
		if (!morphData)
		{
			logger::error("{} couldn't get morph data for {} / {}", a_geometry->name.c_str(), morphName, morphBasePath);
			return false;
		}

		auto vertexDatas = vertices.find(morphBasePath);
		if (vertexDatas == vertices.end())
		{
			logger::error("{} / {} invalid vertex data for morphData of {}", a_geometry->name.c_str(), morphBasePath, morphName);
			return false;
		}
		if (vertexDatas->second.size() != morphData->vertexCount)
		{
			logger::error("{} / {} wrong vertex count {} for morphData {} of {}", a_geometry->name.c_str(), morphBasePath, vertexDatas->second.size(), morphData->vertexCount, morphName);
			return false;
		}
		auto& vertexData = vertexDatas->second;

		for (std::size_t i = 0; i < morphData->vertices.size(); i++)
		{
			auto& vert = morphData->vertices.at(i);

			vertexData.at(i).x += (float)((double)vert.x * (double)morphData->multiplier * (double)a_value);
			vertexData.at(i).y += (float)((double)vert.y * (double)morphData->multiplier * (double)a_value);
			vertexData.at(i).z += (float)((double)vert.z * (double)morphData->multiplier * (double)a_value);
		}
		logger::debug("{} vertexData applied", a_geometry->name.c_str());
		return true;
	}
	bool MorphManagerRecord::Apply(Vertices& vertices, RE::BGSHeadPart** a_headparts, std::uint8_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode, int32_t a_value)
	{
		if (!a_headparts || !a_facegenNinode)
			return false;

		float newValue = (a_value - value) * 0.01f;
		if (IsEqual(newValue, 0.0f))
		{
			logger::trace("value is not changed");
			return true;
		}
		for (std::uint8_t i = 0; i < a_numHeadparts; i++) {
			auto headpart = a_headparts[i];
			if (!headpart || headpart->formEditorID.empty())
				continue;
			Apply(vertices, headpart, skyrim_cast<RE::BSGeometry*>(a_facegenNinode->GetObjectByName(headpart->formEditorID)), newValue);
		}
		value = a_value;
		return true;
	}

	bool MorphManager::Initial(int32_t slot)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return false;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return false;

		RE::BSFaceGenNiNode* facegenNiNode = actor->GetFaceNode();
		if (!facegenNiNode)
			return false;

		m_lock.lock();
		if (slot == -1)
		{
			vertices.clear();
			orig_vertices.clear();
		}
		RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
		std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

		for (std::uint8_t i = 0; i < numHeadParts; i++) {
			auto headpart = headParts[i];
			if (!headpart || headpart->formEditorID.empty())
				continue;

			if (slot >= 0)
			{
				if (slot == headpart->type.underlying())
				{
					GetOriginalVertexData(headpart, skyrim_cast<RE::BSGeometry*>(facegenNiNode->GetObjectByName(headpart->formEditorID)));
					for (auto extra : headpart->extraParts)
					{
						GetOriginalVertexData(extra, skyrim_cast<RE::BSGeometry*>(facegenNiNode->GetObjectByName(extra->formEditorID)));
					}
					break;
				}
				else
					continue;
			}
			else if (slot == -1)
				GetOriginalVertexData(headpart, skyrim_cast<RE::BSGeometry*>(facegenNiNode->GetObjectByName(headpart->formEditorID)));
			else if (slot == -2)
			{
				std::string morphBasePath = headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
				if (morphBasePath.empty())
					continue;
				morphBasePath = fixPath(morphBasePath);
				if (auto found = orig_vertices.find(morphBasePath); found == orig_vertices.end())		
					GetOriginalVertexData(headpart, skyrim_cast<RE::BSGeometry*>(facegenNiNode->GetObjectByName(headpart->formEditorID)));

			}
		}
		SetVerticesAsOriginalVertexData();
		clear();
		m_lock.unlock();
		logger::debug("{:x} {} : expression initialed", id, name);

		return true;
	}
	bool MorphManager::GetOriginalVertexData(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry)
	{
		if (!a_headpart || !a_geometry)
			return false;

		std::string morphBasePath = a_headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
		if (morphBasePath.empty())
			return false;
		morphBasePath = fixPath(morphBasePath);

		RE::BSFaceGenBaseMorphExtraData* extraData = MorphManager::GetMorphExtraData(a_geometry);
		if (!extraData)
			return false;

		orig_vertices[morphBasePath].clear();
		for (std::uint32_t i = 0; i < extraData->vertexCount; i++)
		{
			RE::NiPoint3 vertex = extraData->vertexData[i];
			orig_vertices[morphBasePath].emplace_back(vertex);
		}
		return true;
	}
	void MorphManager::SetVerticesAsOriginalVertexData()
	{
		vertices.clear();
		for (auto& vertices_ : orig_vertices)
		{
			std::string morphBasePath = vertices_.first;
			for (auto& vertex : vertices_.second)
			{
				RE::NiPoint3 vertex_ = vertex;
				vertices[morphBasePath].emplace_back(vertex_);
			}
		}
	}
	bool MorphManager::Apply(std::string a_morphName, int32_t a_value)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return false;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return false;

		RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
		std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

		a_morphName = lowLetter(a_morphName);
		auto found = find(a_morphName);
		m_lock.lock();
		if (found != end()) {
			found->second.Apply(vertices, headParts, numHeadParts, actor->GetFaceNode(), a_value);
		}
		else {
			if (MorphDataBaseManager::GetSingleton().find(a_morphName) == MorphDataBaseManager::GetSingleton().end())
			{
				logger::error("Couldn't get {} morph data", a_morphName);
			}
			else
			{
				MorphManagerRecord newMorphManagerRecord = MorphManagerRecord(a_morphName);
				newMorphManagerRecord.Apply(vertices, headParts, numHeadParts, actor->GetFaceNode(), a_value);
				insert(std::make_pair(a_morphName, newMorphManagerRecord));
			}
		}
		m_lock.unlock();
		logger::debug("{:x} {} : expression updated", id, name);
		return true;
	}
	void MorphManager::Revert(std::string category)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;

		RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
		std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

		m_lock.lock();
		if (category.empty())
		{
			clear();
			SetVerticesAsOriginalVertexData();
		}
		else
		{
			category = lowLetter(category);
			auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
			for (auto name : morphNames) 
			{
				auto found = find(name);
				if (found != end())
					found->second.Apply(vertices, headParts, numHeadParts, actor->GetFaceNode(), 0.0f);
			}
		}
		m_lock.unlock();
	}
	void MorphManager::Update()
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return;
		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;
		RE::BSFaceGenNiNode* facegenNiNode = actor->GetFaceNode();
		if (!facegenNiNode)
			return;

		RE::BGSHeadPart** headParts = actorBase->HasOverlays() ? actorBase->GetBaseOverlays() : actorBase->headParts;
		std::uint32_t numHeadParts = actorBase->HasOverlays() ? actorBase->GetNumBaseOverlays() : actorBase->numHeadParts;

		m_lock.lock();
		for (std::uint32_t i = 0; i < numHeadParts; i++) {
			auto headpart = headParts[i];
			if (!headpart || headpart->formEditorID.empty())
				continue;
			Update(headpart, skyrim_cast<RE::BSGeometry*>(facegenNiNode->GetObjectByName(headpart->formEditorID)));
		}
		m_lock.unlock();
	}

	int32_t MorphManager::GetValue(std::string a_morphName) const
	{
		a_morphName = lowLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end())
			return found->second.GetValue();
		return 0.0f;
	}

	bool MorphManager::Update(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry)
	{
		if (!a_headpart || !a_geometry)
			return false;

		std::string morphBasePath = a_headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
		if (morphBasePath.empty())
			return false;
		morphBasePath = fixPath(morphBasePath);

		RE::BSFaceGenBaseMorphExtraData* extraData = MorphManager::GetMorphExtraData(a_geometry);
		if (!extraData)
		{
			logger::error("{} couldn't get extra data", a_geometry->name.c_str());
			return false;
		}

		const auto vertexDatas = vertices.find(morphBasePath);
		if (vertexDatas == vertices.end())
		{
			logger::error("{} invalid vertex data : {}", a_geometry->name.c_str(), morphBasePath);
			return false;
		}
		if (vertexDatas->second.size() != extraData->vertexCount)
		{
			logger::error("wrong vertex count {} for extraData {} of {} / {}", vertexDatas->second.size(), extraData->vertexCount, a_geometry->name.c_str(), morphBasePath);
			return false;
		}
		const auto& vertexData = vertexDatas->second;

		for (std::size_t i = 0; i < vertexData.size(); i++)
		{
			extraData->vertexData[i].x = vertexData.at(i).x;
			extraData->vertexData[i].y = vertexData.at(i).y;
			extraData->vertexData[i].z = vertexData.at(i).z;
		}
		UpdateModelFace(a_geometry);
		logger::debug("{} morph updated", a_geometry->name.c_str());
		return true;
	}

	RE::BSFaceGenBaseMorphExtraData* MorphManager::GetMorphExtraData(RE::BSGeometry* a_geometry)
	{
		return netimmerse_cast<RE::BSFaceGenBaseMorphExtraData*>(a_geometry->GetExtraData("FOD"));
	}
	std::uint32_t MorphManager::UpdateModelFace(RE::NiAVObject* obj) {
		using func_t = decltype(&MorphManager::UpdateModelFace);
		REL::VariantID offset(26458, 27044, 0x003EB710);
		REL::Relocation<func_t> func{ offset };
		return func(obj);
	}
}
