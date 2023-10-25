#pragma once

#include "ZEvent0.h"
#include "ZEvent1.h"
#include "ZEvent2.h"
#include "ZEvent3.h"
#include "ZEvent4.h"

class ZEventNull;

template <typename T1 = ZEventNull, typename T2 = ZEventNull, typename T3 = ZEventNull, typename T4 = ZEventNull, typename T5 = ZEventNull>
class ZEvent;

template <>
class ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull> : public ZEvent0
{
};

template <typename T1>
class ZEvent<T1, ZEventNull, ZEventNull, ZEventNull, ZEventNull> : public ZEvent1<T1>
{
};

template <typename T1, typename T2>
class ZEvent<T1, T2, ZEventNull, ZEventNull, ZEventNull> : public ZEvent2<T1, T2>
{
};

template <typename T1, typename T2, typename T3>
class ZEvent<T1, T2, T3, ZEventNull, ZEventNull> : public ZEvent3<T1, T2, T3>
{
};

template <typename T1, typename T2, typename T3, typename T4>
class ZEvent<T1, T2, T3, T4, ZEventNull> : public ZEvent4<T1, T2, T3, T4>
{
};
