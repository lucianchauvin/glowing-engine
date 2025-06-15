#ifndef FONT_H
#define FONT_H

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <json.hpp>

#include "shader.h"
#include "texture_manager.h"

// TODO make not shit font manager thing

class Font {
public:
    struct Glyph {
        float advance;

        // Plane bounds (logical coordinates)
        float planeLeft, planeBottom, planeRight, planeTop;
        float atlasLeft, atlasBottom, atlasRight, atlasTop;
    };

    Font() = default;

    Font(const std::string& font_name) {
        atlas_texture_id = Texture_manager::load_from_path("../resources/fonts/" + font_name + "/" + font_name + ".png");

        // load glyphs into datastructure
        std::ifstream json_file("../resources/fonts/" + font_name +"/" + font_name + ".json");
        nlohmann::json json_data;
        //try {
            json_file >> json_data;
        /*}
        catch (nlohmann::json::parse_error& e) {
            std::cerr << "Parse error: " << e.what() << "\n";
            std::cerr << "Exception id: " << e.id << "\n";
            std::cerr << "Byte position: " << e.byte << "\n";
        }*/

        for (const auto& glyph : json_data["glyphs"]) {
            int unicode = glyph["unicode"];
            char c = (char)unicode;

            Glyph g;
            g.advance = glyph["advance"];

            auto& plane = glyph["planeBounds"];
            g.planeLeft = plane["left"];
            g.planeBottom = plane["bottom"];
            g.planeRight = plane["right"];
            g.planeTop = plane["top"];

            auto& atlas = glyph["atlasBounds"];
            //g.atlasLeft = atlas["left"];
            //g.atlasBottom = atlas["bottom"];
            //g.atlasRight = atlas["right"];
            //g.atlasTop = atlas["top"];

            float atlasWidth = json_data["atlas"]["width"];
            float atlasHeight = json_data["atlas"]["height"];
            g.atlasLeft = atlas["left"].get<float>() / atlasWidth;
            g.atlasRight = atlas["right"].get<float>() / atlasWidth;
            //g.atlasTop = atlas["top"] / atlasHeight;
            //g.atlasBottom = atlas["bottom"] / atlasHeight

            g.atlasTop = 1.0f - (atlas["top"].get<float>() / atlasHeight);
            g.atlasBottom = 1.0f - (atlas["bottom"].get<float>() / atlasHeight);

            characters[c] = g;

            //printf("char: %d\n", unicode);
        }
    }

    unsigned int atlas_texture_id;
    std::unordered_map<char, Glyph> characters;
};
#endif
