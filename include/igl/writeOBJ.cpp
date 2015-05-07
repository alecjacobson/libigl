// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "writeOBJ.h"

#include "matrix_to_list.h"

#include <iostream>
#include <limits>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cassert>

template <typename DerivedV, typename DerivedF, typename DerivedT>
IGL_INLINE bool igl::writeOBJ(
  const std::string str,
  const Eigen::PlainObjectBase<DerivedV>& V,
  const Eigen::PlainObjectBase<DerivedF>& F,
  const Eigen::PlainObjectBase<DerivedV>& CN,
  const Eigen::PlainObjectBase<DerivedF>& FN,
  const Eigen::PlainObjectBase<DerivedT>& TC,
  const Eigen::PlainObjectBase<DerivedF>& FTC)
{
  std::vector<std::vector< typename DerivedF::Scalar > > vF;
  std::vector<std::vector< typename DerivedF::Scalar > > vFN;
  std::vector<std::vector< typename DerivedF::Scalar > > vFTC;

  igl::matrix_to_list(F,vF);
  igl::matrix_to_list(FN,vFN);
  igl::matrix_to_list(FTC,vFTC);

  return igl::writeOBJPoly(str, V, vF, CN, vFN, TC, vFTC);
}

template <typename DerivedV, typename DerivedF>
IGL_INLINE bool igl::writeOBJ(
  const std::string str,
  const Eigen::PlainObjectBase<DerivedV>& V,
  const Eigen::PlainObjectBase<DerivedF>& F)
{
  using namespace std;
  using namespace Eigen;
  assert(V.cols() == 3 && "V should have 3 columns");
  ofstream s(str);
  if(!s.is_open())
  {
    fprintf(stderr,"IOError: writeOBJ() could not open %s\n",str.c_str());
    return false;
  }
  s<<
    V.format(IOFormat(FullPrecision,DontAlignCols," ","\n","v ","","",""))<<
    (F.array()+1).format(IOFormat(FullPrecision,DontAlignCols," ","\n","f ","","",""));
  return true;
}

template <typename DerivedV, typename DerivedF, typename DerivedT, typename Index>
IGL_INLINE bool igl::writeOBJPoly(
        const std::string str,
        const Eigen::PlainObjectBase<DerivedV>& V,
        const std::vector<std::vector< Index > >& F,
        const Eigen::PlainObjectBase<DerivedV>& CN,
        const Eigen::PlainObjectBase<DerivedF>& FN,
        const Eigen::PlainObjectBase<DerivedT>& TC,
        const Eigen::PlainObjectBase<DerivedF>& FTC)
{
  std::vector<std::vector< Index > > vFN; //typename DerivedF::Scalar
  std::vector<std::vector< Index > > vFTC; //typename DerivedF::Scalar

  igl::matrix_to_list(FN,vFN);
  igl::matrix_to_list(FTC,vFTC);

  return igl::writeOBJPoly(str, V, F, CN, vFN, TC, vFTC);
}

template <typename DerivedV, typename DerivedT, typename Index>
IGL_INLINE bool igl::writeOBJPoly(
        const std::string str,
        const Eigen::PlainObjectBase<DerivedV>& V,
        const std::vector<std::vector< Index > >& F,
        const Eigen::PlainObjectBase<DerivedV>& CN,
        const std::vector<std::vector< Index > >& FN,
        const Eigen::PlainObjectBase<DerivedT>& TC,
        const std::vector<std::vector< Index > >& FTC)
{
  FILE * obj_file = fopen(str.c_str(),"w");
  if(NULL==obj_file)
  {
    printf("IOError: %s could not be opened for writing...",str.c_str());
    return false;
  }
  // Loop over V
  for(int i = 0;i<(int)V.rows();i++)
  {
    fprintf(obj_file,"v %0.15g %0.15g %0.15g\n",
      V(i,0),
      V(i,1),
      V(i,2)
      );
  }
  bool write_N = CN.rows() >0;

  if(write_N)
  {
    for(int i = 0;i<(int)CN.rows();i++)
    {
      fprintf(obj_file,"v %0.15g %0.15g %0.15g\n",
              CN(i,0),
              CN(i,1),
              CN(i,2)
              );
    }
    fprintf(obj_file,"\n");
  }

  bool write_texture_coords = TC.rows() >0;

  if(write_texture_coords)
  {
    for(int i = 0;i<(int)TC.rows();i++)
    {
      fprintf(obj_file, "vt %0.15g %0.15g\n",TC(i,0),TC(i,1));
    }
    fprintf(obj_file,"\n");
  }

  // loop over F
  for(int i = 0;i<(int)F.size();++i) //rows()
  {
    fprintf(obj_file,"f");
    for(int j = 0; j<(int)F[i].size();++j) //cols()
    {
      // OBJ is 1-indexed
      fprintf(obj_file," %u",Index(F[i][j])+1);

      if(write_texture_coords)
        fprintf(obj_file,"/%u",Index(FTC[i][j])+1);
      if(write_N)
      {
        if (write_texture_coords)
          fprintf(obj_file,"/%u",Index(FN[i][j])+1);
        else
          fprintf(obj_file,"//%u",Index(FN[i][j])+1);
      }
    }
    fprintf(obj_file,"\n");
  }
  fclose(obj_file);
  return true;
}
template <typename DerivedV, typename Index>
IGL_INLINE bool igl::writeOBJPoly(
        const std::string str,
        const Eigen::PlainObjectBase<DerivedV>& V,
        const std::vector<std::vector< Index > >& F)
{
  using namespace std;
  using namespace Eigen;
  assert(V.cols() == 3 && "V should have 3 columns");
  ofstream s(str);
  //s.precision(std::numeric_limits<double>::digits10 + 1);
  if(!s.is_open())
  {
    fprintf(stderr,"IOError: writeOBJ() could not open %s\n",str.c_str());
    return false;
  }
  s << V.format(IOFormat(FullPrecision,DontAlignCols," ","\n","v ","","",""));
  for(int i=0;i<(int)F.size();++i) { //rows
    s << "f";
    for(int c =0;c<(int)F[i].size();++c) { //cols
      s << " " << Index(F[i][c])+1;
    }
    s << std::endl;
  }
  s.close();
  return true;
}
#ifdef IGL_STATIC_LIBRARY
// Explicit template specialization
// generated by autoexplicit.sh
template bool igl::writeOBJ<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&);
template bool igl::writeOBJ<Eigen::Matrix<double, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1>, Eigen::Matrix<double, -1, 2, 1, -1, 2> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 2, 1, -1, 2> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&);
template bool igl::writeOBJ<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1>, Eigen::Matrix<float, -1, 2, 1, -1, 2> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 2, 1, -1, 2> > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> > const&);
template bool igl::writeOBJ<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&);
template bool igl::writeOBJ<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, 2, 0, -1, 2> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 2, 0, -1, 2> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&);
template bool igl::writeOBJ<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 3, 0, -1, 3> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&);
template bool igl::writeOBJ<Eigen::Matrix<double, -1, 3, 0, -1, 3>, Eigen::Matrix<int, -1, 3, 0, -1, 3>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> > const&);
// this needs to be generated properly
//template bool igl::writeOBJPoly<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, int >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&);
//template bool igl::writeOBJPoly<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, int >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&,Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&,Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&);
//template bool igl::writeOBJPoly<Eigen::Matrix<double, -1, -1, 0, -1, -1>, int >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&);
//template bool igl::writeOBJPoly<Eigen::Matrix<double, -1, 3, 0, -1, 3>, int >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> > const&, std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&);
#endif
