#include"ShaderTools.h"

#include"RUT/Config.h"

#include<stdexcept>
#include<cstring>

#include<shaderc/shaderc.hpp>

#ifdef RUT_HAS_OPENGL
#include<spirv_glsl.hpp>
#endif

static shaderc_shader_kind SHADER_TYPE_TO_SHADERC_KIND[] =
{
    shaderc_vertex_shader,
    shaderc_fragment_shader,
    shaderc_geometry_shader
};

namespace rut
{
    rut::CodeBlob GenerateVulkanShader(const std::string &source, rut::ShaderType type)
    {
        shaderc::Compiler compiler;
        shaderc::CompilationResult result = compiler.CompileGlslToSpv(source, SHADER_TYPE_TO_SHADERC_KIND[type], "shader");
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            throw std::runtime_error("Error compiling shader: " + result.GetErrorMessage());
        
        rut::CodeBlob blob;
        blob.bytes = std::distance(result.begin(), result.end()) * sizeof(uint32_t);
        blob.data = new char[blob.bytes];
        std::memcpy(blob.data, result.begin(), blob.bytes);
        return blob;
    }

#ifdef RUT_HAS_OPENGL
    rut::CodeBlob GenerateOpenGLShader(const std::string &source, rut::ShaderType type)
    {
        rut::CodeBlob blob = GenerateVulkanShader(source, type);

        spirv_cross::CompilerGLSL compiler(reinterpret_cast<uint32_t*>(blob.data), blob.bytes / sizeof(uint32_t));
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        /*if (type != rut::ST_VERTEX)
        {
            for (auto &input : resources.stage_inputs)
            {
                compiler.unset_decoration(input.id, spv::DecorationBinding);
                compiler.unset_decoration(input.id, spv::DecorationDescriptorSet);
            }
        }
        if (type != rut::ST_FRAGMENT)
        {
            for (auto &output : resources.stage_outputs)
            {
                compiler.unset_decoration(output.id, spv::DecorationBinding);
                compiler.unset_decoration(output.id, spv::DecorationDescriptorSet);
            }
        }*/
        for (auto &uniform_buffers : resources.uniform_buffers)
        {
            compiler.set_decoration(uniform_buffers.id, spv::DecorationGLSLShared, 1);
        }

        spirv_cross::CompilerGLSL::Options options{};
        options.version = 330;
        options.es = false;
        options.separate_shader_objects = false;
        compiler.set_common_options(options);

        std::string res = compiler.compile();
        delete[] blob.data;

        blob.bytes = res.length();
        blob.data = new char[blob.bytes];
        std::memcpy(blob.data, res.data(), blob.bytes);
        return blob;
    }
#endif
}