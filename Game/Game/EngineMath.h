#pragma once

struct Vec3 final
{
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };
};


struct Mat4 final
{
	float m[4][4]{ {0} };
};

Vec3 operator*(const Mat4& a, const Vec3& b);