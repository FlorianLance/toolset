
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

// local
#include "vertices.hpp"

namespace tool::geo {

struct ColorCloud{

    ColorCloud() = default;
    ColorCloud(const ColorCloud& other) = default;
    ColorCloud& operator=(const ColorCloud& other) = default;
    ColorCloud(ColorCloud&& other) = default;
    ColorCloud& operator=(ColorCloud&& other) = default;

    constexpr explicit ColorCloud(size_t size, bool useNormals = false){
        resize(size, useNormals);
    }

    constexpr ColorCloud(const geo::Pt3f &vertex, const geo::Pt3f &color){
        vertices.push_back(vertex);
        colors.push_back(color);
    }

    // get
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {return vertices.empty();}
    [[nodiscard]] constexpr auto size() const noexcept -> size_t {return vertices.size();}
    [[nodiscard]] constexpr auto has_vertices() const noexcept -> bool{return !vertices.empty();}
    [[nodiscard]] constexpr auto has_colors() const noexcept -> bool{return !colors.empty();}
    [[nodiscard]] constexpr auto has_normals() const noexcept -> bool{return !normals.empty();}
    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool{
        if(has_colors()){
            if(vertices.size() != colors.size()){
                return false;
            }
        }
        if(has_normals()){
            if(vertices.size() != normals.size()){
                return false;
            }
        }
        return true;
    }

    // geometry modifiers
    constexpr auto remove_outliers(const Pt3f &target, float maxDistance) -> void{
        keep_from_ids(vertices.get_outliers_id(target, maxDistance));
    }

    // add
    auto merge(const ColorCloud &cloud)-> void;

    // remove
    constexpr auto keep_from_ids(const std::vector<size_t> &ids) noexcept -> void{
        vertices.keep_from_ids(ids);
        if(has_colors()){
            colors.keep_from_ids(ids);
        }
        if(has_normals()){
            normals.keep_from_ids(ids);
        }
    }

    // modify
    constexpr auto fill_colors(const geo::Pt3f &color) -> void{
        colors.fill(color);
    }

    // memory
    constexpr auto resize(size_t size, bool useNormals = false) -> void{

        vertices.resize(size);
        colors.resize(size);

        if(useNormals){
            normals.resize(size);
        }
    }

    constexpr auto reserve(size_t capacity, bool useNormals = false) -> void{

        vertices.reserve(capacity);
        colors.reserve(capacity);

        if(useNormals){
            normals.reserve(capacity);
        }
    }

    constexpr auto clear() -> void{
        vertices.clear();
        colors.clear();
        normals.clear();
    }

    constexpr auto shrink_to_fit() -> void{
        vertices.shrink_to_fit();
        colors.shrink_to_fit();
        normals.shrink_to_fit();
    }

    Vertices3D vertices;
    Buffer<geo::Pt3f> colors;
    Vertices3D normals;
};

}


// struct Geometry {

//     enum class Type : std::int8_t{
//         Unspecified = 0,
//         Triangle = 1,
//         PointCloud = 2,
// //        PointCloud = 1,
// //        VoxelGrid = 2,
// //        Octree = 3,
// //        LineSet = 4,
// //        MeshBase = 5,
// //        TriangleMesh = 6,
// //        HalfEdgeTriangleMesh = 7,
// //        Image = 8,
// //        RGBDImage = 9,
// //        TetraMesh = 10,
// //        OrientedBoundingBox = 11,
// //        AxisAlignedBoundingBox = 12,
//     };

//     Geometry(Type type, int dimension) : m_type(type), m_dim(dimension) {
//     }
//     virtual ~Geometry() {}

//     // get
//     constexpr auto type() const -> Type{return m_type;}
//     constexpr auto dimension() const -> int{return m_dim;}
//     virtual auto is_empty() const -> bool = 0;

// private:
//     Type m_type = Type::Unspecified;
//     int m_dim = 0;
// };

// struct Geometry3D : public Geometry{

// //    ~Geometry3D() override {}

// //protected:
// //    /// \brief Parameterized Constructor.
// //    ///
// //    /// \param type type of object based on GeometryType.
// //    Geometry3D(GeometryType type) : Geometry(type, 3) {}

