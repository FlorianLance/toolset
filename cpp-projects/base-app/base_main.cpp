/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

// #define TF_ENABLE_PROFILER = simple.json ./simple

// base
#include "utility/logger.hpp"

// local
#include "tests.hpp"

#include "depth-camera/client/dc_client.hpp"
#include "depth-camera/dc_device.hpp"
#include "depth-camera/network/dc_server.hpp"
#include <iostream>
#include "utility/time.hpp"
#include "utility/geometry_formatter.hpp"

#include "thirdparty/taskflow/taskflow.hpp"
#include "thirdparty/taskflow/algorithm/for_each.hpp"

#include "utility/constants.hpp"

#include <Eigen/Dense>



using namespace tool;
using namespace std::chrono;

int main(int argc, char *argv[]){

    std::puts(std::format("Argc: [{}]\n", argc).c_str());
    for(int ii = 0; ii < argc; ++ii){
        std::puts(std::format("\t->: [{}]\n", argv[ii]).c_str());
    }

    std::puts("Start tests.\n");
    
    auto logger = std::make_unique<Logger>();
    logger->init("./", "base_app.html");
    auto &cfg = logger->config(Logger::MessageType::normal);
    cfg.saveToFile = true;

    logger->message_signal.connect([&](std::string message){
        // std::puts(message.c_str());
        std::flush(std::cout);
    });
    logger->warning_signal.connect([&](std::string warning){
        // std::puts(warning.c_str());
        std::flush(std::cout);
    });
    logger->error_signal.connect([&](std::string error){
        // std::puts(error.c_str());
        std::flush(std::cout);
    });
    Logger::set_logger_instance(std::move(logger));

    // multi_devices_benchmark();

    test_bdf_reader("C:/Users/SDKim/Downloads/bdf_test.bdf");
    // test_bdf_reader("D:/DEV/Clients/Takasago/TakasagoVision/cpp-projects/_build/bin/takasago-eeg/recordings/direct_25-06-25-15-37-31.bdf");


    // cam::DCDevice dev;
    // // auto tf = dev.tf();

    // tf::Executor executor2(8);
    
    
    // executor2.run(*dev.read_frames_task()).wait();

    // cam::DCDeviceSettings ds;
    // ds.configS.typeDevice = cam::DCType::AzureKinect;
    // ds.configS.openDevice = true;
    // ds.configS.startReading = true;
    // dev.update_device_settings(ds);
    
    // executor2.run(*dev.read_frames_task()).wait();
    
    // dev.read_frames_task()->dump(std::cout);
    return 0;
    // cam::DCClient client;
    // client.initialize("D:/DEV/Perso/toolset/cpp-projects/_build/bin/dc-manager/config/client_FLORIAN-PC_0.json", false);
    // // client.initialize(,false);

    // size_t countFrames = 0;
    // client.new_data_frame_signal.connect([&](size_t idG, std::shared_ptr<cam::DCDataFrame> dframe){
    //     Log::fmessage("grabber {} dframe {}\n", idG, dframe->volume_buffer(cam::DCVolumeBufferType::ColoredCloud)->size());
    //     ++countFrames;
    // });

    // client.new_frame_signal.connect([&](size_t idG, std::shared_ptr<cam::DCFrame> frame){
    //     Log::fmessage("grabber {} frame {}\n", idG, frame->volume_buffer<geo::ColorCloud>(cam::DCVolumeBufferType::ColoredCloud)->vertices.size());
    //     ++countFrames;
    // });

    // for(size_t id = 0; id < client.devices_nb(); ++id){
    //     auto t = Time::nanoseconds_since_epoch();
    //     client.apply_device_settings(id);
    //     Log::fmessage("device id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
    // }
    // for(size_t id = 0; id < client.devices_nb(); ++id){
    //     auto t = Time::nanoseconds_since_epoch();
    //     client.apply_filters_settings(id);
    //     Log::fmessage("filters id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
    // }
    // for(size_t id = 0; id < client.devices_nb(); ++id){
    //     auto t = Time::nanoseconds_since_epoch();
    //     client.apply_color_settings(id);
    //     Log::fmessage("color id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
    // }

    // for(size_t id = 0; id < client.devices_nb(); ++id){
    //     client.read_from_external_thread(id);
    // }

    // std::this_thread::sleep_for(1000ms);




    // Log::fmessage("START {} devices \n", client.devices_nb());
    // std::vector<std::unique_ptr<std::thread>> threads;
    // std::vector<size_t> count;
    // for(size_t id = 0; id < client.devices_nb(); ++id){ //
    //     count.push_back(0);

    //     Log::fmessage("Start thread {}\n", id);
    //     threads.push_back(std::make_unique<std::thread>([&,id](){
    //         Log::fmessage("THREAD {}\n", id);
    //         for(int ii = 0; ii < 10000; ++ii){
    //             auto t1 = Time::nanoseconds_since_epoch();
    //             client.read_from_external_thread(id);
    //             auto frames = client.process_frames_from_external_thread(id);

    //             // if(std::get<0>(frames) != nullptr){
    //             //     Log::fmessage("grabber {} new frame {}\n", id, std::get<0>(frames)->idCapture);
    //             // }
    //             // Log::fmessage("{} frame {} new frame {}\n", ii, id, std::get<0>(frames) != nullptr);
    //             std::this_thread::sleep_for(10ms);
    //         }
    //     }));
    // }

    // for(auto &t : threads){
    //     if(t->joinable()){
    //         t->join();
    //     }
    // }


    // for(int ii = 0; ii < 500; ++ii){
    //     client.update();
    //     std::this_thread::sleep_for(5ms);
    // }
    // Log::fmessage("nb framees {}\n", countFrames);

    // Log::fmessage("clean");
    // client.clean();
    // threads.clear();

    // Log::fmessage("end");

    // return 0;


    int N = 8;
    tf::Executor executor(N);

    auto mainTF = std::make_unique<tf::Taskflow>();

    std::vector<int> a;

    std::vector<int>::iterator aB;
    std::vector<int>::iterator aE;

    tf::DynamicPartitioner  static_partitioner(100);
    std::atomic<int> ai = 0;
    auto foreachT = mainTF->for_each(std::ref(aB), std::ref(aE), [&](int v){
        std::cout << v << " ";
        ++ai;
    }, static_partitioner).name("foreach");


    a.resize(1000);
    std::iota(a.begin(), a.end(), 0);

    aB = a.begin();
    aE = a.end();

    auto startT = mainTF->emplace([&](){

    }).name("Start");;
    auto task2 = mainTF->emplace([&](tf::Subflow& subflow){
        tf::Task B1 = subflow.emplace([] () {}).name("sub_task2_1");  // subflow task B1
        tf::Task B2 = subflow.emplace([] () {}).name("sub_task2_2");  // subflow task B2
    }).name("Task2");;

    int i;
    auto [init, cond, body, back, done] = mainTF->emplace(
        [&](){
            std::cout << "i=0\n"; i=0;
        },
        [&](){
            std::cout << "while i<5\n"; return i < 5 ? 0 : 1;
        },
        [&](tf::Subflow& subflow){
            std::cout << "i++=" << i++ << '\n';
            tf::Task B1 = subflow.emplace([] () {}).name("sub_body_1");  // subflow task B1
            tf::Task B2 = subflow.emplace([] () {}).name("sub_body_2");  // subflow task B1
        },
        [&](){
            std::cout << "back\n";
            return 0; },
        [&](){
            std::cout << "done\n";
        }
    );


    init.name("init");
    cond.name("cond");
    body.name("body");
    back.name("back");
    done.name("done");

    startT.precede(init);
    foreachT.succeed(startT);

    init.precede(cond);
    cond.precede(body, done);
    body.precede(back);
    back.precede(cond);

    executor.run(*mainTF).get();
    mainTF->dump(std::cout);
    // auto readingDataModuleT = processFrameTF->composed_of(readDataTF).name("reading_data_module");

    // mainTF->emplace(mainTF);


    std::cout << "END: " << ai << "\n";



    return 0;
    // geo::Pt3f v;
    // std::cout << "pt3 " << v.rows() << " " << v.cols() << "\n";
    // auto col = geo::to_col(v);
    // std::cout << "col " << col.rows() << " " << col.cols() << "\n";
    // auto row = geo::to_row(col);
    // std::cout << "row " << row.rows() << " " << row.cols() << "\n";

    // auto col2 = geo::to_col(geo::Pt3f());
    // std::cout << "col2 " << col2.rows() << " " << col2.cols() << "\n";

    // return 0;
    // // task flow

    geo::Vertices3D vertices;
    vertices.resize(100);
    for(auto &v : vertices){
        v = {(rand()%1000) -500.f, (rand()%1000) -500.f, (rand()%1000) -500.f};
    }

    vertices.sort_by_z_ascendant();

    size_t id = 0;
    for(auto &v : vertices){
        Log::fmessage("{} : {} {} {}\n", id, v.x(), v.y(), v.z());
    }
    Log::fmessage("front {}\n", vertices.front());


    // Buffer<geo::Pt3f> bb({{1.f,2.f,3.f},{2.f,3.f,4.f}});
    // Buffer<geo::Pt3f> bb({1.f,2.f,3.f,2.f,3.f,4.f});

    // geo::Pt3f pt(1.f,2.f,3.f,2.f,3.f,4.f);

    geo::Vertices3D vertices2({{1.f,2.f,3.f},{4.f,5.f,6.f}, {7.f,8.f,9.f}});
    Log::fmessage("m11 {}\n", vertices2[0]);
    Log::fmessage("m12 {}\n", vertices2[1]);

    Log::fmessage("mean {}\n", vertices2.mean());

    Log::fmessage("mean00 {}\n", vertices2.mean(0,0));
    Log::fmessage("mean01 {}\n", vertices2.mean(0,1));
    Log::fmessage("mean02 {}\n", vertices2.mean(0,2));
    Log::fmessage("mean03 {}\n", vertices2.mean(0,3));
    Log::fmessage("mean03 {}\n", vertices2.mean(0,4));

    Log::fmessage("mean10 {}\n", vertices2.mean(1,0));
    Log::fmessage("mean11 {}\n", vertices2.mean(1,1));
    Log::fmessage("mean12 {}\n", vertices2.mean(1,2));
    Log::fmessage("mean13 {}\n", vertices2.mean(1,3));
    Log::fmessage("mean13 {}\n", vertices2.mean(1,4));

    Log::fmessage("mean20 {}\n", vertices2.mean(2,0));
    Log::fmessage("mean21 {}\n", vertices2.mean(2,1));
    Log::fmessage("mean22 {}\n", vertices2.mean(2,2));
    Log::fmessage("mean23 {}\n", vertices2.mean(2,3));
    Log::fmessage("mean23 {}\n", vertices2.mean(2,4));


    Log::fmessage("mean30 {}\n", vertices2.mean(3,0));
    Log::fmessage("mean31 {}\n", vertices2.mean(3,1));
    Log::fmessage("mean32 {}\n", vertices2.mean(3,2));
    Log::fmessage("mean33 {}\n", vertices2.mean(3,3));
    Log::fmessage("mean33 {}\n", vertices2.mean(3,4));

    Log::fmessage("mean40 {}\n", vertices2.mean(4,0));
    Log::fmessage("mean41 {}\n", vertices2.mean(4,1));
    Log::fmessage("mean42 {}\n", vertices2.mean(4,2));
    Log::fmessage("mean43 {}\n", vertices2.mean(4,3));
    Log::fmessage("mean43 {}\n", vertices2.mean(4,4));




    // // function [Center,Radius] = sphereFit(X)
    // // % this fits a sphere to a collection of data using a closed form for the
    // // % solution (opposed to using an array the size of the data set).
    // // % Minimizes Sum((x-xc)^2+(y-yc)^2+(z-zc)^2-r^2)^2
    // // % x,y,z are the data, xc,yc,zc are the sphere's center, and r is the radius
    // // % Assumes that points are not in a singular configuration, real numbers, ...
    // // % if you have coplanar data, use a circle fit with svd for determining the
    // // % plane, recommended Circle Fit (Pratt method), by Nikolai Chernov
    // // % http://www.mathworks.com/matlabcentral/fileexchange/22643
    // // % Input:
    // // % X: n x 3 matrix of cartesian data
    // // % Outputs:
    // // % Center: Center of sphere
    // // % Radius: Radius of sphere
    // // % Author:
    // // % Alan Jennings, University of Dayton
    //  A = [
    //         mean(X(:,1).*(X(:,1)-mean(X(:,1)))), 2*mean(X(:,1).*(X(:,2)-mean(X(:,2)))), 2*mean(X(:,1).*(X(:,3)-mean(X(:,3)))) ;
    //         0, mean(X(:,2).*(X(:,2)-mean(X(:,2)))), 2*mean(X(:,2).*(X(:,3)-mean(X(:,3)))) ;
    //         0, 0, mean(X(:,3).*(X(:,3)-mean(X(:,3))))];
    // A=A+A.';
    // // B=[mean((X(:,1).^2+X(:,2).^2+X(:,3).^2).*(X(:,1)-mean(X(:,1))));...
    // //     mean((X(:,1).^2+X(:,2).^2+X(:,3).^2).*(X(:,2)-mean(X(:,2))));...
    // //     mean((X(:,1).^2+X(:,2).^2+X(:,3).^2).*(X(:,3)-mean(X(:,3))))];
    // // Center=(A\B).';
    // // Radius=sqrt(mean(sum([X(:,1)-Center(1),X(:,2)-Center(2),X(:,3)-Center(3)].^2,2)));

    // The following pseudo code compares a point (x, y, z) with the current
    // sphere [center = (cenx, ceny, cenz), and radius = rad]. If (x, y, z) is
    // outside the current sphere, (cenx, ceny, cenz) and rad are updated to
    // reflect the new sphere. The current square of the radius is maintained in
    // rad sq:

    // function getPoint() {
    //     var d, x, y, z;
    //     do {
    //         x = Math.random() * 2.0 - 1.0;
    //         y = Math.random() * 2.0 - 1.0;
    //         z = Math.random() * 2.0 - 1.0;
    //         d = x*x + y*y + z*z;
    //     } while(d > 1.0);
    //     return {x: x, y: y, z: z};
    // }

    geo::ColorCloud sCloud;
    for(size_t id = 0; id < 1000; ++id){
        float theta = (rand()%1000)*0.002f * PI<float>;
        float phi = (rand()%1000)*0.001f * PI<float>;
        geo::Pt3f pt = { std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)};
        if(pt.x() < 0.f && pt.y() < 0.f && pt.z() < 0.f && (rand()%5 == 0)){
            sCloud.vertices.push_back(pt + geo::Pt3f{1.2f,-2.3f, 4.5f});
            sCloud.colors.push_back({0.f,1.f,0.f});
        }
    }


    io::CloudIO::save_cloud("E:/_clouds/sphere.obj", sCloud);

    // geo::Pt3f center{0.f,0.f,0.f};
    // float radius_sq = 0.f;
    // float radius = 0.f;

    // for(const auto &v : sCloud.vertices){
    //     geo::Pt3f diff= v - center;
    //     auto ld_to_p_sq = geo::square_norm(diff);

    //     if(ld_to_p_sq > radius_sq){
    //         // Point is outside current sphere. update
    //         auto old_to_p = ld_to_p_sq;
    //         radius = (radius + old_to_p)/2.f;

    //         // update square of radius for next compare
    //         radius_sq = radius*radius;
    //         float old_to_new = old_to_p - radius;
    //         center = (center*radius + v*old_to_new) / old_to_p;
    //         Log::fmessage("point {} center {} radius {}\n", v, center, radius);
    //     }
    // }
    // // dx x – cenx;
    // // dy y – ceny;
    // // dz z – cenz;

    // Log::fmessage("->point {}\n", sCloud.vertices.sphere().position);


    // sCloud.vertices.push_back({});
    // sCloud.colors.push_back({1.f,0.f,0.f});
    // io::CloudIO::save_cloud("E:/_clouds/sphere_center.obj", sCloud);


    // Buffer<geo::Pt4f> A;
    Buffer<float> A;
    // A.resize(sCloud.vertices.size());
    // Assemble the A matrix
    // size_t id2 = 0;
    for(const auto &v : sCloud.vertices){
        A.push_back(v.x()*2);
    }
    for(const auto &v : sCloud.vertices){
        A.push_back(v.y()*2);
    }
    for(const auto &v : sCloud.vertices){
        A.push_back(v.z()*2);
    }
    for(const auto &v : sCloud.vertices){
        A.push_back(1.f);
    }
    //     A[id2] = {
    //         v.x()*2.f,
    //         v.y()*2.f,
    //         v.z()*2.f,
    //         1.f
    //     };
    //     Log::fmessage("v {} : {}\n", A[id2], id2);
    //     ++id2;
    // }


    Eigen::MatrixXf aEigen = Eigen::MatrixXf::Map(reinterpret_cast<float*>(A.get_data()), sCloud.vertices.size(), 4);
    std::cout << "EIGEN A: \n" << aEigen << "\n";

    // Assemble the f matrix
    Buffer<float> F;
    F.resize(sCloud.vertices.size());
    size_t id2 = 0;
    for(const auto &v : sCloud.vertices){
        F[id2] = geo::dot(v,v);
        // Log::fmessage("sn {} v {}\n", geo::square_norm(v), v);
        ++id2;
    }
    Eigen::MatrixXf fEigen = Eigen::MatrixXf::Map(F.get_data(), F.size(), 1);
    std::cout << "EIGEN F: \n" << fEigen << "\n";

    Eigen::BDCSVD<Eigen::MatrixXf> svd(aEigen, Eigen::ComputeThinU | Eigen::ComputeThinV);
    auto C = svd.solve(fEigen);
    // std::cout << "-> " << C << "\n";

    // #  solve for the radius
    auto t = (C(0)*C(0))+(C(1)*C(1))+(C(2)*C(2))+C(3);
    auto nradius = std::sqrt(t);

    std::cout << "-> " << nradius << " " << C(0) << " " << C(1) << " " << C(2)<< "\n";

    // sCloud.vertices.push_back({C(0),C(1),C(2)});
    // sCloud.colors.push_back({1.f,0.f,0.f});
    // io::CloudIO::save_cloud("E:/_clouds/sphere_center.obj", sCloud);

    auto radius2= 1.f;
    auto center2 = sCloud.vertices.mean();
    for(size_t id = 0; id < 10; ++id){
    for(const auto &v : sCloud.vertices){
        geo::Pt3f diffV = vec(v, center2);
        auto no     = norm(diffV);
        auto factor = radius2 / no;
        diffV       = divide(diffV, no);
        center2 += diffV * factor;

        Log::fmessage("{} : diffV {} no {} factor {} center {} ray {}\n", id, diffV, no, factor, center2, norm(vec(v, center2)));
    }
    }


    // Eigen::MatrixXf A_ = Eigen::MatrixXf::Random(3, 2);


    // std::cout << "Here is the matrix A:\n" << A_ << std::endl;
    // Eigen::VectorXf b_ = Eigen::VectorXf::Random(3);
    // std::cout << "Here is the right hand side b:\n" << b_ << std::endl;
    // std::cout << "The least-squares solution is:\n";

    // Eigen::BDCSVD<Eigen::MatrixXf> svd(A_, Eigen::ComputeThinU | Eigen::ComputeThinV);
    // std::cout << "-> " << svd.solve(b_) << "\n";



              // << A_.template Eigen::BDCSVD<Eigen::ComputeThinU | Eigen::ComputeThinV>().solve(b_) << std::endl;


     // = np.zeros((len(spX),4))
    // def sphereFit(spX,spY,spZ):
    // #   Assemble the A matrix


    //     spX = np.array(spX)
    //     spY = np.array(spY)
    //     spZ = np.array(spZ)

    //       A = np.zeros((len(spX),4))
    //       A[:,0] = spX*2
    //       A[:,1] = spY*2
    //       A[:,2] = spZ*2
    //       A[:,3] = 1

    // #   Assemble the f matrix
    //         f = np.zeros((len(spX),1))
    //           f[:,0] = (spX*spX) + (spY*spY) + (spZ*spZ)
    //           C, residules, rank,
                // singval = np.linalg.lstsq(A,f)

    // #   solve for the radius
    //                     t = (C[0]*C[0])+(C[1]*C[1])+(C[2]*C[2])+C[3]
    //             radius = math.sqrt(t)

    //             return radius, C[0], C[1], C[2]

    //     given x, y, z, cenx, ceny, cenz, rad, and rad_sq

    //     dx x – cenx;
    //     dy y – ceny;
    //     dz z – cenz;
    //     old_to_p_sq dx*dx + dy*dy + dz*dz;
    //     do economical r**2 test before calc sqrt
    //     if (old_to_p_sq > rad_sq)
    //         then
    //         Point is outside current sphere. update.
    //         old_to_p old_ to_ p_ sq ;
    //         rad (rad + old_to_p)/2.0;
    //         update square of radius for next compare
    //         rad_sq rad*rad;
    //         old_to_new old_to_p–rad;
    //         cenx (rad*cenx + old_to_new*x)/old_to_p;
    //         ceny (rad*ceny + old_to_new*y) / old_to_p;
    //         cenz (rad*cenz + old_to_new*z) / old_to_p;
    //         end



