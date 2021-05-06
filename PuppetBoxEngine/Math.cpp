#include "pch.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Math.h"

namespace PB
{
	namespace GfxMath
	{
		namespace
		{
			/**
			* \brief Helper function to convert from GLM's mat4 to local mat4 struct
			* 
			* \param m	The glm::mat4 object populated with data
			* 
			* \return The local mat4 representation of the given glm::mat4 object
			*/
			mat4 glmToMat4(glm::mat4 m)
			{
				vec4 v[4] = {
					{m[0][0], m[0][1], m[0][2], m[0][3]},
					{m[1][0], m[1][1], m[1][2], m[1][3]},
					{m[2][0], m[2][1], m[2][2], m[2][3]},
					{m[3][0], m[3][1], m[3][2], m[3][3]}
				};

				return mat4{ v };
			}
		}

		mat4 LookAt(vec3 origin, vec3 target, vec3 up)
		{
			return glmToMat4(glm::lookAt(glm::vec3{ origin.x, origin.y, origin.z }, { target.x, target.y, target.z }, { up.x, up.y, up.z }));
		}

		mat4 Projection(uint32_t viewWidth, uint32_t viewHeight, SceneView::Mode viewMode)
		{
			mat4 projection = mat4::eye();

			// Using +/- (dimension / 2) will center the camera on 0,0
			float left		= -(viewWidth / 2.0f);
			float right		= viewWidth / 2.0f;
			float bottom	= -(viewHeight / 2.0f);
			float top		= viewHeight / 2.0f;
			float zNear		= -1.0f;
			float zFar		= 1.0f;

			if (viewMode == SceneView::Mode::ORTHO)
			{
				projection[0][0] = 2.0f / (right - left);
				projection[1][1] = 2.0f / (top - bottom);
				projection[2][2] = - (2.0f / (zFar - zNear));
				projection[3][0] = -((right + left) / (right - left));
				projection[3][1] = -((top + bottom) / (top - bottom));
				projection[3][2] = -((zFar + zNear) / (zFar - zNear));
			}
			else
			{
				//TODO: Implement perspective projection logic
				//projection = glm::perspective(glm::radians(45.0f), static_cast<float>(viewWidth) / static_cast<float>(viewHeight), 0.1f, 100.0f);
			}

			return projection;
		}

		mat4 Scale(mat4 m, vec3 s)
		{
			vec4 v[4] = {
				m[0] * s[0],
				m[1] * s[1],
				m[2] * s[2],
				m[3]
			};

			return mat4{ v };
		}

		mat4 Translate(mat4 m, vec3 t)
		{
			m[3] = m[0] * t[0] + m[1] * t[1] + m[2] * t[2] + m[3];
			return m;
		}

		mat4 Rotate(mat4 m, float angle, vec3 axies)
		{
			glm::mat4 gM{
					m[0][0], m[0][1], m[0][2], m[0][3],
					m[1][0], m[1][1], m[1][2], m[1][3],
					m[2][0], m[2][1], m[2][2], m[2][3],
					m[3][0], m[3][1], m[3][2], m[3][3]
			};

			glm::mat4 gMat4 = glm::rotate(gM, angle, { axies.x, axies.y, axies.z });

			return glmToMat4(gMat4);
		}

		mat4 Rotate(mat4 m, vec3 angles)
		{
			m = Rotate(m, angles.x, { 1.0f, 0.0f, 0.0f });
			m = Rotate(m, angles.y, { 0.0f, 1.0f, 0.0f });
			m = Rotate(m, angles.z, { 0.0f, 0.0f, 1.0f });

			return m;
		}
	}
}