// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "slice.h"
#include "colon.h"

#include <vector>

template <
    typename TX,
    typename TY,
    typename DerivedR,
    typename DerivedC>
IGL_INLINE void igl::slice(
    const Eigen::SparseMatrix<TX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const Eigen::MatrixBase<DerivedC> &C,
    Eigen::SparseMatrix<TY> &Y)
{
#if 1
  int xm = X.rows();
  int xn = X.cols();
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Build reindexing maps for columns and rows, -1 means not in map
  std::vector<std::vector<typename DerivedR::Scalar>> RI;
  RI.resize(xm);
  for (int i = 0; i < ym; i++)
  {
    RI[R(i)].push_back(i);
  }
  std::vector<std::vector<typename DerivedC::Scalar>> CI;
  CI.resize(xn);
  // initialize to -1
  for (int i = 0; i < yn; i++)
  {
    CI[C(i)].push_back(i);
  }
  // Resize output
  using Triplet = Eigen::Triplet<TY>;
  std::vector<Triplet> entries;
  // Take a guess at the number of nonzeros (this assumes uniform distribution
  // not banded or heavily diagonal)
  entries.reserve((X.nonZeros()/(X.rows()*X.cols())) * (ym*yn));
  // Iterate over outside
  for (int k = 0; k < X.outerSize(); ++k)
  {
    // Iterate over inside
    for (typename Eigen::SparseMatrix<TX>::InnerIterator it(X, k); it; ++it)
    {
      typename std::vector<typename DerivedR::Scalar>::iterator rit;
      typename std::vector<typename DerivedC::Scalar>::iterator cit;
      for (rit = RI[it.row()].begin(); rit != RI[it.row()].end(); rit++)
      {
        for (cit = CI[it.col()].begin(); cit != CI[it.col()].end(); cit++)
        {
          entries.emplace_back(*rit, *cit, it.value());
        }
      }
    }
  }
  Y.resize(ym, yn);
  Y.setFromTriplets(entries.begin(), entries.end());
#else

  // Alec: This is _not_ valid for arbitrary R,C since they don't necessary
  // representation a strict permutation of the rows and columns: rows or
  // columns could be removed or replicated. The removal of rows seems to be
  // handled here (although it's not clear if there is a performance gain when
  // the #removals >> #remains). If this is sufficiently faster than the
  // correct code above, one could test whether all entries in R and C are
  // unique and apply the permutation version if appropriate.
  //

  int xm = X.rows();
  int xn = X.cols();
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // initialize row and col permutation vectors
  Eigen::VectorXi rowIndexVec = igl::LinSpaced<Eigen::VectorXi>(xm, 0, xm - 1);
  Eigen::VectorXi rowPermVec = igl::LinSpaced<Eigen::VectorXi>(xm, 0, xm - 1);
  for (int i = 0; i < ym; i++)
  {
    int pos = rowIndexVec.coeffRef(R(i));
    if (pos != i)
    {
      int &val = rowPermVec.coeffRef(i);
      std::swap(rowIndexVec.coeffRef(val), rowIndexVec.coeffRef(R(i)));
      std::swap(rowPermVec.coeffRef(i), rowPermVec.coeffRef(pos));
    }
  }
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic, int> rowPerm(rowIndexVec);

  Eigen::VectorXi colIndexVec = igl::LinSpaced<Eigen::VectorXi>(xn, 0, xn - 1);
  Eigen::VectorXi colPermVec = igl::LinSpaced<Eigen::VectorXi>(xn, 0, xn - 1);
  for (int i = 0; i < yn; i++)
  {
    int pos = colIndexVec.coeffRef(C(i));
    if (pos != i)
    {
      int &val = colPermVec.coeffRef(i);
      std::swap(colIndexVec.coeffRef(val), colIndexVec.coeffRef(C(i)));
      std::swap(colPermVec.coeffRef(i), colPermVec.coeffRef(pos));
    }
  }
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic, int> colPerm(colPermVec);

  Eigen::SparseMatrix<T> M = (rowPerm * X);
  Y = (M * colPerm).block(0, 0, ym, yn);
#endif
}

template <
    typename TX,
    typename TY,
    typename DerivedR,
    typename DerivedC>
