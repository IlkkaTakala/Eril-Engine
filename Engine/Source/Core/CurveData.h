#pragma once
#include <Core.h>

class CurveData
{
public:
	CurveData(const std::vector<std::pair<float, float>>& data);

	float EvaluateCurve(float delta) const;

private:
	std::vector<std::pair<float, float>> points;
};

class VectorCurveData
{
public:
	VectorCurveData(const std::vector<std::pair<float, Vector>>& data) : points(data) {}

	Vector EvaluateCurve(float delta) const;

private:
	std::vector<std::pair<float, Vector>> points;
};

