
//#pragma once

///**
// * \file Transformation.h
// * \brief defines Mat3 and Transformation
// * \author Florian Lance
// * \date 01/02/15
// */

//// std
//#include <iostream>
//#include <fstream>

//// hbp
////#include "Plane3.h"
//#include "point3.hpp"

//namespace geo {


//template<class TAcc>
//class Mat3;

//template<class TAcc>
//class Mat4;

//template<class TAcc>
///**
// * @brief The Quaternion class
// */
//class Quaternion
//{
//public :

//    Quaternion(const Quaternion&) = default;
//    Quaternion(Quaternion&&) = default;
//    Quaternion& operator=(const Quaternion&) = default;
//    Quaternion& operator=(Quaternion&&) = default;
//    virtual ~Quaternion() = default;

//    /**
//     * @brief Quaternion default constructor
//     */
//    Quaternion() : w(1),x(0),y(0),z(0)
//    {}

//    /**
//     * @brief Constructor which init the quaternion with an axe and an angle
//     * @param [in] axe :  normalize axe vector
//     * @param [in] angle : in degrees
//     */
//    Quaternion(const Vec3<TAcc> &axe, const TAcc angle)
//    {
//        init_with_angle_and_axis(axe, angle);
//    }

//    /**
//     * @brief Init the quaternion with an axe and an angle
//     * @param [in] axe :  normalize axe vector
//     * @param [in] angle : in degrees
//     */
//    void init_with_angle_and_axis(const Vec3<TAcc> &axe, const TAcc angle)
//    {
//        TAcc radAngle = deg2rad(angle/2);
//        TAcc s = std::sin(radAngle);
//        x = axe.x() * s;
//        y = axe.y() * s;
//        z = axe.z() * s;
//        w = cos(radAngle);
//    }

//    /**
//     * @brief Convert quaternion to an axe and an angle
//     * @param [out] angle : in degrees
//     * @param [out] axe : rotation axe vector
//     */
//    void convert_axe_and_angle(TAcc &angle, Vec3<TAcc> &axe) const
//    {
//        normalize();
//        angle = rad2deg(acos(w) * 2);
//        axe.x() = x;
//        axe.y() = y;
//        axe.z() = z;
//    }

//    /**
//     * @brief Return the 4x4 matrix convertion of the quaternion
//     */
//    Mat4<TAcc> convert_to_matrix() const
//    {
//        Mat4<TAcc> mat;
//        TAcc xx      = x * x;
//        TAcc xy      = x * y;
//        TAcc xz      = x * z;
//        TAcc xw      = x * w;

//        TAcc yy      = y * y;
//        TAcc yz      = y * z;
//        TAcc yw      = y * w;

//        TAcc zz      = z * z;
//        TAcc zw      = z * w;

//        mat[0]  = 1 - 2 * ( yy + zz );
//        mat[1]  =     2 * ( xy - zw );
//        mat[2]  =     2 * ( xz + yw );

//        mat[4]  =     2 * ( xy + zw );
//        mat[5]  = 1 - 2 * ( xx + zz );
//        mat[6]  =     2 * ( yz - xw );

//        mat[8]  =     2 * ( xz - yw );
//        mat[9]  =     2 * ( yz + xw );
//        mat[10] = 1 - 2 * ( xx + yy );

//        mat[3]  = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
//        mat[15] = 1;

//        return mat;
//    }

//    /**
//     * @brief Return the norm
//     */
//    TAcc square_norm() const
//    {
//        return (w*w + x*x + y*y + z*z);
//    }

//    /**
//     * @brief Return the norm
//     */
//    TAcc norm() const
//    {
//        return sqrt(square_norm());
//    }

//    /**
//     * @brief Return the norm
//     * @param [in] q
//     */
//    static TAcc norm(const Quaternion &q)
//    {
//        return q.norm();
//    }

//    /**
//     * @brief Normalize the quaternion
//     */
//    void normalize()
//    {
//        TAcc magnitude = norm();
//        w /= magnitude;
//        x /= magnitude;
//        y /= magnitude;
//        z /= magnitude;
//    }

//    /**
//     * @brief Return the multiplication of two quaternion
//     * @param [in] q
//     */
//    Quaternion<TAcc> &operator*=(const Quaternion &q)
//    {
//        multiply(q);
//        return *this;
//    }

