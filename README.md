# Sistema de Control de Parqueo

Proyecto de Programación I — Universidad Mariano Gálvez  
Desarrollado en C++ con sincronización a base de datos MySQL en la nube (Aiven).

---

## Descripción

Sistema de gestión de un estacionamiento que permite registrar ingresos y salidas de vehículos, visualizar el estado del parqueo en tiempo real, y generar reportes de cierre diario sincronizados con una base de datos en la nube.

---

## Archivos

| Archivo | Descripción |
|---|---|
| `Parqueo.cpp` | Sistema principal con menú interactivo |
| `sincronizacion.cpp` | Módulo independiente de cierre y sincronización MySQL |
| `StatusParqueo.dat` | Archivo binario de estado del parqueo |

---

## Funcionalidades

- Registrar ingreso de vehículos (placa, marca, modelo)
- Registrar salida de vehículos con cálculo de tiempo y cobro
- Buscar vehículo por placa con historial de movimientos
- Ver mapa del estado actual de carriles y cola de espera
- Avanzar de día
- **Cierre del día:** genera reporte CSV, reporte HTML y sincroniza con MySQL en la nube

---

## Estructura del Parqueo

- 3 carriles con capacidad de 3 vehículos cada uno (implementados con pilas)
- Cola de espera exterior para cuando el parqueo está lleno
- Tasa de cobro: Q15.00 por hora (se cobra la hora completa)

---

## Compilación

Requiere tener instalado `libmariadbclient` via MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-libmariadbclient
```

Compilar:

```bash
g++ Parqueo.cpp -o Parqueo.exe -I/ucrt64/include/mariadb -L/ucrt64/lib -lmariadb
```

Ejecutar:

```bash
./Parqueo.exe
```

---

## Base de Datos

Conectado a MySQL en la nube via **Aiven**.  
Tablas utilizadas:

- `jornada_cierre` — resumen diario de operaciones
- `placas_registradas` — registro de placas atendidas

---

## Autores

- José Samuel Sigüina Chonay
- Franco Paolo Loza
