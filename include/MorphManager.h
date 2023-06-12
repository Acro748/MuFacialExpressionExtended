#pragma once

namespace Mus {
	static RE::NiExtraData* extraData = RE::NiBooleanExtraData::Create("MFEE_APPLIED", true);
	class MorphManagerRecord
	{
		std::string morphName;
		float value = 0.0f;
	public:
		MorphManagerRecord(std::string a_morphName) : morphName(a_morphName) {};
		~MorphManagerRecord() {};

		bool Apply(RE::BGSHeadPart** a_headparts, std::uint8_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode, float a_value);
		bool Update(RE::BGSHeadPart** a_headparts, std::uint8_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode);

		float GetValue() const { return value; }
	private:
		bool Apply(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry, float a_value);

		bool HasExtraData(RE::BSGeometry* a_geometry) { return a_geometry->HasExtraData(extraData->name); }
		bool AddExtraData(RE::BSGeometry* a_geometry) { return a_geometry->AddExtraData(extraData); }

		RE::BSFaceGenBaseMorphExtraData* GetMorphExtraData(RE::BSGeometry* a_geometry);
		static std::uint32_t UpdateModelFace(RE::NiAVObject* obj);
	};

	class MorphManager :
		public std::unordered_map<std::string, MorphManagerRecord> //morphName, record
	{
		RE::FormID id;
		std::string name;
	public:
		MorphManager(RE::Actor* a_actor) : id(a_actor->formID), name(a_actor->GetName()) {};
		~MorphManager() {};

		float GetValue(std::string a_morphName) const;

		bool Apply(std::string a_morphName, float a_value);
		void Revert(std::string category = "");
		void Update(std::string category = "");
	private:
		std::recursive_mutex m_lock;
	};
}
