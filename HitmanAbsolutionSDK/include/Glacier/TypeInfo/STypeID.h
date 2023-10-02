#pragma once

class IType;

struct STypeID
{
	unsigned short flags;
	unsigned short typeNum;
	const IType* pTypeInfo;
	STypeID* pSourceType;
};