IGL_INLINE void igl::slice(
    const Eigen::SparseMatrix<TX> &X,
    const Eigen::ArrayBase<DerivedR> &R,
    const Eigen::MatrixBase<DerivedC> &C,
    Eigen::SparseMatrix<TY> &Y)
{
  int xm = X.rows();
  int xn = X.cols();
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Build reindexing maps for columns and rows, -1 means not in map
  std::vector<std::vector<typename DerivedR::Scalar>> RI;
  RI.resize(xm);
  for (int i = 0; i < ym; i++)
  {
    RI[R(i)].push_back(i);
  }
  std::vector<std::vector<typename DerivedC::Scalar>> CI;
  CI.resize(xn);
  // initialize to -1
  for (int i = 0; i < yn; i++)
  {
    CI[C(i)].push_back(i);
  }
  // Resize output
  using Triplet = Eigen::Triplet<TY>;
  std::vector<Triplet> entries;
  // Take a guess at the number of nonzeros (this assumes uniform distribution
  // not banded or heavily diagonal)
  entries.reserve((X.nonZeros() / (X.rows() * X.cols())) * (ym * yn));
  // Iterate over outside
  for (int k = 0; k < X.outerSize(); ++k)
  {
    // Iterate over inside
    for (typename Eigen::SparseMatrix<TX>::InnerIterator it(X, k); it; ++it)
    {
      typename std::vector<typename DerivedR::Scalar>::iterator rit;
      typename std::vector<typename DerivedC::Scalar>::iterator cit;
      for (rit = RI[it.row()].begin(); rit != RI[it.row()].end(); rit++)
      {
        for (cit = CI[it.col()].begin(); cit != CI[it.col()].end(); cit++)
        {
          entries.emplace_back(*rit, *cit, it.value());
        }
      }
    }
  }
  Y.resize(ym, yn);
  Y.setFromTriplets(entries.begin(), entries.end());
}

template <
    typename TX,
    typename TY,
    typename DerivedR,
    typename DerivedC>
IGL_INLINE void igl::slice(
    const Eigen::SparseMatrix<TX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const Eigen::ArrayBase<DerivedC> &C,
    Eigen::SparseMatrix<TY> &Y)
{
  int xm = X.rows();
  int xn = X.cols();
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Build reindexing maps for columns and rows, -1 means not in map
  std::vector<std::vector<typename DerivedR::Scalar>> RI;
  RI.resize(xm);
  for (int i = 0; i < ym; i++)
  {
    RI[R(i)].push_back(i);
  }
  std::vector<std::vector<typename DerivedC::Scalar>> CI;
  CI.resize(xn);
  // initialize to -1
  for (int i = 0; i < yn; i++)
  {
    CI[C(i)].push_back(i);
  }
  // Resize output
  using Triplet = Eigen::Triplet<TY>;
  std::vector<Triplet> entries;
  // Take a guess at the number of nonzeros (this assumes uniform distribution
  // not banded or heavily diagonal)
  entries.reserve((X.nonZeros() / (X.rows() * X.cols())) * (ym * yn));
  // Iterate over outside
  for (int k = 0; k < X.outerSize(); ++k)
  {
    // Iterate over inside
    for (typename Eigen::SparseMatrix<TX>::InnerIterator it(X, k); it; ++it)
    {
      typename std::vector<typename DerivedR::Scalar>::iterator rit;
      typename std::vector<typename DerivedC::Scalar>::iterator cit;
      for (rit = RI[it.row()].begin(); rit != RI[it.row()].end(); rit++)
      {
        for (cit = CI[it.col()].begin(); cit != CI[it.col()].end(); cit++)
        {
          entries.emplace_back(*rit, *cit, it.value());
        }
      }
    }
  }
  Y.resize(ym, yn);
  Y.setFromTriplets(entries.begin(), entries.end());
}

template <typename MatX, typename DerivedR, typename MatY>
IGL_INLINE void igl::slice(
    const MatX &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const int dim,
    MatY &Y)
{
  Eigen::Matrix<typename DerivedR::Scalar, Eigen::Dynamic, 1> C;
  switch (dim)
  {
  case 1:
    // boring base case
    if (X.cols() == 0)
    {
      Y.resize(R.size(), 0);
      return;
    }
    igl::colon(0, X.cols() - 1, C);
    return slice(X, R, C, Y);
  case 2:
    // boring base case
    if (X.rows() == 0)
    {
      Y.resize(0, R.size());
      return;
    }
    igl::colon(0, X.rows() - 1, C);
    return slice(X, C, R, Y);
  default:
    assert(false && "Unsupported dimension");
    return;
  }
}

