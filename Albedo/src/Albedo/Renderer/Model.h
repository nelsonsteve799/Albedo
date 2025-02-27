#pragma once

#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tiny_gltf.h>

namespace Albedo {
    struct TextureSampler {
        int minFilter;
        int magFilter;
        int wrapS;
        int wrapT;
    };

    class Model {
    public:
        Model() {}
        bool Load(const std::string& path);
        void Draw(Ref<Shader> shader);
        void SetSkybox(bool skybox) { m_IsSkybox = skybox; }
        const std::string& GetPath() const { return m_Path; }
    private:
        void LoadTextures(const tinygltf::Model& model);
        void LoadMaterials(tinygltf::Model& model);
        void BindMaterial(Ref<Shader> shader, int materialIndex);

        struct VaoRange
        {
            GLsizei begin; // Index of first element in vertexArrayObjects
            GLsizei count; // Number of elements in range
        };

        struct {
            int albedo = -1;
            int metallicroughness = -1;
            int emissive = -1;
            int occlusion = -1;
            int normal = -1;
        } m_UVs;

        std::string m_Path;
        bool m_IsSkybox = false;
        tinygltf::Model m_model;
        //std::vector<GLuint> bufferObjects;
        std::vector<GLuint> vertexArrayObjects;
        std::vector<VaoRange> meshToVertexArrays;
        std::vector<GLuint> textureObjects;
        Ref<Texture2D> m_NullTexture;
        std::vector<Ref<Texture2D>> m_Textures;
    };

}