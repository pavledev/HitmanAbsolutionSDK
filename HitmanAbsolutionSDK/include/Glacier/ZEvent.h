#pragma once

#include "TList.h"
#include "TArray.h"
#include "TPair.h"
#include "ZDelegate.h"

class ZEventNull;

class ZEvent0
{
  private:
    struct SInvocationData
    {
        SInvocationData* pNullOrDelegateAddedIndicator;
        uint32_t nRemoved;
        TList<TPair<int32_t, ZDelegate<void __cdecl(void)>>> added;
    };

    TArray<TPair<int32_t, ZDelegate<void __cdecl(void)>>> m_delegates;
    SInvocationData* m_pInvocation;
};

template<typename T> class ZEvent1
{
  private:
    struct SInvocationData
    {
        SInvocationData* pNullOrDelegateAddedIndicator;
        uint32_t nRemoved;
        TList<TPair<int32_t, ZDelegate<void __cdecl(T)>>> added;
    };

    TArray<TPair<int32_t, ZDelegate<void __cdecl(T)>>> m_delegates;
    SInvocationData* m_pInvocation;
};

template<typename T1, typename T2> class ZEvent2
{
  private:
    struct SInvocationData
    {
        SInvocationData* pNullOrDelegateAddedIndicator;
        uint32_t nRemoved;
        TList<TPair<int32_t, ZDelegate<void __cdecl(T1, T2)>>> added;
    };

    TArray<TPair<int32_t, ZDelegate<void __cdecl(T1, T2)>>> m_delegates;
    SInvocationData* m_pInvocation;
};

template<typename T1, typename T2, typename T3> class ZEvent3
{
  private:
    struct SInvocationData
    {
        SInvocationData* pNullOrDelegateAddedIndicator;
        uint32_t nRemoved;
        TList<TPair<int32_t, ZDelegate<void __cdecl(T1, T2, T3)>>> added;
    };

    TArray<TPair<int32_t, ZDelegate<void __cdecl(T1, T2, T3)>>> m_delegates;
    SInvocationData* m_pInvocation;
};

template<typename T1, typename T2, typename T3, typename T4> class ZEvent4
{
  private:
    struct SInvocationData
    {
        SInvocationData* pNullOrDelegateAddedIndicator;
        uint32_t nRemoved;
        TList<TPair<int32_t, ZDelegate<void __cdecl(T1, T2, T3, T4)>>> added;
    };

    TArray<TPair<int32_t, ZDelegate<void __cdecl(T1, T2, T3, T4)>>> m_delegates;
    SInvocationData* m_pInvocation;
};

template<typename T1 = ZEventNull, typename T2 = ZEventNull, typename T3 = ZEventNull, typename T4 = ZEventNull, typename T5 = ZEventNull>
class ZEvent;

template<> class ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull> : public ZEvent0
{};

template<typename T1> class ZEvent<T1, ZEventNull, ZEventNull, ZEventNull, ZEventNull> : public ZEvent1<T1>
{};

template<typename T1, typename T2> class ZEvent<T1, T2, ZEventNull, ZEventNull, ZEventNull> : public ZEvent2<T1, T2>
{};

template<typename T1, typename T2, typename T3> class ZEvent<T1, T2, T3, ZEventNull, ZEventNull> : public ZEvent3<T1, T2, T3>
{};

template<typename T1, typename T2, typename T3, typename T4> class ZEvent<T1, T2, T3, T4, ZEventNull> : public ZEvent4<T1, T2, T3, T4>
{};
