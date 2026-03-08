# ⛴️ Simulador de Flota de Ferrys (Máquina de Estados)

Este proyecto es una simulación avanzada en **C** de un sistema logístico portuario. El programa gestiona la llegada, encolamiento, abordaje, facturación y zarpe de una flota de ferrys (Tradicionales y Express) utilizando un motor de tiempo universal y manejo dinámico de estructuras de datos.

## ⚙️ Características Principales

* **Reloj Universal:** El sistema evalúa el flujo de vehículos minuto a minuto, controlando tanto las llegadas al muelle como los tiempos de viaje en altamar (35 min para Express, 65 min para Tradicional).
* **Prioridad de Emergencias (Desalojo Dinámico):** Si el ferry alcanza su límite máximo y llega un vehículo de emergencia (Ambulancia, Bomberos o Policía), el sistema expulsa automáticamente a los últimos vehículos civiles abordados, recalcula pesos y los devuelve a la primera posición de la cola.
* **Motor de Zarpe Inteligente:** Evaluaciones estrictas de reglas de salida:
    * Regla del 30% de capacidad mínima.
    * Capacidad máxima operativa (por Peso, Vehículos o Pasajeros).
    * Cierre de jornada (Zarpe forzado a las 11:59 PM).
* **Facturación Automatizada:** Cálculo de ingresos en Bolívares (BsF) aplicando estructuras de costos cruzadas (Tipo de Vehículo vs. Tipo de Ferry) y tarifas de pasajeros (VIP/Turista y Tercera Edad).

## 🛠️ Tecnologías Usadas
* Lenguaje **C** estándar.
* Lectura y escritura de archivos planos (`FILE *`).
* Arreglos de `structs` anidados.

## 📋 Formato del Archivo de Entrada (`.txt`)
El simulador se alimenta de un archivo de texto con el siguiente formato:
1.  **Primera línea:** Orden de prioridad de los 3 ferrys en el muelle (Ej: `1 2 3`).
2.  **Siguientes líneas:** Datos de los vehículos separadas por espacios:
    `[Código] [Nro. Adultos] [Nro. 3ra Edad] [Pasaje Adulto] [Pasaje 3ra Edad] [Peso/Ton] [Hora Llegada] [Placa] [Tipo Ferry]`

## 🚀 Compilación y Ejecución

Para compilar el proyecto a través de la terminal (GCC):
```bash
gcc proyecto_ferrys.c -o simulador_ferrys