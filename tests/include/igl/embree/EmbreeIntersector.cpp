#include <test_common.h>
#include <igl/embree/EmbreeIntersector.h>

TEST(EmbreeIntersector, cube)
{
  //The allowed error for this test
  const double epsilon = 1e-6;

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  // This is a cube of dimensions 1.0x1.0x1.0
  test_common::load_mesh("cube.obj", V, F);

  // Initialize embree
  igl::embree::EmbreeIntersector embree;
  embree.init(V.cast<float>(),F.cast<int>());

  const int expected_id[] = {4,8,5,2,7,0};
  const float expected_u[] = {0.5,0.5,0.5,0.5,0.5,0.5};
  const float expected_v[] = {0.5,0.0,0.0,0.0,0.5,0.0};

  // Shoot ray from inside out
  for (int dim=0; dim<6; ++dim)
  {
    Eigen::Vector3d pos(0,0,0);
    Eigen::Vector3d dir(0,0,0);
    // test each dimension, pos and neg
    dir[dim/2] = dim%2 ? -1 : 1;
    igl::Hit hit;
    bool hitP = embree.intersectRay(pos.cast<float>(), dir.cast<float>(), hit);
    ASSERT_TRUE(hitP);
    ASSERT_NEAR(hit.t, 0.5, epsilon);
    EXPECT_EQ(hit.id, expected_id[dim]);
    EXPECT_NEAR(hit.u, expected_u[dim], epsilon);
    EXPECT_NEAR(hit.v, expected_v[dim], epsilon);
  }

  // Shoot ray from outside in
  for (int dim=0; dim<6; ++dim)
  {
    Eigen::Vector3d dir(0,0,0);
    // test each dimension, pos and neg
    dir[dim/2] = dim%2 ? 1 : -1;

    Eigen::Vector3d pos = -dir;

    igl::Hit hit;
    bool hitP = embree.intersectRay(pos.cast<float>(), dir.cast<float>(), hit);
    ASSERT_TRUE(hitP);
    EXPECT_NEAR(hit.t, 0.5, epsilon);
    EXPECT_EQ(hit.id, expected_id[dim]);
    EXPECT_NEAR(hit.u, expected_u[dim], epsilon); 
    EXPECT_NEAR(hit.v, expected_v[dim], epsilon);
  }

  // Rays that miss
  for (int dim=0; dim<6; ++dim)
  {
    Eigen::Vector3d pos(0,0,0);
    Eigen::Vector3d dir(0,0,0);
    // test each dimension, pos and neg
    dir[dim/2] = dim%2 ? -1 : 1;
    pos[(dim/2+1)%3] = dir[dim/2];

    igl::Hit hit;
    bool hitP = embree.intersectRay(pos.cast<float>(), dir.cast<float>(), hit);
    ASSERT_FALSE(hitP);
  }
}

