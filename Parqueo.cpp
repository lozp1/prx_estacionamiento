#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <stack>
#include <queue>
#include <vector>
#include <cstdlib> 

using namespace std;

// ESTRUCTURAS DE DATOS

struct Vehiculo {
    char placa[15];
    char marca[30];
    char modelo[30];
    long int horaEntrada; 
    long int horaSalida;  
    int dia;              // Guarda el día que ingreso el carro
};

struct RegistroArchivo {
    Vehiculo vehiculo;
    char accion[20];       //Es que hace el algoritmo
    long int horaRegistro; // Guarda las horas
    int diaRegistro;      // Guarda los días
};

//Dimensiones del parqueo
const int CANT_CARRILES = 3;     
const int CAPACIDAD_CARRIL = 3;  
const char* ARCHIVO_DATOS = "StatusParqueo.dat"; 

//Guardado y permanencia de información
vector<stack<Vehiculo> > carriles; 
queue<Vehiculo> colaEntrada;       
int diaActual = 1;        // Cuenta los días (se reinicia según el documento)

//Funciones
void inicializarParqueo();
void cargarEstadoAnterior(); 
bool verificarSpaceLibre();
void registrarMovimiento(Vehiculo v, const char* accion);
void ingresarVehiculo(const char* placa, const char* marca, const char* modelo);
void retirarVehiculo(const char* placaBusqueda);
void buscarVehiculo(const char* placaBusqueda);
void mostrarEstadoParqueo();
void avanzarDia();       

//Programa 
int main() {
    inicializarParqueo();   // 1. Prepara las pilas en memoria
    cargarEstadoAnterior(); // 2.  Lee el archivo .dat y reubica los autos que no han salido
    int opcion;
    char placa[15], marca[30], modelo[30];

    do {
        system("cls"); // Limpia la pantalla antes de mostrar el menú principal
        cout << "    CONTROL DE PARQUEO  |  DIA: " << diaActual << endl;
        cout << "1. Registrar Ingreso de Vehiculo" << endl;
        cout << "2. Registrar Salida de Vehiculo" << endl;
        cout << "3. Buscar Vehiculo por Placa" << endl;
        cout << "4. Ver Estado de los Espacios (Mapa)" << endl;
        cout << "5. Avanzar de Dia" << endl;
        cout << "6. Cerrar" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion; 

        switch (opcion) {
            case 1:
                system("cls"); 
                cout << "INGRESA LA SIGUIENTE INFORMACION" << endl;
                cout << "Ingrese Placa: "; cin >> placa;
                cout << "Ingrese Marca: "; cin >> marca;
                cout << "Ingrese Modelo: "; cin >> modelo;
                ingresarVehiculo(placa, marca, modelo); 
                break;
            case 2:
                system("cls"); 
                cout << "INGRESA LA SIGUIENTE INFORMACION" << endl;
                cout << "Ingrese la Placa a retirar: "; cin >> placa;
                retirarVehiculo(placa); 
                break;
            case 3:
                system("cls"); 
                cout << "INGRESA LA SIGUIENTE INFORMACION" << endl;
                cout << "Ingrese la Placa a buscar: "; cin >> placa;
                buscarVehiculo(placa); 
                break;
            case 4:
                system("cls"); 
                mostrarEstadoParqueo(); 
                break;
            case 5:
                system("cls"); 
                avanzarDia();
                break;
            case 6:
                system("cls");
                cout << "Guardando" << endl;
                break;
            default:
                system("cls");
                cout << "Esta opcion no esta disponible" << endl;
        }
        
        if (opcion != 6) {
            cout << "\nPresione ENTER para continuar...";
            cin.ignore();
            cin.get();
        }
    } while (opcion != 6); 

    return 0; 
}

//Especificaciones de funciones

void inicializarParqueo() {
    for (int i = 0; i < CANT_CARRILES; i++) {
        stack<Vehiculo> carrilIndividual; 
        carriles.push_back(carrilIndividual); 
    }
}

