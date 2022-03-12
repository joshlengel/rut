#include"RUT/Utils.h"
#include"RUT/Api.h"

#include<cmath>

#include<stdexcept>

namespace rut
{
    glm::mat4 CreateOrthoProjection(float left, float right, float bottom, float top, float near, float far)
    {
        switch (Api::GetRenderApi())
        {
            case RENDER_API_NONE:
                throw std::runtime_error("Cannot create ortho projection matrix using RENDER_API_NONE");
            
            default:
                throw std::runtime_error("Cannot create ortho projection matrix: Invalid render api");
            
            case RENDER_API_OPENGL:
            {
                glm::mat4 res(1.0f);
                res[0][0] = 2 / (right - left);
                res[1][1] = 2 / (top - bottom);
                res[2][2] = 2 / (far - near);
                res[3][0] = - (right + left) / (right - left);
                res[3][1] = - (top + bottom) / (top - bottom);
                res[3][2] = - (far + near) / (far - near);
                return res;
            }
            
            case RENDER_API_VULKAN:
            case RENDER_API_DX11:
            {
                glm::mat4 res(1.0f);
                res[0][0] = 2 / (right - left);
                res[1][1] = 2 / (bottom - top);
                res[2][2] = 1 / (far - near);
                res[3][0] = - (right + left) / (right - left);
                res[3][1] = - (top + bottom) / (bottom - top);
                res[3][2] = - near / (far - near);
                return res;
            }
        }
    }

    glm::mat4 CreatePerspectiveProjection(float fov, float aspect, float near, float far)
    {
        switch (Api::GetRenderApi())
        {
            case RENDER_API_NONE:
                throw std::runtime_error("Cannot create perspective projection matrix using RENDER_API_NONE");
            
            default:
                throw std::runtime_error("Cannot create perspective projection matrix: Invalid render api");
            
            case RENDER_API_OPENGL:
            {
                float tan_half_fov = tanf(fov * 0.5f);
                glm::mat4 res(1.0f);
                res[0][0] = 1 / (tan_half_fov * aspect);
                res[1][1] = 1 / tan_half_fov;
                res[2][2] = (far + near) / (far - near);
                res[3][2] = - 2 * far * near / (far - near);
                res[2][3] = 1.0f;
                return res;
            }
            
            case RENDER_API_VULKAN:
            case RENDER_API_DX11:
            {
                float tan_half_fov = tanf(fov * 0.5f);
                glm::mat4 res(1.0f);
                res[0][0] = 1 / (tan_half_fov * aspect);
                res[1][1] = -1 / tan_half_fov;
                res[2][2] = far / (far - near);
                res[3][2] = - far * near / (far - near);
                res[2][3] = 1.0f;
                return res;
            }
        }
    }
}