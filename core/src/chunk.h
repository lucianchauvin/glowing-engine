#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>
#include "PerlinNoise/PerlinNoise.hpp"

#include "model.h"

const int CHUNK_SIZE_X = 16;
const int CHUNK_SIZE_Y = 64;
const int CHUNK_SIZE_Z = 16;

enum class Block_type {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,
    SAND,
    WATER,
};

class Chunk {
public:
    Chunk(int chunkX, int chunkZ) : posX(chunkX), posZ(chunkZ), blocks(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, Block_type::AIR) {
        generate_terrain();
        update();
        model = std::make_unique<Model>(posBuf, norBuf);
    }
    
    ~Chunk() = default;
    
    void set_block(int x, int y, int z, Block_type type) {
        blocks[getIndex(x, y, z)] = type;
        isDirty = true;
    }
    // Get a block at the given local position
    Block_type get_block(int x, int y, int z) const {
        return blocks[getIndex(x, y, z)];
    }
    
    void generate_terrain() {
        // std::cout << "---------------------------------\n";
        // std::cout << "* frequency [0.1 .. 8.0 .. 64.0] \n";
        // std::cout << "* octaves   [1 .. 8 .. 16]       \n";
        // std::cout << "* seed      [0 .. 2^32-1]        \n";
        // std::cout << "---------------------------------\n";
        // Calculate the world position of this chunk's corner
        int worldX = posX * CHUNK_SIZE_X;
        int worldZ = posZ * CHUNK_SIZE_Z;
        // Scale factor for the noise

        double scale = 1.0 / frequency;
        // Generate the height map using Perlin noise
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                // Calculate world coordinates for this block
                double wx = (worldX + x) * scale;
                double wz = (worldZ + z) * scale;
                // Get noise value and convert to height
                double noiseValue = perlin.octave2D_01(wx, wz, octaves);
                // Scale noise to get a height between minHeight and maxHeight
                int minHeight = 10;
                int maxHeight = CHUNK_SIZE_Y - 10;
                int height = minHeight + static_cast<int>((maxHeight - minHeight) * noiseValue);
                // Clamp the height to valid range
                height = std::clamp(height, 1, CHUNK_SIZE_Y - 1);
                // Fill from bottom to height with appropriate blocks
                for (int y = 0; y < height; y++) {
                    set_block(x, y, z, Block_type::STONE);
                }
                // // Add dirt layer
                // for (int y = height - 4; y < height; y++) {
                //     set_block(x, y, z, Block_type::DIRT);
                // }
                // // Add grass or sand on top based on height
                // if (height < 25) { // Water level + a bit
                //     set_block(x, height, z, Block_type::SAND);
                    
                //     // Add water above sand if below water level
                //     int waterLevel = 22;
                //     for (int y = height + 1; y <= waterLevel; y++) {
                //         set_block(x, y, z, Block_type::WATER);
                //     }
                // } else {
                //     set_block(x, height, z, Block_type::GRASS);
                // }
            }
        }
        
        isDirty = true;
    }
    
    void update() {
        if (isDirty) {
            create_mesh();
            isDirty = false;
        }
    }
    
    void create_mesh() {
        // could be dangerous
        posBuf.clear();
        norBuf.clear();
        // texBuf.clear();
        
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (get_block(x, y, z) == Block_type::AIR) {
                        continue;
                    }
                    add_faces_if_needed(x, y, z);
                }
            }
        }
        
        update_model();
    }
    
    void draw(Shader shader) const {
        if (!posBuf.empty()) {
            // printf("DRAWING!\n");
            model->draw(shader);
        } else {
            assert(false);
        }
    }
    
    glm::ivec2 chunk_pos_chunk() const { return glm::ivec2(posX               , posZ               ); }
    glm::ivec2 chunk_pos_world() const { return glm::ivec2(posX * CHUNK_SIZE_X, posZ * CHUNK_SIZE_Z); }

    bool in_chunk(glm::vec3& pos) { return in_chunk (pos.x, pos.z); }
    bool in_chunk(const float world_x, const float world_z) const {
        int chunk_min_x = posX * CHUNK_SIZE_X;
        int chunk_min_z = posZ * CHUNK_SIZE_Z;
        int chunk_max_x = chunk_min_x + CHUNK_SIZE_X;
        int chunk_max_z = chunk_min_z + CHUNK_SIZE_Z;
    
        return (world_x >= chunk_min_x && world_x < chunk_max_x &&
                world_z >= chunk_min_z && world_z < chunk_max_z);
    }
    

    float height_at(glm::vec3& pos) { return height_at (pos.x, pos.z); }
    float height_at(float x, float z) { // world x and z
        int local_x = static_cast<int>(x) % CHUNK_SIZE_X;
        int local_z = static_cast<int>(z) % CHUNK_SIZE_Z;
    
        for (int y = CHUNK_SIZE_Y - 1; y >= 0; y--) {
            if (get_block(local_x, y, local_z) != Block_type::AIR /*&& get_block(local_x, y, local_z) != Block_type::WATER*/) {
                return static_cast<float>(y);
            }
        }
    
        return 0.0f;
    }
    

