#pragma once

#include <Windows.h>

class ScopedSharedGuard
{
public:
	ScopedSharedGuard(SRWLOCK* srwLock) : srwLock(srwLock)
	{
		AcquireSRWLockShared(srwLock);
	}

	~ScopedSharedGuard()
	{
		ReleaseSRWLockShared(srwLock);
	}

private:
	SRWLOCK* srwLock;
};

class ScopedExclusiveGuard
{
public:
	ScopedExclusiveGuard(SRWLOCK* srwLock) : srwLock(srwLock)
	{
		AcquireSRWLockExclusive(srwLock);
	}

	~ScopedExclusiveGuard()
	{
		ReleaseSRWLockExclusive(srwLock);
	}

private:
	SRWLOCK* srwLock;
};
