// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "jet.h"
#include "colormap.h"

template <typename T>
IGL_INLINE void igl::jet(const T x, T * rgb)
{
  igl::colormap(igl::COLOR_MAP_TYPE_JET, x, rgb);
}

template <typename T>
IGL_INLINE void igl::jet(const T x_in, T & r, T & g, T & b)
{
  // Only important if the number of colors is small. In which case the rest is
  // still wrong anyway
  // x = linspace(0,1,jj)' * (1-1/jj) + 1/jj;
  //
  const double rone = 0.8;
  const double gone = 1.0;
  const double bone = 1.0;
  T x = x_in;
  x = (x_in<0 ? 0 : (x>1 ? 1 : x));

  if (x<1. / 8.)
  {
    r = 0;
    g = 0;
    b = bone*(0.5 + (x) / (1. / 8.)*0.5);
  } else if (x<3. / 8.)
  {
    r = 0;
    g = gone*(x - 1. / 8.) / (3. / 8. - 1. / 8.);
    b = bone;
  } else if (x<5. / 8.)
  {
    r = rone*(x - 3. / 8.) / (5. / 8. - 3. / 8.);
    g = gone;
    b = (bone - (x - 3. / 8.) / (5. / 8. - 3. / 8.));
  } else if (x<7. / 8.)
  {
    r = rone;
    g = (gone - (x - 5. / 8.) / (7. / 8. - 5. / 8.));
    b = 0;
  } else
  {
    r = (rone - (x - 7. / 8.) / (1. - 7. / 8.)*0.5);
    g = 0;
    b = 0;
  }
}

template <typename DerivedZ, typename DerivedC>
IGL_INLINE void igl::jet(
  const Eigen::MatrixBase<DerivedZ> & Z,
  const bool normalize,
  Eigen::PlainObjectBase<DerivedC> & C)
{
  igl::colormap(igl::COLOR_MAP_TYPE_JET,Z, normalize, C);
}

template <typename DerivedZ, typename DerivedC>
IGL_INLINE void igl::jet(
  const Eigen::MatrixBase<DerivedZ> & Z,
  const double min_z,
  const double max_z,
  Eigen::PlainObjectBase<DerivedC> & C)
{
  igl::colormap(igl::COLOR_MAP_TYPE_JET, Z, min_z, max_z, C);
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template void igl::jet<Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<double>(double, double*);
template void igl::jet<double>(double, double&, double&, double&);
template void igl::jet<float>(float, float*);
template void igl::jet<Eigen::Array<double, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Array<double, -1, 1, 0, -1, 1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<float>(float, float&, float&, float&);
template void igl::jet<Eigen::Matrix<float, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 1, 0, -1, 1> > const&, double, double, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> > const&, double, double, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, double, double, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::jet<Eigen::CwiseBinaryOp<Eigen::internal::scalar_quotient_op<double, double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const, Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const> const>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::CwiseBinaryOp<Eigen::internal::scalar_quotient_op<double, double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const, Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const> const> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
#endif
