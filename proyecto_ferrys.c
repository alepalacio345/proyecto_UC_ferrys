/*
Integrantes: Saul Lara y Gabriel Rosario
C.I: 31.456.395, 31.165.402
Seccion: 06
Materia: Fundamentos de Programacion
Prof: Brigido Noguera
*/

/**
 * @file simulacion_ferry.c
 * @author Saul Lara & Gabriel Rosario
 * @brief Programa de simulación para el control de carga y zarpe de ferrys.
 * @version 1.0
 * @date 2026-03-17
 * * @copyright Copyright (c) 2026
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//CONSTANTES//
#define MAX_VEHICULOS 1000
#define MAX_FERRYS 3
#define MAX_CANTIDAD_COLAS 1000

// Capacidades Vehiculares (100%)
#define CAP_LILIA 16
#define CAP_ISABELA 20
#define CAP_MARGARITENA 18

// TARIFAS DE PASAJES (TRADICIONAL)
#define T_ADULTO_VIP_TRAD 370.00f
#define T_ADULTO_TUR_TRAD 290.00f
#define T_MAYOR_VIP_TRAD 190.50f
#define T_MAYOR_TUR_TRAD 150.50f

// TARIFAS DE PASAJES (EXPRESS)
#define T_ADULTO_VIP_EXP 1020.00f
#define T_ADULTO_TUR_EXP 620.00f
#define T_MAYOR_VIP_EXP 520.00f
#define T_MAYOR_TUR_EXP 320.00f

// TARIFAS DE VEHÍCULOS (TRADICIONAL)
#define V_LIVIANO_TRAD 590.00f
#define V_RUSTICO_TRAD 710.00f
#define V_MICROBUS_TRAD 850.00f
#define V_CARGA_TRAD 1200.00f

// TARIFAS DE VEHÍCULOS (EXPRESS)
#define V_LIVIANO_EXP 1090.00f
#define V_RUSTICO_EXP 1310.00f
#define V_MICROBUS_EXP 1600.00f
#define V_CARGA_EXP 2400.00f

//ESTRUCTURAS//
    
/**
 * @struct vehiculos
 * @brief Almacena los datos de un vehículo individual para la simulación.
 * Contiene la información desglosada del archivo de entrada (placa, tipo, peso, procedencia),
 * así como la configuración de pasajeros y boletos para el cálculo de ingresos y estadísticas.
 */
