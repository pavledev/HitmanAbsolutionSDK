#pragma once

#include <IModInterface.h>

class Items : public IModInterface
{
  public:
    Items();
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;

  private:
    bool isOpen;
    int selectedItemIndex;
};

DECLARE_MOD(Items)
