#pragma once

struct STypeFunctions
{
	void (*placementConstruct)(void* param1);
	void (*placementCopyConstruct)(void* param1, const void* param2);
	void (*destruct)(void* param1);
	void (*assign)(void* param1, const void* param2);
};
