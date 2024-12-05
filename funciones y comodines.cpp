#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cstdlib> // Para srand() y rand()
#include<ctime>   // Para time()
#include<algorithm>
#include<random>

using namespace std;

// Estructura para almacenar las respuestas y si es correcta
struct Respuesta {
    string respuesta;
    bool respuestaCorrecta;
};

// Estructura para almacenar una pregunta y sus respuestas
struct Pregunta {
    string fase;
    string pregunta;
    Respuesta respuesta1;
    Respuesta respuesta2; 
    Respuesta respuesta3;
    Respuesta respuesta4;
};

// Funci�n para leer el CSV y devolver una pregunta aleatoria
Pregunta buscarPreguntaAleatoria(const string& nombreArchivo, int faseActual) {
    vector<Pregunta> preguntas;
    string linea;
    int indiceAleatorio;

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        return {};
    }

    // Linea para excluir los t�tulos de las columnas
    getline(archivo, linea);

    // Leer el resto de las l�neas del archivo
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string fase, pregunta, respuesta1, respuesta2, respuesta3, respuesta4;

        bool correcta1 = false;
        bool correcta2 = false;
        bool correcta3 = false;
        bool correcta4 = true;

        // Separar los valores por comas
        getline(ss, fase, ',');
        getline(ss, pregunta, ',');
        getline(ss, respuesta1, ',');
        getline(ss, respuesta2, ',');
        getline(ss, respuesta3, ',');
        getline(ss, respuesta4, ',');

        // Guardar en la estructura y a�adir al vector
        preguntas.push_back({fase, pregunta, {respuesta1, correcta1}, {respuesta2, correcta2}, {respuesta3, correcta3}, {respuesta4, correcta4}});
    }
    archivo.close();

    // Verificar que haya preguntas en el archivo
    if (preguntas.empty()) {
        cout << "El archivo no contiene preguntas v�lidas." << endl;
        return {};
    }

    // Semilla para n�meros aleatorios
    srand(double(time(NULL)));

    // Elegir pregunta de la fase actual
    do {
        indiceAleatorio = rand() % preguntas.size();
        if (stoi(preguntas[indiceAleatorio].fase) == faseActual) {
            return preguntas[indiceAleatorio];
        }
    } while (indiceAleatorio != faseActual);
    
    // Si no se retorna nada con el c�digo de arriba
    cout << "No hay preguntas para la fase " << faseActual << endl;
    return {};
}

// Funcion para imprimir las respuestas de forma aleatoria
void mezclarRespuestas(Pregunta& pregunta) {
    vector<Respuesta> respuestasMezclar;

    // Agregar respuestas al vector
    respuestasMezclar.push_back(pregunta.respuesta1);
    respuestasMezclar.push_back(pregunta.respuesta2);
    respuestasMezclar.push_back(pregunta.respuesta3);
    respuestasMezclar.push_back(pregunta.respuesta4);

    random_device rd;
    mt19937 g(rd());

    // Mezclar las respuestas
    shuffle(respuestasMezclar.begin(), respuestasMezclar.end(), g);

    // Agregar las respuestas desordenadas de nuevo a la estructura
    pregunta.respuesta1 = respuestasMezclar[0];
    pregunta.respuesta2 = respuestasMezclar[1];
    pregunta.respuesta3 = respuestasMezclar[2];
    pregunta.respuesta4 = respuestasMezclar[3];
}

