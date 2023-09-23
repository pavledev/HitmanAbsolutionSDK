#pragma once

template <typename T, int U>
class TRenderReferencedCountedImpl : public T
{
public:
	virtual ~TRenderReferencedCountedImpl() = 0;
	virtual void AddRef() = 0;
	virtual unsigned int Release() = 0;

private:
	int m_ReferenceCount;
};
