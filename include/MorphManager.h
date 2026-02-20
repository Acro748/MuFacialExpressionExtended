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
		const RE::FormID id;
        const lString morphName;
		std::int32_t value = 0;
		std::int32_t pvalue = 0;
		float fvalue = 0.0f;
		float pfvalue = 0.0f;
	public:
		MorphManagerRecord(RE::FormID a_actorID, const lString& a_morphName) : id(a_actorID), morphName(a_morphName) {};
		~MorphManagerRecord() {};

        struct MorphGeoData {
            const lString morphBasePath = "";
            RE::BSDynamicTriShape* dynamicShape = nullptr;
            DirectX::XMVECTOR* __restrict dynamicVertices = nullptr;
            const std::uint32_t vertexCount = 0;
            bool isSameHash = false;
        };
		bool Update(const std::vector<MorphGeoData>& a_morphGeoData);
		inline std::string GetMorphName() const { return morphName; }
		void SetValue(std::int32_t a_value, std::int32_t a_lerpTime);
		void SetValue(std::int32_t a_value);
        inline std::int32_t GetValue(bool destination = false) const { return !destination ? value : lerpTask.endValue; }
		bool UpdateLerpValue(std::clock_t processTime); // true : updated
	private:
        bool Update(const MorphGeoData& data);

		mutable std::mutex valueLock;
		struct LerpTask {
			std::clock_t totalProcessTime;
			std::clock_t endTime;
			std::int32_t startValue;
			std::int32_t endValue;
		};
		LerpTask lerpTask = LerpTask(); //LerpTask
	};
    typedef std::shared_ptr<MorphManagerRecord> MorphManagerRecordPtr;

	class MorphManager {
		const RE::FormID id;
        const lString name;
        std::unordered_map<lString, MorphManagerRecordPtr> record; // morphName, record
        mutable std::mutex recordLock;
	public:
		MorphManager(RE::Actor* a_actor) : id(a_actor->formID), name(a_actor->GetName()) {};
		~MorphManager() {};

		bool SetValue(const lString& a_morphName, const std::int32_t a_value, const std::int32_t a_lerpTime);
        bool SetValue(const lString& a_morphName, const std::int32_t a_value);
        std::int32_t GetValue(const lString& a_morphName, bool destination = false) const;

		void Revert(const lString& category = "");
		void Update(std::clock_t processTime);

		struct ActiveMorphSet {
			const lString morphName = "";
			const std::int32_t value = 0;
		};
		std::vector<ActiveMorphSet> GetAllActiveMorphs();

		inline void SetNeedUpdateFacegenMeshes() { isNeedUpdateFacegen = true; };
    private:
        std::vector<MorphManagerRecord::MorphGeoData> morphGeoDatas;
        std::unordered_map<std::string, RE::BSDynamicTriShape*> faceGenMeshes;
        std::atomic<bool> isNeedUpdateFacegen = true;
        std::unordered_map<RE::BSDynamicTriShape*, std::uint64_t> lastHash;
        std::uint64_t GetHash(const MorphManagerRecord::MorphGeoData& data);
        //bool Recalculate(const MorphManagerRecord::MorphGeoData& data);
        static std::uint32_t UpdateModelFace(RE::NiAVObject* obj);
	};
}
