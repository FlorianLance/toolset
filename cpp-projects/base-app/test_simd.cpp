
#include "tests.hpp"


// std
#include <iostream>
#include <algorithm>
#include <numeric>

#define VCL_NAMESPACE
#include "vectorclass.h"

// base
#include "utility/buffer.hpp"
#include "utility/time.hpp"
#include "utility/logger.hpp"

struct Vec16bBuffer{
    std::vector<VCL_NAMESPACE::Vec16sb> data;
    auto init(size_t size) -> void{
        if(size%16 != 0){
            std::cerr << "error\n";
            return;
        }
        data.resize(size/16);

        Vec16sb initV(true);
        for(auto &d : data){
            d = initV;
        }
    }
};

struct Vec16usBuffer{
    // size_t size = 640*576;
    // static constexpr size_t maxSize = 1024*1024/16;
    std::vector<VCL_NAMESPACE::Vec16us> data;
    // std::vector<size_t> ids;

    auto init(size_t size) -> void{
        if(size%16 != 0){
            std::cerr << "error\n";
            return;
        }
        data.resize(size/16);
        // ids.resize(data.size());
        // std::iota(ids.begin(), ids.begin(), 0);
    }

    auto load(std::span<std::uint16_t> iData) -> void{

        if(iData.size() !=  data.size()*16){
            std::cerr << "error\n";
            return;
        }

        // data.resize(iData.size()/16);

        // std::vector<size_t> ids;
        // ids.resize(data.size());
        // std::iota(ids.begin(), ids.begin(), 0);
        // std::for_each(std::execution::par_unseq, std::begin(ids), std::end(ids), [&](const auto &id){
        // data[id].load(iData.data() + 16*id);
        // });

        for(size_t id = 0; id < data.size(); ++id){
            data[id].load(iData.data() + 16*id);
        }
    }
};