//    /**
//     * @brief multiply with the input quaternion
//     * @param [in] q
//     */
//    void multiply(const Quaternion &q)
//    {
//        w = w*q.w - x*q.x - y*q.y - z*q.z;
//        x = w*q.x + x*q.w + y*q.z - z*q.y;
//        y = w*q.y - x*q.z + y*q.w + z*q.x;
//        z = w*q.z + x*q.y - y*q.x + z*q.w;
//    }


//    TAcc w; /**< scalar */
//    TAcc x; /**< i factor */
//    TAcc y; /**< j factor */
//    TAcc z; /**< k factor */

//};


//template<class TAcc>
///**
// * @brief A 3x3 rotation matrix class
// */
//class Mat3
//{
//public :

//    Mat3(const Mat3&) = default;
//    Mat3(Mat3&&) = default;
//    Mat3& operator=(const Mat3&) = default;
//    Mat3& operator=(Mat3&&) = default;
//    virtual ~Mat3() = default;

//    /**
//     * @brief Mat3 constructor
//     * @param identity : if true init matrice with identity
//     */
//    Mat3(bool identity = false)
//    {
//        if(identity)
//            m_mat[0] = m_mat[4] = m_mat[8] = 1;
//    }

//    /**
//     * @brief operator =
//     * @param array
//     * @return
//     */
//    Mat3& operator=(std::array<TAcc,9> array) noexcept
//    {
//        m_mat = array;
//        return *this;
//    }

//    /**
//     * @brief operator += , add another matrix
//     * @param [in] mat : matrix to be added
//     */
//    void operator+=(const Mat3<TAcc>& mat)
//    {
//        for(int ii = 0; ii < 9; ++ii)
//            m_mat[ii] += mat.m_mat[ii];
//    }

