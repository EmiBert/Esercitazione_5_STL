#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;


int main()
{   PolygonalMesh mesh;
    map<unsigned int, double> lenghtEdges = {};
    map<unsigned int, double> areaCell = {};
    string filepath = "PolygonalMesh";
    if(!ImportMesh(filepath,
                   mesh,
                   lenghtEdges,
                   areaCell))
    {
        return 1;
    }

  return 0;
}
