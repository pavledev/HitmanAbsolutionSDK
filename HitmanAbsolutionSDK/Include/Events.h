#pragma once

#include "EventDispatcher.h"
#include "Common.h"

#include "Glacier/TArray.h"
#include "Glacier/ZString.h"

class HitmanAbsolutionSDK_API Events
{
  public:
    static EventDispatcher<TArray<ZString>&>* OnConsoleCommand;
};
