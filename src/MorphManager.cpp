#include "MorphManager.h"

namespace Mus {
	bool MorphManagerRecord::Apply(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry, float a_value)
	{
		if (!a_headpart || !a_geometry)
			return false;

		std::string morphBasePath = a_headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
		const MorphDataBase::Morph* morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, morphBasePath);
		if (!morphData)
			return false;

		RE::BSFaceGenBaseMorphExtraData* extraData = GetMorphExtraData(a_geometry);
		if (!extraData)
			return false;

		if (extraData->vertexCount != morphData->vertexCount)
			return false;

		if (extraData->vertexCount != morphData->vertices.size())
			return false;

		float newValue = a_value - value;
		if (!HasExtraData(a_geometry)) {
			newValue = a_value;
			AddExtraData(a_geometry);
		}
		else if (IsEqual(newValue, value))
			return true;

		for (std::size_t i = 0; i < morphData->vertices.size(); i++)
		{
			auto& vert = morphData->vertices.at(i);
			extraData->vertexData[i].x += (float)((double)vert.x * (double)morphData->multiplier * (double)newValue);
			extraData->vertexData[i].y += (float)((double)vert.y * (double)morphData->multiplier * (double)newValue);
			extraData->vertexData[i].z += (float)((double)vert.z * (double)morphData->multiplier * (double)newValue);
		}

		UpdateModelFace(a_geometry);
		return true;
	}
	bool MorphManagerRecord::Apply(RE::BGSHeadPart** a_headparts, std::uint8_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode, float a_value)
	{
		if (!a_headparts || !a_facegenNinode)
			return false;

		for (std::uint8_t i = 0; i < a_numHeadparts; i++) {
			auto headpart = a_headparts[i];
			if (!headpart || headpart->formEditorID.empty())
				continue;
			Apply(headpart, skyrim_cast<RE::BSGeometry*>(a_facegenNinode->GetObjectByName(headpart->formEditorID)), a_value);

			for (auto extraPart : headpart->extraParts) {
				if (!extraPart)
					continue;
				Apply(headpart, skyrim_cast<RE::BSGeometry*>(a_facegenNinode->GetObjectByName(extraPart->formEditorID)), a_value);
			}
		}
		value = a_value;
		return true;
	}

	bool MorphManagerRecord::Update(RE::BGSHeadPart** a_headparts, std::uint8_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode)
	{
		return Apply(a_headparts, a_numHeadparts, a_facegenNinode, value);
	}

	RE::BSFaceGenBaseMorphExtraData* MorphManagerRecord::GetMorphExtraData(RE::BSGeometry* a_geometry)
	{
		return netimmerse_cast<RE::BSFaceGenBaseMorphExtraData*>(a_geometry->GetExtraData("FOD"));
	}
	std::uint32_t MorphManagerRecord::UpdateModelFace(RE::NiAVObject* obj) {
		using func_t = decltype(&MorphManagerRecord::UpdateModelFace);
		REL::VariantID offset(26458, 27044, 0x003EB710);
		REL::Relocation<func_t> func{ offset };
		return func(obj);
	}

	bool MorphManager::Apply(std::string a_morphName, float a_value)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return false;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return false;

		a_morphName = lowLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end()) {
			found->second.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), a_value);
		}
		else {
			if (MorphDataBaseManager::GetSingleton().find(a_morphName) == MorphDataBaseManager::GetSingleton().end())
			{
				logger::error("Couldn't get {} morph data", a_morphName);
				return false;
			}
			MorphManagerRecord newMorphManagerRecord = MorphManagerRecord(a_morphName);
			newMorphManagerRecord.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), a_value);
			insert(std::make_pair(a_morphName, newMorphManagerRecord));
		}
		return true;
	}
	void MorphManager::Revert()
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;

		for (auto& morph : *this)
		{
			morph.second.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), 0.0f);
		}
	}
	void MorphManager::Update()
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;

		for (auto& morph : *this)
		{
			morph.second.Update(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode());
		}
	}

	float MorphManager::GetValue(std::string a_morphName) const
	{
		lowLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end())
			return found->second.GetValue();
		return 0.0f;
	}
}
