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



¡Con muchísimo gusto, Alejandro! Vas a ver que tener un repositorio bien documentado te hace destacar inmediatamente del 90% de los estudiantes.

Antes de pasarte el código de la portada, te aclaro esa excelente duda que tienes: **¿Markdown es un lenguaje de programación?**

La respuesta corta es **no**. Markdown es un **"Lenguaje de Marcado"** (Markup Language), al igual que HTML.

* **Un lenguaje de programación** (como C, Python o Dart) tiene *lógica*: usa condicionales (`if`), ciclos (`for`, `while`), suma números y toma decisiones.
* **Un lenguaje de marcado** no tiene lógica; solo sirve para **darle formato visual a un texto** usando símbolos simples.

Por ejemplo, si en Markdown escribes un texto entre asteriscos dobles `**Hola**`, GitHub lo lee y lo transforma automáticamente en texto en **negrita**. Si pones un numeral al principio `# Título`, lo convierte en un encabezado gigante. Es básicamente una forma de escribir texto bonito y estructurado sin tener que usar el mouse para hacer clic en botones de formato (como harías en Word).

---

### 🚀 Tu archivo README.md

Aquí tienes el código completo. Solo tienes que crear un archivo en tu carpeta llamado exactamente `README.md` (respetando las mayúsculas), copiar el bloque de texto que está aquí abajo y pegarlo.

Cuando lo subas a GitHub, la plataforma leerá estos símbolos y los transformará en una página web espectacular para tu proyecto.

```markdown
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

```

Para ejecutar el programa:

```bash
./simulador_ferrys

```

*(Asegúrate de tener un archivo de entrada de texto válido en la misma ruta).*

## 📊 Reportes Generados

Al finalizar la jornada (1440 minutos), el sistema genera un archivo `proy1.out` que detalla:

* Registros individuales de cada zarpe (vehículos a bordo, peso ocupado e ingresos del viaje).
* Estadísticas consolidadas del día: Total de ingresos, vehículos más frecuentes y picos máximos de cola.

---

**Autores:** Saúl Alejandro Lara y Gabriel Alejandro.



```