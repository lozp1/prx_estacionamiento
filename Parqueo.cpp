#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <stack>
#include <queue>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <mysql.h>  

using namespace std;

// ESTRUCTURAS DE DATOS

struct Vehiculo {
    char placa[15];
    char marca[30];
    char modelo[30];
    long int horaEntrada; 
    long int horaSalida;  
    int dia;              // Guarda el d�a que ingreso el carro
};

struct RegistroArchivo {
    Vehiculo vehiculo;
    char accion[20];       //Es que hace el algoritmo
    long int horaRegistro; // Guarda las horas
    int diaRegistro;      // Guarda los d�as
};

//Dimensiones del parqueo
const int CANT_CARRILES = 3;     
const int CAPACIDAD_CARRIL = 3;  
const char* ARCHIVO_DATOS = "StatusParqueo.dat"; 

//Guardado y permanencia de informaci�n
vector<stack<Vehiculo> > carriles; 
queue<Vehiculo> colaEntrada;       
int diaActual = 1;        // Cuenta los d�as (se reinicia seg�n el documento)

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
void cerrarDia();  

//Programa 
int main() {
    inicializarParqueo();   // 1. Prepara las pilas en memoria
    cargarEstadoAnterior(); // 2.  Lee el archivo .dat y reubica los autos que no han salido
    int opcion;
    char placa[15], marca[30], modelo[30];

    do {
        system("cls"); // Limpia la pantalla antes de mostrar el men� principal
        cout << "    CONTROL DE PARQUEO  |  DIA: " << diaActual << endl;
        cout << "1. Registrar Ingreso de Vehiculo" << endl;
        cout << "2. Registrar Salida de Vehiculo" << endl;
        cout << "3. Buscar Vehiculo por Placa" << endl;
        cout << "4. Ver Estado de los Espacios (Mapa)" << endl;
        cout << "5. Avanzar de Dia" << endl;
        cout << "6. Cierre del Dia (Reporte + MySQL)" << endl;
        cout << "7. Cerrar" << endl;
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
                cerrarDia();
                break;        
            case 7:
                system("cls");
                cout << "Guardando" << endl;
                break;             
            default:
                system("cls");
                cout << "Esta opcion no esta disponible" << endl;
        }
        
        if (opcion != 7) {
            cout << "\nPresione ENTER para continuar...";
            cin.ignore();
            cin.get();
        }
    } while (opcion != 7); 

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
    
    // Primero leemos el archivo para saber en qu� d�a nos quedamos
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
    // Tambien corregido aqui para evitar errores de compilacion
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

