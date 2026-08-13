
    







// }

// // #include "depth-camera/frame/dc_frame.hpp"


// template <typename acc, int _rows, int _cols>
// constexpr auto ccc(const geo::Matrix<acc,_rows,_cols> &lhs, const geo::Matrix<acc, _rows, _cols> &rhs) noexcept -> bool{
//     if(lhs != rhs){
//         return std::lexicographical_compare(lhs.array.begin(), lhs.array.end(), rhs.array.begin(), rhs.array.end());
//     }
//     return false;
// }

// // turbopfor
// // #include "TurboPFor/vp4.h"

// bool comparator(const tool::geo::CVoxel& lhs, const tool::geo::CVoxel& rhs) {
//     auto *c1 = reinterpret_cast<const std::uint64_t*>(&lhs);
//     auto *c2 = reinterpret_cast<const std::uint64_t*>(&rhs);
//     return *c1 < *c2;
// }/*

// TEST_CASE("geo::Cloud"){ Log::message("geo::Cloud\n");
//     SECTION("ColoredCloudData"){ Log::message("geo::ColoredCloudData\n");
//         geo::ColorCloud cloud;
// //        cloud.vertices.reserve(100);
// //        for(size_t ii = 0; ii < 100; ++ii){
// //            cloud.vertices.emplace_back(100.f-ii,2.f*ii,3.f*ii);
// //        }

//         cloud.vertices.push_back({2.f,1.f,3.f});
//         cloud.vertices.push_back({-5.f, 3.f, -2.f});
//         cloud.vertices.push_back({9.f, 7.f, 0.f});
//         cloud.vertices.push_back({0.f, 0.f, 0.f});

//         auto pt1 = cloud.vertices[0];
//         auto pt2 = cloud.vertices[1];
//         auto pt3 = cloud.vertices[2];

//         cloud.vertices.resize(5);
//         Log::message(std::format("<: {}\n", pt1 < pt1));
//         Log::message(std::format(">: {}\n", pt1 > pt2));
//         Log::message(std::format("<: {}\n", pt1 < pt3));
//         Log::message(std::format(">: {}\n", pt1 > pt3));

//         geo::Pt3f pt{1.f,2.f,3.f};
//         Log::message(std::format("sum: {}\n", geo::sum(pt)));
        
//         Log::message(std::format("sum: {}\n", geo::sum(pt1)));
//         Log::message(std::format("sum: {}\n", geo::sum(pt2)));
        
//         Log::message(std::format("sum: {}\n", to_string(cloud.vertices.sum())));
//         Log::message(std::format("mean: {}\n", to_string(cloud.vertices.mean())));
//         Log::message(std::format("minx: {}\n", std::to_string(cloud.vertices.min_x())));
//         Log::message(std::format("maxx: {}\n", std::to_string(cloud.vertices.max_x())));
//         Log::message(std::format("miny: {}\n", std::to_string(cloud.vertices.min_y())));
//         Log::message(std::format("maxy: {}\n", std::to_string(cloud.vertices.max_y())));
//         Log::message(std::format("minz: {}\n", std::to_string(cloud.vertices.min_z())));
//         Log::message(std::format("maxz: {}\n", std::to_string(cloud.vertices.max_z())));
//         Log::message(std::format("min: {}\n", to_string(cloud.vertices.min())));
//         Log::message(std::format("max: {}\n", to_string(cloud.vertices.max())));
        
//         Log::message(std::format("minx_id: {}\n", cloud.vertices.min_x_id()));
//         Log::message(std::format("maxx_id: {}\n", cloud.vertices.max_x_id()));
//         Log::message(std::format("miny_id: {}\n", cloud.vertices.min_y_id()));
//         Log::message(std::format("maxy_id: {}\n", cloud.vertices.max_y_id()));
//         Log::message(std::format("minz_id: {}\n", cloud.vertices.min_z_id()));
//         Log::message(std::format("maxz_id: {}\n", cloud.vertices.max_z_id()));
//         Log::message(std::format("min_id: {}\n", cloud.vertices.min_id()));
//         Log::message(std::format("max_id: {}\n", cloud.vertices.max_id()));
        
        
// //         cam::DCDeprecatedFrame f;
// //         for(size_t ii = 0; ii < 10000; ++ii){
// // //            f.cloud.vertices.push_back({(rand()%1000)*0.001f,(rand()%2000)*0.001f,(rand()%3000)*0.001f});
// // //            f.cloud.colors.push_back({(rand()%1000)*0.001f,(rand()%1000)*0.001f,(rand()%1000)*0.001f});

// //             f.cloud.vertices.push_back({(rand()%1000)*0.001f,(rand()%2000)*0.001f,(rand()%3000)*0.001f});
// //             f.cloud.colors.push_back({(rand()%1000)*0.001f,(rand()%1000)*0.001f,(rand()%1000)*0.001f});

// //         }
        
//         Log::message("TEST VOXELISATION\n");
//         auto minBound = tool::geo::Pt3f{-1.f,-1.f,-1.f};
//         auto maxBound = tool::geo::Pt3f{1.f,1.f,1.f};
//         tool::geo::ColorVoxelGrid voxelGrid = tool::geo::ColorVoxelGrid::create_from_point_cloud_within_bounds(
//             f.cloud,
//             0.1f,
//             minBound,
//             maxBound
//         );
        
