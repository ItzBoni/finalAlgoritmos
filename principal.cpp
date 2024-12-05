#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cstdlib>
#include<ctime>
#include<algorithm>
#include<random>
#include<Windows.h>
#include<thread>
#include<chrono>
#include<atomic>
#include<mutex>

using namespace std;

//Variables globales
mutex coutMutex;


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

//Función que maneja el temporizador en el juego
void iniciarTemporizador(atomic<int>& duracionSegundos, atomic<bool>& tiempoTerminado, atomic<bool> agregarTiempo = false) {
    int i = duracionSegundos;
    if (agregarTiempo) i += 15;

    for (i; i > 0; --i) {
        if (tiempoTerminado) return; // Salir si el jugador responde
        cout <<"\033[s" << "\033[18;52H" << "Tiempo restante: " << i << " segundos" << "\033[u"; //gracias Donovan por el truco con ASCII
        this_thread::sleep_for(chrono::seconds(1)); // Esperar 1 segundo
    }
    tiempoTerminado = true; // Marcar que el tiempo ha terminado
}


//Función para manejar el dinero en el juego
int dinero(int dineroUsuario, int faseActual) {
    switch (faseActual)
    {
    case 1:
        dineroUsuario += 1000;
        break;
    case 2:
        dineroUsuario += 2000;
        break;
    case 3:
        dineroUsuario += 3000;
        break;
    case 4:
        dineroUsuario += 5000;
        break;
    case 5:
        dineroUsuario += 10000;
        break;
    case 6:
        dineroUsuario += 20000;
        break;
    case 7:
        dineroUsuario += 40000;
        break;
    case 8:
        dineroUsuario += 80000;
        break;
    case 9:
        dineroUsuario += 160000;
        break;
    case 10:
        dineroUsuario += 250000;
        break;
    
    default:
        break;
    }

    return dineroUsuario;
}

// Función para leer el CSV y devolver una pregunta aleatoria
Pregunta buscarPreguntaAleatoria(const string& nombreArchivo, int faseActual) {
    vector<Pregunta> preguntas;
    vector<Pregunta> preguntasFase;
    string linea;

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        return {}; // Retornar estructura vacía
    }

    // Leer la primera línea (encabezados)
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
        getline(ss, respuesta1, ',');
        getline(ss, respuesta2, ',');
        getline(ss, respuesta3, ',');
        getline(ss, respuesta4, ',');

        // Agregar al vector si la fase es válida
        try {
            int faseInt = stoi(fase); // Convertir fase a entero
            preguntas.push_back({fase, pregunta, {respuesta1, correcta1}, {respuesta2, correcta2}, {respuesta3, correcta3}, {respuesta4, correcta4}});
            if (faseInt == faseActual) {
                preguntasFase.push_back(preguntas.back());
            }
        } catch (exception& e) {
            cout << "Error al convertir la fase: " << e.what() << endl;
        }
    }
    archivo.close();

    // Verificar que haya preguntas para la fase actual
    if (preguntasFase.empty()) {
        cout << "No hay preguntas para la fase " << faseActual << endl;
        return {};
    }

    // Elegir una pregunta aleatoria
    int indiceAleatorio = rand() % preguntasFase.size();
    return preguntasFase[indiceAleatorio];
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

//Función que calcula el ancho de la pantalla para hacer display de la pregunta
int obtenerAnchoConsola() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

//Función para imprimir centrado en la pantalla
void imprimirCentrado(const string& texto, const string& texto2 = "", const string& texto3 = "") {
    int anchoConsola = obtenerAnchoConsola();

    string textoCompleto = texto;
    if (!texto2.empty()) textoCompleto += " " + texto2;
    if (!texto3.empty()) textoCompleto += " " + texto3;

    int espacios = (anchoConsola - textoCompleto.size()) / 2;
    if (espacios > 0) {
        cout << string(espacios, ' ');
    }

    cout << textoCompleto << endl;
}

