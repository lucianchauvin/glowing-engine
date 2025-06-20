#include "model_ass.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture_manager.h"

#define CHECK_GL_ERROR() { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        std::cerr << "OpenGL error at line " << __LINE__ << ": " << err << std::endl; \
    } \
}

void Model_ass::draw(const Shader* shader, bool shadow_pass) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader, shadow_pass);
}  

int Model_ass::load_model(const std::string &path, float scale) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return -1;
    }
    directory = path.substr(0, path.find_last_of('/'));
    CHECK_GL_ERROR();
    process_node(scene->mRootNode, scene, path);
    normalize_model(scale);
    return 0;
}

void Model_ass::process_node(aiNode *node, const aiScene *scene, const std::string& path) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(process_mesh(mesh, scene, path));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, path);
    }
} 

Mesh Model_ass::process_mesh(aiMesh *mesh, const aiScene *scene, const std::string& path) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal   = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            const aiVector3D& pTangent = mesh->mTangents[i];
            vertex.Tangent = glm::vec3(pTangent.x, pTangent.y, pTangent.z);

            const aiVector3D& pBitangent = mesh->mBitangents[i];
            vertex.Bitangent = glm::vec3(pBitangent.x, pBitangent.y, pBitangent.z);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertex.Tangent = glm::vec3(0.0f);
            vertex.Bitangent = glm::vec3(0.0f);
        }

        vertices.push_back(vertex);
    }
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    unsigned int albedo = 0, normal = 0, metrough = 0, occ = 0, emis = 0;
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        if (material->GetTextureCount(aiTextureType_BASE_COLOR)) {
            aiString str;
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &str);
            albedo = Texture_manager::load_from_path(path.substr(0, path.size() - 10) + str.C_Str());
        }        
        
        if (material->GetTextureCount(aiTextureType_NORMALS)) {
            aiString str;
            material->GetTexture(aiTextureType_NORMALS, 0, &str);
            normal = Texture_manager::load_from_path(path.substr(0, path.size() - 10) + str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_UNKNOWN)) {
            // Try to find metallic-roughness texture by name patterns
            for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_UNKNOWN); i++) {
                aiString str;
                material->GetTexture(aiTextureType_UNKNOWN, i, &str);
                std::string texName = str.C_Str();
                // Common naming patterns for metallic-roughness maps
                if (texName.find("metallic") != std::string::npos ||
                    texName.find("roughness") != std::string::npos ||
                    texName.find("orm") != std::string::npos) { // ORM = Occlusion/Roughness/Metallic
                    metrough = Texture_manager::load_from_path(path.substr(0, path.size() - 10) + str.C_Str());
                    break;
                }
            }
        }
   
    }

    Material material(albedo, normal, metrough, occ, emis);

    return Mesh(vertices, indices, material);
}  

void Model_ass::normalize_model(float scale) {
    aabb_min = glm::vec3(FLT_MAX);
    aabb_max = glm::vec3(-FLT_MAX);

    for (auto &m : meshes) {
        for (auto &v : m.vertices) {
            aabb_min.x = std::min(aabb_min.x, v.Position.x);
            aabb_min.y = std::min(aabb_min.y, v.Position.y);
            aabb_min.z = std::min(aabb_min.z, v.Position.z);

            aabb_max.x = std::max(aabb_max.x, v.Position.x);
            aabb_max.y = std::max(aabb_max.y, v.Position.y);
            aabb_max.z = std::max(aabb_max.z, v.Position.z);
        }
    }

    //printf("starting aabb max %f %f %f\n", aabb_max.x, aabb_max.y, aabb_max.z);
    //printf("starting aabb min %f %f %f\n", aabb_min.x, aabb_min.y, aabb_min.z);

    glm::vec3 center = 0.5f * (aabb_min + aabb_max);
    glm::vec3 diff   = aabb_max - aabb_min;
    float maxDim     = std::max(diff.x, std::max(diff.y, diff.z));
    if (maxDim < 1e-8f) {
        maxDim = 1.0f;
    }
    //float scale_f = scale / maxDim;  // so the largest dimension goes from -1 to +1
    float scale_f = 1.0f; // dont scale, just center

    for (auto& m : meshes) {
        for (auto& v : m.vertices) {
            // Center around origin
            v.Position = (v.Position - center) * scale_f;
            // Then shift Y so bottom is at y=0
            //v.Position.y += (center.y - aabb_min.y) * scale_f; // why ?>?????? todo figure out bruh
        }
        m.update_vertex_buffer();
    }

    // Recalculate final AABB
    aabb_min = glm::vec3(FLT_MAX);
    aabb_max = glm::vec3(-FLT_MAX);
    for (auto& m : meshes) {
        for (auto& v : m.vertices) {
            aabb_min.x = std::min(aabb_min.x, v.Position.x);
            aabb_min.y = std::min(aabb_min.y, v.Position.y);
            aabb_min.z = std::min(aabb_min.z, v.Position.z);

            aabb_max.x = std::max(aabb_max.x, v.Position.x);
            aabb_max.y = std::max(aabb_max.y, v.Position.y);
            aabb_max.z = std::max(aabb_max.z, v.Position.z);
        }
    }

    //printf("after aabb max %f %f %f\n", aabb_max.x, aabb_max.y, aabb_max.z);
    //printf("after aabb min %f %f %f\n", aabb_min.x, aabb_min.y, aabb_min.z);

    //printf("after aabb max %f %f %f\n", aabb_max.x, aabb_max.y, aabb_max.z);
    //printf("after aabb min %f %f %f\n", aabb_min.x, aabb_min.y, aabb_min.z);
}
