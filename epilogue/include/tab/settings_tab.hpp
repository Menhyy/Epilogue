#pragma once

#include <borealis.hpp>

class SettingsTab : public brls::Box
{
  public:
    SettingsTab();

    BRLS_BIND(brls::InputCell, ipMain, "ipMain");
    BRLS_BIND(brls::InputCell, ipRes, "ipRes");
    BRLS_BIND(brls::InputCell, ipAlt, "ipAlt");
    BRLS_BIND(brls::InputCell, ipResAlt, "ipResAlt");
    BRLS_BIND(brls::BooleanCell, hostBack, "hostBack");
    BRLS_BIND(brls::BooleanCell, noPatchDefault, "noPatchDefault");
    BRLS_BIND(brls::BooleanCell, patchSys, "patchSys");
    BRLS_BIND(brls::BooleanCell, debugMode, "debugMode");
    BRLS_BIND(brls::InputCell, ipBCAT, "ipBCAT");

    static brls::View* create();
};
