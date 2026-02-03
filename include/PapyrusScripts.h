#pragma once

namespace Mus {
    namespace Papyrus {
        constexpr std::string_view ScriptFileName = "MuFacialExpressionExtended";
        bool RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);
    }
}