//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline TAcc& at(cint idRow, cint idCol) {return m_mat[idRow*3 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline const TAcc& at(cint idRow, cint idCol) const {return m_mat[idRow*3 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline TAcc& operator()(cint idRow, cint idCol){return  m_mat[idRow*3 + idCol]; }


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline const TAcc& operator()(cint idRow, cint idCol) const {return  m_mat[idRow*3 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] id
//     */
//    inline TAcc& operator[](cint id){ return m_mat[id];}

//    /**
//     * @brief Return a value of the matrix
//     * @param [in] id
//     */
//    const TAcc& operator[](cint id) const { return m_mat[id];}

//    /**
//     * @brief mat
//     * @return
//     */
//    inline std::array<TAcc,9>& mat() noexcept {return m_mat;}

//    /**
//     * @brief mat
//     * @return
//     */
//    inline const std::array<TAcc,9>& mat() const noexcept {return m_mat;}

//    /**
//     * @brief operator *=, apply a factor on all the matrix
//     * @param [in] f
//     */
//    Mat3<TAcc>& operator*=(const TAcc f)
//    {
//        for(auto &&elem : m_mat)
//            elem *= f;

//        return *this;
//    }

//    /**
//     * @brief Apply the rotation to the input point
//     * @param [in,out] pt
//     */
//    inline void apply_rotation(Pt3<TAcc> &pt) const noexcept
//    {
//        pt = Point3<TAcc>(m_mat[0] * pt.x() + m_mat[1] * pt.y() + m_mat[2] * pt.z(),
//                       m_mat[3] * pt.x() + m_mat[4] * pt.y() + m_mat[5] * pt.z(),
//                       m_mat[6] * pt.x() + m_mat[7] * pt.y() + m_mat[8] * pt.z());
//    }

//    /**
//     * @brief Return the determinant of the rotation matrix
//     */
//    TAcc determinant() const
//    {
//        return at(0,0)*(at(1,1)*at(2,2)-at(2,1)*at(1,2))
//              -at(0,1)*(at(1,0)*at(2,2)-at(1,2)*at(2,0))
//              +at(0,2)*(at(1,0)*at(2,1)-at(1,1)*at(2,0));
//    }

//    /**
//     * @brief Transpose the rotation matrix
//     */
//    void transpose()
//    {
//        Mat3 tMat;
//        tMat.at(0,0) = at(0,0);
//        tMat.at(0,1) = at(1,0);
//        tMat.at(1,0) = at(0,1);
//        tMat.at(1,1) = at(1,1);
//        tMat.at(1,2) = at(2,1);
//        tMat.at(2,1) = at(1,2);
//        tMat.at(2,2) = at(2,2);
//        (*this) = std::move(tMat);
//    }


//    /**
//     * @brief Invert the matrix
//     */
//    void invert()
//    {
//        TAcc det = determinant();
//        if(almost_equal<TAcc>(det,0, 3))
//        {
//            std::cerr << "-Error : Mat3::invert -> determinant is null. " << std::endl;
//            return;
//        }

//        TAcc invdet = 1/det;
//        Mat3 invMat;
//        invMat.at(0,0) =  (at(1,1)*at(2,2)-at(2,1)*at(1,2))*invdet;
//        invMat.at(0,1) =  (at(0,2)*at(2,1)-at(0,1)*at(2,2))*invdet;
//        invMat.at(0,2) =  (at(0,1)*at(1,2)-at(0,2)*at(1,1))*invdet;
//        invMat.at(1,0) =  (at(1,2)*at(2,0)-at(1,0)*at(2,2))*invdet;
//        invMat.at(1,1) =  (at(0,0)*at(2,2)-at(0,2)*at(2,0))*invdet;
//        invMat.at(1,2) =  (at(1,0)*at(0,2)-at(0,0)*at(1,2))*invdet;
//        invMat.at(2,0) =  (at(1,0)*at(2,1)-at(2,0)*at(1,1))*invdet;
//        invMat.at(2,1) =  (at(2,0)*at(0,1)-at(0,0)*at(2,1))*invdet;
//        invMat.at(2,2) =  (at(0,0)*at(1,1)-at(1,0)*at(0,1))*invdet;

//        (*this) = std::move(invMat);
//    }

//    /**
//     * @brief display
//     * @param [in,out] flux
//     */
//    inline void display(std::ostream &flux) const{flux << "[" <<  m_mat[0] << " " <<  m_mat[1] << " " <<  m_mat[2] << "]" << "\n"
//                                                       << "[" <<  m_mat[3] << " " <<  m_mat[4] << " " <<  m_mat[5] << "]" << "\n"
//                                                       << "[" <<  m_mat[6] << " " <<  m_mat[7] << " " <<  m_mat[8] << "]" << "\n";}

//    /**
//     * @brief Display the matrix
//     */
//    void display()const
//    {
//        for(int ii = 0; ii < 9; ++ii)
//        {
//            if(ii%3 == 0 && ii !=0)
//                std::cout << "\n";

//            std::cout << m_mat[ii] << " ";
//        }
//    }

//private :

//    std::array<TAcc,9> m_mat = {{0,0,0,0,0,0,0,0,0}};
//};


//template<class TAcc>
///**
// * @brief A 4x4 transformation matrix class
// */
//class Mat4
//{
//public :


//    constexpr Mat4(const Mat4&) = default;
//    Mat4(Mat4&&) = default;
//    Mat4& operator=(const Mat4&) = default;
//    Mat4& operator=(Mat4&&) = default;
//    virtual ~Mat4() = default;

//    /**
//     * @brief Mat4 constructor
//     * @param identity : if true init matrice with identity
//     */
//    constexpr  Mat4(cbool identity = false) noexcept
//    {
//        if(identity)
//            m_mat[0] =  m_mat[5] =  m_mat[10] = m_mat[15] = 1;
//    }

//    /**
//     * @brief Mat4 constructor
//     * @param [in] rotation
//     * @param [in] translation
//     */
//    constexpr Mat4(Mat3<TAcc> &rotation, Pt3<TAcc> &translation) noexcept
//    {
//        m_mat ={rotation[0], rotation[1], rotation[2], translation[0],
//                rotation[3], rotation[4], rotation[5], translation[1],
//                rotation[6], rotation[7], rotation[8], translation[2],
//                0          , 0          , 0          , 1};
//    }

//    /**
//     * @brief operator =
//     * @param array
//     * @return
//     */
//    Mat4& operator=(std::array<TAcc,16> array) noexcept
//    {
//        m_mat = array;
//        return *this;
//    }


//    /**
//     * @brief operator += , add another matrix
//     * @param [in] mat : matrix to be added
//     */
//    void operator+=(const Mat3<TAcc>& mat)
//    {
//        for(int ii = 0; ii < 16; ++ii)
//            m_mat[ii] += mat.m_mat[ii];
//    }

//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline TAcc& at(cint idRow, cint idCol) {return m_mat[idRow*4 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline const TAcc& at(cint idRow, cint idCol) const {return m_mat[idRow*4 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline TAcc& operator()(cint idRow, cint idCol){return  m_mat[idRow*4 + idCol]; }


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] idRow
//     * @param [in] idCol
//     */
//    inline const TAcc& operator()(cint idRow, cint idCol) const {return  m_mat[idRow*4 + idCol];}


