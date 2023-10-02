#include "Glacier/Input/ZMouseWindows.h"
#include "Global.h"
#include "Glacier/ZGameTimeManager.h"
#include "Connection/SharedMemoryServer.h"

void ZMouseWindows::Update()
{
    IDirectInputDevice8A* directInputDevice = dif.GetDirectInputDevice();
    SharedMemoryServer::Data* data = SharedMemoryServer::GetInstance().GetData();

    m_updatetime = GameTimeManager->GetRealTime();

    if (directInputDevice && m_active)
    {
        if (!dif.GetAcquired())
        {
            Acquire(10);
        }

        m_pMouseState.lX = 0;
        m_pMouseState.lY = 0;
        m_pMouseState.lZ = 0;
        *(unsigned int*)m_pMouseState.rgbButtons = 0;
        *(unsigned int*)&m_pMouseState.rgbButtons[4] = 0;

        HRESULT result = directInputDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_pMouseState);

        if (FAILED(result))
        {
            dif.SetAcquired(false);

            return;
        }

        m_bButtonSwapped = GetSystemMetrics(SM_SWAPBUTTON) != 0;

        if (m_bButtonSwapped)
        {
            m_pMouseState.rgbButtons[0] = m_pMouseState.rgbButtons[1];
            m_pMouseState.rgbButtons[1] = m_pMouseState.rgbButtons[0];
        }

        for (int i = 0; i < 8; ++i)
        {
            buttonSample(i, (unsigned __int8)m_pMouseState.rgbButtons[i] >> 7, m_updatetime);
        }

        AnalogMotionSample(0, static_cast<float>(m_pMouseState.lX), m_updatetime);
        AnalogMotionSample(1, static_cast<float>(m_pMouseState.lY), m_updatetime);
        AnalogMotionSample(2, static_cast<float>(m_pMouseState.lZ), m_updatetime);

        AnalogSample(3, static_cast<float>(data->mouseCoordinates.x), static_cast<float>(data->mouseCoordinates.x), m_updatetime);
        AnalogSample(4, static_cast<float>(data->mouseCoordinates.y), static_cast<float>(data->mouseCoordinates.y), m_updatetime);

        bool state;

        if (m_pMouseState.lZ <= 0)
        {
            state = false;

            if (m_pMouseState.lZ < 0)
            {
                buttonSample(8, 0, m_updatetime);
                buttonSample(9, 1, m_updatetime);
            }
        }
        else
        {
            state = true;
        }

        buttonSample(8, state, m_updatetime);
        buttonSample(9, 0, m_updatetime);
    }
}