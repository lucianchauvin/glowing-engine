#ifndef MODEL_ASS_H
#define MODEL_ASS_H

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"

class Model_ass {
    public:
        Model_ass() = default;

        Model_ass(const std::string &meshName, float scale = 1.0f) {
            load_model(meshName, scale);
        }
        
        void load_model(const std::string &meshName, float scale = 1.0f);
        void draw(Shader &shader);	

    private:
        // model data
        std::vector<Texture> textures_loaded;
        std::vector<Mesh> meshes;
        std::string directory;
        // bool gammaCorrection;

        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        void normalize_model(float scale);
        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string typeName);
        unsigned int texutre_from_file(const char *path, const std::string &directory, bool gamma);
};
#endif
