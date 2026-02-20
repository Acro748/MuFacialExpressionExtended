#include "PrismaUI.h"
#include "json/json.h"

namespace Mus {
    void UIManager::Init() {
        if (const auto Menu = RE::UI::GetSingleton(); Menu) {
            Menu->AddEventSink<RE::MenuOpenCloseEvent>(this);
        }
        CreateUI();
    }

    UIManager::EventResult UIManager::ProcessEvent(const RE::MenuOpenCloseEvent* evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) {
        if (!evn || evn->menuName.empty())
            return EventResult::kContinue;
        if (evn->opening) {
            if (IsSameString(evn->menuName.c_str(), "Console"))
                OpenUI();
        } else if (!evn->opening) {
            if (IsSameString(evn->menuName.c_str(), "Console"))
                CloseUI();
        }
        return EventResult::kContinue;
    }
    UIManager::EventResult UIManager::ProcessEvent(RE::InputEvent* const* evn, RE::BSTEventSource<RE::InputEvent*>*) {
        if (!evn || !*evn)
            return EventResult::kContinue;
        const auto categoryLeftKey = Config::GetSingleton().GetCategoryLeftKey();
        const auto categoryRightKey = Config::GetSingleton().GetCategoryRightKey();
        const auto scrollUpKey = Config::GetSingleton().GetScrollUpKey();
        const auto scrollDownKey = Config::GetSingleton().GetScrollDownKey();

        for (RE::InputEvent* input = *evn; input != nullptr; input = input->next) {
            if (input->eventType.all(RE::INPUT_EVENT_TYPE::kButton)) {
                RE::ButtonEvent* button = input->AsButtonEvent();
                if (!button)
                    continue;

                using namespace InputManager;
                std::uint32_t keyCode = 0;
                std::uint32_t keyMask = button->idCode;
                if (button->device.all(RE::INPUT_DEVICE::kMouse))
                    keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
                else if (REL::Module::IsVR() &&
                         button->device.underlying() >= INPUT_DEVICE_CROSS_VR::kVirtualKeyboard &&
                         button->device.underlying() <= INPUT_DEVICE_CROSS_VR::kDeviceType_WindowsMRSecondary) {
                    keyCode = GetDeviceOffsetForDevice(button->device.underlying()) + keyMask;
                } else if (button->device.all(RE::INPUT_DEVICE::kGamepad))
                    keyCode = InputMap::GamepadMaskToKeycode(keyMask);
                else
                    keyCode = keyMask;

                if (!REL::Module::IsVR()) {
                    if (keyCode >= InputMap::kMaxMacros)
                        continue;
                } else {
                    if (keyCode >= InputMap::kMaxMacrosVR)
                        continue;
                }

                if (keyCode == categoryLeftKey) {
                    if (button->IsDown()) {
                        if (!isCategoryLeftPressed)
                            SelectPrevCategoryUI();
                        isCategoryLeftPressed = true;
                    } 
                    else if (button->IsUp()) {
                        isCategoryLeftPressed = false;
                    }
                } 
                else if (keyCode == categoryRightKey) {
                    if (button->IsDown()) {
                        if (!isCategoryRightPressed)
                            SelectNextCategoryUI();
                        isCategoryRightPressed = true;
                    } 
                    else if (button->IsUp()) {
                        isCategoryRightPressed = false;
                    }
                }
                else if (keyCode == scrollUpKey) {
                    if (button->IsPressed()) {
                        ScrollUI(true);
                    }
                } 
                else if (keyCode == scrollDownKey) {
                    if (button->IsPressed()) {
                        ScrollUI(false);
                    }
                } 
            }
        }
        return EventResult::kContinue;
    }

    void UIManager::onEvent(const FrameEvent& e) {
        RE::Actor* newTarget = GetConsoleActor();
        if (!currentTarget || currentTarget->formID != newTarget->formID) {
            UpdateData(newTarget);
        }
    }

    void UIManager::UpdateData(RE::Actor* a_actor) {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;
        if (!a_actor)
            a_actor = GetConsoleActor();
        if (a_actor)
            currentTarget = a_actor;
        if (!currentTarget)
            return;
        Json::Value root(Json::arrayValue);
        auto allMorphs = ActorManager::GetSingleton().GetAllMorphs(currentTarget, true);
        std::uint32_t categoryNum = 0;
        for (const auto& category : allMorphs) {
            Json::Value categoryObj;
            categoryObj["category"] = std::to_string(categoryNum) + " " + category.first.string();
            categoryNum++;
            Json::Value itemsArray(Json::arrayValue);
            std::uint32_t morphNum = 0;
            for (const auto& morph : category.second) {
                Json::Value item;
                item["name"] = std::to_string(morphNum) + " " + morph.morphName.string();
                morphNum++;
                item["value"] = morph.value;
                itemsArray.append(item);
            }
            categoryObj["items"] = itemsArray;
            root.append(categoryObj);
        }
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string jsonString = Json::writeString(builder, root);
        g_prismaUI->InteropCall(g_myView, "UpdateMorphData", jsonString.c_str());
        logger::debug("UpdateData called for actor : {:x} {}", currentTarget->formID, currentTarget->GetName());
    }

