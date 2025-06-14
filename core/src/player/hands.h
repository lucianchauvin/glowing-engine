// #ifndef HANDS_MODEL_H
// #define HANDS_MODEL_H

// #include <glm/glm.hpp>
// #include <glm/gtc/quaternion.hpp>
// #include "weapon.h"

// class HandsModel {
// public:
//     // Hand positions (in camera space)
//     glm::vec3 left_hand_pos;
//     glm::vec3 right_hand_pos;
    
//     // Hand rotations
//     glm::vec3 left_hand_rot;
//     glm::vec3 right_hand_rot;
    
//     // Hand offsets from base position (for animations)
//     glm::vec3 left_hand_offset;
//     glm::vec3 right_hand_offset;
    
//     // References to models
//     Model* left_hand_model;
//     Model* right_hand_model;
    
//     // Current animation state
//     float anim_time;
//     bool is_firing;
//     bool is_reloading;
    
//     HandsModel(Model* left_model, Model* right_model) 
//         : left_hand_model(left_model),
//           right_hand_model(right_model),
//           left_hand_pos(glm::vec3(0.3f, -0.6f, -0.8f)),
//           right_hand_pos(glm::vec3(-0.3f, -0.6f, -0.8f)),
//           left_hand_rot(glm::vec3(0.0f)),
//           right_hand_rot(glm::vec3(0.0f)),
//           left_hand_offset(glm::vec3(0.0f)),
//           right_hand_offset(glm::vec3(0.0f)),
//           anim_time(0.0f),
//           is_firing(false),
//           is_reloading(false) {}
    
        
//         // Apply animations (firing, reloading, etc.)
//         if (is_firing) {
//             apply_firing_animation(weapon, deltaTime);
//         }
        
//         if (is_reloading) {
//             apply_reload_animation(weapon, deltaTime);
//         }
        
//         // Apply ADS position adjustment
//         bool is_ads = glm::distance(weapon->wep_pos, weapon->ads_pos) < 0.1f;
//         if (is_ads) {
//             // Move hands closer to face when aiming
//             left_hand_pos.z += 0.2f;
//             right_hand_pos.z += 0.2f;
//         }
        
//         // Apply weapon shake to hand positions
//         if (weapon->is_shaking) {
//             left_hand_offset += weapon->shake_offset;
//             right_hand_offset += weapon->shake_offset;
//         }
//     }
    
//     void start_firing_animation() {
//         is_firing = true;
//         anim_time = 0.0f;
//     }
    
//     void start_reload_animation() {
//         is_reloading = true;
//         anim_time = 0.0f;
//     }
    
// private:
//     // Apply firing animation based on weapon type
//     void apply_firing_animation(Weapon* weapon, float deltaTime) {
//         // Simple recoil animation
//         float recoil = std::max(0.0f, 0.05f - anim_time * 0.2f);
        
//         if (weapon->type == MELEE) {
//             // Slashing animation for melee
//             float swing_progress = std::min(anim_time * 4.0f, 1.0f);
//             float swing_angle = sin(swing_progress * 3.14159f) * 0.5f;
            
//             right_hand_offset.x += swing_angle * 0.3f;
//             left_hand_offset.x += swing_angle * 0.3f;
            
//             // End animation after one cycle
//             if (anim_time > 0.5f) {
//                 is_firing = false;
//             }
//         }
//         else {
//             // Recoil animation for guns
//             right_hand_offset.z += recoil;
//             left_hand_offset.z += recoil;
            
//             // End animation quickly
//             if (anim_time > 0.1f) {
//                 is_firing = false;
//             }
//         }
//     }
    
//     // Apply reload animation based on weapon type
//     void apply_reload_animation(Weapon* weapon, float deltaTime) {
//         Gun* gun = dynamic_cast<Gun*>(weapon);
//         if (!gun) {
//             is_reloading = false;
//             return;
//         }
        
//         // Calculate animation progress (0.0 to 1.0)
//         float progress = anim_time / gun->reload_time;
        
//         if (progress < 0.3f) {
//             // Initial magazine drop motion
//             float drop_factor = progress / 0.3f;
//             right_hand_offset.y -= drop_factor * 0.2f;
//             right_hand_offset.x -= drop_factor * 0.1f;
//         }
//         else if (progress < 0.7f) {
//             // New magazine insertion
//             float insert_factor = (progress - 0.3f) / 0.4f;
//             right_hand_offset.y = -0.2f + insert_factor * 0.2f;
//             right_hand_offset.x = -0.1f + insert_factor * 0.1f;
//         }
//         else {
//             // Finishing motion
//             float finish_factor = (progress - 0.7f) / 0.3f;
//             // Small tap motion at the end
//             if (finish_factor > 0.5f) {
//                 left_hand_offset.z -= 0.02f;
//             }
//         }
        
//         // End animation when reload is complete
//         if (progress >= 1.0f) {
//             is_reloading = false;
//         }
//     }
    
//     // Get the final position for a hand (base + offset)
//     glm::vec3 get_final_hand_position(bool is_left) {
//         if (is_left) {
//             return left_hand_pos + left_hand_offset;
//         } else {
//             return right_hand_pos + right_hand_offset;
//         }
//     }
// };

// #endif