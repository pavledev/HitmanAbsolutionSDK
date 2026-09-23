#pragma once

#include <IModInterface.h>

class Items : public IModInterface
{
  public:
    Items();
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;

  private:
    bool isOpen;
    int selectedItemIndex;
};

DECLARE_HMASDK_MOD(Items)
