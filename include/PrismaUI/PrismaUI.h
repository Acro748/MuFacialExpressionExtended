#pragma once

namespace Mus {
    class UIManager  : 
        public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
        public IEventListener<FrameEvent>, 
		public RE::BSTEventSink<RE::InputEvent*> {
    public:
        UIManager() {};
        ~UIManager() {};
        [[nodiscard]] static UIManager& GetSingleton() {
            static UIManager instance;
            return instance;
        }

        void Init();
        void UpdateData(RE::Actor* a_actor = nullptr);
        void OpenUI();
        void CloseUI();
        void ScrollUI(bool isUp) const;
        void SelectNextCategoryUI() const;
        void SelectPrevCategoryUI() const;
        void SetUIPositionUI(float xPercent, float yPercent);
        void SetUIScaleUI(float scaleMultiplier);

        inline void StoreMFGExpressionOverride(const std::uint32_t index, const float value) { 
            mfgExpressionIndex = index; 
            mfgExpressionValue = value;
        }
    protected:
        using EventResult = RE::BSEventNotifyControl;
        EventResult ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;
        EventResult ProcessEvent(RE::InputEvent* const* evn, RE::BSTEventSource<RE::InputEvent*>*) override;
        void onEvent(const FrameEvent& e) override;

    private:
        void CreateUI();
        RE::Actor* GetConsoleActor();

        bool IsPrismaUIExists = true;
        PRISMA_UI_API::IVPrismaUI1* g_prismaUI;
        PrismaView g_myView;
        RE::Actor* currentTarget = nullptr;
        bool isCategoryLeftPressed = false;
        bool isCategoryRightPressed = false;

        std::uint32_t mfgExpressionIndex = 0;
        float mfgExpressionValue = 0.0f;
    };
}