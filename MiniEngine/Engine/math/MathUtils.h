#pragma once
#include "Engine/pch.h"
#undef min
#undef max

class MathUtils
{
public:
    static constexpr float PI = 3.141592654f;
    static constexpr float DEG_TO_RAD = 0.01745329f;
    static constexpr float RAD_TO_DEG = 57.2957795f;
    static constexpr float FLOAT_EPSILON = 0.00001f;

    static constexpr float kPi = 3.141592654f;
    static constexpr float k2Pi = 6.283185307f;
    static constexpr float kPiOver2 = 1.570796327f;
    static constexpr float k1OverPi = 0.318309886f;
    static constexpr float k1Over2Pi = 0.159154943f;

    static float inverseSafe(float f)
    {
        return abs(f) > FLOAT_EPSILON ? 1.0f / f : .0f;
    }

    static float lerp(float start, float end, float t)
    {
        
        return (1 - t) * start + t * end;
    }

    static float clamp(float v, float min, float max)
    {
        return std::max(std::max(v, min), max);;
    }

    // ??????2pi???theta??? -pi ? pi ????
    static float wrapPi(float theta) 
    {
        theta += kPi;
        theta -= std::floor(theta * k1Over2Pi) * k2Pi;
        theta -= kPi;
        return theta;
    }

    // ???????
    static float safeAcos(float x) 
    {
        if (x <= -1.0f)
        {
            return kPi;
        }
        if (x >= 1.0f)
        {
            return 0.0f;
        }
        return acos(x);
    }

	static inline void sinCos(float* returnSin, float* returnCos, float theta)
	{
		*returnSin = sin(theta);
		*returnCos = cos(theta);
	}
}
;