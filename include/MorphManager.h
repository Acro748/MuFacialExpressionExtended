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

		void Initial() { errorMap.clear(); };

		void FlushMorphDataErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName, const std::string& morphBasePath, const std::uint32_t vertexCount);
        void FlushDynamicTriErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName);
        void FlushDynamicVerticesErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName);
        void FlushVertexCountErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName, const std::uint32_t geoVertexCount, const std::size_t triVertexCount);

	private:
        bool FindErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName);
        void AssignErrorLog(const RE::FormID actorID, const std::string& geometryName, const std::string& morphName);

		typedef std::unordered_set<std::string> MorphNameMap; //morphName
		typedef concurrency::concurrent_unordered_map<std::string, MorphNameMap> GeometryMorphMap; //geometryName, morphName
		concurrency::concurrent_unordered_map<RE::FormID, GeometryMorphMap> errorMap; //actorID, count
	};

	class MorphManagerRecord
	{
		RE::FormID id;
		std::string morphName;
		std::int32_t value = 0;
		float fvalue = 0.0f;
	public:
		MorphManagerRecord(RE::FormID a_actorID, std::string a_morphName) : id(a_actorID), morphName(a_morphName) {};
		~MorphManagerRecord() {};

		struct MorphGeoData {
			std::string morphBasePath;
			RE::BSGeometry* geometry;
		};

		bool Update(const std::vector<MorphGeoData>& a_morphGeoData);
		std::string GetMorphName() const { return morphName; }
		void SetValue(std::int32_t a_value, std::int32_t a_lerpTime);
		void SetValue(std::int32_t a_value);
		std::int32_t GetValue() const { return value; }
		bool UpdateLerpValue(std::clock_t processTime); // true : updated
	private:
		bool Recalculate(RE::BSGeometry* a_geometry);
        bool Update(const std::string& a_morphBasePath, RE::BSGeometry* a_geometry);

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

		bool SetValue(const std::string& a_morphName, const std::int32_t a_value, const std::int32_t a_lerpTime);
        bool SetValue(const std::string& a_morphName, const std::int32_t a_value);
        int32_t GetValue(const std::string& a_morphName) const;

		void Revert(const std::string& category = "");
		void Update(std::clock_t processTime);

		struct ActiveMorphSet {
			std::string morphName = "";
			std::int32_t value = 0;
		};
		std::vector<ActiveMorphSet> GetAllActiveMorphs();
	private:
		static std::uint32_t UpdateModelFace(RE::NiAVObject* obj);

		bool isNeedUpdate = false;
		std::recursive_mutex m_lock;
	};
}
