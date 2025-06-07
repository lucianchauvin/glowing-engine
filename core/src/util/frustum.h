#include <glm/glm.hpp>
#include <array>

#include "physics.h"

enum FrustumPlane {
    LEFT = 0,
    RIGHT,
    BOTTOM,
    TOP,
    NEAR,
    FAR,
    COUNT
};

// Frustum class for culling
class Frustum {
public:
    struct Plane {
        glm::vec3 normal;
        float distance;

        Plane() : normal(0.0f), distance(0.0f) {}

        void set(const glm::vec3& n, const glm::vec3& point) {
            normal = glm::normalize(n);
            distance = -glm::dot(normal, point);
        }

        void set(const glm::vec3& n, float d) {
            normal = glm::normalize(n);
            distance = d;
        }

        float getSignedDistance(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    std::array<Plane, FrustumPlane::COUNT> planes;

    Frustum() = default;

    // Extract frustum planes from combined view-projection matrix
    void update(const glm::mat4& viewProjection) {
        // Left plane
        planes[LEFT].normal.x = viewProjection[0][3] + viewProjection[0][0];
        planes[LEFT].normal.y = viewProjection[1][3] + viewProjection[1][0];
        planes[LEFT].normal.z = viewProjection[2][3] + viewProjection[2][0];
        planes[LEFT].distance = viewProjection[3][3] + viewProjection[3][0];
        
        // Right plane
        planes[RIGHT].normal.x = viewProjection[0][3] - viewProjection[0][0];
        planes[RIGHT].normal.y = viewProjection[1][3] - viewProjection[1][0];
        planes[RIGHT].normal.z = viewProjection[2][3] - viewProjection[2][0];
        planes[RIGHT].distance = viewProjection[3][3] - viewProjection[3][0];
        
        // Bottom plane
        planes[BOTTOM].normal.x = viewProjection[0][3] + viewProjection[0][1];
        planes[BOTTOM].normal.y = viewProjection[1][3] + viewProjection[1][1];
        planes[BOTTOM].normal.z = viewProjection[2][3] + viewProjection[2][1];
        planes[BOTTOM].distance = viewProjection[3][3] + viewProjection[3][1];
        
        // Top plane
        planes[TOP].normal.x = viewProjection[0][3] - viewProjection[0][1];
        planes[TOP].normal.y = viewProjection[1][3] - viewProjection[1][1];
        planes[TOP].normal.z = viewProjection[2][3] - viewProjection[2][1];
        planes[TOP].distance = viewProjection[3][3] - viewProjection[3][1];
        
        // Near plane
        planes[NEAR].normal.x = viewProjection[0][3] + viewProjection[0][2];
        planes[NEAR].normal.y = viewProjection[1][3] + viewProjection[1][2];
        planes[NEAR].normal.z = viewProjection[2][3] + viewProjection[2][2];
        planes[NEAR].distance = viewProjection[3][3] + viewProjection[3][2];
        
        // Far plane
        planes[FAR].normal.x = viewProjection[0][3] - viewProjection[0][2];
        planes[FAR].normal.y = viewProjection[1][3] - viewProjection[1][2];
        planes[FAR].normal.z = viewProjection[2][3] - viewProjection[2][2];
        planes[FAR].distance = viewProjection[3][3] - viewProjection[3][2];

        // Normalize all planes
        for (auto& plane : planes) {
            float invLength = 1.0f / glm::length(plane.normal);
            plane.normal *= invLength;
            plane.distance *= invLength;
        }
    }

    // Manually build frustum from camera parameters
    void buildFrustum(const glm::vec3& position, const glm::vec3& front, 
                     const glm::vec3& up, const glm::vec3& right,
                     float nearDist, float farDist, float fovY, float aspectRatio) {
        // Calculate frustum corners
        float tanHalfFovY = tanf(glm::radians(fovY) * 0.5f);
        float tanHalfFovX = tanHalfFovY * aspectRatio;
        
        // Get frustum points
        glm::vec3 nearCenter = position + front * nearDist;
        glm::vec3 farCenter = position + front * farDist;
        
        float nearHeight = 2.0f * tanHalfFovY * nearDist;
        float nearWidth = nearHeight * aspectRatio;
        float farHeight = 2.0f * tanHalfFovY * farDist;
        float farWidth = farHeight * aspectRatio;
        
        glm::vec3 nearTopLeft = nearCenter + up * (nearHeight * 0.5f) - right * (nearWidth * 0.5f);
        glm::vec3 nearTopRight = nearCenter + up * (nearHeight * 0.5f) + right * (nearWidth * 0.5f);
        glm::vec3 nearBottomLeft = nearCenter - up * (nearHeight * 0.5f) - right * (nearWidth * 0.5f);
        glm::vec3 nearBottomRight = nearCenter - up * (nearHeight * 0.5f) + right * (nearWidth * 0.5f);
        
        glm::vec3 farTopLeft = farCenter + up * (farHeight * 0.5f) - right * (farWidth * 0.5f);
        glm::vec3 farTopRight = farCenter + up * (farHeight * 0.5f) + right * (farWidth * 0.5f);
        glm::vec3 farBottomLeft = farCenter - up * (farHeight * 0.5f) - right * (farWidth * 0.5f);
        glm::vec3 farBottomRight = farCenter - up * (farHeight * 0.5f) + right * (farWidth * 0.5f);
        
        // Set frustum planes
        planes[NEAR].set(-front, nearCenter);
        planes[FAR].set(front, farCenter);
        
        planes[LEFT].set(glm::cross(farBottomLeft - nearBottomLeft, farTopLeft - nearBottomLeft), nearBottomLeft);
        planes[RIGHT].set(glm::cross(farTopRight - nearTopRight, farBottomRight - nearTopRight), nearTopRight);
        planes[TOP].set(glm::cross(farTopLeft - nearTopLeft, farTopRight - nearTopLeft), nearTopLeft);
        planes[BOTTOM].set(glm::cross(farBottomRight - nearBottomRight, farBottomLeft - nearBottomRight), nearBottomRight);
    }

    // Check if a point is inside the frustum
    bool isPointInside(const glm::vec3& point) const {
        for (const auto& plane : planes) {
            if (plane.getSignedDistance(point) < 0.0f) {
                return false;
            }
        }
        return true;
    }

    // Check if a sphere is inside or intersects the frustum
    bool isSphereInside(const Bounding_sphere& sphere) const {
        for (const auto& plane : planes) {
            float distance = plane.getSignedDistance(sphere.center);
            if (distance < -sphere.radius) {
                return false; // Completely outside
            }
        }
        return true; // Inside or intersecting
    }

    // Check if an AABB is inside or intersects the frustum
    bool isAABBInside(const AABB& aabb) const {
        for (const auto& plane : planes) {
            // Find the positive vertex (furthest in direction of normal)
            glm::vec3 positiveVertex = aabb.min;
            if (plane.normal.x >= 0.0f) positiveVertex.x = aabb.max.x;
            if (plane.normal.y >= 0.0f) positiveVertex.y = aabb.max.y;
            if (plane.normal.z >= 0.0f) positiveVertex.z = aabb.max.z;
            
            // If positive vertex is outside, whole AABB is outside
            if (plane.getSignedDistance(positiveVertex) < 0.0f) {
                return false;
            }
        }
        return true;
    }

    // Check if AABB is completely inside frustum
    bool isAABBFullyInside(const AABB& aabb) const {
        for (const auto& plane : planes) {
            // Find the negative vertex (furthest in opposite direction of normal)
            glm::vec3 negativeVertex = aabb.max;
            if (plane.normal.x >= 0.0f) negativeVertex.x = aabb.min.x;
            if (plane.normal.y >= 0.0f) negativeVertex.y = aabb.min.y;
            if (plane.normal.z >= 0.0f) negativeVertex.z = aabb.min.z;
            
            // If negative vertex is outside, AABB is not fully inside
            if (plane.getSignedDistance(negativeVertex) < 0.0f) {
                return false;
            }
        }
        return true;
    }
};