private:
    // chunk pos in x and z
    // chunk 0 -> chunk 1 = chunk_Size * voxel size[tbd] units
    int posX;
    int posZ;
    
    // Block data
    std::vector<Block_type> blocks;
    
    // Flag for dirty (needs update)
    bool isDirty = false;

    siv::PerlinNoise perlin;
    double frequency = 32.0f;
    int32_t octaves = 1;
    
    // Buffers for mesh data
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    // std::vector<float> texBuf;
    
    // Model
    std::unique_ptr<Model> model;
    
    // Helper to calculate array index from 3D position
    int getIndex(int x, int y, int z) const {
        return (y * CHUNK_SIZE_Z * CHUNK_SIZE_X) + (z * CHUNK_SIZE_X) + x;
    }
    
    // Check if a block is transparent/air (for face culling)
    bool is_transparent(int x, int y, int z) const {
        bool out_of_chunk = x < 0 || x >= CHUNK_SIZE_X ||
                            y < 0 || y >= CHUNK_SIZE_Y ||
                            z < 0 || z >= CHUNK_SIZE_Z;

        if (out_of_chunk) {
            // Treat out-of-bounds as air for meshing purposes
            // (Note: In a full implementation, you'd check neighboring chunks here)
            return false;
        }
        // return get_block(x, y, z) == Block_type::AIR || get_block(x, y, z) == Block_type::WATER;
        return get_block(x, y, z) == Block_type::AIR;
    }
    
    // Get texture coordinates based on block type and face
    // void getTextureCoordinates(Block_type type, int face, float& u, float& v) {
    //     // In a real implementation, you'd have a texture atlas and select the appropriate tiles
    //     // This is a simplified version that just assigns different coords for different block types
        
    //     // Texture atlas coordinates (example)
    //     const float atlasSize = 16.0f;  // 16x16 tiles in atlas
    //     float tileSize = 1.0f / atlasSize;
        
    //     // Texture indices for different blocks and faces
    //     // Format: [top, bottom, side]
    //     int textureIndices[6][3] = {
    //         {0, 2, 3},  // GRASS (top, bottom, side)
    //         {2, 2, 2},  // DIRT (all sides)
    //         {1, 1, 1},  // STONE (all sides)
    //         {4, 4, 4},  // SAND (all sides)
    //         {5, 5, 5},  // WATER (all sides)
    //     };
        
    //     int blockIndex = static_cast<int>(type) - 1;  // -1 because AIR is 0
    //     if (blockIndex < 0) blockIndex = 0;
    // }
    
    void add_faces_if_needed(int x, int y, int z) {
        Block_type blockType = get_block(x, y, z);
        const float vertices[8][3] = {
            {0.0f, 0.0f, -1.0f},  // 0: left-bottom-back
            {1.0f, 0.0f, -1.0f},  // 1: right-bottom-back
            {1.0f, 1.0f, -1.0f},  // 2: right-top-back
            {0.0f, 1.0f, -1.0f},  // 3: left-top-back
            {0.0f, 0.0f, 0.0f},   // 4: left-bottom-front
            {1.0f, 0.0f, 0.0f},   // 5: right-bottom-front
            {1.0f, 1.0f, 0.0f},   // 6: right-top-front
            {0.0f, 1.0f, 0.0f},   // 7: left-top-front
        };
        // Face indices (each face has two triangles)
        const int faces[6][6] = {
            {3, 2, 6, 3, 6, 7},  // Top (Y+)
            {0, 1, 5, 0, 5, 4},  // Bottom (Y-)
            {4, 5, 6, 4, 6, 7},  // Front (Z+)
            {1, 0, 3, 1, 3, 2},  // Back (Z-)
            {0, 4, 7, 0, 7, 3},  // Left (X-)
            {5, 1, 2, 5, 2, 6}   // Right (X+)
        };
        // Normals for each face
        const float normals[6][3] = {
            {0.0f, 1.0f, 0.0f},  // Top (Y+)
            {0.0f, -1.0f, 0.0f}, // Bottom (Y-)
            {0.0f, 0.0f, 1.0f},  // Front (Z+)
            {0.0f, 0.0f, -1.0f}, // Back (Z-)
            {-1.0f, 0.0f, 0.0f}, // Left (X-)
            {1.0f, 0.0f, 0.0f}   // Right (X+)
        };
        // Neighbor positions to check each face for transparency
        const int neighbors[6][3] = {
            {0, 1, 0},   // Top (Y+)
            {0, -1, 0},  // Bottom (Y-)
            {0, 0, 1},   // Front (Z+)
            {0, 0, -1},  // Back (Z-)
            {-1, 0, 0},  // Left (X-)
            {1, 0, 0}    // Right (X+)
        };
        
        for (int face = 0; face < 6; face++) {
            int nx = x + neighbors[face][0];
            int ny = y + neighbors[face][1];
            int nz = z + neighbors[face][2];
            if (is_transparent(nx, ny, nz)) {
                for (int i = 0; i < 6; i++) {
                    int vertexIndex = faces[face][i];
                    posBuf.push_back(vertices[vertexIndex][0] + x);
                    posBuf.push_back(vertices[vertexIndex][1] + y);
                    posBuf.push_back(vertices[vertexIndex][2] + z);
                    norBuf.push_back(normals[face][0]);
                    norBuf.push_back(normals[face][1]);
                    norBuf.push_back(normals[face][2]);
                }
            }
        }
    }
    
    void update_model() {
        std::unique_ptr<Model> newModel = std::make_unique<Model>(posBuf, norBuf);
        model.swap(newModel);
    }
};
#endif
