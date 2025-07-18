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
			std::string morphCategory;
			std::vector<std::string> morphNames;
		};

		bool Register(std::string category, std::string morphName);
		bool RegisterCategory(std::string category);
		void UnRegisterAll() { 
			names.clear(); 
			Init();
		};

		std::vector<std::string> GetMorphNames(std::string category);
		std::vector<std::string> GetMorphNames(std::int32_t categoryNumber) { return GetMorphNames(GetCategoryByNumber(categoryNumber)); };

		std::int32_t GetMorphNameNumber(std::string morphName);
		std::string GetMorphNameByNumber(std::string category, std::int32_t morphNumber);
		std::string GetMorphNameByNumber(std::int32_t categoryNumber, std::int32_t morphNumber) { return GetMorphNameByNumber(GetCategoryByNumber(categoryNumber), morphNumber); };

		std::vector<std::string> GetCategories();
		std::int32_t GetCategoryNumber(std::string category);
		std::string GetCategoryByNumber(std::int32_t categoryNumber);
		std::string GetCategoryByMorphName(std::string morphName);

		bool IsValidName(std::string morphName) { return GetMorphNameNumber(morphName) != -1; }
		bool IsValidCategory(std::string category) { return GetCategoryNumber(category) != -1; }
	private:
		std::vector<morphNameEntryData> names; //morphCategory, morphNames
	};


	class MorphDataBase 
	{
		std::string morphName;
	public:
		MorphDataBase(std::string a_morphName) : morphName(lowLetter(a_morphName)) {};
		~MorphDataBase() {};

		struct Morph {
			std::string morphBasePath;
			std::int32_t vertexCount = -1;
			float multiplier = 1.0f;

			struct Vertex
			{
				std::int16_t x, y, z;
				inline DirectX::XMFLOAT4 GetFloat4() {
					return DirectX::XMFLOAT4((float)x, (float)y, (float)z, 0);
				};
			};
			std::vector<DirectX::XMFLOAT4> vertices;

			bool IsValid() { return !morphBasePath.empty() && vertexCount != -1 && vertices.size() > 0; }
		};

		bool Register(Morph a_morph);
		const Morph* GetMorphData(std::string a_morphBasePath) const;
		const Morph* GetMorphData(std::uint32_t a_vertexCount) const;

	private:
		concurrency::concurrent_unordered_map<std::string, Morph> morphData; // morphBasePath, morph
		concurrency::concurrent_unordered_map<std::uint32_t, Morph> morphDataAlt; // vertexCount, morph
	};

	class MorphDataBaseManager :
		public concurrency::concurrent_unordered_map<std::string, MorphDataBase> //morphName, triDataBase
	{
	public:
		MorphDataBaseManager() {};
		~MorphDataBaseManager() {};

		[[nodiscard]] static MorphDataBaseManager& GetSingleton() {
			static MorphDataBaseManager instance;
			return instance;
		}

		bool Register(std::string a_morphBasePath, std::string a_morphPath);
		const MorphDataBase::Morph* GetMorphData(std::string morphName, std::string a_morphBasePath) const;
		const MorphDataBase::Morph* GetMorphData(std::string morphName, std::uint32_t vertexCount) const;

		void UnRegisterAll() { clear(); };
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

