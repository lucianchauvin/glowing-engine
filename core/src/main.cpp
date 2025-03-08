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
    Model sphere;
    sphere.load_mesh("../resources/models/bunny.obj");
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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    // setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(renderer.window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // render loop
    int frame = 0;
    printf("RENDERING\n");
    while (renderer.open()) {
        // printf("[%d]", frame++);
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        player.process_input(renderer.window, deltaTime, scene, &sphere);
        // update player physics
        player.update_player_physics(deltaTime);
        // render scene
        // renderer.render_scene(player);
        renderer.render_scene(player, scene);

        // // gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        
        ImGui::Begin("Controls");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", player.player_physics.player_position.x, player.player_physics.player_position.y, player.player_physics.player_position.z);
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", player.camera.position.x, player.camera.position.y, player.camera.position.z);
        ImGui::Text("Facing: (%.1f, %.1f, %.1f)", player.camera.Front.x, player.camera.Front.y, player.camera.Front.z);
        ImGui::Text("Velocity: (%.1f, %.1f, %.1f)", player.player_physics.velocity.x, player.player_physics.velocity.y, player.player_physics.velocity.z);
        ImGui::Text("On Ground: %s", player.player_physics.isOnGround ? "Yes" : "No");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        renderer.flush();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    renderer.shutdown();
    return 0;
}