template <typename MatX, typename DerivedR, typename MatY>
IGL_INLINE void igl::slice(
    const MatX &X,
    const Eigen::ArrayBase<DerivedR> &R,
    const int dim,
    MatY &Y)
{
  Eigen::Matrix<typename DerivedR::Scalar, Eigen::Dynamic, 1> C;
  switch (dim)
  {
  case 1:
    // boring base case
    if (X.cols() == 0)
    {
      Y.resize(R.size(), 0);
      return;
    }
    igl::colon(0, X.cols() - 1, C);
    return slice(X, R, C, Y);
  case 2:
    // boring base case
    if (X.rows() == 0)
    {
      Y.resize(0, R.size());
      return;
    }
    igl::colon(0, X.rows() - 1, C);
    return slice(X, C, R, Y);
  default:
    assert(false && "Unsupported dimension");
    return;
  }
}

template <
    typename DerivedX,
    typename DerivedR,
    typename DerivedC,
    typename DerivedY>
IGL_INLINE void igl::slice(
    const Eigen::ArrayBase<DerivedX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const Eigen::MatrixBase<DerivedC> &C,
    Eigen::PlainObjectBase<DerivedY> &Y)
{
#ifndef NDEBUG
  int xm = X.rows();
  int xn = X.cols();
#endif
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Resize output
  Y.resize(ym, yn);
  // loop over output rows, then columns
  for (int i = 0; i < ym; i++)
  {
    for (int j = 0; j < yn; j++)
    {
      Y(i, j) = X(R(i, 0), C(j, 0));
    }
  }
}

template <
    typename DerivedX,
    typename DerivedR,
    typename DerivedC,
    typename DerivedY>
