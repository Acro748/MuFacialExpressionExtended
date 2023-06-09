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

		enum class morphType {
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

		enum class moodType {
			MoodAhegao,
			MoodSulky,
			MoodLaugh,
			total
		};
		enum class miscType {
			
			total
		};
		enum class earsType {
			EarsFront,
			EarsBack,
			EarsIn,
			EarsOut,
			total
		};
		enum class tailType {
			TailUp,
			TailDown,
			total
		};
		enum class faceType {
			FaceInflateCheeks,
			total
		};
		enum class eyesType {
			EyesIn,
			EyesOut,
			total
		};
		enum class browsType {

			total
		};
		enum class mouthType {

			total
		};
		enum class tongueType {
			TongueOut,
			TongueUp,
			TongueDown,
			TongueLeft,
			TongueRight,
			total
		};

		bool Register(morphType type, std::string morphName);
		bool Register(std::string morphName);
		std::vector<std::string> GetNames(morphType type);
		std::vector<std::string> GetMorphTypes();
		bool IsValidName(std::string morphName);
		morphType GetMorphTypeByString(std::string a_morphType) const;
	private:
		std::unordered_map<std::string, morphType> morphTypes; //morphType_str, morphType
		std::unordered_map<std::string, std::vector<std::string>> names; //morphType, morphNames
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
			};
			std::vector<Vertex> vertices;

			bool IsValid() { return !morphBasePath.empty() && vertexCount != -1 && vertices.size() > 0; }
		};

		bool Register(Morph a_morph);
		const Morph* GetMorphData(std::string a_morphBasePath) const;

	private:
		concurrency::concurrent_unordered_map<std::string, Morph> morphData; // morphBasePath, morph
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

	private:
		RE::BSResourceNiBinaryStream GetFile(std::string a_filePath);
	};
}