//Función para imprimir las preguntas y las respuestas
void imprimirPreguntas(Pregunta& preguntaAleatoria, bool comodinA = false, bool comodinB = false, bool comodinC = false){
    string divisor(90,'_');
    string marcoPregunta;
    string respuesta1, respuesta2, respuesta3, respuesta4;
    string comodinPregunta = "";
    string comodinTimer = "";
    string comodinRespuesta = "";

    if(!comodinA) comodinPregunta = "A) Cambiar Pregunta";
    if(!comodinB) comodinTimer = "B) Agregar Tiempo";
    if(!comodinC) comodinRespuesta = "C) Eliminar dos respuestas";

    if (preguntaAleatoria.pregunta.empty()) {
            cerr << "No se pudo obtener una pregunta." << endl;
    }

    for(int tamano; tamano<size(preguntaAleatoria.pregunta); tamano++){
            marcoPregunta += (tamano, '_');
    }

        //Mostrar pregunta y respuestas (con formato bonito ahora sí)
        imprimirCentrado(marcoPregunta);
        imprimirCentrado(preguntaAleatoria.pregunta);
        imprimirCentrado(marcoPregunta);

        cout<<"\n";        
    
        respuesta1 = "1) " + preguntaAleatoria.respuesta1.respuesta;
        respuesta2 = "2) " + preguntaAleatoria.respuesta2.respuesta;
        respuesta3 = "3) " + preguntaAleatoria.respuesta3.respuesta;
        respuesta4 = "4) " + preguntaAleatoria.respuesta4.respuesta;

        cout<<"\n";
        imprimirCentrado("Comodines");
        cout<<"\n";

        imprimirCentrado(divisor);
        imprimirCentrado(comodinPregunta, comodinTimer, comodinRespuesta);
        imprimirCentrado(divisor);
        
        cout<<endl;

        imprimirCentrado(respuesta1, respuesta2);
        imprimirCentrado(respuesta3, respuesta4); 

}


//Si llegas al punto de una fase alta poner que pierdan todo el dinero.

