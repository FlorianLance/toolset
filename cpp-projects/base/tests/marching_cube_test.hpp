
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


//#pragma once


//// std
//#include <iostream>

//// local
//#include "geometry/point3.hpp"
//#include "geometry/geometry2.hpp"
//#include "geometry/matrix4.hpp"
//#include "geometry/plane3.hpp"
//#include "geometry/octree.hpp"
//#include "geometry/mesh.hpp"
//#include "algorithms/maching_cube.hpp"
//#include "utility/vector_utility.hpp"

//namespace tool {

//void save_mesh(const std::string path, std::vector<tool::algo::TRIANGLE<float>> &triangles){

//    std::ofstream flow2(path);
//    if(!flow2.is_open()){
//        return;
//    }

//    std::vector<geo::Pt3<int>> triId;
//    int totalId = 0;
//    std::ostringstream osV;
//    for(const auto &tri : triangles){
//        osV.str(std::string());
//        osV << "v " << tri.p[0].x() << " " <<tri.p[0].y() << " " << tri.p[0].z()<< " 0 1 0 0\n";
//        osV << "v " << tri.p[1].x() << " " <<tri.p[1].y() << " " << tri.p[1].z()<< " 0 1 0 0\n";
//        osV << "v " << tri.p[2].x() << " " <<tri.p[2].y() << " " << tri.p[2].z()<< " 0 1 0 0\n";
//        flow2 << osV.str();

//        triId.emplace_back(totalId+1,totalId+2,totalId+3);
//        totalId += 3;
//    }

//    for(const auto &tri : triId){
//        osV.str(std::string());
//        osV << "f " << tri.x() << " " <<tri.y() << " " << tri.z()<< "\n";
//        flow2 << osV.str();
//    }
//}

//bool test_marching_cube(){
//    std::cout << "# Marching cube #\n";

//    // laod sample mesh
//    geo::Mesh m;
//    if(geo::load_from_obj<float>("E:/example.obj", m)){
//        std::cout << "Mesh loaded, nb vertices: " << m.vertices.size() << "\n";
//    }

//    std::vector<geo::Voxel<float>> voxels;

//    // set AABB to a cube
//    auto aabb = m.AABB();
//    auto max = std::max({aabb.size.x(),aabb.size.y(),aabb.size.z()});
//    aabb.size = geo::Vec3<float>(max,max,max);

//    // set voxels array from mesh
//    voxels.reserve(m.vertices.size());
//    for(size_t ii = 0; ii < m.vertices.size(); ++ii){
//        voxels.emplace_back(geo::Voxel<float>{m.vertices[ii], geo::Color4{1.f,0.f,0.1f,1.f}});
//    }

//    // split tree with voxel octree
////    const size_t depth = 2;
////    const size_t side = ipow(2, depth);
////    VoxelOctreeNode<float> node(m.AABB());
////    split_tree(voxels, &node, depth);

////    std::vector<int> depths = {1,2,3,4,5,6,7,8};//,4,5,6,7};
////    std::vector<float> isos = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.};

//    std::vector<int> depths = {3,4,5,6,7};
//    std::vector<float> isos = { 0.3f, 0.4f, 0.5f, 0.6f, 0.7f,};
////    std::vector<float> isos = {0.01f, 0.05f, 0.1f, 1.f, 2.f, 3.f, 5.f, 10.f, 50.f, 100.f, 1000.f, 10000.f};

//    for(auto d : depths){

//        const size_t side = static_cast<size_t>(math::ipow(2, d));

//        geo::VoxelOctreeNode<float> node(aabb);
//        split_tree(voxels, &node, d);

//        // ###################################### DEBUG
//        // save octree cloud
////        std::ofstream flow("E:/octree_" + std::to_string(d) + ".obj");
////        if(!flow.is_open()){
////            return {};
////        }

//        // sort nodes
//        auto sortedNodes = node.sorted_nodes<float>(d);
//        std::cout << "sizes " << sortedNodes.size() << " " << sortedNodes[0].size() << " " << sortedNodes[0][0].size() << "\n";



//        std::ofstream fileStream("E:/nodes_center_" + std::to_string(d) + ".obj");
//        std::ostringstream osV;
//        if(!fileStream.is_open()){
//             return false;
//        }

//        size_t nb = 0;
//        std::cout << "save nodes centers : \n";
//        size_t maxI = sortedNodes.size(),maxJ= sortedNodes[0].size(),maxK = sortedNodes[0][0].size();
////        maxI = 100;
////        maxJ = 100;
////        maxK = 100;
//        std::cout << "sizes " << maxI << " " << maxJ << " " << maxK << "\n";

//        for(size_t ii = 0; ii < maxI; ++ii){
//            for(size_t jj = 0; jj < maxJ; ++jj){
//                for (size_t kk = 0; kk < maxK; ++kk) {
//                    auto n = sortedNodes[ii][jj][kk];
//                    if(n == nullptr){
//                        continue;
//                    }
//                    auto b = n->bounds;
//                    auto o = b.origin;

//                    if(rand()%100 != 0){
//                        continue;
//                    }


//                    ++nb;

//                    std::vector<geo::Pt3<float>> pts;
//                    for(const auto &mul : geo::mu<float>){
//                        pts.emplace_back(o+mul*b.size);
//                    }


//                    osV.str(std::string());
//                    osV << "v " << o.x() << " " << o.y() << " " << o.z() << " 0 1 0 0\n";
//                    for(const auto &pt : pts){
////                        osV << "v " << pt.x() << " " << pt.y() << " " << pt.z() << " 0 0 1 0\n";
//                    }

