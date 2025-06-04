#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>

namespace Texture_manager {
    void init();
    void cleanup();
    unsigned int load_from_path(const std::string& file_path);
    void bind(int texture_id, unsigned int texture_unit = 0);
    size_t get_texture_count();
    std::string get_name(unsigned int texture_id);
}
#endif
