#pragma once

#include <borealis.hpp>

class EpilogueMain : public brls::Box
{
  public:
    EpilogueMain();

    static brls::View* create();

  private:
    static bool onPrimaryButtonClicked(brls::View* view);
    static bool onNetButtonClicked(brls::View* view);
};