//Carga los datos desde el .dat
void cargarEstadoAnterior() {
    ifstream archivo;
    archivo.open(ARCHIVO_DATOS, ios::binary); // abre el archivo para revisarlo
    
    if (!archivo.is_open()) {
        return; 
    }

    RegistroArchivo registro;
    
    // Primero leemos el archivo para saber en qué día nos quedamos
    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(RegistroArchivo))) {
        if (registro.diaRegistro > diaActual && registro.diaRegistro < 10000) {
            diaActual = registro.diaRegistro; 
        }
    }
    
    // Regresamos al inicio del archivo para meter los carros que sigan adentro
    archivo.clear(); 
    archivo.seekg(0, ios::beg);

    cout << "Recuperando estado del parqueo desde StatusParqueo.dat..." << endl;

    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(RegistroArchivo))) {
        if (strcmp(registro.accion, "INGRESO") == 0) {
            bool insertado = false;
            for (int i = 0; i < CANT_CARRILES; i++) {
                if (carriles[i].size() < CAPACIDAD_CARRIL) {
                    carriles[i].push(registro.vehiculo);
                    insertado = true;
                    break;
                }
            }
            if (!insertado) {
                colaEntrada.push(registro.vehiculo); 
            }
        }
        else if (strcmp(registro.accion, "SALIDA") == 0) {
            for (int i = 0; i < CANT_CARRILES; i++) {
                if (!carriles[i].empty() && strcmp(carriles[i].top().placa, registro.vehiculo.placa) == 0) {
                    carriles[i].pop();
                }
            }
        }
    }
    archivo.close();
}

bool verificarSpaceLibre() {
    for (int i = 0; i < CANT_CARRILES; i++) {
        if (carriles[i].size() < CAPACIDAD_CARRIL) {
            return true; 
        }
    }
    return false; 
}

void registrarMovimiento(Vehiculo v, const char* accion) {
    ofstream archivo(ARCHIVO_DATOS, ios::binary | ios::app);
    if (archivo.is_open()) {
        RegistroArchivo registro; 
        registro.vehiculo = v;    
        strcpy(registro.accion, accion); 
        registro.horaRegistro = time(0); 
        registro.diaRegistro = diaActual; 

        archivo.write(reinterpret_cast<char*>(&registro), sizeof(RegistroArchivo));
        archivo.close(); 
    }
}

void ingresarVehiculo(const char* placa, const char* marca, const char* modelo) {
    Vehiculo nuevoAuto;
    strcpy(nuevoAuto.placa, placa);   
    strcpy(nuevoAuto.marca, marca);   
    strcpy(nuevoAuto.modelo, modelo); 
    nuevoAuto.horaEntrada = time(0); 
    nuevoAuto.horaSalida = 0;          
    nuevoAuto.dia = diaActual; 

    if (verificarSpaceLibre()) {
        for (int i = 0; i < CANT_CARRILES; i++) {
            if (carriles[i].size() < CAPACIDAD_CARRIL) {
                carriles[i].push(nuevoAuto); 
                cout << "Vehiculo " << placa << " estacionado en Carril " << (i + 1) << endl;
                registrarMovimiento(nuevoAuto, "INGRESO"); 
                break; 
            }
        }
    } else {
        colaEntrada.push(nuevoAuto); 
        cout << "Parqueo lleno. " << placa << " enviado a la cola de espera exterior." << endl;
        registrarMovimiento(nuevoAuto, "EN_COLA"); 
    }
}

void retirarVehiculo(const char* placaBusqueda) {
    int carrilObjetivo = -1; 

    for (int i = 0; i < CANT_CARRILES; i++) {
        stack<Vehiculo> copia = carriles[i]; 
        while (!copia.empty()) {
            if (strcmp(copia.top().placa, placaBusqueda) == 0) {
                carrilObjetivo = i; 
            }
            copia.pop(); 
        }
    }

    if (carrilObjetivo == -1) {
        cout << "El vehiculo con placa " << placaBusqueda << " no esta en el parqueo activo." << endl;
        return; 
    }

    stack<Vehiculo> rampaTemporal; 

    while (strcmp(carriles[carrilObjetivo].top().placa, placaBusqueda) != 0) {
        Vehiculo autoEstorbo = carriles[carrilObjetivo].top(); 
        carriles[carrilObjetivo].pop(); 
        
        cout << "[MOVIENDO] Sacando temporalmente auto: " << autoEstorbo.placa << endl;
        registrarMovimiento(autoEstorbo, "MOV_INTERNO"); 
        
        rampaTemporal.push(autoEstorbo); 
    }

    Vehiculo autoSalida = carriles[carrilObjetivo].top();
    carriles[carrilObjetivo].pop(); 
    autoSalida.horaSalida = time(0); 
    cout << "[SALIDA] El vehiculo " << autoSalida.placa << " ha salido del parqueo." << endl;
    registrarMovimiento(autoSalida, "SALIDA"); 

    while (!rampaTemporal.empty()) {
        Vehiculo autoDevuelto = rampaTemporal.top(); 
        rampaTemporal.pop(); 
        carriles[carrilObjetivo].push(autoDevuelto); 
        
        cout << "Auto " << autoDevuelto.placa << " volvio a entrar al carril." << endl;
        registrarMovimiento(autoDevuelto, "MOV_INTERNO"); 
    }

    if (!colaEntrada.empty()) {
        Vehiculo proximoAuto = colaEntrada.front(); 
        colaEntrada.pop(); 
        
        proximoAuto.horaEntrada = time(0); 
        carriles[carrilObjetivo].push(proximoAuto); 
        
        cout << "El vehiculo " << proximoAuto.placa << " entro al Carril " << (carrilObjetivo + 1) << endl;
        registrarMovimiento(proximoAuto, "INGRESO"); 
    }
}