int main() {
    // Semilla para números aleatorios
    srand(double(time(NULL)));
    system("cls");
    //Declarar variables a usar
    Pregunta preguntaAleatoria;
    bool esCorrecta = false;
    bool comodinPregunta = false;
    bool comodinTimer = false; //Este es para saber si ya se usó el powerup del temporizador
    bool comodinTimer2 = true; //Este es para manejar el temporizador y que no se siga multiplicando
    bool comodinRespuestas = false;
    bool comodinRespuestas2 = true;
    bool inputCorrecto = true;
    char inputUsuario;
    int faseActual = 1;
    int dineroUsuario = 0;
    int contador = 0;
    string nombreArchivo = "preguntas.csv"; 

    //Variables para el temporizador
    atomic<int> tiempoLimite = 30;
    atomic<bool> tiempoTerminado = false;

    //Variables para la pantalla
    string divisor(90,'_');
    string nombreJuego = R"(
  ___        _                          _                                           _ _ _                        _        
 / _ \ _   _(_) ___ _ __     __ _ _   _(_) ___ _ __ ___   ___  ___ _ __   _ __ ___ (_) | | ___  _ __   __ _ _ __(_) ___   
| | | | | | | |/ _ \ '_ \   / _` | | | | |/ _ \ '__/ _ \ / __|/ _ \ '__| | '_ ` _ \| | | |/ _ \| '_ \ / _` | '__| |/ _ \  
| |_| | |_| | |  __/ | | | | (_| | |_| | |  __/ | |  __/ \__ \  __/ |    | | | | | | | | | (_) | | | | (_| | |  | | (_) |
 \__\_\\__,_|_|\___|_| |_|  \__, |\__,_|_|\___|_|  \___| |___/\___|_|    |_| |_| |_|_|_|_|\___/|_| |_|\__,_|_|  |_|\___/  
                               |_|                                                                                        )";

    
    //Parte principal del programa para seguir repitiendo hasta ganar o que el usuario pierda.
    while (faseActual <= 10) {
        system("cls");
        tiempoTerminado = false;

        do {
            imprimirCentrado(divisor);
            cout<<nombreJuego<<endl;
            imprimirCentrado(divisor);
            if(faseActual < 2) imprimirCentrado("Presione enter para iniciar");
        } while (cin.get() != '\n');

        imprimirCentrado("Bienvenido a la fase ", to_string(faseActual));
        imprimirCentrado("Su dinero en el banco es: $", to_string(dineroUsuario));
        cout<<endl;

        //Creo pregunta Aleatoria dentro de esta función
        if (comodinTimer2 || comodinRespuestas2 || inputCorrecto) {
            preguntaAleatoria = buscarPreguntaAleatoria(nombreArchivo, faseActual);
            mezclarRespuestas(preguntaAleatoria);
        }

        if (comodinRespuestas2) comodinTimer2 = false;

        imprimirPreguntas(preguntaAleatoria, comodinPregunta, comodinTimer, comodinRespuestas);

        //Espera tantito para que se cargue todo y el temporizador quede acomodado en la parte correcta
        Sleep(500);

        //Inicializa el temporizador en otro hilo de procesador
        thread temporizador(iniciarTemporizador, ref(tiempoLimite), ref(tiempoTerminado), comodinTimer2);
        
        //Hacen que no se pueda usar de nuevo el comodin, pero que 
        if (comodinTimer) comodinTimer2 = true;
        if (comodinRespuestas) comodinRespuestas2 = true;

        {
            lock_guard<mutex> lock(coutMutex);
            imprimirCentrado("Su respuesta es: ");
            cin>>inputUsuario;
        }

        inputUsuario = toupper(inputUsuario);

        //Termina el temporizador si el tiempo es el correcto, y después lo agrega al programa
        tiempoTerminado = true;
        temporizador.join();

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
            case 'A':
                comodinPregunta = true;
                continue;
                break;
            //Caso para usar función de agregar tiempo
            case 'B':
                //(Santi): Hice esta lógica a las 2 AM y solo Dios y yo sabíamos como funcionaba. Ahora solo es Dios.
                comodinTimer = true;
                comodinTimer2 = false;
                comodinRespuestas2 = false;
                break;
             //Caso para usar borrar dos respuestas
            case 'C':
                eliminarDosRespuestas(preguntaAleatoria);
                comodinRespuestas = true;
                comodinRespuestas2 = false;
                comodinTimer2 = false;
                break;

            default:
                cout << "Entrada no valida. Intente de nuevo." << endl;
                inputCorrecto = false;
                continue;
            }
            // Ver resultado y motrar la cantidad de dinero en el banco
            if (esCorrecta) {
                system("cls");
                imprimirCentrado(divisor);
                cout<<nombreJuego<<endl;
                imprimirCentrado(divisor);
                imprimirCentrado("Respuesta correcta!");
                faseActual+=1;
                dineroUsuario += dinero(dineroUsuario, faseActual);
                imprimirCentrado("Ahora tiene ", to_string(dineroUsuario));

                imprimirCentrado(divisor);
                imprimirCentrado("Desea continuar? (Y/N)");
                cin>>inputUsuario;
                inputUsuario = toupper(inputUsuario);

                //Bucle que verifica si el usuario quiere continuar.
                do{
                    if(inputUsuario == 'Y') {
                        continue;
                    } else if (inputUsuario == 'N') {
                        imprimirCentrado("Termina el juego con $", to_string(dineroUsuario));
                        imprimirCentrado("Gracias por jugar nuestro jueguito :3");
                        exit(0);
                    } else {
                        imprimirCentrado("Entrada no válida, intente de nuevo.");
                    }

                } while (inputUsuario != 'Y' || inputUsuario != 'N');
            }  else if (comodinPregunta || comodinTimer || comodinRespuestas) {
                continue;
            } else {
                system("cls");
                cout<<nombreJuego<<endl;
                imprimirCentrado(divisor);
                imprimirCentrado("Respuesta incorrecta. Fin del juego.");
                imprimirCentrado("Termina el juego con: $", to_string(dineroUsuario));
                exit(1);//Cambiar el código para que no me saque de una vez.
            }
    }

    do {
        if(faseActual < 10){
            cout<<"Has ganado!!! Presiona enter para salir del juego"<<endl;
        }
    } while (cin.get() != '\n');
    
    return 0;
}


//Proyecto hecho por Santiago Andrés Bonilla Ospina, Antonio Enrique Velasco, Pablo Gianni Jaled.
//No es el más bonito ni el más eficiente, pero es trabajo honesto

//Agradecimiento especial a Donovan por explicarnos cosas cuando no las entendíamos.