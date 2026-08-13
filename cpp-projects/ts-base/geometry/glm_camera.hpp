
// #pragma once

// // geometry
// #include "geometry/point2.hpp"
// #include "geometry/point3.hpp"
// #include "geometry/point4.hpp"
// #include "geometry/matrix2.hpp"
// #include "geometry/matrix3.hpp"
// #include "geometry/matrix4.hpp"
// #include "geometry/quaternion.hpp"

// // glm
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/quaternion.hpp>
// #include <glm/gtc/matrix_access.hpp>

// namespace tool::geo{

// class GlmCamera{
// public:

//     // Constructor
//     GlmCamera(
//         glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
//         glm::vec3 eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f)
//         ) : position(position) {
//         // Convert Euler angles (pitch, yaw, roll) to a quaternion
//         orientation = glm::quat(eulerAngles);
//         updateVectors();
//     }

//     // Getters
//     glm::vec3 getPosition() const { return position; }
//     glm::vec3 getFront() const { return front; }
//     glm::vec3 getRight() const { return right; }
//     glm::vec3 getUp() const { return up; }
//     glm::quat getOrientation() const { return orientation; }

//     // Setters
//     void setPosition(glm::vec3 newPosition) {
//         position = newPosition;
//     }

//     void setOrientation(glm::quat newOrientation) {
//         orientation = newOrientation;
//         updateVectors();
//     }

//     // Rotate the camera by Euler angles (pitch, yaw, roll) in radians
//     void rotate(float pitch, float yaw, float roll);

//     // Rotate the camera around its position using a quaternion
//     void rotate(glm::quat rotation) {
//         orientation = rotation * orientation;
//         updateVectors();
//     }

//     // Move the camera in world space
//     void move(glm::vec3 direction) {
//         position += direction;
//     }

//     // Move the camera relative to its orientation (e.g., forward, backward, left, right)
//     void moveRelative(glm::vec3 direction) {
//         position += orientation * direction;
//     }

//     void moveFront(float speed) { position += front * speed; }
//     void moveBack(float speed) { position -= front * speed; }
//     void moveRight(float speed) { position += right * speed; }
//     void moveLeft(float speed) { position -= right * speed; }
//     void moveUp(float speed) { position += up * speed; }
//     void moveDown(float speed) { position -= up * speed; }



//     // Get the view matrix
//     glm::mat4 getViewMatrix2() const;
//     geo::Mat4f getViewMatrix() const;

//     // Get the projection matrix (optional, for completeness)
//     glm::mat4 getProjectionMatrix(float fov, float aspect, float near, float far) const;

//     static inline auto from_glm(const glm::fmat4 &m) -> geo::Mat4f{return {m[0][0], m[1][0], m[2][0], m[3][0],m[0][1], m[1][1], m[2][1], m[3][1],m[0][2], m[1][2], m[2][2], m[3][2],m[0][3], m[1][3], m[2][3], m[3][3],};}
//     static inline auto from_glm(const glm::dmat4 &m) -> geo::Mat4d{return {m[0][0], m[1][0], m[2][0], m[3][0],m[0][1], m[1][1], m[2][1], m[3][1],m[0][2], m[1][2], m[2][2], m[3][2],m[0][3], m[1][3], m[2][3], m[3][3],};}
//     static inline auto from_glm(const glm::fquat &q) -> geo::Quatf{return {q.w, q.x, q.y, q.z};}

// private:

//     glm::vec3 position;
//     glm::quat orientation;

//     glm::vec3 front;
//     glm::vec3 right;
//     glm::vec3 up;

//     // Update front, right, and up vectors based on the quaternion
//     void updateVectors();
// };
// }