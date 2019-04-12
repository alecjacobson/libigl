// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "volume.h"
#include "cross.h"
#include <Eigen/Geometry>
template <
  typename DerivedV,
  typename DerivedT,
  typename Derivedvol>
IGL_INLINE void igl::volume(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedT>& T,
  Eigen::PlainObjectBase<Derivedvol>& vol)
{
  using namespace Eigen;
  const int m = T.rows();
  vol.resize(m,1);
  for(int t = 0;t<m;t++)
  {
    typedef Eigen::Matrix<typename DerivedV::Scalar,1,3> RowVector3S;
    const RowVector3S & a = V.row(T(t,0));
    const RowVector3S & b = V.row(T(t,1));
    const RowVector3S & c = V.row(T(t,2));
    const RowVector3S & d = V.row(T(t,3));
    vol(t) = -(a-d).dot((b-d).cross(c-d))/6.;
  }
}

template <
  typename DerivedA,
  typename DerivedB,
  typename DerivedC,
  typename DerivedD,
  typename Derivedvol>
IGL_INLINE void igl::volume(
  const Eigen::MatrixBase<DerivedA> & A,
  const Eigen::MatrixBase<DerivedB> & B,
  const Eigen::MatrixBase<DerivedC> & C,
  const Eigen::MatrixBase<DerivedD> & D,
  Eigen::PlainObjectBase<Derivedvol> & vol)
{
  const auto & AmD = A-D;
  const auto & BmD = B-D;
  const auto & CmD = C-D;
  Derivedvol BmDxCmD;
  cross(BmD.eval(),CmD.eval(),BmDxCmD);
  const auto & AmDdx = (AmD.array() * BmDxCmD.array()).rowwise().sum();
  vol = -AmDdx/6.;
}

template <
  typename VecA,
  typename VecB,
  typename VecC,
  typename VecD>
IGL_INLINE typename VecA::Scalar igl::volume_single(
  const VecA & a,
  const VecB & b,
  const VecC & c,
  const VecD & d)
{
  return -(a-d).dot((b-d).cross(c-d))/6.;
}


template <
  typename DerivedL,
  typename Derivedvol>
IGL_INLINE void igl::volume(
  const Eigen::MatrixBase<DerivedL>& L,
  Eigen::PlainObjectBase<Derivedvol>& vol)
{
  using namespace Eigen;
  const int m = L.rows();
  typedef typename Derivedvol::Scalar ScalarS;
  vol.resize(m,1);
  for(int t = 0;t<m;t++)
  {
    const ScalarS u = L(t,0);
    const ScalarS v = L(t,1);
    const ScalarS w = L(t,2);
    const ScalarS U = L(t,3);
    const ScalarS V = L(t,4);
    const ScalarS W = L(t,5);
    const ScalarS X = (w - U + v)*(U + v + w);
    const ScalarS x = (U - v + w)*(v - w + U);
    const ScalarS Y = (u - V + w)*(V + w + u);
    const ScalarS y = (V - w + u)*(w - u + V);
    const ScalarS Z = (v - W + u)*(W + u + v);
    const ScalarS z = (W - u + v)*(u - v + W);
    const ScalarS a = sqrt(x*Y*Z);
    const ScalarS b = sqrt(y*Z*X);
    const ScalarS c = sqrt(z*X*Y);
    const ScalarS d = sqrt(x*y*z);
    vol(t) = sqrt(
       (-a + b + c + d)*
       ( a - b + c + d)*
       ( a + b - c + d)*
       ( a + b + c - d))/
       (192.*u*v*w);
  }
}


#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// Nonsense template
namespace igl{ template<> void volume<Eigen::Matrix<double, -1, 2, 0, -1, 2>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 2, 0, -1, 2> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&){} }
// generated by autoexplicit.sh
template void igl::volume<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&);
// generated by autoexplicit.sh
template void igl::volume<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&);
// generated by autoexplicit.sh
template void igl::volume<Eigen::Matrix<double, -1, 6, 0, -1, 6>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 6, 0, -1, 6> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&);
// generated by autoexplicit.sh
template void igl::volume<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&);
template Eigen::Matrix<double, 1, 3, 1, 1, 3>::Scalar igl::volume_single<Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, 1, 3, 1, 1, 3> >(Eigen::Matrix<double, 1, 3, 1, 1, 3> const&, Eigen::Matrix<double, 1, 3, 1, 1, 3> const&, Eigen::Matrix<double, 1, 3, 1, 1, 3> const&, Eigen::Matrix<double, 1, 3, 1, 1, 3> const&);
template void igl::volume<Eigen::Matrix<double,-1,3,0,-1,3>,Eigen::Matrix<int,-1,3,0,-1,3>,Eigen::Matrix<double,-1,1,0,-1,1> >(Eigen::MatrixBase<Eigen::Matrix<double,-1,3,0,-1,3> > const &,Eigen::MatrixBase<Eigen::Matrix<int,-1,3,0,-1,3> > const &,Eigen::PlainObjectBase<Eigen::Matrix<double,-1,1,0,-1,1> > &);
#endif
