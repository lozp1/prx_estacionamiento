#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath> 
#include <mysql.h>

using namespace std;

// 1. ESTRUCTURAS EXACTAS DE Parqueo.cpp
struct Vehiculo {
    char placa[15];
    char marca[30];
    char modelo[30];
    long int horaEntrada; 
    long int horaSalida;  
    int dia;              
};

struct RegistroArchivo {
    Vehiculo vehiculo;
    char accion[20];       
    long int horaRegistro; 
    int diaRegistro;      
};

// 2. ESTRUCTURA PARA EL REPORTE
struct DatosCierre {
    int diaOperacion;
    int totalVehiculos;
    double montoTotal;
    double promedioTiempo; 
};

// 3. GENERACIÓN DE REPORTES (CSV y HTML)
void generarReporteCSV(DatosCierre datos) {
    string nombreArchivo = "Reporte_Dia_" + to_string(datos.diaOperacion) + ".csv";
    ofstream archivo(nombreArchivo);
    
    if(archivo.is_open()) {
        archivo << "Dia,Vehiculos Atendidos,Total Recaudado,Promedio Tiempo (min)\n";
        archivo << datos.diaOperacion << "," << datos.totalVehiculos << ","
                << datos.montoTotal << "," << datos.promedioTiempo << "\n";
        archivo.close();
        cout << "[OK] Reporte CSV generado: " << nombreArchivo << endl;
    } else {
        cout << "[ERROR] No se pudo crear el archivo CSV." << endl;
    }
}

void generarReporteHTML(DatosCierre datos) {
    string nombreArchivo = "Reporte_Dia_" + to_string(datos.diaOperacion) + ".html";
    ofstream archivo(nombreArchivo);
    
    if(archivo.is_open()) {
        archivo << "<!DOCTYPE html>\n<html>\n<head>\n";
        archivo << "<title>Reporte de Parqueo</title>\n";
        archivo << "<style>\n";
        archivo << "body { font-family: Arial, sans-serif; background-color: #f4f4f4; text-align: center; }\n";
        archivo << ".contenedor { background-color: white; width: 60%; margin: 20px auto; padding: 20px; border-radius: 8px; box-shadow: 2px 2px 10px gray; }\n";
        archivo << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        archivo << "th, td { border: 1px solid #dddddd; padding: 10px; }\n";
        archivo << "th { background-color: #0056b3; color: white; }\n";
        archivo << "</style>\n";
        archivo << "</head>\n<body>\n";
        archivo << "<div class='contenedor'>\n";
        archivo << "<h2>Resumen de Jornada - Dia " << datos.diaOperacion << "</h2>\n";
        archivo << "<table>\n";
        archivo << "<tr><th>Vehiculos Atendidos</th><th>Total Recaudado</th><th>Tiempo Promedio</th></tr>\n";
        archivo << "<tr><td>" << datos.totalVehiculos << "</td><td>Q" << datos.montoTotal << "</td><td>" << datos.promedioTiempo << " min</td></tr>\n";
        archivo << "</table>\n";
        archivo << "</div>\n";
        archivo << "</body>\n</html>";
        archivo.close();
        cout << "[OK] Reporte HTML generado exitosamente." << endl;
    } else {
        cout << "[ERROR] No se pudo crear el archivo HTML." << endl;
    }
}

// 4. CONEXIÓN Y SINCRONIZACIÓN CON MYSQL (API NATIVA)
void sincronizarMySQL(DatosCierre datos, const vector<string>& placasNuevas) {
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) {
        cout << "[ERROR SQL] No se pudo inicializar la libreria MySQL." << endl;
        return;
    }

    cout << "Conectando a la base de datos en la nube..." << endl;

    unsigned int ssl_mode = 0;  // 0 = SSL deshabilitado en MariaDB
    mysql_options(conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl_mode);
    
if (mysql_real_connect(conn, "mysql-35378f5d-parqueoumg.d.aivencloud.com", "avnadmin", "COLOCAR PASSWORD", "defaultdb", 21973, NULL, 0) == NULL) {
        cout << "[ERROR SQL] Error de conexion: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    // Insertar el resumen de la jornada
    string queryResumen = "INSERT INTO jornada_cierre (dia_operacion, total_vehiculos_atendidos, monto_total_recaudado, promedio_tiempo_minutos) VALUES (" 
                          + to_string(datos.diaOperacion) + ", " 
                          + to_string(datos.totalVehiculos) + ", " 
                          + to_string(datos.montoTotal) + ", " 
                          + to_string(datos.promedioTiempo) + ")";
    
    if (mysql_query(conn, queryResumen.c_str())) {
        cout << "[ERROR SQL] Al insertar resumen: " << mysql_error(conn) << endl;
    }

    // Insertar las placas nuevas
    if (!placasNuevas.empty()) {
        for (const string& placa : placasNuevas) {
            string queryPlaca = "INSERT IGNORE INTO placas_registradas (placa) VALUES ('" + placa + "')";
            if (mysql_query(conn, queryPlaca.c_str())) {
                cout << "[ERROR SQL] Al insertar placa: " << mysql_error(conn) << endl;
            }
        }
    }

    mysql_close(conn);
    cout << "[OK] Sincronizacion con MySQL exitosa. Datos guardados en la nube." << endl;
}

// 5. LECTURA DEL ARCHIVO BINARIO Y CÁLCULOS
void procesarCierreDia(int diaObjetivo, double tasaPorHora) {
    ifstream archivo("StatusParqueo.dat", ios::binary);
    
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo abrir StatusParqueo.dat. Verifique que exista." << endl;
        return;
    }

    RegistroArchivo reg;
    int vehiculosAtendidos = 0;
    double totalRecaudado = 0.0;
    long int tiempoTotalSegundos = 0;
    vector<string> placasNuevas;

    while (archivo.read(reinterpret_cast<char*>(&reg), sizeof(RegistroArchivo))) {
        if (reg.diaRegistro == diaObjetivo && strcmp(reg.accion, "SALIDA") == 0) {
            vehiculosAtendidos++;

            long int segundosDentro = reg.vehiculo.horaSalida - reg.vehiculo.horaEntrada;
            tiempoTotalSegundos += segundosDentro;

            double horas = ceil(segundosDentro / 3600.0);
            if (horas == 0) horas = 1; 
            totalRecaudado += (horas * tasaPorHora);

            placasNuevas.push_back(reg.vehiculo.placa);
        }
    }
    archivo.close();

    if (vehiculosAtendidos == 0) {
        cout << "No hubo salidas registradas en el Dia " << diaObjetivo << "." << endl;
        return;
    }

    double promedioMinutos = (tiempoTotalSegundos / 60.0) / vehiculosAtendidos;
    DatosCierre resumen = {diaObjetivo, vehiculosAtendidos, totalRecaudado, promedioMinutos};

    generarReporteCSV(resumen);
    generarReporteHTML(resumen);

    sincronizarMySQL(resumen, placasNuevas);
}

// Main temporal solo para que pruebes tu código
int main() {
    int diaACerrar = 1; 
    double tasaUsuario = 15.0; 
    
    cout << "--- INICIANDO PROCESO DE CIERRE ---" << endl;
    procesarCierreDia(diaACerrar, tasaUsuario);
    
    return 0;
}