#pragma once

#include "Ngin/Core.h"
#include "Camera.h"

namespace Ngin {

	class NG_API PerspectiveCamera : public Camera
	{
	public:
		// fovY: vertical field of view in radians
		PerspectiveCamera(float fovY, float aspectRatio, float nearZ, float farZ);

		void SetPosition(float x, float y, float z);
		void SetRotation(float pitch, float yaw); // radians
		void SetAspectRatio(float aspectRatio);

		const float* GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

	private:
		void RecalculateProjectionMatrix();
		void RecalculateViewMatrix();

		float m_ProjectionMatrix[16];
		float m_ViewProjectionMatrix[16];

		float m_FovY        = 0.785398f;
		float m_AspectRatio = 1.0f;
		float m_NearZ       = 0.1f;
		float m_FarZ        = 100.0f;

		float m_Position[3] = { 0.0f, 0.0f, 0.0f };
		float m_Pitch = 0.0f;
		float m_Yaw   = 0.0f;
	};

}
