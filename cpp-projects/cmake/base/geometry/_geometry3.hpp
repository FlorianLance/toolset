

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once

// std
#include <numeric>
#include <list>

// local
//#include "geometry/aabb3.hpp"
//#include "geometry/sphere.hpp"
//#include "geometry/obb3.hpp"
//#include "geometry/mesh.hpp"
//#include "geometry/line3.hpp"
//#include "geometry/raycast.hpp"

namespace tool::geo {

//inline Vec3<acc> project(const Vec3<acc> &dir) const{return Vec<acc,3>::project(dir);}
//inline Vec3<acc> perpendicular(const Vec3<acc> &dir) const{return Vec<acc,3>::perpendicular(dir);}
//inline Vec3<acc> reflection(const Vec3<acc> &normal) const{return Vec<acc,3>::reflection(normal);}


//// ############################################################################### SPHERE
//template <typename acc>
//bool line_test(const Sphere<acc> &s, const Line3<acc> &l);

//// ############################################################################### PLANE
//template <typename acc>
//Plane3<acc> from_triangle(const Triangle3<acc> &t);

//// ############################################################################### TRIANGLE
//template<typename acc>
//bool overlap_on_axis(const AABB3<acc> &aabb, const Triangle3<acc> &t, const Vec3<acc> &axis);
//template<typename acc>
//bool triangle_AABB(const Triangle3<acc> &t, const AABB3<acc> &a);

//// ############################################################################### MESH
//template<typename acc>
//Mesh<acc> mesh_from_aabb(const AABB3<acc> &aabb);
//template<typename acc>
//Mesh<acc> blend_meshes(const std::vector<Mesh<acc>> &meshes);
//template<typename acc>
//acc mesh_ray(const Mesh<acc> &mesh, const Ray3<acc> &ray);


//template <typename acc>
//bool line_test(const Sphere<acc> &s, const Line3<acc> &l){
//    return (s.position-(closest_point(l, s.position))).square_norm() <= (s.radius*s.radius);
//}

//template <typename acc>
//Plane3<acc> from_triangle(const Triangle3<acc> &t){

//    Vec3<acc> normal = normalized(cross<acc>(vec(t.a(),t.b()),vec(t.a(),t.c())));
//    return Plane3<acc>(normal, dot(normal, t.a()));
//}

//template<typename acc>
//bool overlap_on_axis(const AABB3<acc> &aabb, const Triangle3<acc> &t, const Vec3<acc> &axis){

//    Interval<acc> a = interval(aabb, axis);
//    Interval<acc> b = interval(t, axis);
//    return ((b.min() <= a.max()) && (a.min() <= b.max()));
//}

//template<typename acc>
//bool triangle_AABB(const Triangle3<acc> &t, const AABB3<acc> &a){

//    Vec3<acc> f0 = vec(t.a(),t.b());
//    Vec3<acc> f1 = vec(t.b(),t.c());
//    Vec3<acc> f2 = vec(t.c(),t.a());

//    Vec3<acc> u0 = {1,0,0};
//    Vec3<acc> u1 = {0,1,0};
//    Vec3<acc> u2 = {0,0,1};

//    std::array<Vec3<acc>,13> tests = {
//        u0, u1, u2, cross(f0,f1),
//        cross(u0, f0), cross(u0, f1), cross(u0, f2),
//        cross(u1, f0), cross(u1, f1), cross(u1, f2),
//        cross(u2, f0), cross(u2, f1), cross(u2, f2)
//    };

//    for(const auto &test : tests){
//        if(!overlap_on_axis(a, t, test)){
//            return false; // separating axis found
//        }
//    }
//    return true; // separating axis not found
//}

//template<typename acc>
//Mesh<acc> mesh_from_aabb(const AABB3<acc> &aabb){

//    Mesh<acc> mesh;
//    Pt3<acc> p0 = aabb.origin + Vec3<acc>(-aabb.size.x(),-aabb.size.y(),-aabb.size.z());
//    Pt3<acc> p1 = aabb.origin + Vec3<acc>(+aabb.size.x(),-aabb.size.y(),-aabb.size.z());
//    Pt3<acc> p2 = aabb.origin + Vec3<acc>(+aabb.size.x(),+aabb.size.y(),-aabb.size.z());
//    Pt3<acc> p3 = aabb.origin + Vec3<acc>(-aabb.size.x(),+aabb.size.y(),-aabb.size.z());
//    Pt3<acc> p4 = aabb.origin + Vec3<acc>(-aabb.size.x(),-aabb.size.y(),+aabb.size.z());
//    Pt3<acc> p5 = aabb.origin + Vec3<acc>(+aabb.size.x(),-aabb.size.y(),+aabb.size.z());
//    Pt3<acc> p6 = aabb.origin + Vec3<acc>(+aabb.size.x(),+aabb.size.y(),+aabb.size.z());
//    Pt3<acc> p7 = aabb.origin + Vec3<acc>(-aabb.size.x(),+aabb.size.y(),+aabb.size.z());

//    mesh.vertices = {p0,p1,p2,p3,p4,p5,p6,p7};
//    mesh.triIds = {TriIds(0,2,1), TriIds(0,3,2), TriIds(4,1,5), TriIds(4,0,1), TriIds(7,5,6), TriIds(7,4,5),
//                   TriIds(1,6,5), TriIds(1,2,6), TriIds(4,3,0), TriIds(4,7,3), TriIds(3,6,2), TriIds(3,7,6)};

//    return mesh;
//}

//template<typename acc>
//Mesh<acc> blend_meshes(const std::vector<Mesh<acc>> &meshes){

//    Mesh<acc> result;
//    size_t offset = 0;
//    for(const auto& mesh : meshes){

//        std::copy(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(result.vertices));
//        std::vector<TriIds> idTriangles;
//        std::copy(mesh.triIds.begin(), mesh.triIds.end(), std::back_inserter(idTriangles));

//        for(auto &idT : idTriangles){
//            idT.ids += offset;
//        }

//        offset += mesh.vertices.size();
//        std::move(idTriangles.begin(), idTriangles.end(),  std::back_inserter(result.triIds));
//    }

//    return result;
//}

//template<typename acc>
//acc mesh_ray(const Mesh<acc> &mesh, const Ray3<acc> &ray){

//    if (!mesh.accelerator) {

//        for (size_t ii = 0; ii < mesh.triIds.size(); ++ii) {
//            acc result = raycast_triangle(mesh.triangle(ii), ray);
//            if(!(result < 0)){ // TODO
//                return result;
//            }
//        }
//    }else {

//        std::list<BVHNode<acc>*> toProcess;
//        toProcess.push_front(mesh.accelerator.get());

//        // Recursivley walk the BVH tree
//        while (!toProcess.empty()) {

//            BVHNode<acc>* iterator = *(toProcess.begin());
//            toProcess.erase(toProcess.begin());

//            if (iterator->triangles.size() >= 0) {

//                // Iterate trough all triangles of the node
//                for (size_t ii = 0; ii < iterator->triangles.size(); ++ii) {

//                    // Triangle indices in BVHNode index the mesh
//                    RaycastResult<acc> raycast;
//                    raycast_triangle(mesh.triangle(iterator->triangles[ii]), ray, &raycast);
//                    acc r = raycast.t;
//                    if (r >= 0) { // TODO
//                        return r;
//                    }
//                }
//            }

//            for (int ii = 8 - 1; ii >= 0; --ii) {
//                if(iterator->children[ii] == nullptr){
//                    continue;
//                }

//                // Only push children whos bounds intersect the test geometry
//                RaycastResult<acc> raycast;
//                raycast_aabb(iterator->children[ii]->bounds, ray, &raycast);
//                if (raycast.t >= 0) {
//                    toProcess.push_front(iterator->children[ii].get());
//                }
//            }
//        }
//    }
//    return -1;
//}
}
