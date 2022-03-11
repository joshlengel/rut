#pragma once

#include<glm/mat4x4.hpp>

namespace rut
{
    glm::mat4 CreateOrthoProjection(float left, float right, float bottom, float top, float near, float far);
    glm::mat4 CreatePerspectiveProjection(float fov, float aspect, float near, float far);
}