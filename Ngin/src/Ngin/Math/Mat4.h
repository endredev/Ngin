#pragma once

#include <cmath>
#include <cstring>

namespace Ngin {

	// Column-major 4x4 matrix (OpenGL native format).
	// data[col*4 + row]
	struct Mat4
	{
		float data[16] = {};

		static Mat4 Identity()
		{
			Mat4 m;
			m.data[0]  = 1.0f;
			m.data[5]  = 1.0f;
			m.data[10] = 1.0f;
			m.data[15] = 1.0f;
			return m;
		}

		// Orthographic projection, near=-1, far=1
		static Mat4 Ortho(float left, float right, float bottom, float top)
		{
			Mat4 m = Identity();
			m.data[0]  =  2.0f / (right - left);
			m.data[5]  =  2.0f / (top - bottom);
			m.data[10] = -1.0f;
			m.data[12] = -(right + left) / (right - left);
			m.data[13] = -(top + bottom) / (top - bottom);
			return m;
		}

		static Mat4 Translate(float x, float y, float z)
		{
			Mat4 m = Identity();
			m.data[12] = x;
			m.data[13] = y;
			m.data[14] = z;
			return m;
		}

		static Mat4 RotateZ(float radians)
		{
			Mat4 m = Identity();
			float c = std::cos(radians);
			float s = std::sin(radians);
			m.data[0] =  c;
			m.data[1] =  s;
			m.data[4] = -s;
			m.data[5] =  c;
			return m;
		}

		Mat4 operator*(const Mat4& rhs) const
		{
			Mat4 result;
			for (int col = 0; col < 4; col++)
				for (int row = 0; row < 4; row++)
				{
					float sum = 0.0f;
					for (int k = 0; k < 4; k++)
						sum += data[k * 4 + row] * rhs.data[col * 4 + k];
					result.data[col * 4 + row] = sum;
				}
			return result;
		}
	};

}