//         Log::message(std::format("frame cloud: {}\n", cloud.vertices.size()));
//         Log::message(std::format("minx_id: {}\n", f.cloud.vertices.min_x()));
//         Log::message(std::format("maxx_id: {}\n", f.cloud.vertices.max_x()));
//         Log::message(std::format("miny_id: {}\n", f.cloud.vertices.min_y()));
//         Log::message(std::format("maxy_id: {}\n", f.cloud.vertices.max_y()));
//         Log::message(std::format("minz_id: {}\n", f.cloud.vertices.min_z()));
//         Log::message(std::format("maxz_id: {}\n", f.cloud.vertices.max_z()));
        
//         Log::message(std::format("voxel size: {}\n", sizeof(geo::Voxel)));
//         Log::message(std::format("CVoxel size: {}\n", sizeof(geo::CVoxel)));
//         Log::message(std::format("std::int64_t size: {}\n", sizeof(std::int64_t)));
//         Log::message(std::format("std::int32_t size: {}\n", sizeof(std::int32_t)));
//         Log::message(std::format("geo::Pt3<int> size: {}\n", sizeof(geo::Pt3<int>)));
//         Log::message(std::format("geo::Pt3f size: {}\n", sizeof(geo::Pt3f)));

//         std::vector<tool::geo::CVoxel> cvoxels;
//         cvoxels.reserve(voxelGrid.grid.size());
//         for(const auto &grid : voxelGrid.grid){

//             const auto &id = grid.second.index;
//             const auto &col = grid.second.color;
//             tool::geo::CVoxel cv;
//             cv.red   = static_cast<int>(col.x()*255.f);
//             cv.green = static_cast<int>(col.y()*255.f);
//             cv.blue  = static_cast<int>(col.z()*255.f);
//             cv.xIndex= id.x();
//             cv.yIndex= id.y();
//             cv.zIndex= id.z();
//             cvoxels.push_back(cv);
//         }

//         // pass it to sort:
//         sort(cvoxels.begin(), cvoxels.end(), &comparator);

//         cvoxels.resize(cvoxels.size() + 128- cvoxels.size()%128);

//         std::vector<std::uint8_t> encoded;
//         encoded.resize(cvoxels.size()*16);
//         Log::message(std::format("cvoxels size: {} : bytes {}\n", cvoxels.size(), cvoxels.size()*sizeof(tool::geo::CVoxel)));
//         Log::message(std::format("encoded size: {}\n", encoded.size()));


//         // size_t encodedBytesNb = p4nzenc64(
//         //     reinterpret_cast<std::uint64_t*>(cvoxels.data()),
//         //     cvoxels.size(),
//         //     encoded.data()
//         // );
//         // Log::message(std::format("p4nzenc64: {}\n", encodedBytesNb));

//         //  encodedBytesNb = p4nd1enc64(
//         //      reinterpret_cast<std::uint64_t*>(cvoxels.data()),
//         //      cvoxels.size(),
//         //      encoded.data()
//         //  );
//         //  Log::message(std::format("p4nd1enc64: {}\n", encodedBytesNb));

//         // encodedBytesNb = p4ndenc64(
//         //   reinterpret_cast<std::uint64_t*>(cvoxels.data()),
//         //   cvoxels.size(),
//         //   encoded.data()
//         // );
//         // Log::message(std::format("p4ndenc64: {}\n", encodedBytesNb));

//         //  encodedBytesNb = p4nenc64(
//         //    reinterpret_cast<std::uint64_t*>(cvoxels.data()),
//         //    cvoxels.size(),
//         //    encoded.data()
//         //  );
//         //  Log::message(std::format("p4nenc64: {}\n", encodedBytesNb));

//         //   encodedBytesNb = p4nenc128v64(
//         //     reinterpret_cast<std::uint64_t*>(cvoxels.data()),
//         //     cvoxels.size(),
//         //     encoded.data()
//         //   );
//         //   Log::message(std::format("p4nenc128v64: {}\n", encodedBytesNb));


// //                                  p4nenc128v64
// //                                 // compress depth buffer
// //                                 cFrame->encodedDepthData.resize(depthSize*2);

// //                                 // depth sizes for every mode have already a 128 padded size
// //                                 size_t encodedBytesNb = p4nzenc128v16(
// //                                     depthBuffer,
// //                                     depthSize,
// //                                     cFrame->encodedDepthData.data()
// //                                 );
// //                                 cFrame->encodedDepthData.resize(encodedBytesNb);

//         //  // init cloud from voxel grid
//         //  cloud.vertices.resize(grid.grid.size());
//         //  cloud.colors.resize(grid.grid.size());

//         //  size_t idVoxel = 0;
//         //  for(const auto &voxel : grid.grid){

//         //      const auto &id = voxel.second.index;
//         //      cloud.vertices[idVoxel] = (minBound +
//         //          geo::Pt3f{static_cast<float>(id.x()), static_cast<float>(id.y()), static_cast<float>(id.z())})
//         //          *static_cast<float>(grid.voxelSize);
//         //      cloud.colors[idVoxel++] = voxel.second.color;
//         //  }


//     }
// }*/
