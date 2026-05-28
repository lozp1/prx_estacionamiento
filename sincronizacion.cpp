#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath> // Necesario para la función ceil()

using namespace std;

// 1. ESTRUCTURAS EXACTAS DE Parqueo.cpp
// Si no son idénticas, la lectura del binario arrojará basura en memoria.
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
    double promedioTiempo; // en minutos
};

// 3. GENERACIÓN DE REPORTES (CSV y HTML)
void generarReporteCSV(DatosCierre datos) {
    string nombreArchivo = "Reporte_Dia_" + to_string(datos.diaOperacion) + ".csv";
    ofstream archivo(nombreArchivo);
    
    if(archivo.is_open()) {
        // Cabeceras del CSV
        archivo << "Dia,Vehiculos Atendidos,Total Recaudado,Promedio Tiempo (min)\n";
        // Datos
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
        archivo << "<!DOCTYPE html>\n<html>\n<head>\n<title>Reporte Dia " << datos.diaOperacion << "</title>\n";
        archivo << "<style>table { border-collapse: collapse; width: 50%; margin-top: 20px; } th, td { border: 1px solid black; padding: 8px; text-align: center; } th { background-color: #f2f2f2; }</style>\n";
        archivo << "</head>\n<body>\n";
        archivo << "<h2>Resumen de Jornada - Dia " << datos.diaOperacion << "</h2>\n";
        archivo << "<table>\n<tr><th>Vehiculos Atendidos</th><th>Monto Recaudado (Q)</th><th>Promedio Tiempo (min)</th></tr>\n";
        archivo << "<tr><td>" << datos.totalVehiculos << "</td><td>" << datos.montoTotal << "</td><td>" << datos.promedioTiempo << "</td></tr>\n";
        archivo << "</table>\n</body>\n</html>";
        archivo.close();
        cout << "[OK] Reporte HTML generado: " << nombreArchivo << endl;
    } else {
        cout << "[ERROR] No se pudo crear el archivo HTML." << endl;
    }
}

// 4. LECTURA DEL ARCHIVO BINARIO Y CÁLCULOS
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

    // Recorremos el archivo de principio a fin
    while (archivo.read(reinterpret_cast<char*>(&reg), sizeof(RegistroArchivo))) {
        // Solo nos interesan los registros de SALIDA del día que estamos cerrando
        if (reg.diaRegistro == diaObjetivo && strcmp(reg.accion, "SALIDA") == 0) {
            vehiculosAtendidos++;

            long int segundosDentro = reg.vehiculo.horaSalida - reg.vehiculo.horaEntrada;
            tiempoTotalSegundos += segundosDentro;

            // El cálculo se realiza cobrando la hora o fracción de hora [cite: 35]
            double horas = ceil(segundosDentro / 3600.0);
            if (horas == 0) horas = 1; // Cobro mínimo de 1 hora
            totalRecaudado += (horas * tasaPorHora);

            // Almacenamos la placa para luego enviarla a MySQL
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

    // TODO: Llamar a sincronizarMySQL(resumen, placasNuevas);
}

// Main temporal solo para que pruebes tu código aislado del resto
int main() {
    int diaACerrar = 1; // Puedes cambiar esto según el día que quieras probar
    double tasaUsuario = 15.0; // Q15 por hora
    
    cout << "--- INICIANDO PROCESO DE CIERRE ---" << endl;
    procesarCierreDia(diaACerrar, tasaUsuario);
    
    return 0;
}