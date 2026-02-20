#pragma once

namespace MFEE {
	class Interface 
	{
	public:
		Interface() {};
		virtual ~Interface() {};

		virtual std::uint32_t GetVersion() = 0; //current version is 7
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
			virtual void String(const char* a_str) = 0;
		};
		//	lString example
		//	class IString_ : public IString{
		//	public:
		//		void String(const char* a_str) {
		//			str = a_str;
		//		};
        //		std::string str;
		//	};
		//	
		class IContainer{
		public:
            virtual void Container(const char* a_morphCategory, const char* a_morphName, std::int32_t a_value) = 0;
		};
        //	lContainer example
        //	class IContainer_ : public IContainer{
        //	public:
        //		void Container(const char* a_morphCategory, const char* a_morphName, std::int32_t a_value) {
        //			if (!container_.empty() && container_.back().first == a_morphCategory) {
		//				container_.back().second.emplace_back(a_morphName, a_value);
		//				return;
        //			}
		//			container_.emplace_back(a_morphCategory, std::vector<MorphSet>{{.morphName = a_morphName, .value = a_value}});
        //		};
		//		struct MorphSet {
		//		std::string morphName;
		//		std::int32_t value;
		//		};
		//		std::vector<std::pair<std::string, std::vector<MorphSet>>> container_;
		//	};
		//

        // Add morph data on the morph name
		// write without read - thread safe / write and read - no thread safe
		// recommend to use when main menu (after data loaded)
        virtual bool RegisterNewMorphData(const char* a_morphBasePath, const char* a_morphPath) = 0;

		// Create category/morph name if there are no
        // write without read - thread safe / write and read - no thread safe
        // recommend to use when main menu (after data loaded)
		virtual bool RegisterNewMorphNameOnCategory(const char* a_morphCategory, const char* a_morphName) = 0;

		virtual std::uint32_t GetExpressionCategorySize() = 0;
		virtual void GetExpressionCategory(std::int32_t a_categoryNumber, IString& result) = 0;
		virtual void GetExpressionCategory(const char* a_morphName, IString& result) = 0;
		virtual std::int32_t GetExpressionCategoryNumber(const char* a_morphCategory) = 0;
		virtual bool IsValidExpressionCategory(const char* a_morphCategory) = 0;

		virtual std::uint32_t GetExpressionMorphSize(const char* a_morphCategory) = 0;
		virtual std::uint32_t GetExpressionMorphSize(std::int32_t a_categoryNumber) = 0;
		virtual void GetExpressionMorph(const char* a_morphCategory, std::int32_t a_morphNumber, IString& result) = 0;
		virtual void GetExpressionMorph(std::int32_t a_categoryNumber, std::int32_t a_morphNumber, IString& result) = 0;

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

		virtual void GetAllExpressionList(IContainer& a_container) = 0;
        // If destination = true, returns the final value instead of the current one
        virtual void GetAllActiveMorph(RE::Actor* a_actor, IContainer& a_container, bool a_destination) = 0;
        virtual void GetAllMorph(RE::Actor* a_actor, IContainer& a_container, bool a_destination) = 0;
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
