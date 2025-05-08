#pragma once

namespace MFEE {
	class Interface 
	{
	public:
		Interface() {};
		virtual ~Interface() {};

		virtual std::uint32_t GetVersion() = 0; //current version is 3
	};

	struct InterfaceExchangeMessage
	{
		enum
		{
			kMessage_ExchangeInterface = 0x4D464545
		};

		Interface* Interface = nullptr;
	};

	class IFacialExpressionExtended :
		public Interface
	{
	public:
		virtual bool RegisterNewMorphData(std::string a_morphBasePath, std::string a_morphPath) = 0;
		virtual bool RegisterNewMorphNameOnCategory(std::string a_morphCategory, std::string a_morphName) = 0;

		virtual std::vector<std::string> GetExpressionCategories() = 0;
		virtual std::string GetExpressionCategoryByNumber(std::int32_t a_categoryNumber) = 0;
		virtual std::string GetExpressionCategoryByMorphName(std::string a_morphName) = 0;
		virtual std::int32_t GetExpressionCategoryNumber(std::string a_morphCategory) = 0;
		virtual bool IsValidExpressionCategory(std::string a_morphCategory) = 0;

		virtual std::vector<std::string> GetExpressionMorphNames(std::string a_morphCategory) = 0;
		virtual std::vector<std::string> GetExpressionMorphNamesByNumber(std::int32_t a_categoryNumber) = 0;
		virtual std::string GetExpressionMorphNameByNumber(std::string a_morphCategory, std::int32_t a_morphNumber) = 0;
		virtual std::string GetExpressionMorphNameByNumbers(std::int32_t a_categoryNumber, std::int32_t a_morphNumber) = 0;
		virtual std::int32_t GetExpressionMorphNameNumber(std::string a_morphName) = 0;
		virtual bool IsValidExpressionMorphName(std::string a_morphName) = 0;

		virtual std::int32_t GetExpressionValueByName(RE::Actor* a_actor, std::string a_morphName) = 0;
		virtual std::int32_t GetExpressionValueByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) = 0;

		virtual void SetExpressionByName(RE::Actor* a_actor, std::string a_morphName, int32_t a_value) = 0;
		virtual void SetExpressionByCategory(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, int32_t a_value) = 0;
		virtual void SetExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, int32_t a_value) = 0;
		virtual void RevertExpression(RE::Actor* a_actor, std::string a_morphCategory = "") = 0;
		virtual void UpdateExpression(RE::Actor* a_actor) = 0;
		virtual void InitialMorphData(RE::Actor* a_actor) = 0;

		struct IActiveMorphSet {
			std::string morphName = "";
			std::int32_t value = 0;
		};
		virtual std::vector<IActiveMorphSet> GetAllActiveMorphs(RE::Actor* a_actor) = 0;

		virtual void SetExpressionByNameEx(RE::Actor* a_actor, std::string a_morphName, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
		virtual void SetExpressionByCategoryEx(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
		virtual void SetExpressionByNumberEx(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, std::int32_t a_value, std::int32_t a_lerpTime) = 0;
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
				if (exchangeMessage.Interface->GetVersion() == 0)
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
