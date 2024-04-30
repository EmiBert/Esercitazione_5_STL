#pragma once

#include <iostream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;


namespace PolygonalLibrary {
struct PolygonalMesh
{
    unsigned int NumberCell0D = 0; ///< number of Cell0D
    vector<unsigned int> Cell0DId = {}; ///< Cell0D id, size 1 x NumberCell0D
    vector<Vector2d> Cell0DCoordinates = {}; ///< Cell0D coordinates, size 2 x NumberCell0D (x,y)
    map<unsigned int, list<unsigned int>> Cell0DMarkers = {}; ///< Cell0D markers, size 1 x NumberCell0D (marker)

    unsigned int NumberCell1D = 0; ///< number of Cell1D
    vector<unsigned int> Cell1DId = {}; ///< Cell1D id, size 1 x NumberCell1D
    vector<Vector2i> Cell1DVertices = {}; ///< Cell1D vertices indices, size 2 x NumberCell1D (fromId,toId)
    map<unsigned int, list<unsigned int>> Cell1DMarkers = {}; ///< Cell1D marker, size 1 x NumberCell1D (marker)

    unsigned int NumberCell2D = 0; ///< number of Cell2D
    map<unsigned int, array<unsigned int,2> > Cell2DId = {}; ///< Cell2D {id,{dimensione, marker}}; size 2 x NumberCell2D
    /// note: dimensione = Cell2D[id][0] = NumVertcies = NumEdges (when they are equal, else dimensione = 0)
    ///< Cell2D Vertices indices, size NumberCell2D x Cell2D[id][0] (dynamic, not a table with fixed dimension)
    vector<vector<unsigned int>> Cell2DVertices = {};
    ///< Cell2D Edges indices, size NumberCell2D x Cell2D[id][0] (dynamic, not a table with fixed dimension)
    vector<vector<unsigned int>> Cell2DEdges = {};
};
}