// //public:
// //    Geometry3D& Clear() override = 0;
// //    bool IsEmpty() const override = 0;
// //    /// Returns min bounds for geometry coordinates.
// //    virtual Eigen::Vector3d GetMinBound() const = 0;
// //    /// Returns max bounds for geometry coordinates.
// //    virtual Eigen::Vector3d GetMaxBound() const = 0;
// //    /// Returns the center of the geometry coordinates.
// //    virtual Eigen::Vector3d GetCenter() const = 0;

// //    /// Creates the axis-aligned bounding box around the points of the object.
// //    /// Further details in AxisAlignedBoundingBox::CreateFromPoints()
// //    virtual AxisAlignedBoundingBox GetAxisAlignedBoundingBox() const = 0;

// //    /// Creates an oriented bounding box around the points of the object.
// //    /// Further details in OrientedBoundingBox::CreateFromPoints()
// //    /// \param robust If set to true uses a more robust method which works
// //    ///               in degenerate cases but introduces noise to the points
// //    ///               coordinates.
// //    virtual OrientedBoundingBox GetOrientedBoundingBox(
// //            bool robust = false) const = 0;

// //    /// Creates the minimal oriented bounding box around the points of the
// //    /// object. Further details in
// //    /// OrientedBoundingBox::CreateFromPointsMinimal()
// //    /// \param robust If set to true uses a more robust method which works
// //    ///               in degenerate cases but introduces noise to the points
// //    ///               coordinates.
// //    virtual OrientedBoundingBox GetMinimalOrientedBoundingBox(
// //            bool robust = false) const = 0;

// //    /// \brief Apply transformation (4x4 matrix) to the geometry coordinates.
// //    virtual Geometry3D& Transform(const Eigen::Matrix4d& transformation) = 0;

// //    /// \brief Apply translation to the geometry coordinates.
// //    ///
// //    /// \param translation A 3D vector to transform the geometry.
// //    /// \param relative If `true`, the \p translation is directly applied to the
// //    /// geometry. Otherwise, the geometry center is moved to the \p translation.
// //    virtual Geometry3D& Translate(const Eigen::Vector3d& translation,
// //                                  bool relative = true) = 0;
// //    /// \brief Apply scaling to the geometry coordinates.
// //    /// Given a scaling factor \f$s\f$, and center \f$c\f$, a given point
// //    /// \f$p\f$ is transformed according to \f$s (p - c) + c\f$.
// //    ///
// //    /// \param scale The scale parameter that is multiplied to the
// //    /// points/vertices of the geometry.
// //    /// \param center Scale center that is used to resize the geometry.
// //    virtual Geometry3D& Scale(const double scale,
// //                              const Eigen::Vector3d& center) = 0;

// //    /// \brief Apply rotation to the geometry coordinates and normals.
// //    /// Given a rotation matrix \f$R\f$, and center \f$c\f$, a given point
// //    /// \f$p\f$ is transformed according to \f$R (p - c) + c\f$.
// //    ///
// //    /// \param R A 3x3 rotation matrix
// //    /// \param center Rotation center that is used for the rotation.
// //    virtual Geometry3D& Rotate(const Eigen::Matrix3d& R,
// //                               const Eigen::Vector3d& center) = 0;

// //    virtual Geometry3D& Rotate(const Eigen::Matrix3d& R);

// //    /// Get Rotation Matrix from XYZ RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromXYZ(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from YZX RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromYZX(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from ZXY RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromZXY(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from XZY RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromXZY(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from ZYX RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromZYX(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from YXZ RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromYXZ(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from AxisAngle RotationType.
// //    static Eigen::Matrix3d GetRotationMatrixFromAxisAngle(
// //            const Eigen::Vector3d& rotation);
// //    /// Get Rotation Matrix from Quaternion.
// //    static Eigen::Matrix3d GetRotationMatrixFromQuaternion(
// //            const Eigen::Vector4d& rotation);