typedef struct {
    char placa[30];
    int tipo;                       // 1-7 (Liviano, Rustico, etc.)
    float peso;                     //
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
 * Gestiona las capacidades máximas (peso y vehículos), tiempos de ciclo y 
 * la máquina de estados (Carga, Viaje, Espera) para el control del flujo.
 */
typedef struct {
    char nombre[30];
    int tipo;                       // 0=Express, 1=Tradicional
    int estado;                     // 1=Carga, 2=Viaje, 3=Espera
    int tiempo_de_viaje;            // En minutos (35 o 65)
    int hora_salida;                // hora a la que zarpa
    float peso_actual;

    int peso_maximo;
    int cap_max_vehiculos;  //capacidad maxima de vehiculos
    int cap_min_vehiculos;  //capacidad minima de vehiculos
    int cap_pasajeros;      //capacidad maxima de personas

    int vehiculos_actuales;  //total vehiculos
    int pasajeros_actuales;  // total pasajeros
    vehiculos info_vehiculos_actuales[MAX_VEHICULOS]; // vector de registros para guardar la info de los carros 
} ferrys;

//PROTIPADO DE FUNCIONES//
void cargar_vehiculos(FILE *entrada, vehiculos vector_vehiculos[],int *cantidad_vehiculos);
void cargar_ferrys(int orden[], ferrys vector_ferrys[]);
int convertir_a_minutos(int hora_militar);
void ordenar_por_hora(vehiculos vector[], int n);
void reiniciar_tiempo_de_viaje(ferrys vector_ferrys[],int tiempo_actual);
void actualizar_rampa(bool *rampa_libre, int tiempo_universal, int *tiempo_carga);
void manejar_emergencia(ferrys *ferry, vehiculos cola[], int *indice_cola);
void cargar_vehiculo_en_ferry(ferrys *ferry,vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga,int tiempo_universal);
bool hacer_espacio_emergencia(ferrys *ferry_actual, vehiculos cola_actual[], int *indice_cola);
void insertar_emergencia_cola(vehiculos cola[], int *indice, vehiculos nuevo);
bool debe_zarpar(ferrys ferry_actual,int vehiculos_en_ferry,int vehiculos_en_cola,int pasajeros_actuales,float peso_actual,vehiculos siguiente_en_cola,int tiempo_universal);
void procesar_carga_ferry(ferrys vector_ferrys[],int turno_ferry, vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga, int tiempo_universal,FILE *salida,int *numero_viaje_global,int *total_dia_vehiculos,int *total_dia_pasajeros,float *total_dia_ingresos,int frecuencia_tipos[], int *turno_control);
void procesar_salida(ferrys ferry_actual, FILE *salida, int numero_viaje, int *dia_total_vehiculos, int *dia_total_pasajeros, float *dia_total_ingresos, int frec_vehiculos[]);
void limpiar_consola();

int main(){
    //variables de archivos
    FILE *entrada;
    FILE *salida;

    //var normales
    int orden[3];

    int turno_ferry = 0; //iniciar en el primero y al zarpar incrementar

    int indice_tradicional = 0;
    int indice_express = 0;    
    bool rampa_libre  = true; //rampa_libre (para no confundir bandera con otra cosa)
    int cantidad_vehiculos;  //var acumuladora
    int tiempo_universal = 0; //tiempo de todo el dia
    int tiempo_carga = 0; //tiempo de todo el dia

    // Variables Acumuladoras Globales para el reporte final
    int numero_viaje_global = 1;
    int total_dia_vehiculos = 0;
    int total_dia_pasajeros = 0;
    float total_dia_ingresos = 0.0;
    int frecuencia_tipos[8] = {0}; 
    int mayor_frecuencia_espera = 0;
    int hora_mayor_espera = 0;
    bool es_emergencia;

    //Estructuras//
    vehiculos vector_vehiculos[MAX_VEHICULOS];
    vehiculos vector_colaExpress[MAX_CANTIDAD_COLAS];
    vehiculos vector_colaTradicional[MAX_CANTIDAD_COLAS]; //Estaba como vector_colaTradiccional
    ferrys vector_ferrys[MAX_FERRYS];

    // Apertura y validación de flujos de datos (Entrada/Salida)
    entrada = fopen("proy1.in","r");
    salida  = fopen("proy1.out","w");

    limpiar_consola(); // limpiar mensajes anteriores de la pantalla

    if(entrada == NULL){
        printf("error al abrir el archivo\n");
        return 1;
    }else{

        // ========================================================
        //mensaje de bienvenida 
        printf("\n");
        printf("  ======================================================\n");
        printf("  |                                                    |\n");
        printf("  |               FERRYS MARGARITA                     |\n");
        printf("  |      Simulador de Operaciones Navieras v1.0        |\n");
        printf("  |               El Margariteno C.A.                  |\n");
        printf("  |                                                    |\n");
        printf("  ======================================================\n");
        printf("          \\                           /                 \n");
        printf("           \\_________________________//                 \n");
        printf("     _______|_______|_______|_______|_______            \n");
        printf("    \\                                       /           \n");
        printf("  ~~~\\_____________________________________/~~~         \n");
        printf("       ~~~~       ~~~~~        ~~~~      ~~~~           \n");
        printf("  ======================================================\n\n");
        printf("  [+] Iniciando reloj universal...\n");
        printf("  [+] Abriendo terminal de pasajeros...\n\n");
        // ========================================================

        // 1. Validar que el archivo no esté vacío y tenga 3 números
        if(fscanf(entrada,"%i %i %i\n",&orden[0],&orden[1],&orden[2]) != 3){
            printf("Error: El archivo proy1.in esta vacio o la primera linea es invalida.\n");
            exit(1);
        }

        // 2. Validar que los IDs sean exactamente 1, 2 y 3 sin repetirse
        int check_ferrys[4] = {0};
        for(int i = 0; i < 3; i++){
            if(orden[i] < 1 || orden[i] > 3 || check_ferrys[orden[i]] == 1){
                printf("Error: El orden inicial de los ferrys debe contener los ID 1, 2 y 3 sin repetirse.\n");
                fclose(entrada);
                fclose(salida);
                exit(1);
            }
            check_ferrys[orden[i]] = 1; // Marcamos el ID como usado
        }

        // Configuración inicial de la flota y lectura del inventario de vehículos
        cargar_ferrys(orden,vector_ferrys);
        cargar_vehiculos(entrada,vector_vehiculos,&cantidad_vehiculos);

        // Normalización de datos: Asegura que el procesamiento sea cronológico
        //Ordena los vehiculos por tiempo de llegada para facilitar la simulacion
        ordenar_por_hora(vector_vehiculos, cantidad_vehiculos);
        
        if(cantidad_vehiculos > 0){
            tiempo_universal = vector_vehiculos[0].tiempo_llegada;
        }
        
        /**
         * @brief Punto de entrada principal de la simulación.
         * @details Gestiona el ciclo de vida de un día de operación (00:00 - 23:59). 
         * Orquestra la lectura de archivos, la clasificación de vehículos en colas,
         * el control de tiempos de los ferrys y la generación de reportes finales.
         * @return int 0 si la ejecución fue exitosa, 1 en caso de error de archivos.
         */

        // Ciclo que simula el paso del dia minuto a minuto (hasta las 11:59 PM = 1439 mins)
        
        while(tiempo_universal <= 1439){
            // --- Fase 1: Gestión de Arribos ---
            // Clasifica vehículos entrantes en sus respectivas colas (Express o Tradicional)
            // priorizando servicios de emergencia (Ambulancia, Bomberos, Policía).
            int i = 0; //control del ciclo
            while(i < cantidad_vehiculos){
                // Si la hora de llegada del vehiculo coincide con el reloj actual
                if(vector_vehiculos[i].tiempo_llegada == tiempo_universal){
                    
                    es_emergencia = (vector_vehiculos[i].tipo == 5) || (vector_vehiculos[i].tipo == 6) || (vector_vehiculos[i].tipo == 7);

                    if(es_emergencia){
                        vector_vehiculos[i].tipo_ferry = vector_ferrys[turno_ferry].tipo;
                    }

                    // Verificamos a qué cola va
                    if(vector_vehiculos[i].tipo_ferry){
                        //COLA DEL TRADICIONAL
                        if((vector_vehiculos[i].tipo == 5) || (vector_vehiculos[i].tipo == 6) || (vector_vehiculos[i].tipo == 7)){

                            //movemos toda la fila una posicion asi la derecha
                            if(indice_tradicional >= MAX_CANTIDAD_COLAS){
                                printf("Cola tradicional llena, no se puede insertar emergencia\n");
                            }else{
                                //Emergencia
                                insertar_emergencia_cola(vector_colaTradicional,&indice_tradicional, vector_vehiculos[i]);
                            }
                        }else{
                            // Va a la tradicional.
                            //validando cantidad tradicional
                            if(indice_tradicional >= MAX_CANTIDAD_COLAS){
                                printf("Cola tradicional llena. Vehiculo %s ignorado\n", vector_vehiculos[i].placa);
                            }else{
                                vector_colaTradicional[indice_tradicional] = vector_vehiculos[i];
                                indice_tradicional++; // Solo avanza si metimos un carro
                            }
                        }

                    }else{
                        //COLA DEL EXPRESS
                        if((vector_vehiculos[i].tipo == 5) || (vector_vehiculos[i].tipo == 6) || (vector_vehiculos[i].tipo == 7)){

                            //movemos toda la fila una posicion asi la derecha
                            if(indice_express >= MAX_CANTIDAD_COLAS){
                                printf("Cola express llena, no se puede insertar emergencia\n");
                            }else{
                                insertar_emergencia_cola(vector_colaExpress,&indice_express,vector_vehiculos[i]);
                                //ponemos a el vehiculo con prioridad de primero
                            }
                        }else{
                            //Tipo_ferry == 0
                            // Va a la express. 
                            //validando cantidad express
                            if(indice_express >= MAX_CANTIDAD_COLAS){
                                printf("Cola express llena. Vehiculo %s ignorado\n", vector_vehiculos[i].placa);
                            }else{
                                vector_colaExpress[indice_express] = vector_vehiculos[i];
                                indice_express++; // Solo avanza si metimos un carro
                            }
                        }
                    }
                }
                //CONTADOR DEL CICLO PRINCIPAL
                i++;
            }


            // Actualizar estadística de mayor frecuencia en la cola
            int total_en_cola = indice_tradicional + indice_express;
            if (total_en_cola > mayor_frecuencia_espera) {
                mayor_frecuencia_espera = total_en_cola;
                hora_mayor_espera = tiempo_universal;
            }


            // =========================================================
            // --- Fase 2: Mantenimiento de Flota ---
            // Libera los ferrys que regresan del viaje para que queden disponibles en muelle.
            // =========================================================

            //Preparar para cargar
            if(vector_ferrys[turno_ferry].estado == 3){
                vector_ferrys[turno_ferry].estado = 1;
            }

            //llamos a la funcion reiniciar_tiempo_de_viaje
            reiniciar_tiempo_de_viaje(vector_ferrys,tiempo_universal);


            // =========================================================
            // --- Fase 3: Operaciones de Carga ---
            // Si el ferry en turno está en muelle, intenta cargar vehículos o zarpar 
            // según las restricciones de peso, capacidad y rampa.
            // =========================================================
            
            //validar que el ferry este listo para cargar
            if(vector_ferrys[turno_ferry].estado == 1){

                //condicional para saber que tipo de ferry esta en el muelle (tradicional o express)
                int tipo_ferry_muelle = vector_ferrys[turno_ferry].tipo;
                if(tipo_ferry_muelle){ 
                    // =========================================================
                    // LÓGICA SI EL FERRY EN EL MUELLE ES TRADICIONAL
                    // =========================================================
                    //Llamar funcion para tradicional

                    procesar_carga_ferry(vector_ferrys,turno_ferry,vector_colaTradicional,&indice_tradicional,
                        &rampa_libre,&tiempo_carga,tiempo_universal,salida,&numero_viaje_global,&total_dia_vehiculos,
                        &total_dia_pasajeros,&total_dia_ingresos,frecuencia_tipos,&turno_ferry);
                }else{
                    // =========================================================
                    // LÓGICA SI EL FERRY EN EL MUELLE ES EXPRESS
                    // =========================================================
                    //Llamar funcion para express

                    procesar_carga_ferry(vector_ferrys,turno_ferry,vector_colaExpress,&indice_express,
                        &rampa_libre,&tiempo_carga,tiempo_universal,salida,&numero_viaje_global,&total_dia_vehiculos,
                        &total_dia_pasajeros,&total_dia_ingresos,frecuencia_tipos,&turno_ferry);

                }//PROCESAR CARGA FERRYS
            }

            // --- Control de Parada del Sistema ---
            // Verifica si aún quedan vehículos pendientes por llegar o procesar

            bool quedan_por_llegar = false;
            int i_checkear = 0; // Variable para recorrer el vector cada minuto

            while (i_checkear < cantidad_vehiculos) {
                if (vector_vehiculos[i_checkear].tiempo_llegada > tiempo_universal) {
                    quedan_por_llegar = true;
                    break; // Encontramos uno en el futuro, podemos salir del while
                }
                i_checkear++;
            }
            

            // =========================================================
            // --- CONDICIÓN DE CIERRE ---
            // Solo terminamos el día si se cumplen TODAS estas condiciones:
            // 1. No hay autos en ninguna cola de espera.
            // =========================================================
            if(indice_tradicional == 0 &&
               indice_express == 0 &&
                !quedan_por_llegar && 
               vector_ferrys[turno_ferry].vehiculos_actuales < vector_ferrys[turno_ferry].cap_min_vehiculos){
                
                printf("\n--- Simulacion finalizada: No hay mas operaciones pendientes ---\n");
                break;
            }
                        
            //EL TIEMPO AVANZA
            tiempo_universal++;
           
        }

        // =========================================================
        // RESUMEN Y ESTADÍSTICAS FINALES DEL DÍA
        // =========================================================
        int pasajeros_varados = 0;
        for(int i = 0; i < indice_tradicional; i++){
            pasajeros_varados += vector_colaTradicional[i].Num_pasajeros_adultos + vector_colaTradicional[i].Num_pasajeros_tercera_edad; 
        }
        for(int i = 0; i < indice_express; i++){
            pasajeros_varados += vector_colaExpress[i].Num_pasajeros_adultos + vector_colaExpress[i].Num_pasajeros_tercera_edad; 
        }

        // 2. Contar los que se quedaron ATRAPADOS DENTRO de los ferrys que no zarparon
        for(int f = 0; f < MAX_FERRYS; f++){
            if(vector_ferrys[f].estado != 2 && vector_ferrys[f].vehiculos_actuales > 0){
                for(int i = 0; i < vector_ferrys[f].vehiculos_actuales; i++){
                    pasajeros_varados += vector_ferrys[f].info_vehiculos_actuales[i].Num_pasajeros_adultos + 
                                         vector_ferrys[f].info_vehiculos_actuales[i].Num_pasajeros_tercera_edad + 1;
                }
            }
        }


        int max_freq = 0;
        int tipo_mas_frecuente = 1;
        for(int i = 1; i <= 7; i++){
            if(frecuencia_tipos[i] > max_freq){
                max_freq = frecuencia_tipos[i];
                tipo_mas_frecuente = i;
            }
        }
        
        char str_tipo_frecuente[30];
        switch(tipo_mas_frecuente) {
            case 1: strcpy(str_tipo_frecuente, "liviano"); break;
            case 2: strcpy(str_tipo_frecuente, "rustico"); break;
            case 3: strcpy(str_tipo_frecuente, "microbus"); break;
            case 4: strcpy(str_tipo_frecuente, "carga"); break;
            case 5: strcpy(str_tipo_frecuente, "ambulancia"); break;
            case 6: strcpy(str_tipo_frecuente, "bomberos"); break;
            case 7: strcpy(str_tipo_frecuente, "policial"); break;
        }

        int hora_militar_espera_h = hora_mayor_espera / 60;
        int hora_militar_espera_m = hora_mayor_espera % 60;

        printf("Estadisticas:\n");
        printf("Total vehiculos transportados: %d\n", total_dia_vehiculos);
        printf("Total pasajeros transportados: %d\n", total_dia_pasajeros);
        printf("Total de ingresos: %.2f BsF.\n", total_dia_ingresos);
        printf("Num. pasajeros no trasladados: %d\n", pasajeros_varados);
        printf("Vehiculo mas frecuente: %s\n", str_tipo_frecuente);
        printf("Mayor frec. vehiculos en espera: %d (a las %02d:%02d)\n", mayor_frecuencia_espera, hora_militar_espera_h, hora_militar_espera_m);

        fprintf(salida, "Estadisticas:\n");
        fprintf(salida, "Total vehiculos transportados: %d\n", total_dia_vehiculos);
        fprintf(salida, "Total pasajeros transportados: %d\n", total_dia_pasajeros);
        fprintf(salida, "Total de ingresos: %.2f BsF.\n", total_dia_ingresos);
        fprintf(salida, "Num. pasajeros no trasladados: %d\n", pasajeros_varados);
        fprintf(salida, "Vehiculo mas frecuente: %s\n", str_tipo_frecuente);
        fprintf(salida, "Mayor frec. vehiculos en espera: %d (a las %02d:%02d)\n", mayor_frecuencia_espera, hora_militar_espera_h, hora_militar_espera_m);

    }

    
    //Cerrar archivos 
    // El día ha terminado. Todos los pasajeros (que pudieron) llegaron a su destino.
    fclose(entrada);
    fclose(salida);

    return 0;
}

//FUNCIONES//

/**
 * @brief Convierte una hora en formato militar a minutos totales
 * * @param hora_militar La hora de llegada del vehículo leída del archivo en formato militar (HHMM o HMM).
 * @return int El tiempo total transcurrido en minutos
 */
int convertir_a_minutos(int hora_militar){
    int horas = hora_militar / 100;
    int minutos = hora_militar % 100;
    
    int minutos_totales = (horas * 60) + minutos;
    
    return minutos_totales;
}

/**
 * @brief Inicializa y ordena los ferrys según la secuencia leída del archivo.
 * * @param orden Arreglo con los IDs de los ferrys en su orden de llegada (1, 2 o 3).
 * @param vector_ferrys Arreglo destino donde se guardarán los ferrys ya ordenados.
 */
void cargar_ferrys(int orden[], ferrys vector_ferrys[]){

    // Catálogo base con los datos fijos de los 3 ferrys
    ferrys vectores_auxiliar[3]; 

    // Ferry ID 1 (Express)
    strcpy(vectores_auxiliar[0].nombre, "Lilia Concepcion");
    vectores_auxiliar[0].tiempo_de_viaje = 35;
    vectores_auxiliar[0].peso_maximo = 60;
    vectores_auxiliar[0].tipo = 0; 
    vectores_auxiliar[0].estado = 3;
    vectores_auxiliar[0].cap_max_vehiculos = CAP_LILIA;
    // Se suma 0.9 para forzar el redondeo matemático hacia arriba (Ej: 4.8 pasa a ser 5)
    vectores_auxiliar[0].cap_min_vehiculos = (int)((CAP_LILIA * 0.30) + 0.9); 
    vectores_auxiliar[0].cap_pasajeros = 50;

    // Ferry ID 2 (Tradicional)
    strcpy(vectores_auxiliar[1].nombre, "La Isabela");
    vectores_auxiliar[1].cap_max_vehiculos = CAP_ISABELA;
    vectores_auxiliar[1].cap_min_vehiculos = (int)((CAP_ISABELA * 0.30) + 0.9);
    vectores_auxiliar[1].cap_pasajeros = 70;
    vectores_auxiliar[1].peso_maximo = 80;

    // Ferry ID 3 (Tradicional)
    strcpy(vectores_auxiliar[2].nombre, "La Margaritena");
    vectores_auxiliar[2].cap_max_vehiculos = CAP_MARGARITENA;
    vectores_auxiliar[2].cap_min_vehiculos = (int)((CAP_MARGARITENA * 0.30) + 0.9);
    vectores_auxiliar[2].cap_pasajeros = 60;
    vectores_auxiliar[2].peso_maximo = 80;

    for(int j = 1; j <= 2;j++){
        vectores_auxiliar[j].tiempo_de_viaje = 65;
        vectores_auxiliar[j].tipo = 1;
        vectores_auxiliar[j].estado = 3;
    }

    //inicializar peso_actual, hora_salida, pasajeros y vehiculos
    for(int i = 0;i < 3;i++){
        vectores_auxiliar[i].hora_salida = 0;
        vectores_auxiliar[i].peso_actual = 0; 
        vectores_auxiliar[i].pasajeros_actuales = 0; 
        vectores_auxiliar[i].vehiculos_actuales = 0;
    }

    // Asignación al arreglo final según el ticket de 'orden'
    for(int i = 0; i < 3; i++) {
        int id_ferry = orden[i]; 
        
        // Se resta 1 porque los IDs son 1-3, pero los índices son 0-2
        int indice_catalogo = id_ferry - 1; 

        // Se copia el struct completo a su posición definitiva
        vector_ferrys[i] = vectores_auxiliar[indice_catalogo];
    }
}


/**
 * @brief Lee los datos del archivo de texto y puebla el arreglo de vehículos.
 */
void cargar_vehiculos(FILE *entrada, vehiculos vector_vehiculos[],int *cantidad_vehiculos){

    //var normales
    int codigo_vehiculo;
    int numero_pasajeros_adut;
    int numero_pasajeros_TCED;
    int tipo_pasaje_adut;
    int tipo_pasaje_TCED;
    float peso; //GR estaba en int peso
    int Hora_llegada;
    char placa[30];
    int tipo_ferry;

    int indice = 0;
    //inicializar contador
    *cantidad_vehiculos = 0;

    //recorrer el archivo hasta fin de archivo 
    while(fscanf(entrada,"%i %i %i %i %i %f %i %29s %i",&codigo_vehiculo,
                 &numero_pasajeros_adut,&numero_pasajeros_TCED,&tipo_pasaje_adut,
                 &tipo_pasaje_TCED,&peso,&Hora_llegada,placa,&tipo_ferry) == 9){
        
        if(indice >= MAX_VEHICULOS){
            printf("Advertencia: se excedio la capacidad de vehiculos\n");
            break;
        }

        ////Validar tipo de vehiculos
        int tipo = codigo_vehiculo / 100;

        if(tipo < 1 || tipo > 7) continue;//ignorar vehiculo invalido
        vector_vehiculos[indice].tipo = tipo;

        int proced = vector_vehiculos[indice].procedencia = (codigo_vehiculo / 10) % 10;
        if(proced < 0 || proced > 2) continue;//Procedencia invalida
        
        //llenar sin validar 
        vector_vehiculos[indice].traslada_pasajeros = codigo_vehiculo % 10;
        
        //Evitar pasajeros negativos
        if(numero_pasajeros_adut < 0 || numero_pasajeros_adut > 20) numero_pasajeros_adut = 0;
        vector_vehiculos[indice].Num_pasajeros_adultos = numero_pasajeros_adut;
        
        //Evitar pasajeros negativos TCED
        if(numero_pasajeros_TCED < 0 || numero_pasajeros_TCED > 20) numero_pasajeros_TCED = 0;
        vector_vehiculos[indice].Num_pasajeros_tercera_edad = numero_pasajeros_TCED;
        
        if((vector_vehiculos[indice].Num_pasajeros_adultos + vector_vehiculos[indice].Num_pasajeros_tercera_edad) > 20){
            vector_vehiculos[indice].Num_pasajeros_adultos = 0;
            vector_vehiculos[indice].Num_pasajeros_tercera_edad = 0;
        }

        //Validar tipo de pasaje (VIP 0, Turista 1)
        if(tipo_pasaje_adut != 0 && tipo_pasaje_adut != 1)tipo_pasaje_adut = 1;
        vector_vehiculos[indice].pasaje_adquirido_adut = tipo_pasaje_adut;

        //Validar tipo de pasaje (VIP 0, Turista 1) TCED
        if(tipo_pasaje_TCED != 0 && tipo_pasaje_TCED != 1) tipo_pasaje_TCED = 1;
        vector_vehiculos[indice].pasaje_adquirido_tercera_ed = tipo_pasaje_TCED;
        
        //Validar tipo de ferry
        if(tipo_ferry != 0 && tipo_ferry != 1){
            tipo_ferry = 0;
        }

        //validar codigo de placa
        if(strlen(placa) < 3 || strlen(placa) > 10){
            printf("Placa invalida\n");
            continue;
        }

        bool duplicado = false;
        int j = 0;
        while(j < indice){
            if(strcmp(vector_vehiculos[j].placa, placa) == 0){
                duplicado = true;
                break;
            }
            j++;
        }
        if(duplicado) continue;
      

        vector_vehiculos[indice].peso = peso;
         
        if(Hora_llegada < 0 || Hora_llegada > 2359) continue; 

        int minutos = Hora_llegada % 100;

        if(minutos >= 60) continue; 
        if(peso <= 0) continue;
        if(vector_vehiculos[indice].tipo == 4){
            vector_vehiculos[indice].peso = peso;
        }else{
            vector_vehiculos[indice].peso = peso / 1000.0f;  
        }

        vector_vehiculos[indice].tiempo_llegada = convertir_a_minutos(Hora_llegada);
        strcpy(vector_vehiculos[indice].placa,placa);
        vector_vehiculos[indice].tipo_ferry = tipo_ferry;
        
        (*cantidad_vehiculos)++;
        indice++;
    }
}


/**
 * @brief Ordena un arreglo de vehículos por su hora de llegada de menor a mayor.
 */
void ordenar_por_hora(vehiculos vector[], int n){

    for(int i = 0; i < n-1; i++){
        for(int j = 0; j < n-i-1; j++){
            if(vector[j].tiempo_llegada > vector[j+1].tiempo_llegada){
                vehiculos temp = vector[j];
                vector[j] = vector[j+1];
                vector[j+1] = temp;
            }
        }
    }
}

/**
 * @brief Verifica el tiempo de viaje de los ferrys y actualiza su estado.
 */
void reiniciar_tiempo_de_viaje(ferrys vector_ferrys[],int tiempo_actual){

    int tiempo_regreso;

    for(int i = 0; i < MAX_FERRYS; i++){
        if((vector_ferrys[i].hora_salida > 0) && (vector_ferrys[i].estado == 2) ){
            tiempo_regreso = vector_ferrys[i].hora_salida + vector_ferrys[i].tiempo_de_viaje;

            if(tiempo_actual >= tiempo_regreso){
                vector_ferrys[i].estado = 3; 
                vector_ferrys[i].hora_salida = 0; 
            }
        }
    }      
}

/**
 * @brief Libera la rampa del ferry una vez que finaliza el tiempo de carga.
 */
void actualizar_rampa(bool *rampa_libre, int tiempo_universal, int *tiempo_carga){
    if(!(*rampa_libre) && tiempo_universal == *tiempo_carga){
        *rampa_libre = true;
    }
}

/**
 * @brief Gestiona el ingreso de vehículos de emergencia.
 */
void manejar_emergencia(ferrys *ferry, vehiculos cola[], int *indice_cola){
    if (*indice_cola > 0){

        bool es_emergencia = (cola[0].tipo >= 5);

        if (es_emergencia){

            bool falta_espacio = true;

            while (falta_espacio){

                bool full_carros = (ferry->vehiculos_actuales >= ferry->cap_max_vehiculos);

                bool full_pasajeros =
                    (ferry->pasajeros_actuales +
                     cola[0].Num_pasajeros_adultos +
                     cola[0].Num_pasajeros_tercera_edad + 1 >
                     ferry->cap_pasajeros);

                bool full_peso =
                    (ferry->peso_actual + cola[0].peso >
                     ferry->peso_maximo);

                if (full_carros || full_pasajeros || full_peso){

                    bool pudo_sacar =
                        hacer_espacio_emergencia(ferry, cola, indice_cola);

                    if (!pudo_sacar){
                        falta_espacio = false;
                    }else{
                        continue;
                    }

                }else{
                    falta_espacio = false;
                }
            }
        }
    }
}

/**
 * @brief Traslada el primer vehículo de la cola al interior del ferry.
 */
void cargar_vehiculo_en_ferry(ferrys *ferry,vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga, int tiempo_universal){
    
    ferry->info_vehiculos_actuales[ferry->vehiculos_actuales] = cola[0];

    ferry->vehiculos_actuales++;

    ferry->peso_actual += cola[0].peso = cola[0].peso;

    ferry->pasajeros_actuales +=
        cola[0].Num_pasajeros_adultos +
        cola[0].Num_pasajeros_tercera_edad + 1;

    for(int j=0;j<(*indice_cola)-1;j++){
        cola[j] = cola[j+1];
    }

    (*indice_cola)--;

    *tiempo_carga = tiempo_universal + 3;
    *rampa_libre = false;
}

/**
 * @brief Coordina el proceso completo de carga y zarpe de un ferry.
 * @param turno_ferry PASADO POR VALOR.
 * @param turno_control PUNTERO para actualizar el estado en el main.
 */
void procesar_carga_ferry(ferrys vector_ferrys[], int turno_ferry, vehiculos cola[], int *indice_cola, bool *rampa_libre, int *tiempo_carga, int tiempo_universal, FILE *salida, int *numero_viaje_global, int *total_dia_vehiculos, int *total_dia_pasajeros, float *total_dia_ingresos, int frecuencia_tipos[], int *turno_control){

    // Ahora turno_ferry entra como valor directo al índice
    ferrys *ferry = &vector_ferrys[turno_ferry];

    actualizar_rampa(rampa_libre, tiempo_universal, tiempo_carga);
    manejar_emergencia(ferry, cola, indice_cola);

    if (*rampa_libre) {
        vehiculos *siguiente = NULL;
        vehiculos vacio = {0};

        if (*indice_cola > 0) {
            siguiente = &cola[0];
        }

        bool zarpar_por_reglas = debe_zarpar(*ferry, ferry->vehiculos_actuales, *indice_cola, ferry->pasajeros_actuales, ferry->peso_actual, (siguiente ? *siguiente : vacio), tiempo_universal);

        if (zarpar_por_reglas) {
            procesar_salida(*ferry, salida, (*numero_viaje_global)++, total_dia_vehiculos, total_dia_pasajeros, total_dia_ingresos, frecuencia_tipos);

            ferry->hora_salida = tiempo_universal;
            ferry->estado = 2; 

            ferry->peso_actual = 0;
            ferry->pasajeros_actuales = 0;
            ferry->vehiculos_actuales = 0;

            // Se usa el puntero de control para avisar al main que el ferry ya no está en el muelle
            *turno_control = (*turno_control + 1) % MAX_FERRYS;
            *rampa_libre = true;
            *tiempo_carga = 0;
        } 
        else if (*indice_cola > 0) {
            bool full_peso = (ferry->peso_actual + cola[0].peso > ferry->peso_maximo);
            bool full_pasajeros = (ferry->pasajeros_actuales + cola[0].Num_pasajeros_adultos + cola[0].Num_pasajeros_tercera_edad + 1 > ferry->cap_pasajeros);
            bool full_carros = (ferry->vehiculos_actuales >= ferry->cap_max_vehiculos);

            if (full_peso || full_pasajeros || full_carros) {
                if (ferry->vehiculos_actuales > 0) {
                    procesar_salida(*ferry, salida, (*numero_viaje_global)++, total_dia_vehiculos, total_dia_pasajeros, total_dia_ingresos, frecuencia_tipos);
                    ferry->hora_salida = tiempo_universal;
                    ferry->estado = 2;
                    ferry->peso_actual = 0;
                    ferry->pasajeros_actuales = 0;
                    ferry->vehiculos_actuales = 0;
                    *turno_control = (*turno_control + 1) % MAX_FERRYS;
                    *rampa_libre = true;
                    *tiempo_carga = 0;
                }
                return;
            }
            cargar_vehiculo_en_ferry(ferry, cola, indice_cola, rampa_libre, tiempo_carga, tiempo_universal);
        }
    }
}

/**
 * @brief Inserta un vehículo de emergencia en la primera posición de la cola.
 */
void insertar_emergencia_cola(vehiculos cola[], int *indice, vehiculos nuevo){
    
    if(*indice >= MAX_CANTIDAD_COLAS){
        printf("Cola llena, no se puede insertar emergencia\n");
        return;
    }

    for(int i = *indice; i > 0; i--){
        cola[i] = cola[i-1];
    }

    cola[0] = nuevo;
    (*indice)++;
}


/**
 * @brief Desaloja un vehículo convencional para hacer espacio a una emergencia.
 */
bool hacer_espacio_emergencia(ferrys *ferry_actual, vehiculos cola_actual[], int *indice_cola) {
    
    for (int i = ferry_actual->vehiculos_actuales - 1; i >= 0; i--) {
        int tipo_vehiculo = ferry_actual->info_vehiculos_actuales[i].tipo;
        
        if (tipo_vehiculo < 5) {
            vehiculos vehiculo_sacrificado = ferry_actual->info_vehiculos_actuales[i];
            
            ferry_actual->peso_actual -= vehiculo_sacrificado.peso;
            ferry_actual->pasajeros_actuales -= (vehiculo_sacrificado.Num_pasajeros_adultos + 
                                                 vehiculo_sacrificado.Num_pasajeros_tercera_edad + 1);
            
            for (int k = i; k < ferry_actual->vehiculos_actuales - 1; k++) {
                ferry_actual->info_vehiculos_actuales[k] = ferry_actual->info_vehiculos_actuales[k + 1];
            }
            ferry_actual->vehiculos_actuales--;
            
            for (int j = *indice_cola; j > 1; j--) {
                cola_actual[j] = cola_actual[j - 1];
            }
            
            cola_actual[1] = vehiculo_sacrificado;
            
            if(*indice_cola < MAX_CANTIDAD_COLAS){
                (*indice_cola)++;
            }
            return true; 
        }
    }
    return false;
}


/**
 * @brief Evalúa si el ferry cumple las condiciones operativas para zarpar.
 */
bool debe_zarpar(ferrys ferry_actual,int vehiculos_en_ferry,int vehiculos_en_cola,int pasajeros_actuales,float peso_actual,vehiculos siguiente_en_cola,int tiempo_universal){
    bool respuesta_zarpe = false; 
    float peso_max = (ferry_actual.tipo == 0) ? 60.0 : 80.0;

    if (tiempo_universal >= 1439 && vehiculos_en_ferry > 0) {
        respuesta_zarpe = true;
    }
    else if (vehiculos_en_ferry >= ferry_actual.cap_max_vehiculos) {
        respuesta_zarpe = true;
    }
    else if (pasajeros_actuales >= ferry_actual.cap_pasajeros) {
        respuesta_zarpe = true;
    }
    else if (peso_actual + siguiente_en_cola.peso > peso_max) {
        respuesta_zarpe = true;
    }
    else if (vehiculos_en_cola == 0 && vehiculos_en_ferry >= ferry_actual.cap_min_vehiculos) {
        respuesta_zarpe = true;
    }

    return respuesta_zarpe;
}

/**
 * @brief Imprime el reporte del viaje actual y calcula los ingresos generados.
 */
void procesar_salida(ferrys ferry_actual, FILE *salida, int numero_viaje,
        int *dia_total_vehiculos, int *dia_total_pasajeros, float *dia_total_ingresos, int frec_vehiculos[]) {
    
    float ingreso_viaje = 0.0;
    int tercera_edad_viaje = 0;
    
    for (int i = 0; i < ferry_actual.vehiculos_actuales; i++) {
        vehiculos v = ferry_actual.info_vehiculos_actuales[i];
        tercera_edad_viaje += v.Num_pasajeros_tercera_edad;
        
        if(v.tipo >= 1 && v.tipo <= 7) {
            frec_vehiculos[v.tipo]++;
        }
        
        if (ferry_actual.tipo == 1) { 
            if (v.tipo == 1) ingreso_viaje += V_LIVIANO_TRAD;
            else if (v.tipo == 2) ingreso_viaje += V_RUSTICO_TRAD;
            else if (v.tipo == 3) ingreso_viaje += V_MICROBUS_TRAD;
            else if (v.tipo == 4) ingreso_viaje += V_CARGA_TRAD;
            
            if (v.Num_pasajeros_adultos > 0 || v.Num_pasajeros_tercera_edad > 0) {
                if (v.pasaje_adquirido_adut == 0) ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_VIP_TRAD;
                else ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_TUR_TRAD;
                if (v.pasaje_adquirido_tercera_ed == 0) ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_VIP_TRAD;
                else ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_TUR_TRAD;
            }
        } else { 
            if (v.tipo == 1) ingreso_viaje += V_LIVIANO_EXP;
            else if (v.tipo == 2) ingreso_viaje += V_RUSTICO_EXP;
            else if (v.tipo == 3) ingreso_viaje += V_MICROBUS_EXP;
            else if (v.tipo == 4) ingreso_viaje += V_CARGA_EXP;

            if (v.Num_pasajeros_adultos > 0 || v.Num_pasajeros_tercera_edad > 0) {
                if (v.pasaje_adquirido_adut == 0) ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_VIP_EXP;
                else ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_TUR_EXP;
                if (v.pasaje_adquirido_tercera_ed == 0) ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_VIP_EXP;
                else ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_TUR_EXP;
            }
        }
    }

    printf("\nCarga Nro. %d:\n", numero_viaje);
    printf("Ferry %s\n", ferry_actual.nombre);
    printf("Viaje Nro. %d\n", numero_viaje);
    printf("Num. vehiculos: %d\n", ferry_actual.vehiculos_actuales);
    printf("Num. pasajeros: %d (%d de la tercera edad)\n", ferry_actual.pasajeros_actuales, tercera_edad_viaje);
    printf("Peso: %.2f toneladas\n", ferry_actual.peso_actual);
    printf("Ingreso: %.2f BsF.\n", ingreso_viaje);
    
    fprintf(salida, "Carga Nro. %d:\n", numero_viaje);
    fprintf(salida, "Ferry %s\n", ferry_actual.nombre);
    fprintf(salida, "Viaje Nro. %d\n", numero_viaje);
    fprintf(salida, "Num. vehiculos: %d\n", ferry_actual.vehiculos_actuales);
    fprintf(salida, "Num. pasajeros: %d (%d de la tercera edad)\n", ferry_actual.pasajeros_actuales, tercera_edad_viaje);
    fprintf(salida, "Peso: %.2f toneladas\n", ferry_actual.peso_actual);
    fprintf(salida, "Ingreso: %.2f BsF.\n", ingreso_viaje);
    
    for (int i = 0; i < ferry_actual.vehiculos_actuales; i++) {
        vehiculos v = ferry_actual.info_vehiculos_actuales[i];
        char str_tipo[20];
        switch(v.tipo) {
            case 1: strcpy(str_tipo, "liviano"); break;
            case 2: strcpy(str_tipo, "rustico"); break;
            case 3: strcpy(str_tipo, "microbus"); break;
            case 4: strcpy(str_tipo, "carga"); break;
            case 5: strcpy(str_tipo, "ambulancia"); break;
            case 6: strcpy(str_tipo, "bomberos"); break;
            case 7: strcpy(str_tipo, "policial"); break;
            default: strcpy(str_tipo, "desconocido"); break;
        }
        printf("ID: %s \t Tipo: %s\n", v.placa, str_tipo);
        fprintf(salida, "ID: %s \t Tipo: %s\n", v.placa, str_tipo);
    }
    printf("\n");
    fprintf(salida, "\n");

    *dia_total_vehiculos += ferry_actual.vehiculos_actuales;
    *dia_total_pasajeros += ferry_actual.pasajeros_actuales;
    *dia_total_ingresos += ingreso_viaje;
}

/**
 * @brief Limpia la pantalla de la consola o terminal.
 */
void limpiar_consola(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}