//    /**
//     * @brief Return a value of the matrix
//     * @param [in] id
//     */
//    inline TAcc& operator[](cint id){ return m_mat[id];}

//    /**
//     * @brief Return a value of the matrix
//     * @param [in] id
//     */
//    const TAcc& operator[](cint id) const { return m_mat[id];}

//    /**
//     * @brief mat
//     * @return
//     */
//    inline std::array<TAcc,16>& mat() noexcept {return m_mat;}

//    /**
//     * @brief mat
//     * @return
//     */
//    inline const std::array<TAcc,16>& mat() const noexcept {return m_mat;}


//    /**
//     * @brief operator *=, apply a factor on all the matrix
//     * @param [in] f
//     */
//    Mat4<TAcc>& operator*=(const TAcc f)
//    {
//        for(auto &&elem : m_mat)
//            elem *= f;

//        return *this;
//    }

//private :

//    std::array<TAcc,16> m_mat = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
//};


//template <typename TAcc>
///**
// * @brief slow basic 4x4 multiplication
// * @param m1
// * @param m2
// */
//static Mat4<TAcc> multiply(const Mat4<TAcc> &m1, const Mat4<TAcc> &m2)
//{
//    Mat4<TAcc> res;
//    for(int ii = 0; ii < 4; ++ii)
//    {
//        for(int jj = 0; jj < 4; ++jj)
//        {
//            res.at(ii,jj) = m1.at(ii,0) * m2.at(0,jj) + m1.at(ii,1) * m2.at(1,jj) + m1.at(ii,2)* m2.at(2,jj) + m1.at(ii,3) * m2.at(3,jj);
//        }
//    }
//    return res;
//}



//template<class TAcc>
///**
// * @brief A transformation class (3x3 rotation +  3x1 translation)
// */
//class Transformation3
//{
//public :

//    Transformation3(const Transformation3&) = default;
//    Transformation3(Transformation3&&) = default;
//    Transformation3& operator=(const Transformation3&) = default;
//    Transformation3& operator=(Transformation3&&) = default;
//    virtual ~Transformation3() = default;

//    /**
//     * @brief Transformation3 constructor
//     * @param [in] identity
//     */
//    Transformation3(cbool identity = false) : rotation(Mat3<TAcc>(identity)), translation(Pt3<TAcc>())
//    {}

//    /**
//     * @brief Transformation3 constructor
//     * @param [in] rot
//     * @param [in] trans
//     */
//    Transformation3(const Mat3<TAcc> &rot, const Vec3<TAcc> &trans = Vec3<TAcc>(0,0,0)) : rotation(rot), translation(trans) {}


//    /**
//     * @brief Transformation3 constructor
//     * @param [ın] mat
//     */
//    Transformation3(const Mat4<TAcc> &mat)
//    {
//        rotation = {mat[0], mat[1], mat[2],
//                    mat[4], mat[5], mat[6],
//                    mat[8], mat[9], mat[10]};

//        translation = Pt3<TAcc>(mat[3], mat[7], mat[11]);
//    }

//    /**
//     * @brief Transformation3D constructor
//     * @param [in] trmTransfoFilePath : path of the transformation to be loaded
//     */
//    Transformation3(const std::string &transfoFilePath);

//    /**
//     * @brief Load a transformation file (txt or trm)
//     * @param [in] transfoFilePath
//     * @return true if sucess else false
//     */
//    bool load(const std::string &transfoFilePath);

