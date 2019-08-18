#include "ModelLoader.h"
#include <OpenFBX/src/ofbx.h>
#include <iostream>

namespace Engine
{
    namespace Util
    {
        std::unique_ptr<Model> ModelLoader::LoadFBXData(const std::string& model_path)
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

            auto meshes = std::make_unique<std::vector<std::unique_ptr<Mesh>>>();
            for (int i = 0; i < g_scene->getMeshCount(); ++i)
            {
                const ofbx::Mesh& mesh = *g_scene->getMesh(i);
                const ofbx::Geometry& geom = *mesh.getGeometry();
                const ofbx::Vec3* normals = geom.getNormals();
                const ofbx::Vec2* uvs = geom.getUVs();
                const ofbx::Vec3* vertices = geom.getVertices();

                std::unique_ptr<Mesh> mesh_1 = std::make_unique<Mesh>();
                mesh_1->vertexData = std::make_unique<std::vector<std::unique_ptr<VertexData>>>();
                for (int k = 0; k < geom.getIndexCount(); ++k)
                {
                    std::unique_ptr<VertexData> vd = std::make_unique<VertexData>();

                    vd->pos = glm::vec3(vertices[k].x, vertices[k].y, vertices[k].z);
                    if (normals != nullptr) {
                        vd->normal = glm::vec3(normals[k].x, normals[k].y, normals[k].z);
                    }
                    if (uvs != nullptr) {
                        vd->uv = glm::vec2(uvs[k].x, uvs[k].y);
                    }

                    mesh_1->vertexData->push_back(std::move(vd));
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
    }
}