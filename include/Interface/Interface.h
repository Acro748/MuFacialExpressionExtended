#pragma once

namespace MFEE {
	class Interface 
	{
	public:
		Interface() {};
		virtual ~Interface() {};

		virtual std::uint32_t GetVersion() = 0;
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
		virtual bool RegisterNewMorphNameOnType(std::string a_morphCategory, std::string a_morphName) = 0;
		virtual bool RegisterNewMorphName(std::string a_morphName) = 0;

		virtual std::vector<std::string> GetFacialExpressionCategories() = 0;
		virtual std::string GetFacialExpressionCategoryByNumber(std::uint32_t a_categoryNumber) = 0;
		virtual std::string GetFacialExpressionCategoryByMorphName(std::string a_morphName) = 0;
		virtual std::int32_t GetFacialExpressionCategoryNumber(std::string a_category) = 0;
		virtual bool IsValidFacialExpressionCategory(std::string a_category) = 0;

		virtual std::vector<std::string> GetFacialExpressionMorphNames(std::string a_morphCategory) = 0;
		virtual std::vector<std::string> GetFacialExpressionMorphNamesByNumber(std::uint32_t a_categoryNumber) = 0;
		virtual std::string GetFacialExpressionMorphNameByNumber(std::string a_morphCategory, std::int32_t a_morphNumber) = 0;
		virtual std::string GetFacialExpressionMorphNameByNumbers(std::int32_t a_categoryNumber, std::int32_t a_morphNumber) = 0;
		virtual std::int32_t GetFacialExpressionMorphNameNumber(std::string a_morphName) = 0;
		virtual std::int32_t IsValidFacialExpressionMorphName(std::string a_morphName) = 0;

		virtual float GetFacialExpressionValueByName(RE::Actor* a_actor, std::string a_morphName) = 0;
		virtual float GetFacialExpressionValueByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategoryNumber, std::uint32_t a_morphNumber) = 0;

		virtual void SetFacialExpressionByName(RE::Actor* a_actor, std::string a_morphName, float value) = 0;
		virtual void SetFacialExpressionByType(RE::Actor* a_actor, std::string a_morphCategory, std::uint32_t a_morphNumber, float value) = 0;
		virtual void SetFacialExpressionByNumber(RE::Actor* a_actor, std::uint32_t a_morphCategory, std::uint32_t a_morphNumber, float a_value) = 0;
		virtual void RevertFacialExpression(RE::Actor* a_actor) = 0;
		virtual void UpdateFacialExpression(RE::Actor* a_actor) = 0;
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
		inline bool IsLoaded() { return GetModuleHandle("MuFacialExpressionExtended.dll"); }

		bool load_Check = false;
		InterfaceExchangeMessage exchangeMessage;
	};
	static InterfaceManager MFEEAPI;
}
