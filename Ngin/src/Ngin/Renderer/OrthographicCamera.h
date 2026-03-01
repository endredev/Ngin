#pragma once

#include "Ngin/Core.h"

namespace Ngin {

	class NG_API OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetPosition(float x, float y, float z = 0.0f);
		void SetRotation(float radians);

		const float* GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

		float m_ProjectionMatrix[16];
		float m_ViewProjectionMatrix[16];

		float m_Position[3] = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

}
