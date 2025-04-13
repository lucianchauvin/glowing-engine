#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "renderer.h"
#include "player.h"
#include "shader.h"
#include "entity.h"
#include "scene.h"
#include "chunk.h"
#include "physics.h"
#include "model_ass.h"
#include "audio.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// weapon
float waveAmplitude = 0.03f;
float waveFrequency = 5.0f;
float waveSpeed = 2.0f;
glm::vec3 clr = glm::vec3(0.0f, 0.4f, 0.0f);
glm::vec3 emis = glm::vec3(0.0f);
glm::vec3 fres = glm::vec3(0.0f, 0.0f, 0.0f);
float expon = 1.0f;

int main() {
    Audio::init();

    Renderer renderer;
    if (!renderer.init(SCR_WIDTH, SCR_HEIGHT, "GLOW")) {
        return -1;
    };

    Player player;
    renderer.sync_callbacks(player);
    
    Scene scene;
    Model_ass plane("../resources/models/plane.obj");

    Model_ass sphere("../resources/models/backpack/backpack.obj");

    Model_ass fly("../resources/models/plane/scene.gltf");

    for (int i = -5; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            int k = 1;
            // for (int k = 0; k < 10; k++) {
                glm::vec3 pos   = glm::vec3(3.0f * i, j * 3.0f, -2.0f * k); 
                glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
                glm::vec3 color = glm::vec3(0.1f * i, 0.1f * j, 0.1f * k);
                if ((i + j) % 2) {
                    Entity e(&sphere, pos, true, scale, color);
                    scene.include(e);
                } else {
                    Entity e(&sphere, pos, true, scale, color, 1.0f, glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
                    scene.include(e);
                }
            // }
        }
    }

    glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(100.0f, 1.0f, 100.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    Entity e(&plane, pos, false, scale, color);
    // scene.include(e);

    std::vector<Chunk*> chunks = std::vector<Chunk*>();
    // for (int x = -2; x < 1; x++) {
    //     for (int z = -2; z < 1; z++) {
    //         // if (x == 0 && z == 0) continue;
    //         Chunk* chunk = new Chunk(x, z);
    //         chunks.push_back(chunk);
    //     }
    // }

    Physics physics;
    physics.load_scene(scene);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(renderer.window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // render loop
    // uint step = 0;
    // int frame = 0;
    printf("RENDERING\n");
    while (renderer.open()) {
		// ++step;        // printf("[%d]", frame++);
        float currentFrame = renderer.get_time();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // controller step takes input
        // updates physics according to active controller
        // updates camera
        // draws hud (weapon, etc)
        player.controller_step(renderer.window, deltaTime, scene);

        // render scene
        renderer.render_scene(player, scene, deltaTime, chunks);
        // render scene deferred pipeline
        // renderer.render_scene_deferred(player, scene, deltaTime);
        // TODO: clustered forward 


        // renderer.render_world_geometry(scene, player);
		// renderer.draw_player_model(player, fly);
        // renderer.draw_player_stuff(player, clr, emis, fres, expon);



        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();        
        
        ImGui::Begin("Info");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::SliderFloat("waveAmplitude", &waveAmplitude, -10.0, 10.0f); // 0.03f;
        ImGui::SliderFloat("waveFrequency", &waveFrequency, -10.0, 10.0f); // 5.0f;
        ImGui::SliderFloat("waveSpeed", &waveSpeed, -10.0, 10.0f); // 2.0f;
        ImGui::SliderFloat3("color", &clr.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("emis", &emis.x, 0.0f, 1.0f); // 1.0f;     
        ImGui::SliderFloat3("fresclr", &fres.x, 0.0, 1.0f); // 1.0f;     
        ImGui::SliderFloat("exp", &expon, -5.0, 25.0f); // 1.0f;        
        ImGui::End();

        player.debug_hud(io);
        player.controller->debug_hud(io);
        renderer.material.draw_imgui_editor(io);

        ImGui::Begin("Light");
        ImGui::SliderFloat3("pos", &renderer.light.position.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("color", &renderer.light.color.x, 0.0f, 1.0f); // 1.0f;     
        ImGui::SliderFloat("itensity", &renderer.light.intensity, -5.0, 25.0f); // 1.0f;        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        renderer.flush();
        Audio::update();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    renderer.shutdown();
    return 0;
}
