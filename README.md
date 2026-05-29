<h1 align="center">
    <img src="https://readme-typing-svg.herokuapp.com/?font=Righteous&size=35&center=true&vCenter=true&width=700&height=70&duration=4000&color=1877F2&lines=Sistema+de+Control+de+Parqueo+🚗;Proyecto+Programacion+I+💻;Universidad+Mariano+Galvez+🎓" />
</h1>

<h3 align="center">Sistema de gestión de estacionamiento en C++ con sincronización MySQL en la nube ☁️</h3>

<br/>

<div align="center">

🚘 **Gestión de vehículos** con pilas y colas en C++

☁️ **Base de datos en la nube** via Aiven MySQL

📊 **Reportes automáticos** en CSV y HTML al cierre del día

🔐 **Persistencia de datos** con archivo binario `.dat`

</div>

<hr/>

<h2 align="center">⚒️ Tecnologías Utilizadas ⚒️</h2>
<br/>
<div align="center">
    <img src="https://skillicons.dev/icons?i=cpp,mysql,github,git,vscode,windows" />
</div>
<br/>
<hr/>

<h2 align="center">📂 Archivos del Proyecto</h2>
<br/>

<div align="center">

| Archivo | Descripción |
|---|---|
| `Parqueo.cpp` | Sistema principal con menú interactivo |
| `sincronizacion.cpp` | Módulo independiente de cierre y reporte |
| `StatusParqueo.dat` | Estado persistente del parqueo |

</div>

<hr/>

<h2 align="center">🗺️ Estructura del Parqueo</h2>
<br/>

```
┌──────────────────────────────────────────┐
│           PARQUEO - 3 CARRILES           │
│                                          │
│  Carril 1: [ Auto ][ Auto ][ Auto ]      │
│  Carril 2: [ Auto ][ Auto ][ Auto ]      │
│  Carril 3: [ Auto ][ Auto ][ Auto ]      │
│                                          │
│  Cola Exterior: [ ][ ][ ]...             │
└──────────────────────────────────────────┘
  Tasa: Q15.00/hora  |  Cobro por hora completa
```

<hr/>

<h2 align="center">⚙️ Compilación y Uso</h2>
<br/>

**1. Instalar dependencia (MSYS2 UCRT64):**
```bash
pacman -S mingw-w64-ucrt-x86_64-libmariadbclient
```

**2. Compilar:**
```bash
g++ Parqueo.cpp -o Parqueo.exe -I/ucrt64/include/mariadb -L/ucrt64/lib -lmariadb
```

**3. Ejecutar:**
```bash
./Parqueo.exe
```

<hr/>

<h2 align="center">📋 Menú del Sistema</h2>
<br/>

```
    CONTROL DE PARQUEO  |  DIA: 1
1. Registrar Ingreso de Vehiculo
2. Registrar Salida de Vehiculo
3. Buscar Vehiculo por Placa
4. Ver Estado de los Espacios (Mapa)
5. Avanzar de Dia
6. Cierre del Dia (Reporte + MySQL)
7. Cerrar
```

<hr/>

<h2 align="center">👨‍💻 Autores</h2>
<br/>
<div align="center">

| Nombre | Rol |
|---|---|
| Hemerson René López Hernández 7690-25-4844 |
| José Samuel Sigüina Chonay 7690-25-8574 |
| Franco Paolo López Gálvez 7690-25-5523 |
| Anderson Aldana González 7690-25-2440 |

</div>

<br/>

<div align="center">
    <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
    <img src="https://img.shields.io/badge/MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white" />
    <img src="https://img.shields.io/badge/Aiven-FF3E00?style=for-the-badge&logo=aiven&logoColor=white" />
    <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" />
</div>

<br/>