//                    // test
//                    std::vector<geo::Pt3<float>> ptsN;
//                    if(ii > 0 && jj > 0 && kk > 0 && ii < maxI-1 && jj < maxJ-1 && kk < maxK-1){

////                        std::cout << "-> " << ii << " " << jj << " " << kk << "\n";

//                        for(int ni = -1; ni < 2; ++ni){
//                            for(int nj = -1; nj < 2; ++nj){
//                                for(int nk = -1; nk < 2; ++nk){

//                                    if(ni == 0 && nj == 0 && nk == 0){
//                                        continue;
//                                    }
////                                    std::cout << ni << " " << nj << " " << nk << "\n";
//                                    if(sortedNodes[ii+ni][jj+nj][kk+nk] != nullptr){
////                                        std::cout << ni << " " << nj << " " << nk << "\n";

////                                        if(ni == 1 && nj ==1 && nk == 1){
//                                            ptsN.emplace_back(sortedNodes[ii+ni][jj+nj][kk+nk]->bounds.origin);
////                                            osV << "v " << pt.x() << " " << pt.y() << " " << pt.z() << " 0 0 1 0\n";
////                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }



//                    for(const auto &pt : ptsN){
//                        osV << "v " << pt.x() << " " << pt.y() << " " << pt.z() << " 0 0 1 0\n";
//                    }


//                    fileStream << osV.str();
//                }
//            }
//        }
//        std::cout << "nodes count " << nb << "\n";



////        auto sortedNodes = node.sorted_nodes<float>(d);

////        std::cout << "---\n";

//        for(auto i : isos){
//            std::cout <<" depth " << d << " isos " << i << "\n";
//            auto triangles =  algo::marching_cube(&node, d, i);
//            std::cout << "size " << triangles.size() << "\n";
//            if(triangles.size() > 0){
//                save_mesh("E:/marching_" + std::to_string(d) + "_" + std::to_string(i) + ".obj", triangles);
//            }
//        }
//    }


//    // save octree cloud

////    std::ofstream flow("E:/octree_" + std::to_string(depth) + "_" + std::to_string(iso) + ".obj");



//    //        // init gridcell

//    //        if(aa.is_open()){
//    //            std::ostringstream osV;
//    //            auto n = sortedNodes[20][20][20];
//    //            auto b = n->bounds;
//    //            auto o = b.origin;
//    //            std::cout << "o " << o << "\n";




//    //                Point3<int> curr(20,20,20);
//    //                for(int ll = 0; ll < 1; ++ll){

//    //                    for(const auto &id : mu4[ll]){

//    //                        Point3<int> mm = curr + id;

//    //                        if(mm.x() < 0 || mm.y() < 0 || mm.z() < 0 || mm.x() >= sortedNodes.size() || mm.y() >= sortedNodes[0].size() || mm.z() >= sortedNodes[0][0].size()){
//    //                            continue;
//    //                        }

//    //                        std::cout << "i " << mm << "\n";
//    //                        if(sortedNodes[mm.x()][mm.y()][mm.z()]){
//    //                            k = sortedNodes[mm.x()][mm.y()][mm.z()]->bounds.origin;
//    //                            osV << "v " << k.x() << " " << k.y() << " " << k.z() << " 0 0 1 0\n";
//    //                        }
//    //                        std::cout << "j\n";

//    ////                        if(sortedNodes[idCurrN.x()][idCurrN.y()][idCurrN.z()]){
//    ////                            total += 1;// sortedNodes[idCurrN.x()][idCurrN.y()][idCurrN.z()]->ids.size();
//    ////                        }
//    //                    }
//    //                }


//    ////                k = idCurrN(ii+id.x(), jj+id.y(), kk+id.z());
//    //////                float total = 0.f;
//    ////                for(const auto &id : mu4[ll]){
//    ////                    const Pt3<int> idCurrN(ii+id.x(), jj+id.y(), kk+id.z());

//    //////                            std::cout << "id " << id << " " << idCurrN << "\n";
//    ////                    if(idCurrN.x() < 0 || idCurrN.y() < 0 || idCurrN.z() < 0 || idCurrN.x() >= side || idCurrN.y() >= side || idCurrN.z() >= side ){
//    ////                        continue;
//    ////                    }

//    ////                    if(sortedNodes[idCurrN.x()][idCurrN.y()][idCurrN.z()]){
//    ////                        total += 1;// sortedNodes[idCurrN.x()][idCurrN.y()][idCurrN.z()]->ids.size();
//    ////                    }
//    ////                }

//    ////                osV << "v " << b.origin + mu<float>[ll]*b.size;
//    //                aa << osV.str();
//    ////            }
//    //        }




//    //        return 0;

//    //        // add vertices
//    //        std::ostringstream osV;
//    //        for(size_t ii = 0; ii < side; ++ii){
//    //            for(size_t jj = 0; jj < side; ++jj){
//    //                for(size_t kk = 0; kk < side; ++kk){
//    //                    if(sortedNodes[ii][jj][kk]){
//    //                        osV.str(std::string());
//    //                        osV << "v " << sortedNodes[ii][jj][kk]->bounds.origin.x() << " " << sortedNodes[ii][jj][kk]->bounds.origin.y() << " " << sortedNodes[ii][jj][kk]->bounds.origin.z() << " 1 0 0 0\n";
//    //                        flow << osV.str();
//    //                    }
//    //                }
//    //            }
//    //        }
//    //        // ###################################### END DEBUG

////    std::cout << "total tri "  << triangles.size() << "\n";

////                 end\n";

//    return true;
//}
//}