//    /**
//     * @brief Save a transformation to trm format
//     * @param [in] trmTransfoFileSavePath
//     * @return true if sucess else false
//     */
//    bool save_to_trm(const std::string &trmTransfoFileSavePath);

//    /**
//     * @brief Save a transformation to txt format
//     * @param [in] txtTransfoFileSavePath
//     * @return true if sucess else false
//     */
//    bool save_to_txt(const std::string &txtTransfoFileSavePath);

//    /**
//     * @brief Return the transformed input point
//     * @param [in] point
//     */
//    inline Point3<TAcc> transform(const Point3<TAcc> &point) const;

//    /**
//     * @brief Apply the transformation to the input point
//     * @param [in,out] point
//     */
//    inline void apply_transformation(Point3<TAcc> &point) const;

//    /**
//     * @brief Apply the translation to the input point
//     * @param [in,out] point
//     */
//    inline void apply_translation(Point3<TAcc> &point) const;

//    /**
//     * @brief Apply the rotation to the input point
//     * @param [in,out] point
//     */
//    inline void apply_rotation(Point3<TAcc> &point) const;

//    /**
//     * @brief Compose with the input transformation
//     * @param [in] transfo
//     */
//    void compose_transfo(Transformation3<TAcc> &transfo)
//    {
//        Mat4<TAcc> t1(rotation, translation);
//        Mat4<TAcc> t2(transfo.rotation, transfo.translation);
//        Mat4<TAcc> t3 = multiply(t1,t2);
//        *this = Transformation3<TAcc>(t3);
//    }

//    /**
//     * @brief Invert the transformation
//     */
//    void invert()
//    {
//        rotation.invert();
//        Mat3<TAcc> negRot = rotation;
//        negRot *= -1;
//        negRot.apply_rotation(translation);
//    }

//    /**
//     * @brief Return the transformation matrix
//     */
//    inline std::array<TAcc,9>& rot_mat(){return rotation.mat();}

//    /**
//     * @brief Return the transformation matrix
//     */
//    inline const std::array<TAcc,9>& rot_mat()const {return rotation.mat();}

//    /**
//     * @brief add the transformation coordinates in the flux
//     * @param [in,out] flux
//     */
//    inline void display(std::ostream &flux) const{flux << rotation << translation;}

//    Mat3<TAcc>    rotation;   /**< rotation matrix */
//    Point3<TAcc>  translation;/**< translation vector */


//private :

//    /**
//     * @brief Load a trm transformation file
//     * @param [in] trmTransfoFilePath : path of the transformation to be loaded
//     * @return true if sucess else false
//     */
//    bool load_trm(const std::string &trmTransfoFilePath);

//    /**
//     * @brief Load a raw txt transformation file
//     * @param [in] rawTxtTransfoFilePath : path of the transformation to be loaded
//     * @return true if sucess else false
//     */
//    bool load_raw_txt(const std::string &rawTxtTransfoFilePath);

//};

//template <typename TAcc>
///**
// * @brief operator <<
// * @param flux
// * @param pt
// * @return
// */
//static std::ostream &operator<<(std::ostream &flux, Mat3<TAcc> const& mat3)
//{
//    mat3.display(flux);
//    return flux;
//}


//template <typename TAcc>
///**
// * @brief operator <<
// * @param flux
// * @param pt
// * @return
// */
//static std::ostream &operator<<(std::ostream &flux, Transformation3<TAcc> const& transform)
//{
//    transform.display(flux);
//    return flux;
//}

//// ############################################## CLASS DEFINITIONS

//template <typename TAcc>
//bool Transformation3<TAcc>::load_trm(const std::string &trmTransfoFilePath)
//{
//    // open file
//    std::ifstream fileStream(trmTransfoFilePath);

//    if (!fileStream.is_open())
//    {
//        std::cerr << "-Error : Transformation3::loadTrmTransformation -> Can't open file : " << trmTransfoFilePath << std::endl;
//        return false;
//    }

//    // retrieve the full file in one string
//    std::stringstream buffer;
//    buffer << fileStream.rdbuf();
//    std::string fileString = buffer.str();

//    // replace all backlines with comas
//    for(size_t ii = 0; ii < fileString.size(); ++ii)
//    {
//        if(fileString[ii] == '\n')
//        {
//           fileString[ii] = ' ';
//        }
//    }