// Funcion para eliminar dos respuestas incorrectas
void eliminarDosRespuestas(Pregunta& pregunta) {
    vector<int> indicesIncorrectos;
    if (!pregunta.respuesta1.respuestaCorrecta) indicesIncorrectos.push_back(1);
    if (!pregunta.respuesta2.respuestaCorrecta) indicesIncorrectos.push_back(2);
    if (!pregunta.respuesta3.respuestaCorrecta) indicesIncorrectos.push_back(3);
    if (!pregunta.respuesta4.respuestaCorrecta) indicesIncorrectos.push_back(4);

    // Barajar los �ndices y eliminar dos
    random_device rd;
    mt19937 g(rd());
    shuffle(indicesIncorrectos.begin(), indicesIncorrectos.end(), g);

    // Marcar como "eliminadas" las respuestas incorrectas
    int aEliminar1 = indicesIncorrectos[0];
    int aEliminar2 = indicesIncorrectos[1];

    switch (aEliminar1) {
        case 1: pregunta.respuesta1.respuesta = "ELIMINADA"; break;
        case 2: pregunta.respuesta2.respuesta = "ELIMINADA"; break;
        case 3: pregunta.respuesta3.respuesta = "ELIMINADA"; break;
        case 4: pregunta.respuesta4.respuesta = "ELIMINADA"; break;
    }
    switch (aEliminar2) {
        case 1: pregunta.respuesta1.respuesta = "ELIMINADA"; break;
        case 2: pregunta.respuesta2.respuesta = "ELIMINADA"; break;
        case 3: pregunta.respuesta3.respuesta = "ELIMINADA"; break;
        case 4: pregunta.respuesta4.respuesta = "ELIMINADA"; break;
    }
}

// Funci�n para cambiar la pregunta
Pregunta cambiarPregunta(const string& nombreArchivo, int faseActual, Pregunta preguntaActual) {
    Pregunta preguntaNueva = {};
    do {
        preguntaNueva = buscarPreguntaAleatoria(nombreArchivo, faseActual);
    } while (preguntaNueva.pregunta == preguntaActual.pregunta);

    if (preguntaNueva.pregunta.empty()) {
        cout << "No se pudo encontrar una nueva pregunta diferente." << endl;
        return preguntaActual; // Puedes decidir si mantener la pregunta actual o devolver una vac�a
    }

    return preguntaNueva;
}

int main() {
    Pregunta preguntaAleatoria;
    bool esCorrecta = false;
    int faseActual = 1;
    char inputUsuario;
    string nombreArchivo = "preguntas.csv";

    do {
        preguntaAleatoria = buscarPreguntaAleatoria(nombreArchivo, faseActual);
        mezclarRespuestas(preguntaAleatoria);

        if (preguntaAleatoria.pregunta.empty()) {
            cerr << "No se pudo obtener una pregunta." << endl;
            return 1;
        }

        cout << "Fase " << faseActual << endl;
        cout << "Pregunta: " << preguntaAleatoria.pregunta << endl;
        cout << "1) " << preguntaAleatoria.respuesta1.respuesta << endl;
        cout << "2) " << preguntaAleatoria.respuesta2.respuesta << endl;
        cout << "3) " << preguntaAleatoria.respuesta3.respuesta << endl;
        cout << "4) " << preguntaAleatoria.respuesta4.respuesta << endl;

        cin >> inputUsuario;

        switch (inputUsuario) {
            case '1':
                esCorrecta = preguntaAleatoria.respuesta1.respuestaCorrecta;
                break;
            case '2':
                esCorrecta = preguntaAleatoria.respuesta2.respuestaCorrecta;
                break;
            case '3':
                esCorrecta = preguntaAleatoria.respuesta3.respuestaCorrecta;
                break;
            case '4':
                esCorrecta = preguntaAleatoria.respuesta4.respuestaCorrecta;
                break;
            case 'A': // Cambiar pregunta
                preguntaAleatoria = cambiarPregunta(nombreArchivo, faseActual, preguntaAleatoria);
                mezclarRespuestas(preguntaAleatoria);
                break;
            case 'C': // Eliminar respuestas
                eliminarDosRespuestas(preguntaAleatoria);
                break;
            default:
                cout << "Entrada no v�lida. Intente de nuevo." << endl;
                continue;
        }

        if (esCorrecta) {
            cout << "�Respuesta correcta!" << endl;
            faseActual++;
        } else {
            cout << "Respuesta incorrecta. Fin del juego." << endl;
            break;
        }
    } while (faseActual <= 10);

    if (faseActual > 10) {
        cout << "�Felicidades, has ganado!" << endl;
    }

    return 0;
}

