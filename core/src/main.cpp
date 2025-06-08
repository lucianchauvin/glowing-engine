#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/renderer.h"
#include "core/entity.h"
#include "core/scene.h"
#include "core/physics.h"
#include "core/audio.h"
#include "player/player.h"
#include "asset/crosshair.h"
#include "asset/text.h"
#include "asset/model_ass.h"
#include "asset/texture_manager.h"
#include "asset/model_manager.h"

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 900;

float delta_time = 0.0f;
float lastFrame = 0.0f;

int main() {

    Renderer renderer;
    if (!renderer.init(SCR_WIDTH, SCR_HEIGHT, "GLOW")) {
        return -1;
    };
    
    Audio::init();
    Physics::init();

    Texture_manager::init();
    Model_manager::init("../resources/models/");

    Player player;
    renderer.sync_callbacks(player);

    Crosshair crosshair(1.0f, 6.0f, 10.0f, 10.0f, 1.0f, glm::vec3(1.0f, 0.5f, 1.0f));
    
    Scene scene("star"); 
    Model_ass plane("../resources/models/plane.obj");

    //Model_ass sphere("../resources/models/backpack/backpack.obj", 1.0f);
    //Model_ass sphere("../resources/models/cat/cat.obj");
    //Model_ass sphere("../resources/models/bunny.obj");
    Model_ass sphere2("../resources/models/sword_ice/scene.gltf");
    //Model_ass sphere("../resources/models/sponza/main1_sponza/NewSponza_Main_glTF_003.gltf");

    //Model_ass fly("../resources/models/plane/scene.gltf");

    //for (int i = -5; i < 5; i++) {
        //for (int j = 0; j < 10; j++) {
    ////         // int k = 1;
    ////         int j = 1;
    ////         for (int k = 0; k < 10; k++) {
                //glm::vec3 pos   = glm::vec3(6.0f * i, j * 6.0f + 1, -6.0f * j); 
                //glm::vec3 scale = glm::vec3(1.0f);
                //glm::vec3 color = glm::vec3(0.1f * i, 0.1f * j, 0.1f * j);
    //            if ((i + j) % 2) {
    //                Entity e(&sphere, pos, true, scale, color);
    //                scene.include(e);
    //            } else {
                    //Entity e(&sphere2, pos, true, scale, color, 1.0f, glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
                    //scene.include(e);
    //            }
            //}
        //}
     //}

    glm::vec3 pos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 scale = glm::vec3(100.0f, 1.0f, 100.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    Entity e(&plane, pos, false, scale, color);
    scene.include(e);

    model_handle plane_id = Model_manager::load_model("f22");
    pos = glm::vec3(5.0f, 0.0f, -10.0f);
    scale = glm::vec3(1.0f);
    color = glm::vec3(0.7f);
    Entity e2(plane_id, pos, false, scale, color, 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scene.include(e2);

    model_handle car232323 = Model_manager::load_model("911-2");
    pos = glm::vec3(-3.0f, 0.0f, -3.0f);
    scale = glm::vec3(2.0f);
    color = glm::vec3(0.7f);
    Entity e5(car232323, pos, false, scale, color, 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scene.include(e5);

    Model_ass car("../resources/models/911-2/scene.gltf");
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    color = glm::vec3(0.7f);
    Entity e3(&car, pos, false, scale, color, 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scene.include(e3);

    Model_ass car23("../resources/models/sword/scene.gltf");
    pos = glm::vec3(-1.0f, 2.0f, -2.0f);
    scale = glm::vec3(1.0f);
    color = glm::vec3(0.7f);
    Entity e4(&car23, pos, false, scale, color, 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scene.include(e4);

    // pos   = glm::vec3(0.0f, 30.0f, 0.0f); 
    // scale = glm::vec3(100.0f, 100.0f, 100.0f);
    // color = glm::vec3(1.0f, 1.0f, 1.0f);
    // Entity e2(&sphere, pos, false, scale, color);
    // scene.include(e2);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(renderer.window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    Font font("tx02");
    Text screen_text(font, "999", 0, 1, 50.0f, glm::vec3(0.5f, 0.2f, 0.7f));
    //Font font2("roughsplash");
    //Text screen_text2(font2, "LET ME OUTTTTT", 0, 700, 200.0f, glm::vec3(1.0f, 0.1f, 0.1f));
    /*Font font3("jianjianti");
    Text screen_text3(font3, u8"我爱你", 600, 200, 50.0f, glm::vec3(1.0f, 0.1f, 0.1f));
    Text screen_text4(font3, "hello world!?", 800, 300, 50.0f, glm::vec3(1.0f, 0.1f, 0.1f));*/

    JPH::BodyID ground = Physics::addBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f), true);
    printf("Created ground body\n");
    JPH::BodyID jsphere = Physics::addSphere(glm::vec3(0.0f, 10.0f, -25.0f), 1.0f, false);
    printf("Created falling box\n");
    JPH::BodyID jsphere2 = Physics::addSphere(glm::vec3(0.125f, 15.0f, -25.0f), 1.0f, false);
    printf("Created sphere\n");

    Physics::setBodyVelocity(jsphere2, glm::vec3(0.0f, 5.0f, 0.0f));

    Physics::optimize_broad_phase();

    // render loop
    unsigned int step = 0;
    printf("RENDERING\n");
    while (renderer.open()) {
        float currentFrame = renderer.get_time();

        delta_time = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (!(step % 10))
            screen_text.updateText(std::to_string((int)(1.0f / delta_time)));

        // todo maybe refactor all into jolt controller hmmm
        // controller step takes input
        // updates physics according to active controller
        // updates camera
        // draws hud (weapon, etc)
        player.controller_step(renderer.window, delta_time, scene);

        Physics::update(); // default 1/60 delta time


        // render scene
        renderer.render_scene(player, scene, delta_time);

        glm::vec3 position = Physics::getBodyPosition(jsphere2);
        renderer.debug_sphere_at(position);        
        position = Physics::getBodyPosition(jsphere);
        renderer.debug_sphere_at(position);

        if (!player.key_toggles[(unsigned)'r'])
            renderer.render_debug(player);
        // render scene deferred pipeline
        // renderer.render_scene_deferred(player, scene, delta_time);
        // TODO: clustered forward 

        renderer.render_crosshair(crosshair);
        renderer.render_hud_text(screen_text);
        //renderer.render_hud_text(screen_text2);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        player.debug_hud();
        //player.controller->debug_hud(io);
        //crosshair.gui();

        ImGui::Begin("Light");
        ImGui::SliderFloat3("pos", &renderer.light.position.x, -20.0f, 20.0f);
        ImGui::SliderFloat3("color", &renderer.light.color.x, 0.0f, 1.0f); // 1.0f;     
        ImGui::SliderFloat("itensity", &renderer.light.intensity, 0.0f, 1.0f); // 1.0f;        
        ImGui::End();

        {
            ImGui::Begin("Entity Inspector");

            for (size_t i = 0; i < scene.entities.size(); ++i) {
                Entity& entity = scene.entities[i];
                ImGui::PushID(static_cast<int>(i));

                if (ImGui::TreeNode("Entity")) {
                    // Show & edit position
                    ImGui::DragFloat3("Position", glm::value_ptr(entity.position), 0.1f);

                    // Show & edit scale
                    ImGui::DragFloat3("Scale", glm::value_ptr(entity.scale), 0.1f);

                    // Convert quaternion to Euler angles (in degrees)
                    glm::vec3 euler = glm::degrees(glm::eulerAngles(entity.rotation));
                    if (ImGui::DragFloat3("Rotation (Euler)", glm::value_ptr(euler), 1.0f)) {
                        // Convert back to quaternion if changed
                        glm::vec3 radians = glm::radians(euler);
                        entity.rotation = glm::quat(radians);
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        renderer.flush();
        Audio::update();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //Model_manager::cleanup();
    Texture_manager::cleanup();
    Physics::shutdown();
    renderer.shutdown();
    return 0;
}