//    // split the string with comas
//    std::vector<std::string> splited = split(fileString, ' ');
//    for(int ii = 0; ii < static_cast<int>(splited.size()); ++ii)
//    {
//        std::istringstream buffer(splited[ii]);

//        if(ii < 3)
//        {
//            buffer >> translation(ii);
//        }
//        else
//        {
//            buffer >> rot_mat()[ii-3];
//        }
//    }

//    return true;
//}

//template <typename TAcc>
//bool Transformation3<TAcc>::load_raw_txt(const std::string &rawTxtTransfoFilePath)
//{
//    // open file
//    std::ifstream fileStream(rawTxtTransfoFilePath);

//    if (!fileStream.is_open())
//    {
//        std::cerr << "-Error : Transformation3::loadRawTxtTransformation -> Can't open file : " << rawTxtTransfoFilePath << std::endl;
//        return false;
//    }

//    std::string line;
//    std::vector<std::string> elements;
//    for(int ii = 0; ii < 3; ++ii)
//    {
//        std::getline(fileStream, line);
//        elements = split(line, ' ');
//        if(elements.size() == 4)
//        {
//            rotation[ii*3] = static_cast<TAcc>(std::stod(elements[0]));
//            rotation[ii*3+1] = static_cast<TAcc>(std::stod(elements[1]));
//            rotation[ii*3+2] = static_cast<TAcc>(std::stod(elements[2]));
//            translation[ii] = static_cast<TAcc>(std::stod(elements[3]));
//        }
//        else
//            return false;
//    }

//    return true;
//}

//template <typename TAcc>
//bool Transformation3<TAcc>::load(const std::string &transfoFilePath)
//{
//    if(transfoFilePath.size() >= 5)
//    {
//        std::string ext =transfoFilePath.substr(transfoFilePath.size()-3);
//        std::transform(ext.begin(), ext.end(),ext.begin(), ::toupper);

//        if(ext == "TXT")
//        {
//            return load_raw_txt(transfoFilePath);
//        }

//        if(ext == "TRM")
//        {
//            return load_trm(transfoFilePath);
//        }
//    }
//    std::cerr << "-ERROR : Transformation3::loadTransformation -> invalid file save path : " << transfoFilePath << "\n";

//    return false;
//}

//template <typename TAcc>
//bool Transformation3<TAcc>::save_to_trm(const std::string &trmTransfoFileSavePath)
//{
//    std::ofstream flowTRM(trmTransfoFileSavePath);

//    // open file to write
//    if(!flowTRM.is_open())
//    {
//        std::cerr << "-Error : Transformation3<TAcc>::saveToTrm -> Can't open trm file. " << std::endl;
//        return false;
//    }

//    flowTRM << translation[0] << " " << translation[1] << " " << translation[2] << "\n";
//    flowTRM << rotation[0] << " " << rotation[1] << " " << rotation[2] << "\n";
//    flowTRM << rotation[3] << " " << rotation[4] << " " << rotation[5] << "\n";
//    flowTRM << rotation[6] << " " << rotation[7] << " " << rotation[8] << "\n";

//    return true;
//}

//template <typename TAcc>
//bool Transformation3<TAcc>::save_to_txt(const std::string &txtTransfoFileSavePath)
//{
//    std::ofstream flowTxt(txtTransfoFileSavePath);

//    // open file to write
//    if(!flowTxt.is_open())
//    {
//        std::cerr << "-Error : Transformation3<TAcc>::saveToTxt -> Can't open txt file. " << std::endl;
//        return false;
//    }

//    flowTxt << rotation[0] << " " << rotation[1] << " " << rotation[2] << " " << translation[0] << "\n";
//    flowTxt << rotation[3] << " " << rotation[4] << " " << rotation[5] << " " << translation[1] << "\n";
//    flowTxt << rotation[6] << " " << rotation[7] << " " << rotation[8] << " " << translation[2] << "\n";
//    flowTxt << 0 << " " << 0 << " " << 0 << " " << 1<< "\n";

//    return true;
//}

//template <typename TAcc>
//Transformation3<TAcc>::Transformation3(const std::string &transfoFilePath)
//{
//    load(transfoFilePath);
//}

