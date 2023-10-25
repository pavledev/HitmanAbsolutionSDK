#pragma once

class IReusableProp
{
public:
	virtual ~IReusableProp() = 0;
	virtual void UseOnce() = 0;
	virtual void ResetUsedFlag() = 0;
	virtual bool IsUsed() const = 0;
};
