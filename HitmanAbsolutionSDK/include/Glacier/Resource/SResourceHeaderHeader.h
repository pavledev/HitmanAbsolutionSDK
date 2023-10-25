#pragma once

struct SResourceHeaderHeader
{
	unsigned int m_type;
	unsigned int m_nReferencesChunkSize;
	unsigned int m_nStatesChunkSize;
	unsigned int m_nDataSize;
	unsigned int m_nSystemMemoryRequirement;
	unsigned int m_nVideoMemoryRequirement;
};
