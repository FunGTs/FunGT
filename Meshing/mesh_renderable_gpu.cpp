#include "mesh_renderable_gpu.hpp"

#include "mesh_renderable.hpp"
#include "Shaders/shader.hpp"

Shader* BuildMeshRenderableShader()
{
    return Shader::create().release();
}

void FreeMeshRenderableShader(Shader* shader)
{
    delete shader;
}

void RegisterMeshRenderableGPUCallbacks()
{
    MeshRenderable::s_shaderCreate = &BuildMeshRenderableShader;
    MeshRenderable::s_shaderFree = &FreeMeshRenderableShader;
}
