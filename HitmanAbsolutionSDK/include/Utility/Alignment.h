#pragma once

#ifndef CONCAT_IMPL
#define CONCAT_IMPL(x, y) x##y
#endif

#ifndef MACRO_CONCAT
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#endif

#ifndef PAD
#define PAD(SIZE) unsigned char MACRO_CONCAT(__padding, __COUNTER__)[SIZE]
#endif
