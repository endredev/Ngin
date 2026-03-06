#include "ngpch.h"
#include "PerspectiveCamera.h"
#include "Ngin/Math/Mat4.h"

namespace Ngin {

	PerspectiveCamera::PerspectiveCamera(float fovY, float aspectRatio, float nearZ, float farZ)
	{
		Mat4 proj = Mat4::Perspective(fovY, aspectRatio, nearZ, farZ);
		memcpy(m_ProjectionMatrix, proj.data, sizeof(float) * 16);
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetPosition(float x, float y, float z)
	{
		m_Position[0] = x;
		m_Position[1] = y;
		m_Position[2] = z;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetRotation(float pitch, float yaw)
	{
		m_Pitch = pitch;
		m_Yaw   = yaw;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		// View = RotateX(-pitch) * RotateY(-yaw) * Translate(-position)
		Mat4 translate = Mat4::Translate(-m_Position[0], -m_Position[1], -m_Position[2]);
		Mat4 rotX      = Mat4::RotateX(-m_Pitch);
		Mat4 rotY      = Mat4::RotateY(-m_Yaw);
		Mat4 view      = rotX * rotY * translate;

		Mat4 proj;
		memcpy(proj.data, m_ProjectionMatrix, sizeof(float) * 16);

		Mat4 viewProj = proj * view;
		memcpy(m_ViewProjectionMatrix, viewProj.data, sizeof(float) * 16);
	}

}
