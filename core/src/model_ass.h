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

        glm::vec3 aabb_min;
        glm::vec3 aabb_max;

    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;

        // bool gammaCorrection;

        void process_node(aiNode *node, const aiScene *scene, const std::string& path);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene, const std::string& path);
        void normalize_model(float scale);
};
#endif
