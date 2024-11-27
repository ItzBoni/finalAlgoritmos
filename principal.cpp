#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cstdlib> // Para srand() y rand()
#include<ctime>   // Para time()
#include<algorithm>
#include<random>
//#include<timer.h>

using namespace std;

//Estructura para almacenar las respuestas y si es correcta
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

//Función de comodin que elimina dos respuestas al azar
int eliminarRespuestas() {    
    srand(int(time(NULL)));
    int indiceBorrar = rand() % 3 + 1;

    return indiceBorrar;
}

//Función para añadir tiempo
void añadirTiempo(){

}

// Función para leer el CSV y devolver una pregunta aleatoria
Pregunta buscarPreguntaAleatoria(const string& nombreArchivo, int faseActual) {
    vector<Pregunta> preguntas;
    string linea;
    int indiceAleatorio;

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        return {}; // Retornar estructura vacía
    }

    //Linea para excluir los titulos de las columnas
    getline(archivo, linea);

    // Leer el resto de las líneas del archivo
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
        getline(ss, respuesta1  , ',');
        getline(ss, respuesta2, ',');
        getline(ss, respuesta3, ',');
        getline(ss, respuesta4, ',');

        // Guardar en la estructura y añadir al vector
        preguntas.push_back({fase, pregunta, {respuesta1, correcta1}, {respuesta2, correcta2}, {respuesta3, correcta3}, {respuesta4, correcta4}});
    }
    archivo.close();

    // Verificar que haya preguntas en el archivo
    if (preguntas.empty()) {
        cout << "El archivo no contiene preguntas válidas." << endl;
        return {};
    }

    // Semilla para números aleatorios
    srand(double(time(NULL)));

    // Elegir pregunta de la fase actual
    do {
        indiceAleatorio = rand() % preguntas.size();
        if(stoi(preguntas[indiceAleatorio].fase) == faseActual){
            return preguntas[indiceAleatorio];
        }
    } while (indiceAleatorio != faseActual);
    
    //Si no se retorna nada con el código de arriba se hace esto
    cout << "No hay preguntas para la fase " << faseActual << endl;
    return {};
}

//Función para imprimir las respuestas de forma aleatoria
void mezclarRespuestas(Pregunta& pregunta){
    vector<Respuesta> respuestasMezclar;
    
    //Agregar respuestas al vector
    respuestasMezclar.push_back(pregunta.respuesta1);
    respuestasMezclar.push_back(pregunta.respuesta2);
    respuestasMezclar.push_back(pregunta.respuesta3);
    respuestasMezclar.push_back(pregunta.respuesta4);
    
    random_device rd;
    mt19937 g(rd());

    // Mezclar las respuestas
    shuffle(respuestasMezclar.begin(), respuestasMezclar.end(), g);

    //Agregar las respuestas desordenadas de nuevo a la estructura
    pregunta.respuesta1 = respuestasMezclar[0];
    pregunta.respuesta2 = respuestasMezclar[1];
    pregunta.respuesta3 = respuestasMezclar[2];
    pregunta.respuesta4 = respuestasMezclar[3];
}

//Función para powerup de cambiar la pregunta
Pregunta cambiarPregunta(string nombreArchivo, int faseActual, Pregunta preguntaActual){
    Pregunta preguntaNueva = {};
    do {
        preguntaNueva = buscarPreguntaAleatoria(nombreArchivo, faseActual);
    } while (preguntaNueva.pregunta == preguntaActual.pregunta); 
    
    if (preguntaNueva.pregunta.empty()) {
            cout << "No se pudo encontrar una nueva pregunta diferente." << endl;
            return preguntaActual; // Puedes decidir si mantener la pregunta actual o devolver una vacía
        }
    
    return preguntaNueva;
}

int main() {
    //Declarar variables a usar
    Pregunta preguntaAleatoria;
    Pregunta preguntaNueva;
    bool esCorrecta = false;
    int dineroUsuario = 0;
    char inputUsuario;
    string nombreArchivo = "preguntas.csv"; 
    int faseActual = 1;

    do {
        cout<<"Bienvenido a como ser millonario! Presione enter para iniciar."<<endl;
    } while (cin.get() != '\n');

    do {
        preguntaAleatoria = buscarPreguntaAleatoria(nombreArchivo, faseActual);
        mezclarRespuestas(preguntaAleatoria);
        cout<<"Bienvenido a la fase "<<faseActual<<endl;

        if (preguntaAleatoria.pregunta.empty()) {
            cerr << "No se pudo obtener una pregunta." << endl;
            return 1;
        }

        //Mostrar pregunta y respuestas
        cout << "Pregunta: " << preguntaAleatoria.pregunta << endl;
        
        cout << "1) " << preguntaAleatoria.respuesta1.respuesta << endl;
        cout << "2) " << preguntaAleatoria.respuesta2.respuesta << endl;
        cout << "3) " << preguntaAleatoria.respuesta3.respuesta << endl;
        cout << "4) " << preguntaAleatoria.respuesta4.respuesta << endl;

        cin>>inputUsuario;

        //Si la respuesta introducida es correcta, aumentar la fase por uno
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

            //Caso para usar un comodín (NO ESTÁ TERMINADO)
            case 'A':
                preguntaNueva = cambiarPregunta(nombreArchivo, faseActual, preguntaAleatoria);
                cout << "Pregunta: " << preguntaNueva.pregunta << endl;
        
                cout << "1) " << preguntaNueva.respuesta1.respuesta << endl;
                cout << "2) " << preguntaNueva.respuesta2.respuesta << endl;
                cout << "3) " << preguntaNueva.respuesta3.respuesta << endl;
                cout << "4) " << preguntaNueva.respuesta4.respuesta << endl;

                break;
            //Caso para usar función de agrear tiempo
            case 'B':
                break;
            //Caso para usar borrar dos respuestas
            case 'C':
                break;
            default:
                cout << "Entrada no válida. Intente de nuevo." << endl;
                continue;
            }

            // Ver resultado
            if (esCorrecta) {
                cout << "¡Respuesta correcta!" << endl;
                faseActual+=1;
            } else {
                cout << "Respuesta incorrecta. Fin del juego." << endl;
                //Agregar codigo para aumentar dinero
            }
    } while(faseActual <= 10 || esCorrecta == true); // Parte para sacar del programa si gana o pierde xd

    do {
        if(faseActual < 10){
            cout<<"Has ganado!!! Presiona enter para salir del juego"<<endl;
        }
    } while (cin.get() != '\n');
    
    return 0;
}