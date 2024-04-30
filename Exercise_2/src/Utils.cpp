#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


namespace PolygonalLibrary {

bool ImportMesh(const string& filepath,
                PolygonalMesh& mesh,
                map<unsigned int, double>& lenghtEdges,
                map<unsigned int, double>& areaCell)
{

    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv", mesh))
    {
        cerr<<"errore apertura file "<<filepath<<"/Cell0Ds.csv"<<endl;
        return false;
    }
    else
    {
        // controlla che i nodi in input siano ordinati per id
        // (e quindi coerenti con le operazioni seguenti)
        unsigned int expectedId = 0;
        for (auto it = mesh.Cell0DId.begin(); it != mesh.Cell0DId.end(); it++)
        {
            if (expectedId != *it)
            {
                cerr<<"I nodi non sono ordinati"<<endl;
                cerr<<"prima anomalia riscontrata in posione "<<expectedId<<endl;
                return false;
            }
            else
                expectedId++;
        }

        // stampa i nodi con marker non nullo
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv", mesh))
    {
        cerr<<"errore apertura file "<<filepath<<"/Cell1Ds.csv"<<endl;
        return false;
    }
    else
    {
        // controllo che i lati in input siano ordinati per id
        // (e quindi coerenti con le operazioni seguenti)
        unsigned int expectedId = 0;
        for (auto it = mesh.Cell1DId.begin(); it != mesh.Cell1DId.end(); it++)
        {
            if (expectedId != *it)
            {
                cerr<<"I lati non sono ordinati"<<endl;
                cerr<<"prima anomalia riscontrata in posione "<<expectedId<<endl;
                return false;
            }
            else
                expectedId++;
        }


        // controllo che non ci siano lati degeneri (lunghezza nulla)
        bool flag = false;
        for (auto it = mesh.Cell1DId.begin(); it != mesh.Cell1DId.end(); it++)
        {
            int idOrigin = mesh.Cell1DVertices[*it][0];
            int idEnd    = mesh.Cell1DVertices[*it][1];
            double x1 = mesh.Cell0DCoordinates[idOrigin][0];
            double y1 = mesh.Cell0DCoordinates[idOrigin][1];
            double x2 = mesh.Cell0DCoordinates[idEnd][0];
            double y2 = mesh.Cell0DCoordinates[idEnd][1];
            // calcolo della lunghezza del lato
            double lenght = sqrt(pow(x1-x2,2) + pow(y1-y2,2));

            if(lenght == 0)
            {
                flag = true;
                cerr<<"il lato "<<*it<<" ha lunghezza nulla"<<endl;
            }
            else
                lenghtEdges.insert({*it, lenght});
        }

        if(flag)
            return false;

        // stampa i lati con marker non nullo
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv", mesh))
    {
        return false;
    }

    else
    {
        // controllo che le celle in input siano ordinati per id
        // (e quindi coerenti con le operazioni seguenti)
        unsigned int expectedId = 0;
        for (auto it = mesh.Cell2DId.begin(); it != mesh.Cell2DId.end(); it++)
        {
            if (expectedId != it->first)
            {
                cerr<<"I lati non sono ordinati"<<endl;
                cerr<<"prima anomalia riscontrata in posione "<<expectedId<<endl;
                return false;
            }
            else
                expectedId++;
        }


        bool flag = false;
        // controllo che per ogni cella le informazioni sui lati e vertici siano coerenti
        // (ovvero che gli estremi di ciascun lato della cella
        // appartengano al set di vertici della cella stessa)
        for(auto it = mesh.Cell2DId.begin(); it != mesh.Cell2DId.end(); it++)
        {
            unsigned int dimensione = it->second[0];
            vector<unsigned int> edges = {};
            vector<unsigned int> vertices = {};
            edges.reserve(dimensione);
            vertices.reserve(dimensione);
            unsigned int id = it->first;
            edges = mesh.Cell2DEdges[id];
            vertices = mesh.Cell2DVertices[id];


            for(unsigned int e = 0; e < dimensione; e++)
            {
                unsigned int origin = mesh.Cell1DVertices[edges[e]][0];
                unsigned int end    = mesh.Cell1DVertices[edges[e]][1];

                auto findOrigin = find(vertices.begin(), vertices.end(), origin);
                auto findEnd = find(vertices.begin(), vertices.end(), end);

                if(findOrigin == vertices.end() or findEnd == vertices.end())
                {
                    cerr << "Mesh sbagliata" << endl;
                    cerr << "La cella "<<id<<" contiene un lato non coerente"<<endl;
                    return false;
                }
            }
            // calcola l'area delle celle
            // vengo fatte due assunzioni sui dati:
            // 1) le celle sono convesse
            // 2) i nodi sono inseriti in ordine:
            // due nodi consecutivi sono connessi da un lato
            // così come il primo e l'ultimo
            // Il poligono viene diviso in NumVertices-2 triangoli
            // tenendo fisso un vertice e facendo variare ordinatamente gli altri due
            // Esempio: per un pentagono i nodi saranno chiamati 0,1,2,3,4
            // ed i triangoli in cui verrà suddiviso saranno:
            // (0,1,2) , (0,2,3) e (0,3,4)
            double area = 0;
            double x0 = mesh.Cell0DCoordinates[vertices[0]][0];
            double y0 = mesh.Cell0DCoordinates[vertices[0]][1];

            for (unsigned int v = 1; v<dimensione-1; v++)
            {
                double x1 = mesh.Cell0DCoordinates[vertices[v]][0]- x0;
                double y1 = mesh.Cell0DCoordinates[vertices[v]][1]- y0;
                double x2 = mesh.Cell0DCoordinates[vertices[v+1]][0]- x0;
                double y2 = mesh.Cell0DCoordinates[vertices[v+1]][1]- y0;
                // per ogni trinagolo l'area viene calcolata tramite il metodo
                // del prodotto vettoriale tra due lati (opportunamente riscalati
                // in modo che l'origine del nuovo sistema di riferimento sia
                // il loro vertice comune)
                area += 0.5*abs(x1*y2-x2*y1);
            }

            if (area != 0)
            {
                areaCell.insert({id, area});
            }
            else
            {
                flag = true;
                cerr<<"La cella "<<id<<" e' degenere (ha area nulla)"<<endl;
            }
        }

        if(flag)
            return false;

    }

    return true;

}
// ***************************************************************************
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);
    bool flag = false;

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;
        string x;
        getline(converter, x, ';');
        id = stoi(x);
        getline(converter, x, ';');
        marker = stoi(x);
        getline(converter, x, ';');
        coord(0) = stod(x);
        getline(converter, x);
        coord(1) = stod(x);


        if (coord(0)>=0 and coord(0)<=1 and coord(1)>=0 and coord(1)<=1)
        {
            mesh.Cell0DId.push_back(id);
            mesh.Cell0DCoordinates.push_back(coord);
        }
        else
        {
            flag = true;
            cerr<<"il punto "<<id<<" si trova al di fuori dell'area [0,1]x[0,1]"<<endl;
        }


        if( marker != 0)
        {
            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
                mesh.Cell0DMarkers.insert({marker, {id}});
            else
                mesh.Cell0DMarkers[marker].push_back(id);
       }

    }

