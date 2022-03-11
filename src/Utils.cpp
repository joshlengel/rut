#include"RUT/Utils.h"
#include"RUT/Api.h"

#include<glm/gtc/matrix_transform.hpp>

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
                return glm::orthoLH_NO(left, right, bottom, top, near, far);
            
            case RENDER_API_VULKAN:
            case RENDER_API_DX11:
                return glm::orthoRH_ZO(left, right, bottom, top, near, far);
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
                return glm::perspectiveLH_NO(fov, aspect, near, far);
            
            case RENDER_API_VULKAN:
            case RENDER_API_DX11:
                return glm::perspectiveRH_ZO(fov, aspect, near, far);
        }
    }
}