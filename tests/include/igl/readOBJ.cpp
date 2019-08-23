#include <test_common.h>
#include <iostream>
#include <string>
#include <tuple>

TEST_CASE("readOBJ: simple", "[igl]")
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    // wait... so this is actually testing test_common::load_mesh not readOBJ
    // directly...
    test_common::load_mesh("cube.obj", V, F);
    REQUIRE (V.rows() == 8);
    REQUIRE (F.rows() == 12);
}

TEST_CASE("readOBJ: Obj with material", "[igl]")
{
    std::vector<std::vector<double > > V;
    std::vector<std::vector<double > > TC;
    std::vector<std::vector<double > > N;
    std::vector<std::vector<int > > F;
    std::vector<std::vector<int > > FTC;
    std::vector<std::vector<int > >  FN;
    std::vector<std::tuple<std::vector<int>, std::vector<int>, std::vector<int>, std::string> > FM;
    test_common::load_obj_with_material<double, int>("objwithmaterial.obj", V, TC, N, F, FTC, FN, FM);
    REQUIRE (V.size() == 8);
    REQUIRE (F.size() == 6);
    for ( const auto& i : FM ) {
        std::cout << "vertex ";
        for (const auto& v : std::get<0>(i))
            std::cout << v << ' ';
        std::cout << "face ";
        for (const auto& v : std::get<1>(i))
            std::cout << v << ' ';
        std::cout << "normal ";
        for (const auto& v : std::get<2>(i))
            std::cout << v << ' ';
        std::cout << "material ";
        for (const auto& v : std::get<3>(i))
            std::cout << v << ' ';
        std::cout << std::endl;
    }
    REQUIRE (FM.size() == 6);

}
