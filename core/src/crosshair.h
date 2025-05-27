#ifndef CROSSHAIR_H
#define CROSSHAIR_H

//#include <glad/glad.h>
#include <glm/glm.hpp>

#include <shader.h>

class Crosshair {
public:
    //Crosshair(const char* file) {}

    //Crosshair(float thickness, float gap, float length, glm::vec3 color) {
    //    Crosshair(thickness, gap, length, length, color);
    //}

    Crosshair(float thickness, float gap, float height, float width, float opacity, glm::vec3 color)
    : thickness(thickness), gap(gap), height(height), width(width), opacity(opacity), color(color)
    {
        glGenVertexArrays(1, &idiot);
    }

    ~Crosshair() {
        glDeleteVertexArrays(1, &idiot);
    }

    void draw(const Shader& shader, const int& screen_width, const int& screen_height) const {

        shader.setFloat("thickness", thickness);
        shader.setFloat("gap", gap);
        shader.setFloat("height", height);
        shader.setFloat("width", width);
        shader.setFloat("opacity", opacity);
        shader.setVec3("color", color);
        shader.setVec2("screen_size", glm::vec2(screen_width, screen_height));

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(idiot);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void gui() {
        ImGui::Begin("Crosshair");

        ImGui::SliderFloat("Thickness", &thickness, 0.5f, 10.0f);
        ImGui::SliderFloat("Gap", &gap, 0.0f, 50.0f);
        ImGui::SliderFloat("Height", &height, 1.0f, 100.0f);
        ImGui::SliderFloat("Width", &width, 1.0f, 100.0f);
        ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);

        ImGui::ColorEdit3("Color", &color.x);

        ImGui::End();
    }

//private:
    unsigned int idiot;
    float thickness, gap, height, width, opacity;
    glm::vec3 color;
};
#endif
