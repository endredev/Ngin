#include "ngpch.h"
#include "OrthographicCamera.h"
#include "Ngin/Math/Mat4.h"

namespace Ngin {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		Mat4 proj = Mat4::Ortho(left, right, bottom, top);
		memcpy(m_ProjectionMatrix, proj.data, sizeof(float) * 16);
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetPosition(float x, float y, float z)
	{
		m_Position[0] = x;
		m_Position[1] = y;
		m_Position[2] = z;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(float radians)
	{
		m_Rotation = radians;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		// View = RotateZ(-rotation) * Translate(-position)
		Mat4 translate = Mat4::Translate(-m_Position[0], -m_Position[1], -m_Position[2]);
		Mat4 rotate    = Mat4::RotateZ(-m_Rotation);
		Mat4 view      = rotate * translate;

		Mat4 proj;
		memcpy(proj.data, m_ProjectionMatrix, sizeof(float) * 16);

		Mat4 viewProj = proj * view;
		memcpy(m_ViewProjectionMatrix, viewProj.data, sizeof(float) * 16);
	}

}
