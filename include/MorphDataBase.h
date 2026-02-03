#pragma once

namespace Mus {
	class morphNameEntry 
	{
	public:
		morphNameEntry() {};
		~morphNameEntry() {};

		[[nodiscard]] static morphNameEntry& GetSingleton() {
			static morphNameEntry instance;
			return instance;
		}

		void Init();

		enum class morphCategory {
			Mood,
			Misc,
			Ears,
			Tail,
			Face,
			Eyes,
			Brows,
			Mouth,
			Tongue,

			total
		};

		enum class moodCategory {
			MoodAhegao,
			MoodAhegao2,
			MoodSulky,
			MoodLaugh,
			MoodFlabbergasted,
			MoodTemptation,

			total
		};
		enum class miscCategory {

			total
		};
		enum class earsCategory {
			EarsFront,
			EarsBack,
			EarsIn,
			EarsOut,

			total
		};
		enum class tailCategory {
			TailUp,
			TailDown,
			TailLeft,
			TailRight,

			total
		};
		enum class faceCategory {
			FaceInflateCheeks,

			total
		};
		enum class eyesCategory {
			EyesIn,
			EyesOut,
			EyesSurprise,
			EyesShock,

			total
		};
		enum class browsCategory {

			total
		};
		enum class mouthCategory {
			MouthLaugh,
			MouthTemptation,
			MouthSmacklips,
			MouthFlabbergasted,
			MouthCry,
			MouthCat,
			MouthClench,
			MouthClenchLip,
			MouthPucker,
			MouthUpperUp,
			MouthBow,

			total
		};
		enum class tongueCategory {
			TongueOut,
			TongueUp,
			TongueDown,
			TongueLeft,
			TongueRight,
			TongueTwistLeft,
			TongueTwistRight,

			total
		};

		struct morphNameEntryData {
            const lString morphCategory;
            std::vector<lString> morphNames;
		};

		bool Register(const lString& category, const lString& morphName);
        bool RegisterCategory(const lString& category);
		void UnRegisterAll() { 
			{
                std::lock_guard lg(namesLock);
                names.clear();
            }
			Init();
		};

		std::vector<std::string> GetMorphNamesBasic(const lString& category);
		std::vector<lString> GetMorphNames(const lString& category);
        std::vector<std::string> GetMorphNamesBasic(std::int32_t categoryNumber) { return GetMorphNamesBasic(GetCategoryByNumber(categoryNumber)); };
        std::vector<lString> GetMorphNames(std::int32_t categoryNumber) { return GetMorphNames(GetCategoryByNumber(categoryNumber)); };

		std::int32_t GetMorphNameNumber(const lString& morphName);
        lString GetMorphNameByNumber(const lString& category, std::int32_t morphNumber);
		lString GetMorphNameByNumber(std::int32_t categoryNumber, std::int32_t morphNumber) { return GetMorphNameByNumber(GetCategoryByNumber(categoryNumber), morphNumber); };

		std::vector<std::string> GetCategories();
        std::int32_t GetCategoryNumber(const lString& category);
		lString GetCategoryByNumber(std::int32_t categoryNumber);
		lString GetCategoryByMorphName(const lString& morphName);

		bool IsValidName(const lString& morphName) { return GetMorphNameNumber(morphName) != -1; }
        bool IsValidCategory(const lString& category) { return GetCategoryNumber(category) != -1; }
	private:
        std::shared_mutex namesLock;
		std::vector<morphNameEntryData> names; //morphCategory, morphNames
	};


	class MorphDataBase 
	{
        const lString morphName;
	public:
        MorphDataBase(const lString& a_morphName) : morphName(a_morphName) {};
		~MorphDataBase() {};

		struct Morph {
            const lString morphBasePath;
            const std::int32_t vertexCount = -1;
			float multiplier = 1.0f;

			struct Vertex
			{
				std::int16_t x, y, z;
				inline DirectX::XMFLOAT4 GetFloat4() {
					return DirectX::XMFLOAT4((float)x, (float)y, (float)z, 0);
				};
                inline DirectX::XMVECTOR GetXMVector() {
                    return DirectX::XMVectorSet((float)x, (float)y, (float)z, 0);
				};
			};
            std::vector<DirectX::XMVECTOR> vertices;

			inline bool IsValid() const { return !morphBasePath.empty() && vertexCount != -1 && vertices.size() > 0; }
		};

		bool Register(const Morph& a_morph);
        const Morph* GetMorphData(const lString& a_morphBasePath) const;
        const Morph* GetMorphData(const std::uint32_t a_vertexCount) const;

	private:
        mutable std::mutex morphDataLock;
        std::unordered_map<lString, Morph> morphData;          // morphBasePath, morph
		std::unordered_map<std::uint32_t, Morph> morphDataAlt; // vertexCount, morph
	};
    typedef std::shared_ptr<MorphDataBase> MorphDataBasePtr;

	class MorphDataBaseManager {
        mutable std::mutex dbLock;
        std::unordered_map<lString, MorphDataBasePtr> db; // morphName, triDataBase
	public:
		MorphDataBaseManager() {};
		~MorphDataBaseManager() {};

		[[nodiscard]] static MorphDataBaseManager& GetSingleton() {
			static MorphDataBaseManager instance;
			return instance;
		}

		bool Register(const lString& a_morphBasePath, const lString& a_morphPath);
        const MorphDataBase::Morph* GetMorphData(const lString& morphName, const lString& a_morphBasePath) const;
        const MorphDataBase::Morph* GetMorphData(const lString& morphName, const std::uint32_t vertexCount) const;

		void UnRegisterAll() { 
			std::lock_guard lg(dbLock);
			db.clear(); 
		};
		
		bool IsValidMorphName(const lString& morphName);
	private:
		static std::uint32_t Read(RE::BSResourceNiBinaryStream* stream, void* dst, std::uint32_t len) {
			using func_t = decltype(&MorphDataBaseManager::Read);
			REL::VariantID offset(69647, 71027, 0x00CBCBE0);
			REL::Relocation<func_t> func{ offset };
			return func(stream, dst, len);
		}
		static void Seek(RE::BSResourceNiBinaryStream* stream, std::int32_t delta) {
			using func_t = decltype(&MorphDataBaseManager::Seek);
			REL::VariantID offset(69640, 71018, 0x00CBC8C0);
			REL::Relocation<func_t> func{ offset };
			return func(stream, delta);
		}
	};
}

