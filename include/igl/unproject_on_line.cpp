#include "unproject_on_line.h"
#include "projection_constraint.h"

template <
  typename DerivedUV,
  typename DerivedM,
  typename DerivedVP,
  typename Derivedorigin,
  typename Deriveddir>
void igl::unproject_on_line(
  const Eigen::MatrixBase<DerivedUV> & UV,
  const Eigen::MatrixBase<DerivedM> & M,
  const Eigen::MatrixBase<DerivedVP> & VP,
  const Eigen::MatrixBase<Derivedorigin> & origin,
  const Eigen::MatrixBase<Deriveddir> & dir,
  typename DerivedUV::Scalar & t)
{
  using namespace Eigen;
  typedef typename DerivedUV::Scalar Scalar;
  Matrix<Scalar,2,3> A;
  Matrix<Scalar,2,1> B;
  projection_constraint(UV,M,VP,A,B);
  // min_z,t ‖Az - B‖²  subject to z = origin + t*dir
  // min_t  ‖A(origin + t*dir) - B‖²
  // min_t  ‖A*t*dir + A*origin - B‖²
  // min_t  ‖D*t + C‖²
  // t = -(D'D)\(D'*C)
  Matrix<Scalar,2,1> C = A*origin.template cast<Scalar>() - B;
  Matrix<Scalar,2,1> D =    A*dir.template cast<Scalar>();
  // Solve least squares system directly
  const Matrix<Scalar,1,1> t_mat = D.jacobiSvd(ComputeFullU | ComputeFullV).solve(-C);
  t = t_mat(0,0);
}

template <
  typename DerivedUV,
  typename DerivedM,
  typename DerivedVP,
  typename Derivedorigin,
  typename Deriveddir,
  typename DerivedZ>
void igl::unproject_on_line(
  const Eigen::MatrixBase<DerivedUV> & UV,
  const Eigen::MatrixBase<DerivedM> & M,
  const Eigen::MatrixBase<DerivedVP> & VP,
  const Eigen::MatrixBase<Derivedorigin> & origin,
  const Eigen::MatrixBase<Deriveddir> & dir,
  Eigen::PlainObjectBase<DerivedZ> & Z)
{
  typedef typename DerivedZ::Scalar Scalar;
  typename DerivedUV::Scalar t;
  unproject_on_line(UV,M,VP,origin,dir,t);
  Z = origin + dir*Scalar(t);
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
template void igl::unproject_on_line<Eigen::Matrix<double, 2, 1, 0, 2, 1>, Eigen::Matrix<float, 4, 4, 0, 4, 4>, Eigen::Matrix<float, 4, 1, 0, 4, 1>, Eigen::Matrix<double, 3, 1, 0, 3, 1>, Eigen::Matrix<double, 3, 1, 0, 3, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, 2, 1, 0, 2, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 4, 4, 0, 4, 4> > const&, Eigen::MatrixBase<Eigen::Matrix<float, 4, 1, 0, 4, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, 3, 1, 0, 3, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, 3, 1, 0, 3, 1> > const&, Eigen::Matrix<double, 2, 1, 0, 2, 1>::Scalar&);
#endif
