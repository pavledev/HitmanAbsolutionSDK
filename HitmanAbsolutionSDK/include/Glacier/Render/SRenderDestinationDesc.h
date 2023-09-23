#pragma once

#include "../ZString.h"
#include "ERenderFormat.h"
#include "ERenderDestinationType.h"

struct SRenderDestinationDesc
{
	ZString m_sName;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nWindowHandle;
	unsigned int m_nAdapterID;
	unsigned int m_nMonitorID;
	unsigned int m_nRefreshRate;
	unsigned int m_nParentWindowHandle;
	bool m_bFullScreen;
	bool m_bExclusiveFullscreen;
	bool m_bStereoscopic;
	float m_fAspectRatio;
	float m_fPrimaryAspectRatio;
	unsigned int m_nMultiSampleCount;
	ERenderFormat m_eFormat;
	ERenderDestinationType m_eDestinationType;
	bool m_bProvideDepth : 1;
};
