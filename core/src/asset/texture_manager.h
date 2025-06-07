#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>

typedef size_t texture_handle;

namespace Texture_manager {
    void init();
    void cleanup();
    texture_handle load_from_path(const std::string& file_path);
    void bind(texture_handle texture_id, unsigned int texture_unit = 0);
    size_t get_texture_count();
    std::string get_name(texture_handle texture_id);
}
#endif
