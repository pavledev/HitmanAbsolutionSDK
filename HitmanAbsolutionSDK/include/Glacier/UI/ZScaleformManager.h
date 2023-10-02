#pragma once

#include "IScaleformManager.h"

class HitmanAbsolutionSDK_API ZScaleformManager : public IScaleformManager
{
public:
	bool IsInMainMenu() const;

private:
	PAD(0xEB);
	bool m_bIsInMainMenu; //0xEF
};