// //protected:
// //    /// Compute min bound of a list points.
// //    Eigen::Vector3d ComputeMinBound(
// //            const std::vector<Eigen::Vector3d>& points) const;
// //    /// Compute max bound of a list points.
// //    Eigen::Vector3d ComputeMaxBound(
// //            const std::vector<Eigen::Vector3d>& points) const;
// //    /// Computer center of a list of points.
// //    Eigen::Vector3d ComputeCenter(
// //            const std::vector<Eigen::Vector3d>& points) const;

// //    /// \brief Resizes the colors vector and paints a uniform color.
// //    ///
// //    /// \param colors An array of eigen vectors specifies colors in RGB.
// //    /// \param size The resultant size of the colors array.
// //    /// \param color The final color in which the colors will be painted.
// //    void ResizeAndPaintUniformColor(std::vector<Eigen::Vector3d>& colors,
// //                                    const size_t size,
// //                                    const Eigen::Vector3d& color) const;

// //    /// \brief Transforms all points with the transformation matrix.
// //    ///
// //    /// \param transformation 4x4 matrix for transformation.
// //    /// \param points A list of points to be transformed.
// //    void TransformPoints(const Eigen::Matrix4d& transformation,
// //                         std::vector<Eigen::Vector3d>& points) const;

// //    /// \brief Transforms the normals with the transformation matrix.
// //    ///
// //    /// \param transformation 4x4 matrix for transformation.
// //    /// \param normals A list of normals to be transformed.
// //    void TransformNormals(const Eigen::Matrix4d& transformation,
// //                          std::vector<Eigen::Vector3d>& normals) const;

// //    /// \brief Transforms all covariance matrices with the transformation.
// //    ///
// //    /// \param transformation 4x4 matrix for transformation.
// //    /// \param covariances A list of covariance matrices to be transformed.
// //    void TransformCovariances(const Eigen::Matrix4d& transformation,
// //                              std::vector<Eigen::Matrix3d>& covariances) const;

// //    /// \brief Apply translation to the geometry coordinates.
// //    ///
// //    /// \param translation A 3D vector to transform the geometry.
// //    /// \param points A list of points to be transformed.
// //    /// \param relative If `true`, the \p translation is directly applied to the
// //    /// \p points. Otherwise, the center of the \p points is moved to the \p
// //    /// translation.
// //    void TranslatePoints(const Eigen::Vector3d& translation,
// //                         std::vector<Eigen::Vector3d>& points,
// //                         bool relative) const;

// //    /// \brief Scale the coordinates of all points by the scaling factor \p
// //    /// scale.
// //    ///
// //    /// \param scale The scale factor that is used to resize the geometry
// //    /// \param points A list of points to be transformed
// //    /// \param center Scale center that is used to resize the geometry..
// //    void ScalePoints(const double scale,
// //                     std::vector<Eigen::Vector3d>& points,
// //                     const Eigen::Vector3d& center) const;

// //    /// \brief Rotate all points with the rotation matrix \p R.
// //    ///
// //    /// \param R A 3x3 rotation matrix
// //    /// defines the axis of rotation and the norm the angle around this axis.
// //    /// \param points A list of points to be transformed.
// //    /// \param center Rotation center that is used for the rotation.
// //    void RotatePoints(const Eigen::Matrix3d& R,
// //                      std::vector<Eigen::Vector3d>& points,
// //                      const Eigen::Vector3d& center) const;

// //    /// \brief Rotate all normals with the rotation matrix \p R.
// //    ///
// //    /// \param R A 3x3 rotation matrix
// //    /// \param normals A list of normals to be transformed.
// //    void RotateNormals(const Eigen::Matrix3d& R,
// //                       std::vector<Eigen::Vector3d>& normals) const;

// //    /// \brief Rotate all covariance matrices with the rotation matrix \p R.
// //    ///
// //    /// \param R A 3x3 rotation matrix
// //    /// \param covariances A list of covariance matrices to be transformed.
// //    void RotateCovariances(const Eigen::Matrix3d& R,
// //                           std::vector<Eigen::Matrix3d>& covariances) const;
// };

// struct Cloud2D{

// };

// struct Cloud3D{

// };