// lstsq_sphere_fitting: fit a given set of 3D points (x, y, z) to a sphere.
//         Args:
//                pos_xyz: a two dimentional numpy array, of which each row represents (x, y, z) coordinates of a point
//             Returns:
//                       radius, x0, y0, z0
//                       """
//                       def lstsq_sphere_fitting(pos_xyz):
// # add column of ones to pos_xyz to construct matrix A
//         row_num = pos_xyz.shape[0]
//                   A = np.ones((row_num, 4))
//           A[:,0:3] = pos_xyz

// # construct vector f
//             f = np.sum(np.multiply(pos_xyz, pos_xyz), axis=1)

//           sol, residules, rank, singval = np.linalg.lstsq(A,f)

// # solve the radius
//                     radius = math.sqrt((sol[0]*sol[0]/4.0)+(sol[1]*sol[1]/4.0)+(sol[2]*sol[2]/4.0)+sol[3])

//             return radius, sol[0]/2.0, sol[1]/2.0, sol[2]/2.0




    return 0;
    Buffer<int> b1({1,2,3});
    Buffer<int> b2 = {2,3,4};
    Buffer<int> b3 = b1;

    for(auto &v : b1){
        Log::fmessage("{} \n", v);
    }
    b1 = {2,3,4};
    for(auto &v : b1){
        Log::fmessage("{} \n", v);
    }
    for(auto &v : b2){
        Log::fmessage("{} \n", v);
    }
    for(auto &v : b3){
        Log::fmessage("{} \n", v);
    }
    return 0;



    return 0;
    // simd();
    // time();
    // microphone();
    // dc_device_idle();
    // femto_mega();
    // raw_orbbec_femto();
    // raw_kinect2();
    // kinect2();
    // test_kinect4();
    // test_raw_k4device();
    // pcl_encoding();
    // process_kvid();
    // dc_player_test();
    // test_dc_player_merge("E:/_kvid/21-02-24-k4x4-3.kvid");
    // dc_settings();
    // encode1();
    // encode2();
    // convert_kvid("D:/test_kvid/thirdPartR.kvid", "D:/test_kvid/thirdPartR_conv.kvid");

    {
        // cam::DCDeviceSettings ds;
        // ds.configS.idDevice = 0;
        // ds.configS.openDevice = true;
        // ds.configS.typeDevice = cam::DCType::AzureKinect;

        // ds.dataS.server.capture.color = true;
        // ds.dataS.server.capture.depth = true;

        // ds.dataS.clientGeneration.cloud = true;
        // ds.dataS.clientGeneration.depthSizedColorImage = true;

        // cam::DCDevice device;
        // device.update_device_settings(ds);

        // device.start_thread();

        // for(int ii = 0; ii < 10000; ++ii){
        //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //     std::cout << ii << " ";
        // }


        // device.stop_thread();

        // return 0;
    }

    {
        using namespace tool::cam;
        cam::DCClient client;
        if(client.initialize("E:/_json/loc8_2.json", false)){

            Log::fmessage("Set devices settings : count [{}]\n", client.devices_nb());

            for(size_t id = 0; id < client.devices_nb(); ++id){
                auto t = Time::nanoseconds_since_epoch();
                client.apply_device_settings(id);
                Log::fmessage("device id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
            }
            for(size_t id = 0; id < client.devices_nb(); ++id){
                auto t = Time::nanoseconds_since_epoch();
                client.apply_filters_settings(id);
                Log::fmessage("filters id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
            }
            for(size_t id = 0; id < client.devices_nb(); ++id){
                auto t = Time::nanoseconds_since_epoch();
                client.apply_color_settings(id);
                Log::fmessage("color id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // client.update()

            // client.new_frame_signal.connect([&](size_t idD, std::shared_ptr<cam::DCFrame> frame){
            //     auto c = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud);
            //     Log::fmessage("new_frame_signal {} {} {}\n", idD, frame->idCapture, c->size());
            // });

            Log::message("START\n");
            std::vector<std::unique_ptr<std::thread>> threads;
            std::vector<size_t> count;
            for(size_t id = 0; id < 4; ++id){ //
                count.push_back(0);
                threads.push_back(std::make_unique<std::thread>([&,id](){
                    for(int ii = 0; ii < 10000; ++ii){
                        auto t1 = Time::nanoseconds_since_epoch();
                        
                        Log::fmessage("READ {}\n", id);
                        client.read_from_external_thread(id);
                        auto frames = client.process_frames_from_external_thread(id);
                        if(auto frame = std::get<0>(frames); frame != nullptr){
                            // if(id == 0){
                                Log::fmessage("new frame {} {}\n", id, frame->idCapture);
                            // }
                            count[id]++;
                        }
                        if(auto dframe = std::get<1>(frames); dframe != nullptr){
                            // if(id == 0){
                                Log::fmessage("new dframe {} {}\n", id, dframe->idCapture);
                            // }
                            count[id]++;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                            // client.update();
                            //

                        continue;
                        
                        
                        // if(client.read_from_external_thread(id) != 0){
                        //     if(client.process_frames_from_external_thread(id)){
                        //         std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        //     }else{
                        //         std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        //     }
                        // }else{
                        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        // }
                        // Log::message("READ DATA 2\n");

                        // Log::message("READ DATA 3\n");

                        auto t2 = Time::nanoseconds_since_epoch();



                        // if(id == 0){
                        //     Log::fmessage("[0time {} {}]\n", Time::difference_micro_s(t1,t2));
                        // }
                        // std::cout << r << " | ";
                            // std::cout << "[BOUH]";
                        // if(r != 0){
                        //     client.process_frames_from_external_thread(id);
                        //     auto t3 = Time::nanoseconds_since_epoch();

                        //     std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        //     // auto t4 = Time::nanoseconds_since_epoch();

                        //     // if((id == 0) && (rand()%10 == 0)){
                        //     Log::fmessage("[time {} {} {} {}]\n", id, Time::difference_micro_s(t1,t2), Time::difference_micro_s(t2,t3));//, Time::difference_micro_s(t3,t4));
                        // }else{
                        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        // }
                        // }else{
                            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        // }
                        //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            // if(id == 0){
                            //     Log::fmessage("[0time {} {}]\n", Time::difference_micro_s(t1,t2));
                            // }
                        //     continue;
                        // }


                        // if(id == 0){
                        //     std::cout << ii << " ";
                        // }
                        // std::this_thread::sleep_for(std::chrono::milliseconds(5));

                    }
                }));
            }

            for(auto &c : count){
                Log::fmessage("frames {}\n", c);
            }

            Log::flog("JOINT\n");

            for(auto &thread : threads){
                thread->join();
            }
            // // client.
            // // client.process_frames_from_external_thread()
            // // for(size_t id = 0; id < client.devices_nb(); ++id){
            // //     auto t = Time::nanoseconds_since_epoch();
            // //     client.update_model_settings(id, client.settings.devicesS[id].modelS);
            // //     Log::fmessage("model id [{}] time [{}]\n", id, Time::now_difference_micro_s(t));
            // // }


            // // client.read_data_from_external_thread();
            // // client.process_frames_from_external_thread();


            // for(int ii = 0; ii < 10000; ++ii){
            //     client.update();


            // }
        }
    }



    return 0;

}