void cerrarDia() {
    cout << "--- INICIANDO CIERRE DEL DIA " << diaActual << " ---" << endl;

    /*
    DEBUG TEMPORAL - borrar despues
    ifstream debug(ARCHIVO_DATOS, ios::binary);
    RegistroArchivo tmp;
    int contador = 0;
    while (debug.read(reinterpret_cast<char*>(&tmp), sizeof(RegistroArchivo))) {
        cout << "[DEBUG] placa=" << tmp.vehiculo.placa 
             << " accion=" << tmp.accion 
             << " diaRegistro=" << tmp.diaRegistro << endl;
        contador++;
    }
    debug.close();
    cout << "[DEBUG] Total registros leidos: " << contador << endl;
    cout << "[DEBUG] Buscando diaActual=" << diaActual << endl;
    FIN DEBUG

    */

    ifstream archivo(ARCHIVO_DATOS, ios::binary);
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo abrir StatusParqueo.dat." << endl;
        return;
    }

    RegistroArchivo reg;
    int vehiculosAtendidos = 0;
    double totalRecaudado = 0.0;
    long int tiempoTotalSegundos = 0;
    double tasaPorHora = 15.0;
    vector<string> placasCierre;

    while (archivo.read(reinterpret_cast<char*>(&reg), sizeof(RegistroArchivo))) {
        if (reg.diaRegistro == diaActual && strcmp(reg.accion, "SALIDA") == 0) {
            vehiculosAtendidos++;
            long int segundos = reg.vehiculo.horaSalida - reg.vehiculo.horaEntrada;
            tiempoTotalSegundos += segundos;
            double horas = ceil(segundos / 3600.0);
            if (horas == 0) horas = 1;
            totalRecaudado += (horas * tasaPorHora);
            placasCierre.push_back(reg.vehiculo.placa);
        }
    }
    archivo.close();

    if (vehiculosAtendidos == 0) {
        cout << "No hubo salidas registradas en el Dia " << diaActual << "." << endl;
        return;
    }

    double promedioMinutos = (tiempoTotalSegundos / 60.0) / vehiculosAtendidos;

    // --- Reporte CSV ---
    string csv = "Reporte_Dia_" + to_string(diaActual) + ".csv";
    ofstream fcsv(csv);
    if (fcsv.is_open()) {
        fcsv << "Dia,Vehiculos Atendidos,Total Recaudado,Promedio Tiempo (min)\n";
        fcsv << diaActual << "," << vehiculosAtendidos << "," << totalRecaudado << "," << promedioMinutos << "\n";
        fcsv.close();
        cout << "[OK] Reporte CSV generado: " << csv << endl;
    }

    // --- Reporte HTML ---
    string html = "Reporte_Dia_" + to_string(diaActual) + ".html";
    ofstream fhtml(html);
    if (fhtml.is_open()) {
        fhtml << "<!DOCTYPE html>\n<html>\n<head>\n";
        fhtml << "<title>Reporte de Parqueo</title>\n";
        fhtml << "<style>\n";
        fhtml << "body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; }\n";
        fhtml << ".contenedor { background-color: white; width: 60%; margin: 20px auto; padding: 20px; border-radius: 8px; box-shadow: 2px 2px 10px gray; }\n";
        fhtml << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        fhtml << "th, td { border: 1px solid #dddddd; padding: 10px; }\n";
        fhtml << "th { background-color: #0056b3; color: white; }\n";
        fhtml << "</style>\n</head>\n<body>\n";
        fhtml << "<div class='contenedor'>\n";
        fhtml << "<h2>Resumen de Jornada - Dia " << diaActual << "</h2>\n";
        fhtml << "<table>\n";
        fhtml << "<tr><th>Vehiculos Atendidos</th><th>Total Recaudado</th><th>Tiempo Promedio</th></tr>\n";
        fhtml << "<tr><td>" << vehiculosAtendidos << "</td><td>Q" << totalRecaudado << "</td><td>" << promedioMinutos << " min</td></tr>\n";
        fhtml << "</table>\n</div>\n</body>\n</html>";
        fhtml.close();
        cout << "[OK] Reporte HTML generado." << endl;
    }

    // --- MySQL ---
    MYSQL *conn = mysql_init(NULL);
    if (conn) {
        unsigned int ssl_mode = 0;
        mysql_options(conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl_mode);
        if (mysql_real_connect(conn, "mysql-35378f5d-parqueoumg.d.aivencloud.com", "avnadmin", "AVNS_cDXXUn2LbO82VWFko9x", "defaultdb", 21973, NULL, 0)) {
            string q = "INSERT INTO jornada_cierre (dia_operacion, total_vehiculos_atendidos, monto_total_recaudado, promedio_tiempo_minutos) VALUES ("
                       + to_string(diaActual) + "," + to_string(vehiculosAtendidos) + ","
                       + to_string(totalRecaudado) + "," + to_string(promedioMinutos) + ")";
            if (mysql_query(conn, q.c_str()))
                cout << "[ERROR SQL] " << mysql_error(conn) << endl;
            for (const string& p : placasCierre) {
                string qp = "INSERT IGNORE INTO placas_registradas (placa) VALUES ('" + p + "')";
                mysql_query(conn, qp.c_str());
            }
            mysql_close(conn);
            cout << "[OK] Datos sincronizados con MySQL." << endl;
        } else {
            cout << "[ERROR SQL] " << mysql_error(conn) << endl;
            mysql_close(conn);
        }
    }

    avanzarDia();
}

/*

De paso si ven alguna linea que no se entiende me avisan o agregan el comentario explicandola
Creo que ahi esta la idea es que los comentarios nos ayuden a entenderlo todos pero me cuentan si hay algo que no esta claro
Jose Samuel Sig�ina Chonay - Proyecto Programacion I 

*/

// Orale esta bueno

// Anotaciones importantes

/* 

1. libreria agregada: 
    <cmath> permite hacer operaciones matematicas
    #include <mysql.h> permitir conexion a mysql

2. nueva funcion: void cerrarDia();
3. case actualizado: opcion 6 cambiada y opcion 7 agregada
4. menu de opciones: Cierre del Dia (Reporte +  MySQL) agregado
5. en el while de las opciones del menu, fue cambiado el 6 por 7

*/