    if(flag)
        return false;

    file.close();
    return true;
}
// ***************************************************************************
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
        string x;
        getline(converter, x, ';');
        id = stoi(x);
        getline(converter, x, ';');
        marker = stoi(x);
        getline(converter, x, ';');
        vertices(0) = stod(x);
        getline(converter, x);
        vertices(1) = stod(x);


        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            if (mesh.Cell1DMarkers.find(marker) == mesh.Cell1DMarkers.end())
                mesh.Cell1DMarkers.insert({marker, {id}});
            else
                mesh.Cell1DMarkers[marker].push_back(id);
        }
    }

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);
    file.close();

    listLines.pop_front();
    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    //mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);
    bool flag = false;

    for (const string& line : listLines)
    {
        istringstream converter(line);
        unsigned int id;
        unsigned int marker;
        unsigned int NumVertices;
        unsigned int NumEdges;
        string x;
        getline(converter, x, ';');
        id = stoi(x);
        getline(converter, x, ';');
        marker = stoi(x);

        getline(converter, x, ';');
        NumVertices = stoi(x);

        vector<unsigned int> Vertices = {};
        Vertices.reserve(NumVertices);
        for(int i=0; i<NumVertices; i++)
        {
            getline(converter, x, ';');
            Vertices.push_back(stoi(x));
        }


        getline(converter, x, ';');
        NumEdges = stoi(x);

        vector<unsigned int> Edges = {};
        Edges.reserve(NumEdges);
        for(int i=0; i<NumEdges; i++)
        {
            getline(converter, x, ';');
            Edges.push_back(stoi(x));
        }

        mesh.Cell2DVertices.push_back(Vertices);
        mesh.Cell2DEdges.push_back(Edges);



        if (NumVertices == NumEdges)
            mesh.Cell2DId.insert({id, {NumVertices, marker}});
        else
            // se il numero di vertici è diverso dal numero di
            // lati il poligono non può esistere.
            // Viene impostata come dimensione zero
        {
            flag = true;
            mesh.Cell2DId.insert({id, {0, marker}}); // la dimensione pari a zero indica un errore
            cerr<<"Il poligono "<<id<<" ha il numero di vertici diverso da quello dei lati"<<endl;
        }
    }


    if (flag)
        return false;


    return true;
}

}
