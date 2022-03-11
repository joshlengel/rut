#include"RUT/rut.h"

#include<iostream>
#include<array>

#include<glm/vec2.hpp>

struct Vertex
{
    glm::vec2 position;
};

static const std::array<Vertex, 6> VERTICES =
{{
    { glm::vec2(-0.5f, -0.5f) },
    { glm::vec2( 0.5f, -0.5f) },
    { glm::vec2( 0.5f,  0.5f) },
    { glm::vec2(-0.5f, -0.5f) },
    { glm::vec2( 0.5f,  0.5f) },
    { glm::vec2(-0.5f,  0.5f) }
}};

static const std::string VERTEX_SOURCE = R"(
    #version 450

    layout(location = 0) in vec2 position;

    layout(location = 0) out vec2 _uv;

    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        _uv = position + 0.5;
    }
)";

static const std::string FRAGMENT_SOURCE = R"(
    #version 450

    layout(location = 0) in vec2 _uv;

    layout(location = 0) out vec4 color;

    void main() {
        color = vec4(_uv.x, 1.0 - _uv.y, 0.0, 1.0);
    }
)";

class Application
{
public:
    Application():
        m_should_close(false)
    {
        // Create window
        rut::WindowProperties props;
        props.width = 900;
        props.height = 600;
        props.title = "RUT example";
        m_window = rut::Window::Create(props);

        std::cout << "Context version: " << m_window->GetContext()->GetVersionMajor() << "." << m_window->GetContext()->GetVersionMinor() << std::endl;

        // Setup mesh
        m_mesh = rut::Mesh::Create(m_window->GetContext(),
            {
                { "position", rut::VT_FVEC2 }
            }
        );
        m_mesh->SetVertices(VERTICES.size(), VERTICES.data());

        // Setup shader
        rut::ShaderProgramProperties shader_props;
        shader_props.vertex_shader = rut::ShaderUnit::Create(m_window->GetContext(), rut::ST_VERTEX, VERTEX_SOURCE);
        shader_props.fragment_shader = rut::ShaderUnit::Create(m_window->GetContext(), rut::ST_FRAGMENT, FRAGMENT_SOURCE);
        shader_props.layout =
        {
            { "position", rut::VT_FVEC2 }
        };

        m_shader = rut::ShaderProgram::Create(m_window->GetContext(), shader_props);

        // Setup renderer
        rut::RendererProperties renderer_props;
        renderer_props.shader = m_shader;
        m_renderer = rut::Renderer::Create(m_window->GetContext(), renderer_props);

        // Show window
        m_window->Show();
    }

    bool ShouldClose() const { return m_should_close || m_window->ShouldClose(); }

    void Update()
    {
        m_window->PollEvents();

       if (m_window->KeyDown(rut::KC_ESCAPE))
            m_should_close = true;
        
        // Render
        m_window->GetContext()->Begin();

        m_renderer->Begin();
        m_renderer->Render(m_mesh);
        m_renderer->End();

        m_window->GetContext()->End();
    }

private:
    std::shared_ptr<rut::Window> m_window;
    std::shared_ptr<rut::Mesh> m_mesh;
    std::shared_ptr<rut::ShaderProgram> m_shader;
    std::shared_ptr<rut::Renderer> m_renderer;
    bool m_should_close;
};

int main()
{
    rut::Api::ChooseDefaults();
    rut::Api::SetContextApi(rut::CONTEXT_API_KHR_SURFACE);
    rut::Api::SetRenderApi(rut::RENDER_API_VULKAN);
    rut::Api::CheckCompatibility();
    rut::Api::PrintInfo();

    Application app;
    
    while (!app.ShouldClose())
        app.Update();
}