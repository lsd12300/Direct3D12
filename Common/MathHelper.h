#pragma once

#include "stdafx.h"
#include <float.h>
#include <cmath>
#include <Windows.h>


class MathHelper
{
public:
	MathHelper();
	~MathHelper();


	template<typename T>
	static T Lerp(const T& a, const T& b, const T& t)
	{
		return a + (b-a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	//static XMFLOAT4X4 Identity4x4;

	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		return I;
	}

	static const float Pi;
	static const float Infinity;
	//static const XMFLOAT4X4 Identify4x4;

private:

};

const float MathHelper::Pi = 3.1415926535f;
const float MathHelper::Infinity = FLT_MAX;
//const XMFLOAT4X4 MathHelper::Identity4x4 = XMFLOAT4X4(
//			1.0f, 0.0f, 0.0f, 0.0f,
//			0.0f, 1.0f, 0.0f, 0.0f,
//			0.0f, 0.0f, 1.0f, 0.0f,
//			0.0f, 0.0f, 0.0f, 1.0f
//		);


MathHelper::MathHelper()
{
}

MathHelper::~MathHelper()
{
}