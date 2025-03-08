#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "renderer.h"
#include <controller.h>
#include <shader.h>
#include <entity.h>
#include <scene.h>

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    Controller player;

    Renderer renderer;
    if (!renderer.init(SCR_WIDTH, SCR_HEIGHT, "GLOW", player)) {
        return -1;
    };

    Scene scene;
    Model plane;
    plane.load_mesh("../resources/models/plane.obj");
    plane.init();
    // plane.init();
    // Model* cube;
    Model sphere;
    sphere.load_mesh("../resources/models/sphere.obj");
    sphere.init();
    
    for (int i = 0; i < 10; i++) {
        glm::vec3 pos   = glm::vec3(2.0f * i, 1.0f, 0.0f); 
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 color = glm::vec3(.05f * i, 0.0f, 0.0f);
        Entity e(&sphere, pos, scale, color);
        scene.include(e);
    }

    glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(10.0f, 1.0f, 10.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    Entity e(&plane, pos, scale, color);
    scene.include(e);

    // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    // setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    // ImGui_ImplOpenGL3_Init();

    // render loop
    int frame = 0;
    printf("RENDERING\n");
    while (renderer.open()) {
        // printf("[%d]", frame++);
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        player.process_input(renderer.window, deltaTime);
        // update player physics
        player.update_player_physics(deltaTime);
        // render scene
        // renderer.render_scene(player);
        renderer.render_scene_2(player, scene);
        // // gui
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        
        // ImGui::Begin("Controls");
        // ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        // ImGui::Text("Position: (%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
        // ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player_physics.velocity.x, player_physics.velocity.y, player_physics.velocity.z);
        // ImGui::Text("On Ground: %s", player_physics.isOnGround ? "Yes" : "No");
        // ImGui::Checkbox("Wireframe [t]", &key_toggles[(unsigned)'t']);
        // ImGui::SliderFloat("Texture 1 Blend", &t1, 0.0f, 1.0f);
        // ImGui::SliderFloat("Texture 2 Blend", &t2, 0.0f, 1.0f);
        // ImGui::Text("Camera Mode: %s", is_third_person ? "Third Person" : "First Person");
        // ImGui::Text("Press TAB to toggle camera mode");
        // ImGui::End();

        // ImGui::Begin("Settings");
        // ImGui::SliderFloat("GRAVITY", &GRAVITY, 0.1f, 20.0f);
        // ImGui::SliderFloat("JUMP_FORCE", &JUMP_FORCE, 1.0f, 10.0f);
        // ImGui::SliderFloat("FRICTION", &FRICTION, 0.5f, 1.0f);
        // ImGui::SliderFloat("ACCELERATION", &ACCELERATION, 1.0f, 200.0f);
        // ImGui::SliderFloat("MAX_VELOCITY", &MAX_VELOCITY, 0.1f, 10.0f);
        // ImGui::SliderFloat("PLAYER_HEIGHT", &PLAYER_HEIGHT, 1.0f, 10.0f);
        // ImGui::End();
    }

    renderer.shutdown();
    return 0;
}
