
// #include "glm_camera.hpp"


// #include "quaternion.hpp"
// #include "geometry/matrix4.hpp"

// using namespace tool;


// void geo::GlmCamera::rotate(float pitch, float yaw, float roll) {


//     glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
//     glm::quat yawQuat   = glm::angleAxis(yaw,   glm::vec3(0.0f, 1.0f, 0.0f));
//     glm::quat rollQuat  = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));

//     // Combine rotations (order: roll * yaw * pitch)
//     orientation = rollQuat * yawQuat * pitchQuat * orientation;
//     updateVectors();
// }

// // glm::mat4 geo::GlmCamera::getViewMatrix() const {
// geo::Mat4f geo::GlmCamera::getViewMatrix() const {

//     auto gposition  = Vec3f{position.x,position.y,position.z};
//     auto gfront     = Vec3f{front.x,front.y,front.z};
//     auto gup        = Vec3f{up.x,up.y,up.z};

//     // return glm::lookAt(position, position + front, up);
//     return geo::look_at(gposition, gposition + gfront, gup);
// }


// glm::mat4 geo::GlmCamera::getViewMatrix2() const {
//     return glm::lookAt(position, position + front, up);
// }


// glm::mat4 geo::GlmCamera::getProjectionMatrix(float fov, float aspect, float near, float far) const {
//     return glm::perspective(fov, aspect, near, far);
// }



// void geo::GlmCamera::updateVectors() {
//     // Default front vector (looking down the negative Z-axis)
//     // front = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
//     // right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
//     // up    = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

//     Quatf orient = from_glm(orientation);
//     auto gfront = geo::rotate(orient, Vec3f{0.0f, 0.0f, -1.0f});
//     auto gright = geo::rotate(orient, Vec3f{1.0f, 0.0f, 0.0f});
//     auto gup    = geo::rotate(orient, Vec3f{0.0f, 1.0f, 0.0f});

//     front = {gfront.x(),gfront.y(),gfront.z()};
//     right = {gright.x(),gright.y(),gright.z()};
//     up    = {gup.x(),gup.y(),gup.z()};
// }
