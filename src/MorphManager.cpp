#include "MorphManager.h"

namespace Mus {
	bool MorphManagerRecord::Apply(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry, float a_value)
	{
		if (!a_headpart || !a_geometry)
			return false;

		std::string morphBasePath = a_headpart->morphs[RE::BGSHeadPart::MorphIndices::kDefaultMorph].GetModel();
		const MorphDataBase::Morph* morphData = MorphDataBaseManager::GetSingleton().GetMorphData(morphName, morphBasePath);
		if (!morphData)
		{
			logger::debug("{} couldn't get morph data for {} / {}", a_geometry->name.c_str(), morphName, morphBasePath);
			return false;
		}

		RE::BSFaceGenBaseMorphExtraData* extraData = GetMorphExtraData(a_geometry);
		if (!extraData)
		{
			logger::debug("{} couldn't get extra data", a_geometry->name.c_str());
			return false;
		}

		if (extraData->vertexCount != morphData->vertexCount) 
		{
			logger::debug("{} vertexCount is not equal", a_geometry->name.c_str());
			return false;
		}

		if (extraData->vertexCount != morphData->vertices.size())
		{
			logger::debug("{} vertexCount is not equal", a_geometry->name.c_str());
			return false;
		}

		float newValue = a_value - value;
		if (!HasExtraData(a_geometry)) {
			newValue = a_value;
			AddExtraData(a_geometry);
		}
		
		if (!IsEqual(newValue, value))
		{
			logger::debug("{} value is not changed", a_geometry->name.c_str());
		}
		else
		{
			newValue *= 0.01f;
			for (std::size_t i = 0; i < morphData->vertices.size(); i++)
			{
				auto& vert = morphData->vertices.at(i);
				extraData->vertexData[i].x += (float)((double)vert.x * (double)morphData->multiplier * (double)newValue);
				extraData->vertexData[i].y += (float)((double)vert.y * (double)morphData->multiplier * (double)newValue);
				extraData->vertexData[i].z += (float)((double)vert.z * (double)morphData->multiplier * (double)newValue);
			}
		}
		UpdateModelFace(a_geometry);
		logger::debug("{} morph updated", a_geometry->name.c_str());
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

			//is no need to get extra parts?, are extra parts already included in headparts?
			/*for (auto extraPart : headpart->extraParts) {
				if (!extraPart)
					continue;
				Apply(extraPart, skyrim_cast<RE::BSGeometry*>(a_facegenNinode->GetObjectByName(extraPart->formEditorID)), a_value);
			}*/
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

		a_morphName = fixLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end()) {
			found->second.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), a_value);
		}
		else {
			if (MorphDataBaseManager::GetSingleton().find(a_morphName) == MorphDataBaseManager::GetSingleton().end())
			{
				logger::debug("Couldn't get {} morph data", a_morphName);
				return false;
			}
			MorphManagerRecord newMorphManagerRecord = MorphManagerRecord(a_morphName);
			newMorphManagerRecord.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), a_value);
			insert(std::make_pair(a_morphName, newMorphManagerRecord));
		}
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

		if (category.empty())
		{
			for (auto& morph : *this)
			{
				morph.second.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), 0.0f);
			}
		}
		else
		{
			category = fixLetter(category);
			auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
			for (auto name : morphNames) 
			{
				auto found = find(name);
				if (found != end())
					found->second.Apply(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode(), 0.0f);
			}
		}
	}
	void MorphManager::Update(std::string category)
	{
		RE::Actor* actor = skyrim_cast<RE::Actor*>(RE::TESForm::LookupByID(id));
		if (!actor)
			return;

		RE::TESNPC* actorBase = actor->GetActorBase();
		if (!actorBase)
			return;
		if (category.empty())
		{
			for (auto& morph : *this)
			{
				morph.second.Update(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode());
			}
		}
		else
		{
			category = fixLetter(category);
			auto morphNames = morphNameEntry::GetSingleton().GetMorphNames(category);
			for (auto name : morphNames)
			{
				auto found = find(name);
				if (found != end())
					found->second.Update(actorBase->headParts, actorBase->numHeadParts, actor->GetFaceNode());
			}
		}
	}

	float MorphManager::GetValue(std::string a_morphName) const
	{
		a_morphName = fixLetter(a_morphName);
		auto found = find(a_morphName);
		if (found != end())
			return found->second.GetValue();
		return 0.0f;
	}
}
