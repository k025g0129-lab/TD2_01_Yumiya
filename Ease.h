#pragma once
#include <cmath>
#include <numbers>

class Ease {
public:
	static float EaseIn(float t);

	static float EaseOut(float t);

	static float EaseInOut(float t);

	static float EaseInBack(float t);

	static float EaseOutBack(float t);

	static float EaseOutElastic(float t);

};
