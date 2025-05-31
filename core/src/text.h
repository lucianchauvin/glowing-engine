#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

#include "font.h"
#include "shader.h"
#include "texture_manager.h"

class Text {
//private:
public:

    struct Vertex {
        glm::vec2 position;
        glm::vec2 texCoord;
    };

    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int atlas_texture_id;
    glm::vec3 color;
    size_t indexCount;

    void buildTextMesh(const Font& font, const std::string& text, float x, float y, float scale) {
        vertices.clear();
        indices.clear();

        float currentX = x;
        float currentY = y;
        unsigned int vertexOffset = 0;

        // Build vertex data for all characters
        for (char c : text) {

            if (c == ' ') {
                currentX += 0.5f * scale;
            }

            auto it = font.characters.find(c);
            if (it == font.characters.end()) {
                continue; // Skip unknown characters
            }

            const Font::Glyph& glyph = it->second;

            // Calculate character quad positions
            float xpos = currentX + glyph.planeLeft * scale;
            float ypos = currentY + glyph.planeBottom * scale;
            float w = (glyph.planeRight - glyph.planeLeft) * scale;
            float h = (glyph.planeTop - glyph.planeBottom) * scale;

            // Create quad vertices (bottom-left, bottom-right, top-right, top-left)
            vertices.push_back({ {xpos, ypos}, {glyph.atlasLeft, glyph.atlasBottom} });
            vertices.push_back({ {xpos + w, ypos}, {glyph.atlasRight, glyph.atlasBottom} });
            vertices.push_back({ {xpos + w, ypos + h}, {glyph.atlasRight, glyph.atlasTop} });
            vertices.push_back({ {xpos, ypos + h}, {glyph.atlasLeft, glyph.atlasTop} });

            // Create indices for two triangles
            indices.push_back(vertexOffset + 0);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 3);
            indices.push_back(vertexOffset + 0);

            vertexOffset += 4;
            currentX += glyph.advance * scale;
        }

        indexCount = indices.size();
        setupBuffers();
    }

    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    Text(const Font& font, const std::string& text, float x, float y, float scale, const glm::vec3& textColor) : color(textColor), atlas_texture_id(font.atlas_texture_id) {
        buildTextMesh(font, text, x, y, scale);
    }

    ~Text() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(Shader& shader, const glm::mat4& projection) const {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setVec3("textColor", color);

        //// Bind MSDF texture
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, atlas_texture_id);
        Texture_manager::bind(atlas_texture_id);
        shader.setInt("msdfTexture", 0);

        // Draw the text in one call
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void setColor(const glm::vec3& newColor) {
        color = newColor;
    }
};

#endif