//template <typename TAcc>
//inline Point3<TAcc> Transformation3<TAcc>::transform(const Point3<TAcc> &pt)  const //noexcept
//{
//    return Point3<TAcc>( rotation[0] * pt.x() + rotation[1] * pt.y() + rotation[2] * pt.z() + translation.x(),
//                         rotation[3] * pt.x() + rotation[4] * pt.y() + rotation[5] * pt.z() + translation.y(),
//                         rotation[6] * pt.x() + rotation[7] * pt.y() + rotation[8] * pt.z() + translation.z());
//}

//template <typename TAcc>
//inline void Transformation3<TAcc>::apply_transformation(Point3<TAcc> &pt)  const// noexcept
//{
//    pt = transform(pt);
//}

//template <typename TAcc>
//inline void Transformation3<TAcc>::apply_translation(Point3<TAcc> &pt)  const// noexcept
//{
//    pt += translation;
//}

//template <typename TAcc>
//inline void Transformation3<TAcc>::apply_rotation(Point3<TAcc> &pt) const// noexcept
//{
//    rotation.apply_rotation(pt);
//}


//// ############################################## STATIC

//template <typename TAcc>
///**
// * @brief skew-symmetric cross-product matrix of v
// * @param v
// * @return
// */
//static inline Mat3<TAcc> skewSymmetricXProduct(const Vec3<TAcc> v)
//{
//    Mat3<TAcc> mat;
//    mat[1] = -v.z();
//    mat[2] = v.y();
//    mat[3] = v.z();
//    mat[5] = -v.x();
//    mat[6] = -v.y();
//    mat[7] = v.x();
//    return mat;
//}

//template <typename TAcc>
///**
// * @brief squareSkewSymmetricXProduct
// * @param v
// * @return
// */
//static inline Mat3<TAcc> squareSkewSymmetricXProduct(const Vec3<TAcc> v)
//{
//    Mat3<TAcc> mat;
//    mat[0] = -v.z()*v.z() - v.y()*v.y();
//    mat[1] = v.x()*v.y();
//    mat[2] = v.x() * v.z();
//    mat[3] = v.x()*v.y();
//    mat[4] = -v.z()*v.z() - v.x()*v.x();
//    mat[5] = v.y()*v.z();
//    mat[6] = v.x()*v.z();
//    mat[7] = v.y()*v.z();
//    mat[8] = -v.y()*v.y() - v.x()*v.x();
//    return mat;
//}


//template <typename TAcc>
///**
// * @brief slow basic 3x3 multiplication
// * @param m1
// * @param m2
// * @return
// */
//static Mat3<TAcc> multiply(const Mat3<TAcc> &m1, const Mat3<TAcc> &m2)
//{
//    Mat3<TAcc> res;
//    for(int ii = 0; ii < 3; ++ii)
//    {
//        for(int jj = 0; jj < 3; ++jj)
//        {
//            res.at(ii,jj) = m1.at(ii,0) * m2.at(0,jj) + m1.at(ii,1) * m2.at(1,jj) + m1.at(ii,2)* m2.at(2,jj);
//        }
//    }
//    return res;
//}


//template <typename TAcc>
///**
// * @brief rotationMatrixFromVectors
// * @param v1
// * @param v2
// * @return
// */
//static Mat3<TAcc> rotationMatrixFromVectors(const Vec3<TAcc> &v1, const Vec3<TAcc> &v2)
//{
//    Point3<TAcc> a = v1; a.normalize();
//    Point3<TAcc> b = v2; b.normalize();
//    Point3<TAcc> V = Pt3<TAcc>::cross_product(a,b);
//    TAcc s = V.norm();
//    TAcc c = Pt3<TAcc>::dot_product(a,b);

//    if(s == static_cast<TAcc>(0))
//        return Mat3<TAcc>(true);

//    Mat3<TAcc> vx = skewSymmetricXProduct<TAcc>(V);
//    Mat3<TAcc> vx_vx = squareSkewSymmetricXProduct(V);
//    vx_vx *= (1-c)/(s*s);
//    vx += Mat3<TAcc>(true);
//    vx += vx_vx;

//    return vx;
//}

//}

