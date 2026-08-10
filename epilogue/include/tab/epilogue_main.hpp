#pragma once

#include <borealis.hpp>

class EpilogueMain : public brls::Box
{
  public:
    EpilogueMain();

    static brls::View* create();

  private:
    static bool onNextendoButtonClicked(brls::View* view);
    static bool onDefaultButtonClicked(brls::View* view);
};