auto simd() -> void{

    using namespace tool;
    using namespace tool::geo;
    using namespace std::chrono;

    VCL_NAMESPACE::Vec4i a(10 , 11, 12, 13);
    VCL_NAMESPACE::Vec4i b(20 , 21, 22, 23);
    VCL_NAMESPACE::Vec4i c = a + b;

    // void A_times_x( float4& out_y, const float4* in_A, const float4& in_x )
    // {
    //     out_y.x = dot(in_A[0], in_x); // 7 FLOPS
    //     out_y.y = dot(in_A[1], in_x); // 7 FLOPS
    //     out_y.z = dot(in_A[2], in_x); // 7 FLOPS
    //     out_y.w = dot(in_A[3], in_x); // 7 FLOPS
    // }


    // aaa.at(2);
    geo::Mat4f m1(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    geo::Pt4f p1(0,1,2,4);
    geo::Pt4f p2(5,6,7,8);

    geo::RowVec<float,4> r1 = m1.row1();

    std::cout << "p2 " << p2[1] << "\n";
    std::cout << "p2 " << p2[1] << "\n";
    std::cout << "row0 " << m1.row0() << "\n";
    std::cout << "row1 " << m1.row1() << "\n";

    float total = 0.f;

    {
        auto t1 = Time::nanoseconds_since_epoch();
        for(int ii = 0; ii < 1000000; ++ii){
            total += std::inner_product(p1.array.cbegin(), p1.array.cend(), p2.array.cbegin(), 0.f);
        }
        Log::fmessage("time dot1 {} {}\n", Time::now_difference_micro_s(t1), total);
    }
    total = 0.f;
    {
        auto t1 = Time::nanoseconds_since_epoch();
        auto &a1 = p1.array;
        auto &a2 = p2.array;
        for(int ii = 0; ii < 1000000; ++ii){
            total += a1[0]*a2[0] + a1[1]*a2[1] +a1[2]*a2[2] +a1[3]*a2[3];
        }
        Log::fmessage("time dot2 {} {}\n", Time::now_difference_micro_s(t1), total);
    }


    total = 0.f;
    {
        auto t1 = Time::nanoseconds_since_epoch();
        auto &a1 = p1.array;
        auto &a2 = p2.array;
        auto s1 = p1.cspan();
        auto s2 = p2.cspan();

        for(int ii = 0; ii < 1000000; ++ii){
            total += geo::dot(p1,p2);
        }
        Log::fmessage("time dot3 {} {}\n", Time::now_difference_micro_s(t1), total);
    }


    // float dot( const float4& in_A, const float4& in_x )
    // {
    //     return in_A.x*in_x.x + in_A.y*in_x.y + in_A.z*in_x.z + in_A.w*in_x.w; // 7 FLOPS
    // }

    geo::Pt4f totalP;
    {
        auto t1 = Time::nanoseconds_since_epoch();

        for(int ii = 0; ii < 1000000; ++ii){
            totalP += geo::Pt4f{geo::dot(p1, m1.col0()),geo::dot(p1, m1.col1()),geo::dot(p1, m1.col2()),geo::dot(p1, m1.col3())};
        }
        Log::fmessage("time mul1 {} {} {} {} {}\n", Time::now_difference_micro_s(t1), totalP.x(), totalP.y(), totalP.z(), totalP.w());
    }

    totalP = {};
    {
        auto t1 = Time::nanoseconds_since_epoch();

        for(int ii = 0; ii < 1000000; ++ii){
            totalP += p1*m1;
        }
        Log::fmessage("time mul2 {} {} {} {} {}\n", Time::now_difference_micro_s(t1), totalP.x(), totalP.y(), totalP.z(), totalP.w());
    }


    totalP = {};
    {
        auto t1 = Time::nanoseconds_since_epoch();



        for(int ii = 0; ii < 1000000; ++ii){
            // totalP += p1*m1;
            // const auto c1 = m1.col1()
            VCL_NAMESPACE::Vec4f pt;
            pt.load(p1.array.data());
            // xmm = _mm_setr_ps(f0, f1, f2, f3); // float __z, float __y, float __x, float __w
            VCL_NAMESPACE::Vec4f col0(m1[0],m1[4],m1[8],m1[12]);

            // _mm_mul_ps(a, b);
            auto resCol0 = pt * col0;

            VCL_NAMESPACE::Vec4f col1(m1[1],m1[5],m1[9],m1[13]);
            auto resCol1 = pt * col1;

            VCL_NAMESPACE::Vec4f col2(m1[2],m1[6],m1[10],m1[14]);
            auto resCol2 = pt * col2;

            VCL_NAMESPACE::Vec4f col3(m1[3],m1[7],m1[11],m1[15]);
            auto resCol3 = pt * col3;

            auto res = VCL_NAMESPACE::Vec4f(VCL_NAMESPACE::horizontal_add(resCol0),VCL_NAMESPACE::horizontal_add(resCol1),VCL_NAMESPACE::horizontal_add(resCol2),VCL_NAMESPACE::horizontal_add(resCol3));


            totalP += 1;//geo::Pt4f{res.extract(0),res.extract(1),res.extract(2),res.extract(3)};
            //
            // geo::Pt4f{geo::dot(p1, m1.col0()),geo::dot(p1, m1.col1()),geo::dot(p1, m1.col2()),geo::dot(p1, m1.col3())};

        }
        Log::fmessage("time mul3 {} {} {} {} {}\n", Time::now_difference_micro_s(t1), totalP.x(), totalP.y(), totalP.z(), totalP.w());
    }


    // void multiplyPointMatrix(const float point[4], const float matrix[16], float result[4]) {
    //     // Initialize SIMD vector for the point
    //     __m128 pointVec = _mm_set_ps(point[3], point[2], point[1], point[0]);

    //     // Initialize SIMD vector for column 0 of matrix
    //     __m128 col0 = _mm_set_ps(matrix[3], matrix[7], matrix[11], matrix[15]);

    //     // Multiply point by column 0
    //     __m128 resultCol0 = _mm_mul_ps(pointVec, col0);

    //     // Initialize SIMD vector for column 1 of matrix
    //     __m128 col1 = _mm_set_ps(matrix[2], matrix[6], matrix[10], matrix[14]);

    //     // Multiply point by column 1
    //     __m128 resultCol1 = _mm_mul_ps(pointVec, col1);

    //     // Initialize SIMD vector for column 2 of matrix
    //     __m128 col2 = _mm_set_ps(matrix[1], matrix[5], matrix[9], matrix[13]);

    //     // Multiply point by column 2
    //     __m128 resultCol2 = _mm_mul_ps(pointVec, col2);

    //     // Initialize SIMD vector for column 3 of matrix
    //     __m128 col3 = _mm_set_ps(matrix[0], matrix[4], matrix[8], matrix[12]);

    //     // Multiply point by column 3
    //     __m128 resultCol3 = _mm_mul_ps(pointVec, col3);

    //     // Sum up the results
    //     float sum0 = _mm_cvtss_f32(_mm_hadd_ss(resultCol0, resultCol1));
    //     float sum1 = _mm_cvtss_f32(_mm_hadd_ss(resultCol2, resultCol3));

    //     // Store the results
    //     result[0] = sum0;
    //     result[1] = sum1;

    //     // Calculate z component (w component of result)
    //     result[2] = _mm_cvtss_f32(_mm_mul_ps(pointVec, _mm_set_ps(matrix[12], matrix[8], matrix[4], matrix[0])));

    //     // Calculate w component (last element of result)
    //     result[3] = _mm_cvtss_f32(_mm_mul_ps(pointVec, _mm_set_ps(matrix[13], matrix[9], matrix[5], matrix[1])));
    // }








    std::cout << "M1xP1 " << p1*m1 << "\n";

    geo::Pt4f d;
    d.x() = geo::dot(p1, m1.col0());
    d.y() = geo::dot(p1, m1.col1());
    d.z() = geo::dot(p1, m1.col2());
    d.w() = geo::dot(p1, m1.col3());

    geo::dot(p1, m1.col0());

    std::cout << "d " << d << "\n";

    return;


    for(size_t ii = 0; ii < c.size(); ++ii){
        std::cout << c[ii] << "\n";
    }



    // 640 x 576
    // -> 512
    // -> 256
    // -> 128

    // Vec16us / Vec16s -> 16 bits integers - 16 elements - 256 bits
    // -> 23040 vectors
    // 1 for each dimension


    // Mask bool
    // Vec16sb | 16

    // compare x coordinates and fill mask
    // compare y coordinates and fill mask
    // compare z coordinates and fill mask


    size_t width = 640;
    size_t height = 576;


    std::vector<std::uint16_t> dvalues;
    dvalues.resize(width*height);
    std::iota(dvalues.begin(), dvalues.end(), 0);

    // VCL_NAMESPACE::Vec16us v1; // 256
    // v1.load(dvalues.data());

    // VCL_NAMESPACE::Vec16us v2;
    // v2.load_a(dvalues.data()+32); // must be divisible by 32 for 256bit arrays

    for(auto &v : dvalues){
        v = rand()% 500;
    }

    // for(size_t ii = 0; ii < v1.size(); ++ii){
    //     std::cout << static_cast<int>(v1[ii]) << " ";
    // }
    // std::cout << "\n";

    // for(size_t ii = 0; ii < v2.size(); ++ii){
    //     std::cout << static_cast<int>(v2[ii]) << " ";
    // }
    // std::cout << "\n";

    // return;

    std::vector<VCL_NAMESPACE::Vec16us> array;
    array.resize(width*height/16);


    // IDS
    std::vector<VCL_NAMESPACE::Vec16us> xIds;
    std::vector<VCL_NAMESPACE::Vec16us> yIds;
    xIds.resize(width*height/16);
    yIds.resize(width*height/16);
    size_t idV = 0;
    for(size_t ii = 0; ii < height; ++ii){
        for(size_t jj = 0; jj < width/16; ++jj){
            yIds[idV] = VCL_NAMESPACE::Vec16us(ii);
            ++idV;
        }
    }

    idV = 0;
    for(size_t ii = 0; ii < height; ++ii){
        for(size_t jj = 0; jj < width/16; ++jj){
            for(size_t kk = 0; kk < 16; ++kk){
                xIds[idV].insert(kk, jj*16+kk);
            }
            ++idV;
        }
    }

    // idy
    // [0000000000000000]..[0000000000000000] (width)
    // [1111111111111111]..[1111111111111111]

    // idx
    // [0][1]...
    // [0]
    // ...


    // for(size_t id = 0; id < 640*576/16; ++id){
    //     array[id].load(dvalues.data() + id*16);
    // }

    // {
    //     std::vector<std::uint16_t> nxIds;
    //     nxIds.resize(640*576);
    //     for(size_t id = 0; id < 640*576/16; ++id){
    //         xIds[id].store(nxIds.data() + id*16);
    //     }

    //     size_t id = 0;
    //     for(auto &v : nxIds){
    //         std::cout << v <<" ";
    //         if(id++ == 10000){
    //             break;
    //         }
    //     }
    // }


    // {
    //     std::vector<std::uint16_t> nyIds;
    //     nyIds.resize(640*576);
    //     for(size_t id = 0; id < 640*576/16; ++id){
    //         yIds[id].store(nyIds.data() + id*16);
    //     }

    //     size_t id = 0;
    //     for(auto &v : nyIds){
    //         std::cout << v <<" ";
    //         if(id++ == 10000){
    //             break;
    //         }
    //     }
    // }

    // dot
    // static inline double dot_product (Vec3Dd const a, Vec3Dd const b) {
    //     Vec4d c  = (a.to_vector() * b.to_vector()).cutoff(3);
    //     return horizontal_add(c);
    // }

    // static inline Vec3Dd cross_product (Vec3Dd const a, Vec3Dd const b) {
    //     Vec4d a1 = permute4<1,2,0,-1>(a.to_vector());
    //     Vec4d b1 = permute4<1,2,0,-1>(b.to_vector());
    //     Vec4d a2 = permute4<2,0,1,-1>(a.to_vector());
    //     Vec4d b2 = permute4<2,0,1,-1>(b.to_vector());
    //     Vec4d c  = a1 * b2 - a2 * b1;
    //     return c.cutoff(3);
    // }


    {
        VCL_NAMESPACE::Vec32uc d;
        // 8 8 8 8

        VCL_NAMESPACE::Vec4f rgbColors;
        VCL_NAMESPACE::Vec4f hsvColors;

        float fCMax = VCL_NAMESPACE::horizontal_max(rgbColors);
        float fCMin = VCL_NAMESPACE::horizontal_min(rgbColors);
        float fDelta = fCMax - fCMin;

        // VCL_NAMESPACE::per
    }

    // Vec16f 16 float values -> 4 points
    // Vec4i 4

    {

        // 4 * 32 -> 4 points
        //
        Vec16fb v;


        VCL_NAMESPACE::Vec16f normalVec; // = normal vec normal vec...
        VCL_NAMESPACE::Vec16f meanPt; // = mean mean mean...
        VCL_NAMESPACE::Vec16f spherePt;
        VCL_NAMESPACE::Vec16f oobPos;
        // auto invO = inverse(oobOrientation);

        VCL_NAMESPACE::Vec16f xyzw;
        VCL_NAMESPACE::Vec16us depth;
        float squareMaxDistanceFromPoint = 5.f;

        // plane
        {
            VCL_NAMESPACE::Vec16f vec = normalVec * (xyzw - meanPt);
            // VCL_NAMESPACE::Vec16f p1 = VCL_NAMESPACE::permute16<0,1,2,3,     -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec);
            auto dot1 = horizontal_add(permute16<0,1,2,3,     -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot2 = horizontal_add(permute16<4,5,6,7,     -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot3 = horizontal_add(permute16<8,9,10,11,   -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot4 = horizontal_add(permute16<12,13,14,15, -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            // ...
            // depending dot res, we cancel the depth value
        }

        // remove from point distance
        {
            VCL_NAMESPACE::Vec16f vec = (xyzw - spherePt);
            auto dot1 = horizontal_add(permute16<0,1,2,3,     -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot2 = horizontal_add(permute16<4,5,6,7,     -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot3 = horizontal_add(permute16<8,9,10,11,   -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            auto dot4 = horizontal_add(permute16<12,13,14,15, -1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1>(vec));
            Vec16fb total(Vec8fb(dot1 < squareMaxDistanceFromPoint,dot2 < squareMaxDistanceFromPoint),Vec8fb(dot3 < squareMaxDistanceFromPoint,dot4 < squareMaxDistanceFromPoint));
            Vec4ib totali(dot1 < squareMaxDistanceFromPoint,dot2 < squareMaxDistanceFromPoint,dot3 < squareMaxDistanceFromPoint,dot4 < squareMaxDistanceFromPoint);
        }

        // keepOnlyPointsInsideOOB
        {
            // VCL_NAMESPACE::Vec16f pointLocal = (xyzw - oobPos);
            // auto pointLocalRotated = pointLocal * invO;
            // auto pointLocal = (fData.depthCloud[idD].template conv<float>()) - oobPos;// settings.filters.oob.position;


            // v = u1*col1 + u2*col2 + u3*col3 + u4*col4.

            // 0  1  2  3
            // 4  5  6  7
            // 8  9 10 11
            // 12 13 14 15

            // 0 4 8 c 1 5 9 d 2 6 a e 3 7 b f

            // VCL_NAMESPACE::Vec16f mat;
        }

        // truncate(vec);

                // __m128 v;
    }


    // rgbColors.

    return;

    for(size_t id = 0; id < 640*576/16; ++id){

    }


    auto tLoad = Time::nanoseconds_since_epoch();
    for(int ii = 0; ii < 1000;++ii){
        for(size_t id = 0; id < 640*576/16; ++id){
            array[id].load(dvalues.data() + id*16);
        }
    }
    Log::fmessage("load time {}\n", Time::now_difference_micro_s(tLoad).count()*0.001);

    std::vector<std::uint16_t> ndvalues;
    ndvalues.resize(640*576);
    std::fill(ndvalues.begin(), ndvalues.end(), 0);

    auto tStore = Time::nanoseconds_since_epoch();
    for(int ii = 0; ii < 1000;++ii){
        for(size_t id = 0; id < 640*576/16; ++id){
            array[id].store(ndvalues.data() + id*16);
        }
    }
    Log::fmessage("store time {}\n", Time::now_difference_micro_s(tStore).count()*0.001);

    size_t id = 0;
    for(auto &v : ndvalues){
        std::cout << v <<" ";
        if(id++ == 200){
            break;
        }
    }
    std::cout << "\n";


    // array[0].load(dvalues.data());
    // array[1].load(dvalues.data()+16);

    // std::cout << "v: " << array[0].extract(5) << "\n";

    // Vec16usBuffer array2;
    // array2.init(dvalues.size());

    // Vec16usBuffer array3;
    // array3.init(dvalues.size());

    // auto t1 = Time::nanoseconds_since_epoch();
    // for(int ii = 0; ii < 1000; ++ii){
    //     array2.load(dvalues);
    //     // std::iota(dvalues.begin(), dvalues.end(), ii);
    // }
    // auto t2 = Time::nanoseconds_since_epoch();
    // std::cout << "time " << Time::difference_micro_s(t1,t2).count() << " " << array2.data.size() << " " << (int)array2.data.front()[0]  <<  "\n";

    // Vec16bBuffer barray;
    // barray.init(array.size());

    std::vector<VCL_NAMESPACE::Vec16sb> barray;
    barray.resize(array.size());

    VCL_NAMESPACE::Vec16us min(35);
    VCL_NAMESPACE::Vec16us max(157);
    std::cout << max[0] << " " << max[15] << "\n";


    {
        VCL_NAMESPACE::Vec16us vv(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
        VCL_NAMESPACE::Vec16us v1 = vv;
        VCL_NAMESPACE::Vec16us v2(0);
        vv = select(vv >= 3 && vv <= 10, v1, v2);

        // for(int ii = 0; ii < vv.size(); ++ii){
        //     std::cout << (int)vv.extract(ii) << " ";
        // }
    }
    // return 0;


    auto t3 = Time::nanoseconds_since_epoch();
    VCL_NAMESPACE::Vec16us vnull(0);

    for(int jj = 0; jj < 100; ++jj){
        for(int ii = 0; ii < array.size(); ++ii){

            array[ii] = select(array[ii] >= 30 && array[ii] <= 75, array[ii], vnull);

            // barray[ii] = array[ii] >= 30 && array[ii] <= 75;
            // array[ii] = select(barray[ii], array[ii], vnull);

            // for(int ii = 0; ii < array[ii].size(); ++ii){
            // std::cout << (int)array[ii].extract(ii) << " ";
            // }
            // std::cout <<  "|";
        }
    }

    // std::cout << "\n";

    // for(int ii = 0; ii < array2.data.size(); ++ii){

    //     auto &vv = array2.data[ii];

    //     // array3.data[ii] = VCL_NAMESPACE::Vec16us(ii%1,ii%2,ii%3,ii%4,ii%1,ii%2,ii%3,ii%4,ii%1,ii%2,ii%3,ii%4,ii%1,ii%2,ii%3,ii%4);
    //     // array3.data[ii].
    //     array3.data[ii] = VCL_NAMESPACE::Vec16us (
    //         vv[0] ? 1 : 0, vv[1]  ? 1 : 0,vv[2]  ? 1 : 0,vv[3] ? 1 : 0,
    //         vv[4] ? 1 : 0, vv[5]  ? 1 : 0,vv[6]  ? 1 : 0,vv[7] ? 1 : 0,
    //         vv[8] ? 1 : 0, vv[9]  ? 1 : 0,vv[10]  ? 1 : 0,vv[11] ? 1 : 0,
    //         vv[12] ? 1 : 0, vv[13]  ? 1 : 0,vv[14]  ? 1 : 0,vv[15] ? 1 : 0);

    //     array2.data[ii] *= array3.data[ii];
    // }
    // for(int ii = 0; ii < array2.data.size(); ++ii){
    //     barray.data[ii] &= array2.data[ii] > min;
    // }
    // for(int ii = 0; ii < array2.data.size(); ++ii){
    //     barray.data[ii] &= array2.data[ii] < max;
    // }
    auto t4 = Time::nanoseconds_since_epoch();
    std::cout << "time simd: " << Time::difference_micro_s(t3,t4).count()*0.001 <<  " " << array.size() << "\n";


    // auto vb = array2.data.front() < max;

    VCL_NAMESPACE::Vec16us vv1(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    VCL_NAMESPACE::Vec16b bb1(0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1);


    {
        VCL_NAMESPACE::Vec4ib b(1,0,1,0);
        VCL_NAMESPACE::Vec4ui c = VCL_NAMESPACE::Vec4ui(b);

        std::cout << "c: ";
        for(int ii = 0; ii < c.size(); ++ii){
            std::cout << c.extract(ii) << " ";
        }
    }

    // VCL_NAMESPACE::Vec16s vBB1 = VCL_NAMESPACE::Vec16s(bb1);
    // VCL_NAMESPACE::Vec16us r = vv1. && bb1;
    // VCL_NAMESPACE::Vec16us vv2;
    auto resb = VCL_NAMESPACE::to_bits(bb1);
    std::cout << "vv1: ";

    {
        VCL_NAMESPACE::Vec4i aaa(0,1,2,3);
        VCL_NAMESPACE::Vec4i bbb(4,5,6,7);
        auto fff = aaa > bbb;
        VCL_NAMESPACE::Vec4i ggg = VCL_NAMESPACE::Vec4i(fff);
    }

    VCL_NAMESPACE::Vec16us vb1(
        bb1[0] ? 1 : 0, bb1[1]  ? 1 : 0,bb1[2]  ? 1 : 0,bb1[3] ? 1 : 0,
        bb1[4] ? 1 : 0, bb1[5]  ? 1 : 0,bb1[6]  ? 1 : 0,bb1[7] ? 1 : 0,
        bb1[8] ? 1 : 0, bb1[9]  ? 1 : 0,bb1[10]  ? 1 : 0,bb1[11] ? 1 : 0,
        bb1[12] ? 1 : 0, bb1[13]  ? 1 : 0,bb1[14]  ? 1 : 0,bb1[15] ? 1 : 0);

    std::cout << "before vv1: ";
    for(int ii = 0; ii < vv1.size(); ++ii){
        std::cout << vv1.extract(ii) << " ";
    }
    std::cout << "\n";

    vv1 *= vb1;
    std::cout << "after vv1: ";
    for(int ii = 0; ii < vv1.size(); ++ii){
        std::cout << vv1.extract(ii) << " ";
    }
    std::cout << "\n";
    // std::cout << "res: " << resb << "\n";

    // VCL_NAMESPACE::Const_int_t

    // VCL_NAMESPACE::truncate_to_int32()

    // auto r1 = Vec16us(bb1) * vv1;
    // std::cout << vv1.extract(0) << " " << r1.extract(1) << " "<< r1.extract(2) << "\n";


    std::vector<std::int8_t> bb;
    bb.resize(dvalues.size());

    // for(int ii = 0; ii < 100;++ii){
    std::fill(bb.begin(), bb.end(), 1);

    auto t5 = Time::nanoseconds_since_epoch();
    for(size_t id = 0; id < dvalues.size(); ++id){
        bb[id] &= (dvalues[id] < 157 ? 1 : 0) && (dvalues[id] > 15 ? 1 : 0);
        // bb[id] &= dvalues[id] > 15 ? 1 : 0;
    }

    auto t6 = Time::nanoseconds_since_epoch();
    std::cout << "time " << dvalues.size() << " " << Time::difference_micro_s(t5,t6).count() << "\n";
    // }


}
