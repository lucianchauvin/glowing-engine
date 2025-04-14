#include "model_ass.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define CHECK_GL_ERROR() { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        std::cerr << "OpenGL error at line " << __LINE__ << ": " << err << std::endl; \
    } \
}

void Model_ass::draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}  

void Model_ass::load_model(const std::string &path, float scale) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = directory = path.substr(0, path.find_last_of('/'));
    CHECK_GL_ERROR();
    process_node(scene->mRootNode, scene);
    normalize_model(scale);
}

void Model_ass::process_node(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(process_mesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
} 

Mesh Model_ass::process_mesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.Position = glm::vec3(mesh->mVertices[i].x,
                                    mesh->mVertices[i].y,
                                    mesh->mVertices[i].z);
        vertex.Normal   = glm::vec3(mesh->mNormals[i].x,
                                    mesh->mNormals[i].y,
                                    mesh->mNormals[i].z);
        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);  

        vertices.push_back(vertex);
    }
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}  

void Model_ass::normalize_model(float scale) {
    // We'll gather the min/max by scanning *all* vertices in all meshes.
    glm::vec3 vmin(FLT_MAX);
    glm::vec3 vmax(-FLT_MAX);

    // 1. Find the bounding box across all meshes
    for (auto &m : meshes) {
        for (auto &v : m.vertices) {
            vmin.x = std::min(vmin.x, v.Position.x);
            vmin.y = std::min(vmin.y, v.Position.y);
            vmin.z = std::min(vmin.z, v.Position.z);

            vmax.x = std::max(vmax.x, v.Position.x);
            vmax.y = std::max(vmax.y, v.Position.y);
            vmax.z = std::max(vmax.z, v.Position.z);
        }
    }

    // 2. Compute center and scale
    glm::vec3 center = 0.5f * (vmin + vmax);
    glm::vec3 diff   = vmax - vmin;
    float maxDim     = std::max(diff.x, std::max(diff.y, diff.z));
    if (maxDim < 1e-8f) {
        // Avoid division by zero if the model is basically a single point
        maxDim = 1.0f;
    }
    float scale_f = scale / maxDim;  // so the largest dimension goes from -1 to +1

    // 3. Shift and scale all vertex positions
    for (auto &m : meshes) {
        for (auto &v : m.vertices) {
            // Shift to center, then scale
            v.Position = (v.Position - center) * scale_f;
        }

        m.update_vertex_buffer();
    }
}

std::vector<Texture> Model_ass::load_material_textures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true; 
                break;
            }
        }
        if(!skip) {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = texutre_from_file(str.C_Str(), directory, true); // true is gamma
            texture.type = typeName;
            texture.path = std::string(str.C_Str());
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

unsigned int Model_ass::texutre_from_file(const char *path, const std::string &directory, bool gamma) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = 0;
        if (nrComponents == 1) {
            format = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        // std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
