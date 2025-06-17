#include <vector>
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <stb_image.h>

#include "model_manager.h"
#include "model_ass.h"

namespace Model_manager {

    static std::vector<Model_ass> models;
    static std::vector<std::string> names;
    static std::string base_path;

    static bool loaded_already(const std::string& new_model_name, size_t& existing_idx) {
        for (size_t i = 0; i < names.size(); i++) {
            if (new_model_name == names[i]) {
                existing_idx = i;
                return true;
            }
        }
        return false;
    }

    void init(std::string path) {
        base_path = path;
        model_handle mh = load_model("cube.obj", 0);
    }

    void cleanup() {
        // need to free resources that model_ass owns, prob implement in destructor
        //models.clear();
        //names.clear();
    }

    model_handle load_model(const std::string& model_name, int gltf) {
        size_t existing_idx;
        if (loaded_already(model_name, existing_idx)) {
            printf("[MODEL] Already loaded: %s\n", model_name.c_str());
            return existing_idx;
        }

        std::string full_path;
        if (gltf)
            full_path = base_path + model_name + "/scene.gltf";
        else
            full_path = base_path + model_name;
        
        printf("[MODEL] Loading: %s\n", full_path.c_str());

        Model_ass model;
        int fail = model.load_model(full_path);
        if (fail)
            return 0; // default model

        size_t new_idx = models.size();
        models.push_back(std::move(model));
        names.push_back(model_name);

        return new_idx;
    }

    Model_ass& get_model_by_name(const std::string& model_name) {
        for (size_t i = 0; i < names.size(); i++) {
            if (model_name == names[i]) {
                return models[i];
            }
        }

        assert(false);
    }

    Model_ass& get_model(const model_handle model_id) {
        return models[model_id];
    }

    void draw(const Shader* shader, const model_handle model_id) {
        models[model_id].draw(shader);
    }


    //Model_ass& get_model_by_name_load(const std::string& model_name) {
    //    size_t index;
    //    if (!loaded_already(model_name, index)) {
    //        index = load_model(model_name);
    //    }
    //    return models[index];
    //}

    size_t get_model_count() {
        return models.size();
    }

    std::string get_name(const model_handle& model_id) {
        return names[model_id];
    }

    Util::aabb get_aabb(const model_handle& model_id) {
        Util::aabb aabb{ models[model_id].aabb_min, models[model_id].aabb_max };
        return aabb;
    }
}
