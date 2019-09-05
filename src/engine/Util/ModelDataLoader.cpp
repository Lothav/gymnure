#include "ModelDataLoader.h"
#include <OpenFBX/src/ofbx.h>
#include <iostream>
#include <tinyobjloader/tiny_obj_loader.h>

namespace Engine
{
    namespace Util
    {
        std::unique_ptr<Model> ModelDataLoader::LoadFBXData(const std::string& model_path)
        {
            auto assets_texture_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + model_path;

            FILE* fp = std::fopen(assets_texture_path.c_str(), "rb");
            if (!fp) { return nullptr; }

            fseek(fp, 0, SEEK_END);
            long file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            auto* content = new ofbx::u8[file_size];
            fread(content, 1, file_size, fp);

            auto g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

            auto meshes = std::make_unique<std::vector<std::shared_ptr<Mesh>>>();
            for (int i = 0; i < g_scene->getMeshCount(); ++i)
            {
                const ofbx::Mesh& mesh = *g_scene->getMesh(i);
                const ofbx::Geometry& geom = *mesh.getGeometry();
                const ofbx::Vec3* normals = geom.getNormals();
                const ofbx::Vec2* uvs = geom.getUVs();
                const ofbx::Vec3* vertices = geom.getVertices();

                std::unique_ptr<Mesh> mesh_1 = std::make_unique<Mesh>();
                mesh_1->vertexData = std::make_shared<std::vector<VertexData>>();
                for (int k = 0; k < geom.getIndexCount(); ++k)
                {
                    VertexData vd{};
                    vd.pos = glm::vec3(vertices[k].x, vertices[k].y, vertices[k].z);
                    if (normals != nullptr) {
                        vd.normal = glm::vec3(normals[k].x, normals[k].y, normals[k].z);
                    }
                    if (uvs != nullptr) {
                        vd.uv = glm::vec2(uvs[k].x, uvs[k].y);
                    }

                    mesh_1->vertexData->push_back(vd);
                }

                std::unique_ptr<Material> mat_1 = std::make_unique<Material>();
                for (int j = 0; j < mesh.getMaterialCount(); ++j)
                {
                    const ofbx::Material& mat = *mesh.getMaterial(j);
                    const ofbx::Texture& texture_diff = *mat.getTexture(ofbx::Texture::TextureType::DIFFUSE);

                    std::cout << mesh.name << std::endl;
                    std::cout << mesh.id << std::endl;

                    char texture_path[255];
                    texture_diff.getRelativeFileName().toString(texture_path);

                    mat_1->texture_path = texture_path;
                }

                mesh_1->material = std::move(mat_1);
                meshes->push_back(std::move(mesh_1));
            }

            std::unique_ptr<Model> model = std::make_unique<Model>();
            model->meshes = std::move(meshes);

            return std::move(model);
        }

        std::unique_ptr<Model> ModelDataLoader::LoadOBJData(const std::string& model_path, const std::string& obj_mtl)
        {
            std::vector <VertexData> vertex_data = {};
            std::vector <uint32_t> index_data = {};

            auto assets_model_path = std::string(ASSETS_FOLDER_PATH_STR) + "/" + model_path;
            auto assets_obj_mtl    = std::string(ASSETS_FOLDER_PATH_STR) + "/" + obj_mtl;

            auto* obj_mtl_ptr = obj_mtl.empty() ? nullptr : assets_obj_mtl.c_str();

            tinyobj::attrib_t attrib;
            std::vector <tinyobj::shape_t> shapes;
            std::vector <tinyobj::material_t> materials;
            std::string err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, assets_model_path.c_str(), obj_mtl_ptr)) {
                throw std::runtime_error(err);
            }

            std::unordered_map<VertexData, uint32_t> uniqueVertices = {};

            for (const auto &shape : shapes) {
                for (const auto &index : shape.mesh.indices) {

                    glm::vec3 pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    glm::vec2 uv = {
                        !attrib.texcoords.empty() ? attrib.texcoords[2 * index.texcoord_index + 0] : 1.0f,
                        !attrib.texcoords.empty() ? 1.0f - attrib.texcoords[2 * index.texcoord_index + 1] : 1.0f
                    };

                    glm::vec3 normal = {
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 0] : 1.0f,
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 1] : 1.0f,
                        index.normal_index > -1 ? attrib.normals[3 * index.normal_index + 2] : 1.0f
                    };

                    struct VertexData vertex = { pos, uv, normal };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertex_data.size());
                        vertex_data.push_back(vertex);
                    }

                    index_data.push_back(uniqueVertices[vertex]);
                }
            }

            return nullptr;
        }

    }
}