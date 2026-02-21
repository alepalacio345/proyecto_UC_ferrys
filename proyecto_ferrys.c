#include <stdio.h>
#include <string.h>

//CONSTANTES//
#define MAX_VEHICULOS 100

//ESTRUCTURAS//

/**
 * @struct vehiculos
 * @brief Almacena los datos de un v
 * ehículo individual para la simulación.
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

//PROTIPADO DE FUNCIONES//
void cargar_vehiculos(FILE *entrada,vehiculos vector_vehiculos[]);


int main(){

    //variables de archivos
    FILE *entrada;
    FILE *salida;

    //var normales
    int orden[3];

    //structuras//
    vehiculos vector_vehiculos[MAX_VEHICULOS];

    //abrir archivos 
    entrada = fopen("ejemplo_entrada.txt","r");
    salida  = fopen("proy1.out","w");


    if(entrada == NULL){
        printf("error al abrir el archivo\n");
    }else{
        printf("welcome\n");

        //orden de carga de ferrys
        fscanf(entrada,"%i %i %i\n",&orden[0],&orden[1],&orden[2]);
        printf("%i %i %i\n",orden[0],orden[1],orden[2]);

        //llamr a al procedimiento cargar_vehiculos
        cargar_vehiculos(entrada,vector_vehiculos);


    }

    //cerrar archivos 
    puts("Melovax");
    fclose(entrada);
    fclose(salida);

    return 0;
}

//FUNCIONES//

/**
 * @brief Lee los datos del archivo de texto y puebla el arreglo de vehículos.
 * * Esta función recorre línea por línea el archivo de entrada hasta llegar al final (EOF).
 * Extrae la información de cada vehículo (pasajeros, peso, hora, placa, etc.) según 
 * el formato establecido en el proyecto y la guarda ordenadamente en las posiciones 
 * consecutivas del arreglo de estructuras.
 * * @param entrada Puntero al archivo de texto (previamente abierto en modo lectura).
 * @param vector_vehiculos Arreglo de estructuras tipo `vehiculos` que será llenado con los datos.
 */
void cargar_vehiculos(FILE *entrada, vehiculos vector_vehiculos[]){

    //var normales
    int codigo_vehiculo;
    int tipo_vehiculo;
    int origen_vehiculo;
    int posee_pasajeros;
    int numero_pasajeros_adut;
    int numero_pasajeros_TCED;
    int tipo_pasaje_adut;
    int tipo_pasaje_TCED;
    int peso;
    int Hora_llegada;
    char placa[15];
    int tipo_ferry;

    int indice = 0;

    //recorrer el archivo hasta fin de archivo 
    while(fscanf(entrada,"%i %i %i %i %i %i %i %s %i\n",&codigo_vehiculo,
                            &numero_pasajeros_adut,&numero_pasajeros_TCED,&tipo_pasaje_adut,
                            &tipo_pasaje_TCED,&peso,&Hora_llegada,placa,&tipo_ferry) != EOF){
        //llenar vector
        vector_vehiculos[indice].tipo = codigo_vehiculo / 100;
        vector_vehiculos[indice].procedencia = (codigo_vehiculo / 10) % 10;
        vector_vehiculos[indice].traslada_pasajeros = codigo_vehiculo % 10;
        vector_vehiculos[indice].Num_pasajeros_adultos = numero_pasajeros_adut;
        vector_vehiculos[indice].Num_pasajeros_tercera_edad = numero_pasajeros_TCED;
        vector_vehiculos[indice].pasaje_adquirido_adut = tipo_pasaje_adut;
        vector_vehiculos[indice].pasaje_adquirido_tercera_ed = tipo_pasaje_TCED;
        vector_vehiculos[indice].peso = (float)peso;
        vector_vehiculos[indice].tiempo_llegada = Hora_llegada;
        strcpy(vector_vehiculos[indice].placa,placa);
        vector_vehiculos[indice].tipo_ferry = tipo_ferry;

        printf("%i %i %i %i %i %i %i %i %0.f %i %s %i\n",
                codigo_vehiculo,
                vector_vehiculos[indice].tipo,
                vector_vehiculos[indice].procedencia,
                vector_vehiculos[indice].traslada_pasajeros,
                vector_vehiculos[indice].Num_pasajeros_adultos,
                vector_vehiculos[indice].Num_pasajeros_tercera_edad,
                vector_vehiculos[indice].pasaje_adquirido_adut,
                vector_vehiculos[indice].pasaje_adquirido_tercera_ed,
                vector_vehiculos[indice].peso,
                vector_vehiculos[indice].tiempo_llegada,
                vector_vehiculos[indice].placa,
                vector_vehiculos[indice].tipo_ferry);
        
        //acumentar indice
        indice++;

    }
}