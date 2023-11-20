#pragma once

#undef min
#undef max

class MathUtility
{
public:
	template <typename T>
	static T Clamp(T value, T min, T max)
	{
		return std::max(min, std::min(max, value));
	}
};