IGL_INLINE void igl::slice(
    const Eigen::MatrixBase<DerivedX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const Eigen::MatrixBase<DerivedC> &C,
    Eigen::PlainObjectBase<DerivedY> &Y)
{
#ifndef NDEBUG
  int xm = X.rows();
  int xn = X.cols();
#endif
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if (ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Resize output
  Y.resize(ym, yn);
  // loop over output rows, then columns
  for (int i = 0; i < ym; i++)
  {
    for (int j = 0; j < yn; j++)
    {
      Y(i, j) = X(R(i,0), C(j,0));
    }
  }
}

template <typename DerivedX, typename DerivedY, typename DerivedR>
IGL_INLINE void igl::slice(
    const Eigen::MatrixBase<DerivedX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    Eigen::PlainObjectBase<DerivedY> &Y)
{
  // phony column indices
  Eigen::Matrix<typename DerivedR::Scalar, Eigen::Dynamic, 1> C;
  C.resize(1);
  C(0) = 0;
  return igl::slice(X, R, C, Y);
}

template <typename DerivedX, typename DerivedR>
IGL_INLINE DerivedX igl::slice(
    const Eigen::MatrixBase<DerivedX> &X,
    const Eigen::MatrixBase<DerivedR> &R)
{
  DerivedX Y;
  igl::slice(X, R, Y);
  return Y;
}

template <typename DerivedX, typename DerivedR>
IGL_INLINE DerivedX igl::slice(
    const Eigen::MatrixBase<DerivedX> &X,
    const Eigen::MatrixBase<DerivedR> &R,
    const int dim)
{
  DerivedX Y;
  igl::slice(X, R, dim, Y);
  return Y;
}

#ifdef IGL_STATIC_LIBRARY
template void igl::slice<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>>(Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::Matrix<double, -1, -1, 0, -1, -1> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::Matrix<double, -1, -1, 0, -1, -1> &);
template void igl::slice<Eigen::SparseMatrix<double, 0, int>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::SparseMatrix<double, 0, int>>(Eigen::SparseMatrix<double, 0, int> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::SparseMatrix<double, 0, int> &);
template void igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::Matrix<double, -1, -1, 0, -1, -1> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::Matrix<double, -1, -1, 0, -1, -1> &);
template void igl::slice<Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::SparseMatrix<double, 0, int>, Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::SparseMatrix<double, 0, int>>(Eigen::SparseMatrix<double, 0, int> const &, Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1>> const &, int, Eigen::SparseMatrix<double, 0, int> &);
template void igl::slice<Eigen::SparseMatrix<double, 0, int>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::SparseMatrix<double, 0, int>>(Eigen::SparseMatrix<double, 0, int> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, int, Eigen::SparseMatrix<double, 0, int> &);
template void igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>>(Eigen::Matrix<double, -1, -1, 0, -1, -1> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, -1, 1, true>>(Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Block<Eigen::Matrix<int, -1, -1, 0, -1, -1> const, -1, 1, true>> const &, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> &);
template Eigen::Matrix<double, -1, -1, 0, -1, -1> igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3>> &);
template void igl::slice<Eigen::Matrix<float, -1, 1, 0, -1, 1>, Eigen::Matrix<float, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>, Eigen::Matrix<long, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<long, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 0, -1, 3>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3>>>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 0, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3>> &);
template void igl::slice<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, 3, 1, -1, 3>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 1, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3>> &);
template void igl::slice<Eigen::SparseMatrix<bool, 0, int>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::SparseMatrix<bool, 0, int>>(Eigen::SparseMatrix<bool, 0, int> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::SparseMatrix<bool, 0, int> &);
template Eigen::Matrix<int, -1, -1, 0, -1, -1> igl::slice<Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &);
template void igl::slice<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, 3, 1, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<double, -1, -1, 1, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 1, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 1, -1, -1>> &);
template void igl::slice<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<float, -1, 3, 1, -1, 3>>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3>> &);
template void igl::slice<Eigen::Matrix<float, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<float, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 0, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<float, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>>(Eigen::Matrix<int, -1, 1, 0, -1, 1> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::Matrix<int, -1, 1, 0, -1, 1> &);
template void igl::slice<Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<long, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<long, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>>>(Eigen::Matrix<int, -1, 1, 0, -1, 1> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::Matrix<long, -1, 1, 0, -1, 1>, Eigen::Matrix<long, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<long, -1, 1, 0, -1, 1>>>(Eigen::Matrix<long, -1, 1, 0, -1, 1> const &, Eigen::MatrixBase<Eigen::Matrix<long, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<long, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::Matrix<int, -1, -1, 0, -1, -1> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
#ifdef WIN32
template void igl::slice<Eigen::Matrix<__int64, -1, 1, 0, -1, 1>, Eigen::Matrix<__int64, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<__int64, -1, 1, 0, -1, 1>>>(Eigen::Matrix<__int64, -1, 1, 0, -1, 1> const &, Eigen::MatrixBase<Eigen::Matrix<__int64, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<__int64, -1, 1, 0, -1, 1>> &);
template void igl::slice<Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<__int64, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<__int64, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<class Eigen::MatrixBase<class Eigen::Matrix<int,-1,-1,0,-1,-1> >,class Eigen::Matrix<__int64,-1,1,0,-1,1>,class Eigen::PlainObjectBase<class Eigen::Matrix<int,-1,-1,0,-1,-1> > >(class Eigen::MatrixBase<class Eigen::Matrix<int,-1,-1,0,-1,-1> > const &,class Eigen::MatrixBase<class Eigen::Matrix<__int64,-1,1,0,-1,1> > const &,int,class Eigen::PlainObjectBase<class Eigen::Matrix<int,-1,-1,0,-1,-1> > &);
#endif
#if EIGEN_VERSION_AT_LEAST(3, 3, 7)
#else
template void igl::slice<Eigen::MatrixBase<Eigen::CwiseUnaryOp<Eigen::internal::scalar_multiple_op<double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::CwiseUnaryOp<Eigen::internal::scalar_multiple_op<double>, Eigen::Matrix<double, -1, 1, 0, -1, 1> const>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::Matrix<double, -1, -1, 0, -1, -1> &);
#endif
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, int, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::slice<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3>>>(Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> >, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > >(Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> > const&, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> >&);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >, Eigen::Matrix<long, -1, 1, 0, -1, 1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > >(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<long, -1, 1, 0, -1, 1> > const&, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>>>(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> const &, int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1>> &);
template void igl::slice<Eigen::SparseMatrix<double, 0, int>, Eigen::Array<int, -1, 1, 0, -1, 1>, Eigen::SparseMatrix<double, 0, int> >(Eigen::SparseMatrix<double, 0, int> const&, Eigen::ArrayBase<Eigen::Array<int, -1, 1, 0, -1, 1> > const&, int, Eigen::SparseMatrix<double, 0, int>&);
template void igl::slice<Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, int, Eigen::Matrix<double, -1, -1, 0, -1, -1>&);
#endif
