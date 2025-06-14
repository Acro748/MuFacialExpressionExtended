#pragma once

namespace MFEE {
	class Interface 
	{
	public:
		Interface() {};
		virtual ~Interface() {};

		virtual std::uint32_t GetVersion() = 0; //current version is 6
	};

	struct InterfaceExchangeMessage
	{
		enum
		{
			kMessage_ExchangeInterface = 0x4D464546
		};

		Interface* Interface = nullptr;
	};

	class IFacialExpressionExtended :
		public Interface
	{
	public:
		class IString{
		public:
			virtual void String(const char* str) = 0;
		};

		virtual bool RegisterNewMorphData(const char* a_morphBasePath, const char* a_morphPath) = 0;
		virtual bool RegisterNewMorphNameOnCategory(const char* a_morphCategory, const char* a_morphName) = 0;

		virtual std::uint32_t GetExpressionCategorySize() = 0;
		virtual void GetExpressionCategory(std::int32_t a_categoryNumber, IString& result) = 0;
		std::vector<std::string> GetExpressionCategories() {
			class IString_ : public IString {
			public:
				virtual void String(const char* str) {
					str_ = str;
				}
				std::string str_;
			};
			std::vector<std::string> result;
			for (std::int32_t i = 0; i < GetExpressionCategorySize(); i++) {
				IString_ str;
				GetExpressionCategory(i, str);
				result.push_back(str.str_);
			}
			return result;
		}

		virtual void GetExpressionCategory(const char* a_morphName, IString& result) = 0;
		virtual std::int32_t GetExpressionCategoryNumber(const char* a_morphCategory) = 0;
		virtual bool IsValidExpressionCategory(const char* a_morphCategory) = 0;

		virtual std::uint32_t GetExpressionMorphSize(const char* a_morphCategory) = 0;
		virtual std::uint32_t GetExpressionMorphSize(std::int32_t a_categoryNumber) = 0;
		virtual void GetExpressionMorph(const char* a_morphCategory, std::int32_t a_morphNumber, IString& result) = 0;
		virtual void GetExpressionMorph(std::int32_t a_categoryNumber, std::int32_t a_morphNumber, IString& result) = 0;
		std::vector<std::string> GetExpressionMorphs(const char* a_morphCategory) {
			class IString_ : public IString {
			public:
				virtual void String(const char* str) {
					str_ = str;
				}
				std::string str_;
			};
			std::vector<std::string> result;
			for (std::int32_t i = 0; i < GetExpressionMorphSize(a_morphCategory); i++) {
				IString_ str;
				GetExpressionMorph(a_morphCategory, i, str);
				result.push_back(str.str_);
			}
			return result;
		}
		std::vector<std::string> GetExpressionMorphs(std::int32_t a_categoryNumber) {
			class IString_ : public IString {
			public:
				virtual void String(const char* str) {
					str_ = str;
				}
				std::string str_;
			};
			std::vector<std::string> result;
			for (std::int32_t i = 0; i < GetExpressionMorphSize(a_categoryNumber); i++) {
				IString_ str;
				GetExpressionMorph(a_categoryNumber, i, str);
				result.push_back(str.str_);
			}
			return result;
		}

		virtual std::int32_t GetExpressionMorphNumber(const char* a_morphName) = 0;
		virtual bool IsValidExpressionMorph(const char* a_morphName) = 0;

		virtual std::int32_t GetExpressionValue(RE::Actor* a_actor, const char* a_morphName) = 0;
		virtual std::int32_t GetExpressionValue(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) = 0;

		virtual void SetExpression(RE::Actor* a_actor, const char* a_morphName, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
		virtual void SetExpression(RE::Actor* a_actor, const char* a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
		virtual void SetExpression(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
		virtual void RevertExpression(RE::Actor* a_actor, const char* a_morphCategory = "") = 0;
		virtual void UpdateExpression(RE::Actor* a_actor) = 0;
		virtual void InitialMorphData(RE::Actor* a_actor) = 0;

		virtual std::uint32_t GetActiveMorphSize(RE::Actor* a_actor) = 0;
		virtual void GetActiveMorphName(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber, IString& result) = 0;
		virtual void GetActiveMorphCategory(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber, IString& result) = 0;
		virtual std::int32_t GetActiveMorphValue(RE::Actor* a_actor, std::uint32_t a_activeMorphNumber) = 0;
		std::unordered_map<std::string, std::int32_t> GetAllActiveMorphValue(RE::Actor* a_actor) {
			class IString_ : public IString {
			public:
				virtual void String(const char* str) {
					str_ = str;
				}
				std::string str_;
			};
			std::unordered_map<std::string, std::int32_t> result;
			for (std::int32_t i = 0; i < GetActiveMorphSize(a_actor); i++) {
				IString_ str;
				GetActiveMorphName(a_actor, i, str);
				std::int32_t value = GetActiveMorphValue(a_actor, i);
				result.emplace(str.str_, value);
			}
			return result;
		}
	};

	class InterfaceManager {
	public:
		InterfaceManager() {};
		~InterfaceManager() {};
		
		inline bool load() {
			if (!load_Check)
			{
				if (IsLoaded())
				{
					const auto messagingInterface = SKSE::GetMessagingInterface();
					if (messagingInterface)
						messagingInterface->Dispatch(InterfaceExchangeMessage::kMessage_ExchangeInterface, &exchangeMessage, 0, "MuFacialExpressionExtended");
				}
				load_Check = true;
			}
			return exchangeMessage.Interface ? true : false;
		}

		IFacialExpressionExtended* GetInterface() { 
			if (load())
			{
				return reinterpret_cast<IFacialExpressionExtended*>(exchangeMessage.Interface);
			}
			return nullptr;
		}

	protected:

	private:
		inline bool IsLoaded() { return GetModuleHandle(L"MuFacialExpressionExtended.dll"); }

		bool load_Check = false;
		InterfaceExchangeMessage exchangeMessage;
	};
	static InterfaceManager MFEEAPI;
}
