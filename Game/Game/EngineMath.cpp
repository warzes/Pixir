#include "stdafx.h"
#include "EngineMath.h"

Vec3 operator*(const Mat4& a, const Vec3& b)
{
	Vec3 result;
	result.x = b.x * a.m[0][0] + b.y * a.m[1][0] + b.z * a.m[2][0] + a.m[3][0];
	result.y = b.x * a.m[0][1] + b.y * a.m[1][1] + b.z * a.m[2][1] + a.m[3][1];
	result.z = b.x * a.m[0][2] + b.y * a.m[1][2] + b.z * a.m[2][2] + a.m[3][2];

	float w = b.x * a.m[0][3] + b.y * a.m[1][3] + b.z * a.m[2][3] + a.m[3][3];
	if (w != 0.0f)
	{
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}