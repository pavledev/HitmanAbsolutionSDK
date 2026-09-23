#pragma once

#include <IImGuiRenderer.h>

namespace UI
{
    class MainMenu
    {
      public:
        void Draw(IImGuiRenderer* p_Renderer, bool p_HasFocus);
    };
}
