#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary{

// funzione che legge i dati da file, esegue controlli su tali dati e
// stampa in output tutti i nodi e tutti i lati con valore di marker
// diverso da zero, memorizza per ogni lato ed ogni cella il valore rispettivamente
// di lunghezza e area su mappe opportune, indicizzando per id
//
// controlla che i file si aprano correttamente
// controlla che i nodi, i lati, le celle siano disposti nel file con ordine di id
// crescete con passo pari a 1 (0,1,2,3,...)
// controlla che non ci siano lati degeneri (con lunghezza nulla)
// controlla che non ci siano cell degeneri (con area nulla)
// controllo che per ogni cella le informazioni sui lati e vertici siano coerenti
// (ovvero che gli estremi di ciascun lato della cella
// appartengano al set di vertici della cella stessa)
//
// filepath = nome della cartella contenete i file richiesti
// mesh = struttura di tipo PolygonalMesh su cui verranno memorizzati i dati
// lenghtEdges = mappa su cui vengono memrizzate le lunghezze dai lati, ordinate per id
// areaCell = mappa su cui vengono memrizzate le aree delle celle, ordinate per id
//
// return = true se il processo va a buon fine e non si incontrano errori, false altrimenti
bool ImportMesh(const string &filepath,
                PolygonalMesh& mesh,
                map<unsigned int, double>& lenghtEdges,
                map<unsigned int, double>& areaCell);


// funzione che legge dal file di input i dati dei nodi e li memorizza su mesh
// controlla che il file contenente i nodi sia non vuoto
// controlla che i nodi appartengano all'insieme [0,1]x[0,1]
// filename = nome della cartella contenete i file richiesti
// mesh = struttura di tipo PolygonalMesh su cui verranno memorizzati i dati
// return = true se il processo va a buon fine e non si incontrano errori, false altrimenti
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh);



// funzione che legge dal file di input i dati dei lati e li memorizza su mesh
// controlla che il file contenente i lati sia non vuoto
// filename = nome della cartella contenete i file richiesti
// mesh = struttura di tipo PolygonalMesh su cui verranno memorizzati i dati
// return = true se il processo va a buon fine e non si incontrano errori, false altrimenti
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh);


// funzione che legge dal file di input i dati dei lati e li memorizza su mesh
// controlla che il file contenente le celle sia non vuoto
// controlla che per ogni cella il numero dei vertici sia uguale al numero dai lati
// filename = nome della cartella contenete i file richiesti
// mesh = struttura di tipo PolygonalMesh su cui verranno memorizzati i dati
// return = true se il processo va a buon fine e non si incontrano errori, false altrimenti
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh);

}
