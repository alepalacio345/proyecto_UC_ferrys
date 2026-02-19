#include <stdio.h>

//ESTRUCTURAS//

/**
 * @struct vehiculos
 * @brief Almacena los datos de un vehículo individual para la simulación.
 * * Contiene la información desglosada del archivo de entrada (placa, tipo, peso, procedencia),
 * así como la configuración de pasajeros y boletos para el cálculo de ingresos y estadísticas.
 */
typedef struct {
    char placa[30];
    int tipo;                       // 1-7 (Liviano, Rustico, etc.)
    float peso;
    int procedencia;                // 0=Nueva Esparta, 1=Anzoátegui, 2=Otros
    // Capacidad y pasajeros
    int traslada_pasajeros;         // 0=No, 1=Si
    int Num_pasajeros_adultos;
    int Num_pasajeros_tercera_edad;
    // Tipo de pasajes (0=VIP/Primera, 1=Turista/Ejecutiva)
    int pasaje_adquirido_adut;
    int pasaje_adquirido_tercera_ed;
    // Logística
    int tiempo_llegada;             // Hora militar convertida
    int tipo_ferry;                 // 0=Express, 1=Tradicional
} vehiculos;


/**
 * @struct ferrys
 * @brief Representa un barco de la flota y su estado actual en la simulación.
 * * Gestiona las capacidades máximas (peso y vehículos), tiempos de ciclo y 
 * la máquina de estados (Carga, Viaje, Espera) para el control del flujo.
 */
typedef struct {
    char nombre[30];
    int tipo;                       // 0=Express, 1=Tradicional
    int estado;                     // 1=Carga, 2=Viaje, 3=Espera
    int capacidad_vehiculos;
    int capacidad_pasajeros;
    int tiempo_de_viaje;            // En minutos (35 o 65)
} ferrys;

int main(){

    //variables de archivos
    FILE *entrada;
    FILE *salida;

    //abrir archivos 

    entrada = fopen("no_se.txt","r");
    salida  = fopen("no_se02.txt","w");


    if(entrada == NULL){
        printf("error al abrir el archivo\n");
    }else{
        printf("welcome\n");


        //algo
    }

    //cerrar archivos 
    fclose(entrada);
    fclose(salida);

    return 0;
}