void buscarVehiculo(const char* placaBusqueda) {
    // También corregido aquí para evitar errores de compilación
    ifstream archivo;
    archivo.open(ARCHIVO_DATOS, ios::binary);
    
    if (!archivo.is_open()) {
        cout << "Archivo no encontrado o vacio." << endl;
        return;
    }

    RegistroArchivo registroActual;
    RegistroArchivo ultimoRegistro;
    bool encontrado = false;

    while (archivo.read(reinterpret_cast<char*>(&registroActual), sizeof(RegistroArchivo))) {
        if (strcmp(registroActual.vehiculo.placa, placaBusqueda) == 0) {
            ultimoRegistro = registroActual; 
            encontrado = true; 
        }
    }
    archivo.close(); 

    if (!encontrado) {
        cout << "No hay registros de la placa " << placaBusqueda << " en el archivo .dat" << endl;
        return;
    }

    cout << "\n DATOS DE " << ultimoRegistro.vehiculo.placa << " "<< endl;
    cout << "Marca:          " << ultimoRegistro.vehiculo.marca << endl;
    cout << "Modelo:         " << ultimoRegistro.vehiculo.modelo << endl;
    cout << "Ingresado el:   Dia " << ultimoRegistro.vehiculo.dia << endl;
    cout << "Ultimo estado:  " << ultimoRegistro.accion << " (Registrado en el Dia " << ultimoRegistro.diaRegistro << ")" << endl;

    long int tiempoFinal = time(0);
    if (strcmp(ultimoRegistro.accion, "SALIDA") == 0) {
        tiempoFinal = ultimoRegistro.vehiculo.horaSalida; 
    }

    long int segundosTotales = tiempoFinal - ultimoRegistro.vehiculo.horaEntrada;
    int horas = segundosTotales / 3600;              
    int minutos = (segundosTotales % 3600) / 60;     
    int segundos = segundosTotales % 60;             

    cout << "Tiempo total:   " << horas << "h " << minutos << "m " << segundos << "s" << endl;
}

void mostrarEstadoParqueo() {
    cout << "MAPA DE CARRIL DE ESTACIONAMIENTO" << endl;
    for (int i = 0; i < CANT_CARRILES; i++) {
        cout << "Carril " << (i + 1) << " -> ";
        
        if (carriles[i].empty()) {
            cout << "[ VACIO ]" << endl;
        } else {
            stack<Vehiculo> copia = carriles[i];
            while (!copia.empty()) {
                cout << "[" << copia.top().placa << " (D" << copia.top().dia << ")] ";
                copia.pop();
            }
            cout << " <- (Fondo del carril)" << endl;
        }
    }

    cout << "Cola de espera exterior: ";
    if (colaEntrada.empty()) {
        cout << "[ Vacia ]" << endl;
    } else {
        queue<Vehiculo> copiaCola = colaEntrada;
        while (!copiaCola.empty()) {
            cout << "[" << copiaCola.front().placa << " (D" << copiaCola.front().dia << ")] <- ";
            copiaCola.pop();
        }
        cout << "(Fin de la linea)" << endl;
    }
}

void avanzarDia() {
    cout << "ATENCION: Procesando cierre del Dia " << diaActual << "..." << endl;
    
    Vehiculo controlDia;
    strcpy(controlDia.placa, "SISTEMA");
    strcpy(controlDia.marca, "CIERRE");
    strcpy(controlDia.modelo, "DIA");
    controlDia.horaEntrada = time(0);
    controlDia.horaSalida = time(0);
    controlDia.dia = diaActual;

    registrarMovimiento(controlDia, "Cambio de dia");

    diaActual++; 
    cout << "El sistema ha avanzado de Dia " << diaActual << "." << endl;
}

//De paso si ven alguna linea que no se entiende me avisan o agregan el comentario explicandola
//Creo que ahí esta la idea es que los comentarios nos ayuden a entenderlo todos pero me cuentan si hay algo que no esta claro
//José Samuel Sigüina Chonay - Proyecto Programación I 
