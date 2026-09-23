#pragma once

#include "imgui.h"

#include <implot.h>

class IImGuiRenderer
{
  public:
    virtual ~IImGuiRenderer() = default;

    virtual bool IsVisible() const = 0;

    virtual void SetFocus(bool p_HasFocus) = 0;

    virtual ImGuiContext* GetContext() const = 0;
    virtual ImPlotContext* GetImPlotContext() const = 0;

    virtual ImGuiMemAllocFunc GetMemAlloc() const = 0;
    virtual ImGuiMemFreeFunc GetMemFree() const = 0;
    virtual void* GetAllocatorUserData() const = 0;

    virtual ImFont* GetLightFont() const = 0;
    virtual ImFont* GetRegularFont() const = 0;
    virtual ImFont* GetMediumFont() const = 0;
    virtual ImFont* GetBoldFont() const = 0;
    virtual ImFont* GetBlackFont() const = 0;
};
