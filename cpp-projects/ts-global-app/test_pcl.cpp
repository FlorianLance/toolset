
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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

// #include "tests.hpp"


// // std
// #include <iostream>

// // pcl
// #include <pcl/point_cloud.h>
// #include <pcl/point_types.h>
// #include <pcl/compression/octree_pointcloud_compression.h>

// // base
// #include "depth-camera/frame/dc_frame.hpp"
// #include "depth-camera/settings/dc_frame_generation_settings.hpp"
// #include "depth-camera/dc_video.hpp"

// using namespace tool;
// using namespace tool::cam;
// using namespace tool::geo;

// auto pcl_encoding() -> void{

//     std::cout << "pcl1\n";
//     pcl::io::compression_Profiles_e compressionProfile = pcl::io::HIGH_RES_ONLINE_COMPRESSION_WITH_COLOR;

//     pcl::io::OctreePointCloudCompression<pcl::PointXYZRGBA>* PointCloudEncoder;
//     pcl::io::OctreePointCloudCompression<pcl::PointXYZRGBA>* PointCloudDecoder;

//     std::cout << "pcl2\n";
//     // // instantiate point cloud compression for encoding and decoding
//     bool showStatistics = true;
//     PointCloudEncoder = new pcl::io::OctreePointCloudCompression<pcl::PointXYZRGBA> (compressionProfile, showStatistics);
//     PointCloudDecoder = new pcl::io::OctreePointCloudCompression<pcl::PointXYZRGBA> ();

//     DCVideo video;
//     std::string path = "E:/_kvid/30-07-24-fam.kvid";
//     video.load_from_file(path);
//     DCFrameGenerationSettings settings;
//     settings.cloud = true;
//     ColorCloud fullCloud;

//     DCFrame d0Frame;
//     video.generate_frame(settings, 0, 50, d0Frame);

//     auto cp = d0Frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
//     fullCloud.merge(*cp);

//     auto startTS = video.device_first_frame_timestamp(0);
//     auto firstCameraCurrentFrameTS = video.get_data_frame(0, 50).lock().get()->afterCaptureTS;
//     auto diffNs = std::chrono::nanoseconds(firstCameraCurrentFrameTS - startTS);
//     auto currentTimeMS = duration_cast<std::chrono::milliseconds>(diffNs);

//     for(size_t idC = 1; idC < video.nb_devices(); ++idC){
//         auto cfid = video.closest_frame_id_from_time(idC, 1.0*currentTimeMS.count());

//         DCFrame dIFrame;
//         video.generate_frame(settings,idC, cfid, dIFrame);
//         cp = dIFrame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);

//         cp->vertices.apply_transformation(video.get_transform(idC).conv<float>());

//         fullCloud.merge(*cp);
//     }

//     std::uint8_t px = 0;
//     std::uint8_t py = 0;
//     std::uint8_t pz = 0;

//     size_t nX =0, nY= 0, nZ = 0, nW = 0;;
//     for(const auto &col : fullCloud.colors){
//         auto cx = static_cast<std::uint8_t>(col.x()*255.f);
//         auto cy = static_cast<std::uint8_t>(col.y()*255.f);
//         auto cz = static_cast<std::uint8_t>(col.z()*255.f);

//         if(px == cx){
//             ++nX;
//         }
//         if(py == cy){
//             ++nY;
//         }
//         if(pz == cz){
//             ++nZ;
//         }
//         if(cx == cy && cy == cz){
//             ++nW;
//         }
//         // if(cy == cz){
//         //     ++nW;
//         // }
//     }

//     std::cout << "NCount " << nX << " " << nY << " " << nZ << " " << fullCloud.colors.size() << " " << nW << "\n";



//     pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGBA> ());
//     cloud->points.resize(fullCloud.size());
//     for(size_t id = 0; id < fullCloud.size(); ++id){
//         pcl::PointXYZRGBA p;
//         p.x = fullCloud.vertices[id].x();
//         p.y = fullCloud.vertices[id].y();
//         p.z = fullCloud.vertices[id].z();
//         p.r = static_cast<std::uint8_t>(fullCloud.colors[id].x()*255.f);
//         p.g = static_cast<std::uint8_t>(fullCloud.colors[id].y()*255.f);
//         p.b = static_cast<std::uint8_t>(fullCloud.colors[id].z()*255.f);
//         p.a = 255;
//         cloud->points[id] = p;
//     }


//     pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudOut (new pcl::PointCloud<pcl::PointXYZRGBA> ());

//     // compress point cloud

//     std::stringstream compressedData;

//     PointCloudEncoder->encodePointCloud (cloud, compressedData);


//     // decompress point cloud

//     PointCloudDecoder->decodePointCloud (compressedData, cloudOut);


//     std::cout << "pcl3\n";
//     delete (PointCloudEncoder);
//     delete (PointCloudDecoder);
//     // pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudOut (new pcl::PointCloud<pcl::PointXYZRGBA> ());
// }
