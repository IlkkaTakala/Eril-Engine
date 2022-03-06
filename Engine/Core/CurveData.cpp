#include "CurveData.h"

CurveData::CurveData(const std::vector<std::pair<float, float>>& data) : points(data)
{
}

float CurveData::EvaluateCurve(float delta) const
{
	for (int i = 0; i < points.size(); i++) {
		if (points[i].first <= delta) {
			if (i + 1 < points.size()) {
				if (points[i + 1].first >= delta) {
					float dist = points[i + 1].first - points[i].first;
					float change = points[i + 1].second - points[i].second;
					float p = (delta - points[i].first) / dist;
					return points[i].second + p * change;
				}
			}
			else return points[i].second;
		}
	}
}

Vector VectorCurveData::EvaluateCurve(float delta) const
{
	for (int i = 0; i < points.size(); i++) {
		if (points[i].first <= delta) {
			if (i + 1 < points.size()) {
				if (points[i].first >= delta) {
					float dist = points[i + 1].first - points[i].first;
					Vector change = points[i + 1].second - points[i].second;
					float p = (delta - points[i].first) / dist;
					return points[i].second + p * change;
				}
			}
			else return points[i].second;
		}
	}
}
