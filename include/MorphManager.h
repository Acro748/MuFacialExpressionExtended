#pragma once

namespace Mus {
	typedef std::vector<RE::NiPoint3> Vertices; //vertieces

	class ErrorLogger
	{
	public:
		[[nodiscard]] static ErrorLogger& GetSingleton() {
			static ErrorLogger instance;
			return instance;
		}

		void FlushMorphDataErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName, std::string morphBasePath);
		void FlushDynamicTriErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName);
		void FlushDynamicVerticesErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName);
		void FlushVertexCountErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName, std::uint32_t geoVertexCount, std::size_t triVertexCount);

	private:
		bool FindErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName);
		void AssignErrorLog(RE::FormID actorID, std::string geometryName, std::string morphName);

		typedef std::unordered_set<std::string> MorphNameMap; //morphName
		typedef concurrency::concurrent_unordered_map<std::string, MorphNameMap> GeometryMorphMap; //geometryName, morphName
		concurrency::concurrent_unordered_map<RE::FormID, GeometryMorphMap> errorMap; //actorID, count
	};

	class MorphManagerRecord
	{
		RE::FormID id;
		std::string morphName;
		std::int32_t value = 0.0f;
	public:
		MorphManagerRecord(RE::FormID a_actorID, std::string a_morphName) : id(a_actorID), morphName(a_morphName) {};
		~MorphManagerRecord() {};

		bool Update(RE::BGSHeadPart** a_headparts, std::uint32_t a_numHeadparts, RE::BSFaceGenNiNode* a_facegenNinode);
		std::string GetMorphName() const { return morphName; }
		void SetValue(std::int32_t a_value, std::int32_t a_lerpTime);
		void SetValue(std::int32_t a_value);
		std::int32_t GetValue() const { return value; }
		void UpdateLerpValue(std::clock_t processTime);
	private:
		bool Recalculate(RE::BSGeometry* a_geometry);
		bool Update(RE::BGSHeadPart* a_headpart, RE::BSGeometry* a_geometry);

		struct LerpTask {
			std::clock_t totalProcessTime;
			std::clock_t endTime;
			std::int32_t startValue;
			std::int32_t endValue;
		};
		LerpTask lerpTask = LerpTask(); //LerpTask
	};

	class MorphManager :
		public std::unordered_map<std::string, MorphManagerRecord> //morphName, record
	{
		RE::FormID id;
		std::string name;
	public:
		MorphManager(RE::Actor* a_actor) : id(a_actor->formID), name(a_actor->GetName()) {};
		~MorphManager() {};

		bool SetValue(std::string a_morphName, std::int32_t a_value, std::int32_t a_lerpTime);
		bool SetValue(std::string a_morphName, std::int32_t a_value);
		int32_t GetValue(std::string a_morphName) const;

		void Revert(std::string category = "");
		void Update(std::clock_t processTime);

		struct ActiveMorphSet {
			std::string morphName = "";
			std::int32_t value = 0;
		};
		std::vector<ActiveMorphSet> GetAllActiveMorphs();
	private:
		static std::uint32_t UpdateModelFace(RE::NiAVObject* obj);

		std::recursive_mutex m_lock;
	};
}
