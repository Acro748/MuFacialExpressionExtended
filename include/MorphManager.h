#pragma once

namespace Mus {
	typedef std::unordered_map<RE::FormID, std::vector<RE::NiPoint3>> Vertices; //headpart, vertieces

	class MorphManagerRecord
	{
		std::string morphName;
		std::int32_t value = 0.0f;
	public:
		MorphManagerRecord(std::string a_morphName) : morphName(a_morphName) {};
		~MorphManagerRecord() {};

		bool Apply(Vertices& vertices, RE::BGSHeadPart** a_headparts, std::uint32_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode, int32_t a_value);
		std::string GetMorphName() const { return morphName; }
		std::int32_t GetValue() const { return value; }
	private:
		bool Apply(Vertices& vertices, RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry, float a_value);
	};

	class MorphManager :
		public std::unordered_map<std::string, MorphManagerRecord> //morphName, record
	{
		RE::FormID id;
		std::string name;
	public:
		MorphManager(RE::Actor* a_actor) : id(a_actor->formID), name(a_actor->GetName()) {};
		~MorphManager() {};

		int32_t GetValue(std::string a_morphName) const;

		bool Initial(int32_t slot = -1);

		bool Apply(std::string a_morphName, int32_t a_value);
		void Revert(std::string category = "");
		void Update();

		static RE::BSFaceGenBaseMorphExtraData* GetMorphExtraData(RE::BSGeometry* a_geometry);

		struct ActiveMorphSet {
			std::string morphName = "";
			std::int32_t value = 0;
		};
		std::vector<ActiveMorphSet> GetAllActiveMorphs();
	private:
		bool Update(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry);
		bool GetOriginalVertexData(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry);
		void SetVerticesAsOriginalVertexData();
		static std::uint32_t UpdateModelFace(RE::NiAVObject* obj);

		std::recursive_mutex m_lock;

		Vertices vertices;
		Vertices orig_vertices;
	};
}