    void UIManager::OpenUI() {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView))
            return;
        g_prismaUI->Show(g_myView);
        g_frameEventDispatcher.addListener(this);
        if (const auto inputManager = RE::BSInputDeviceManager::GetSingleton(); inputManager)
            inputManager->AddEventSink<RE::InputEvent*>(this);
        const std::string LeftText = "¢¸ " + SKSE::InputMap::GetKeyName(Config::GetSingleton().GetCategoryLeftKey());
        const std::string RightText = SKSE::InputMap::GetKeyName(Config::GetSingleton().GetCategoryRightKey()) + " ¢º";
        g_prismaUI->InteropCall(g_myView, "UpdateCateogryHintLeftText", LeftText.c_str());
        g_prismaUI->InteropCall(g_myView, "UpdateCateogryHintRightText", RightText.c_str());
        SetUIPositionUI(Config::GetSingleton().GetPosX(), Config::GetSingleton().GetPosY());
        SetUIScaleUI(Config::GetSingleton().GetScale());
        UIManager::GetSingleton().UpdateData();
    }
    void UIManager::CloseUI() {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView))
            return;
        g_prismaUI->Hide(g_myView);
        g_frameEventDispatcher.removeListener(this);
        if (const auto inputManager = RE::BSInputDeviceManager::GetSingleton(); inputManager)
            inputManager->RemoveEventSink(this);
    }

    void UIManager::SelectNextCategoryUI() const {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;
        g_prismaUI->InteropCall(g_myView, "SwitchNextCategory", "");
    }
    void UIManager::SelectPrevCategoryUI() const {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;
        g_prismaUI->InteropCall(g_myView, "SwitchPrevCategory", "");
    }

    void UIManager::ScrollUI(bool isUp) const {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;
        const std::string deltaStr = isUp ? std::to_string(-Config::GetSingleton().GetScrollSpeed()) : std::to_string(Config::GetSingleton().GetScrollSpeed());
        g_prismaUI->InteropCall(g_myView, "ScrollDataList", deltaStr.c_str());
    }

    void UIManager::SetUIPositionUI(float xPercent, float yPercent) {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;

        Json::Value root;
        root["x"] = xPercent;
        root["y"] = yPercent;

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string jsonStr = Json::writeString(builder, root);

        g_prismaUI->InteropCall(g_myView, "UpdateUIPosition", jsonStr.c_str());
    }

    void UIManager::SetUIScaleUI(float scaleMultiplier) {
        if (!g_prismaUI || !g_prismaUI->IsValid(g_myView) || g_prismaUI->IsHidden(g_myView))
            return;
        std::string scaleStr = std::to_string(scaleMultiplier);
        g_prismaUI->InteropCall(g_myView, "UpdateUIScale", scaleStr.c_str());
    }

    void UIManager::CreateUI() {
        if (!IsPrismaUIExists)
            return;
        if (!g_prismaUI) {
            g_prismaUI = reinterpret_cast<PRISMA_UI_API::IVPrismaUI1*>(PRISMA_UI_API::RequestPluginAPI());
            if (!g_prismaUI) {
                IsPrismaUIExists = false;
                return;
            }
        }
        if (g_prismaUI->IsValid(g_myView)) {
            g_prismaUI->Destroy(g_myView);
        }
        g_myView = g_prismaUI->CreateView("MuFacialExpressionExtended\\index.html", [](PrismaView view) -> void {
            logger::info("Create view done : {}", view);
            if (RE::ConsoleLog::IsConsoleMode()) {
                auto p = RE::PlayerCharacter::GetSingleton();
                if (p && p->loadedData && p->loadedData->data3D) {
                    UIManager::GetSingleton().OpenUI();
                }
            }
        });
    }

    RE::Actor* UIManager::GetConsoleActor() {
        RE::Actor* newTarget = nullptr;
        if (auto ref = RE::Console::GetSelectedRef().get(); ref) {
            newTarget = skyrim_cast<RE::Actor*>(ref);
            if (!newTarget)
                newTarget = RE::PlayerCharacter::GetSingleton();
        } 
        else {
            newTarget = RE::PlayerCharacter::GetSingleton();
        }
        return newTarget;
    }
}
