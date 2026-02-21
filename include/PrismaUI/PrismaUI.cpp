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
        if (!currentTarget || currentTarget->IsDeleted())
            return;
        Json::Value root(Json::arrayValue);
        auto makeEmptyItem = []() {
            Json::Value item;
            item["name"] = "";
            item["value"] = "";
            return item;
        };
        if (Config::GetSingleton().GetMFGState()) {
            Json::Value categoryObj;
            categoryObj["category"] = "MFG State";
            Json::Value itemsArray(Json::arrayValue);
            auto getSafeName = [](const char* name) {
                return name ? std::string(name) : "";
            };
            if (const auto animData = currentTarget->GetFaceGenAnimationData(); animData) {
                //RE::BSSpinLockGuard locker(animData->lock);
                // expression
                if (!animData->exprOverride)
                {
                    float maxValue = 0;
                    std::uint32_t maxIndex = RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral;
                    const bool isExpression2Zero = animData->unk040.IsZero();
                    const std::uint32_t count = isExpression2Zero || animData->expressionKeyFrame.count >= animData->unk040.count 
                        ? animData->expressionKeyFrame.count : animData->unk040.count;
                    for (std::uint32_t i = 0; i < count; i++) {
                        float value = 0.0f;
                        if (i == RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral)
                            continue;
                        if (isExpression2Zero)
                            value = animData->expressionKeyFrame.values[i];
                        else {
                            if (i < animData->unk040.count)
                                value = animData->unk040.values[i];
                            else
                                value = animData->expressionKeyFrame.values[i];
                        }
                        if (value > maxValue) {
                            maxIndex = i;
                            maxValue = value;
                        }
                    }
                    Json::Value item;
                    std::string name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetExpressionName(maxIndex));
                    if (name.empty()) {
                        name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetExpressionName(RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral));
                        if (name.empty())
                            name = "MoodNeutral";
                    } 
                    item["name"] = name;
                    item["value"] = static_cast<std::uint32_t>(std::round(maxValue * 100.0f));
                    itemsArray.append(item);
                    itemsArray.append(makeEmptyItem());
                }
                else {
                    Json::Value item;
                    std::string name = "";
                    if (mfgExpressionValue != 0.0f) {
                        name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetExpressionName(mfgExpressionIndex));
                        if (name.empty()) {
                            name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetExpressionName(RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral));
                        } 
                    } 
                    else
                        name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetExpressionName(RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral));
                    if (name.empty())
                        name = "MoodNeutral";
                    item["name"] = name;
                    item["value"] = static_cast<std::uint32_t>(std::round(mfgExpressionValue * 100.0f));
                    itemsArray.append(item);
                    itemsArray.append(makeEmptyItem());
                }

                // Modifier, Phoneme
                {
                    std::vector<Json::Value> modifierItems;
                    std::vector<Json::Value> phonemeItems;
                    const std::uint32_t modifierCount = std::max(animData->modifier1.count, animData->modifierKeyFrame.count);
                    for (std::uint32_t i = 0; i < modifierCount; i++) {
                        float value = 0.0f;
                        if ((i >= animData->modifierKeyFrame.count || animData->modifierKeyFrame.values[i] == 0.0f) && i < animData->modifier1.count)
                            value = animData->modifier1.values[i];
                        else if (i < animData->modifierKeyFrame.count)
                            value = animData->modifierKeyFrame.values[i];
                        Json::Value item;
                        std::string name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetModifierName(i));
                        if (name.empty())
                            name = "Modifier" + std::to_string(i);
                        item["name"] = std::to_string(i) + " " + name;
                        item["value"] = static_cast<std::uint32_t>(std::round(value * 100.0f));
                        modifierItems.push_back(item);
                    }
                    const std::uint32_t phonemeCount = std::max(animData->phoneme1.count, animData->phenomeKeyFrame.count);
                    for (std::uint32_t i = 0; i < phonemeCount; i++) {
                        float value = 0.0f;
                        if ((i >= animData->phenomeKeyFrame.count || animData->phenomeKeyFrame.values[i] == 0.0f) && i < animData->phoneme1.count)
                            value = animData->phoneme1.values[i];
                        else if (i < animData->phenomeKeyFrame.count)
                            value = animData->phenomeKeyFrame.values[i];
                        Json::Value item;
                        std::string name = getSafeName(RE::BSFaceGenKeyframeMultiple::GetPhonemeName(i));
                        if (name.empty())
                            name = "Phoneme" + std::to_string(i);
                        item["name"] = std::to_string(i) + " " + name;
                        item["value"] = static_cast<std::uint32_t>(std::round(value * 100.0f));
                        phonemeItems.push_back(item);
                    }
                    const std::uint32_t maxSize = modifierItems.size() > phonemeItems.size() ? modifierItems.size() : phonemeItems.size();
                    for (std::uint32_t i = 0; i < maxSize; i++) {
                        if (i < modifierItems.size())
                            itemsArray.append(modifierItems[i]);
                        else
                            itemsArray.append(makeEmptyItem());
                        if (i < phonemeItems.size())
                            itemsArray.append(phonemeItems[i]);
                        else
                            itemsArray.append(makeEmptyItem());
                        
                    }
                }
            }
            else {
                Json::Value item;
                item["name"] = RE::BSFaceGenKeyframeMultiple::GetExpressionName(RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral);
                item["value"] = 0u;
                itemsArray.append(item);
                itemsArray.append(makeEmptyItem());
                const std::uint32_t modifierCount = RE::BSFaceGenKeyframeMultiple::Modifier::HeadYaw + 1;
                const std::uint32_t phonemeCount = RE::BSFaceGenKeyframeMultiple::Phoneme::W + 1;
                const std::uint32_t maxSize = modifierCount > phonemeCount ? modifierCount : phonemeCount;
                for (std::uint32_t i = 0; i < maxSize; i++) {
                    if (i < modifierCount) {
                        item["name"] = std::to_string(i) + " " + RE::BSFaceGenKeyframeMultiple::GetModifierName(i);
                        item["value"] = 0u;
                        itemsArray.append(item);
                    }
                    else
                        itemsArray.append(makeEmptyItem());
                    if (i < phonemeCount) {
                        item["name"] = std::to_string(i) + " " + RE::BSFaceGenKeyframeMultiple::GetPhonemeName(i);
                        item["value"] = 0u;
                        itemsArray.append(item);
                    }
                    else
                        itemsArray.append(makeEmptyItem());
                }
            }
            categoryObj["items"] = itemsArray;
            root.append(categoryObj);
        }
        auto allMorphs = ActorManager::GetSingleton().GetAllMorphs(currentTarget, true);
        std::uint32_t categoryNum = 0;
        for (const auto& category : allMorphs) {
            Json::Value categoryObj;
            categoryObj["category"] = std::to_string(categoryNum) + " " + category.first;
            categoryNum++;
            std::vector<Json::Value> items;
            std::uint32_t morphNum = 0;
            for (const auto& morph : category.second) {
                Json::Value item;
                item["name"] = std::to_string(morphNum) + " " + morph.morphName;
                morphNum++;
                item["value"] = morph.value;
                items.push_back(item);
            }
            Json::Value itemsArray(Json::arrayValue);
            switch (Config::GetSingleton().GetOtherMethod()) {
            default:
            case 0: {
                for (const auto& item : items) {
                    itemsArray.append(item);
                }
            } break;
            case 1: {
                const std::uint32_t totalSize = items.size();
                if (totalSize < 20) {
                    for (const auto& item : items) {
                        itemsArray.append(item);
                        itemsArray.append(makeEmptyItem());
                    }
                } 
                else {
                    if (totalSize < 40) {
                        for (std::uint32_t i = 0; i < 20; i++) {
                            itemsArray.append(items[i]);
                            if (const std::uint32_t right = i + 20; right < totalSize)
                                itemsArray.append(items[right]);
                            else
                                itemsArray.append(makeEmptyItem());
                        }
                    } 
                    else {
                        const std::uint32_t halfSize = (totalSize + 1) / 2;
                        for (std::uint32_t i = 0; i < halfSize; i++) {
                            itemsArray.append(items[i]);
                            if (const std::uint32_t right = i + halfSize; right < totalSize)
                                itemsArray.append(items[right]);
                            else
                                itemsArray.append(makeEmptyItem());
                        }
                    }
                }
            } break;
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
            if (RE::ConsoleLog::IsConsoleMode())
                UIManager::GetSingleton().OpenUI();
            else
                UIManager::GetSingleton().CloseUI();
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
