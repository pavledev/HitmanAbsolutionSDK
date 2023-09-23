#pragma once

class IRenderRefCount
{
public:
	virtual ~IRenderRefCount() = 0;
	virtual void AddRef() = 0;
	virtual unsigned int Release() = 0;
};
