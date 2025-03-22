#inlcude "model_ass.h"

void Model_ass::draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}  