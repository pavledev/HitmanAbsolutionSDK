#pragma once

struct STypeID;

struct SPropertyInfo
{
	STypeID* m_Type;
	unsigned int m_nExtraData;
	unsigned int m_Flags;
	void (*m_PropertySetCallBack)(void* param1, void* param2, unsigned int param3, bool param4);
	void (*m_PropetyGetter)(void* param1, void* param2, unsigned int param3);
};
