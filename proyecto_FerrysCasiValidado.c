/*
Faltan poner informacion de aqui
Integrantes: 
C.I: 
Seccion: 
Materia: Fundamentos de Programacion
Prof:
*/

/**
 * @file simulacion_ferry.c
 * @author Saul Lara & Gabriel Rosario
 * @brief Programa de simulación para el control de carga y zarpe de ferrys.
 * @version 1.0
 * @date 2026-03-17
 * * @copyright Copyright (c) 2026 //bien cacheroso
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//CONSTANTES//
#define MAX_VEHICULOS 100
#define MAX_FERRYS 3
#define MAX_CANTIDAD_COLAS 70

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


/**
 * @brief Lee y valida los vehículos desde el archivo de entrada.
 * @param entrada Puntero al archivo proy1.in.
 * @param vector_vehiculos Arreglo donde se almacenarán los registros leídos.
 * @param cantidad_vehiculos Puntero para retornar el total de vehículos cargados exitosamente.
 */
void cargar_vehiculos(FILE *entrada, vehiculos vector_vehiculos[],int *cantidad_vehiculos);


/**
 * @brief Configura las propiedades iniciales de los ferrys según el orden de entrada.
 * @param orden Arreglo con los IDs de los ferrys (1, 2 o 3).
 * @param vector_ferrys Arreglo destino de estructuras ferrys.
 */
void cargar_ferrys(int orden[], ferrys vector_ferrys[]);


/**
 * @brief Convierte el formato HHMM a minutos transcurridos desde las 00:00.
 * @param hora_militar Entero con la hora (ej. 1430 para las 14:30).
 * @return Entero con los minutos totales.
 */
int convertir_a_minutos(int hora_militar);


/**
 * @brief Ordena el vector de vehículos cronológicamente por hora de llegada.
 * @param vector Arreglo de vehículos a ordenar.
 * @param n Cantidad de elementos en el arreglo.
 */
void ordenar_por_hora(vehiculos vector[], int n);


/**
 * @brief Libera los ferrys que han completado su tiempo de navegación.
 * @param vector_ferrys Arreglo de la flota.
 * @param tiempo_actual Minuto actual de la simulación.
 */
void reiniciar_tiempo_de_viaje(ferrys vector_ferrys[],int tiempo_actual);


/**
 * @brief Actualiza el estado de la rampa del ferry según el tiempo de carga.
 * 
 * @param rampa_libre Indica si la rampa está disponible (true) o en uso (false).
 * @param tiempo_universal Tiempo actual de la simulación en minutos.
 * @param tiempo_carga Tiempo en que la rampa volverá a estar disponible.
 */
void actualizar_rampa(bool *rampa_libre, int tiempo_universal, int *tiempo_carga);


/**
 * @brief Maneja la llegada de un vehículo de emergencia al ferry.
 * 
 * Puede implicar desplazar vehículos normales para dar prioridad al de emergencia.
 * 
 * @param ferry Puntero al ferry donde se intentará cargar la emergencia.
 * @param cola Arreglo de vehículos en espera.
 * @param indice_cola Índice actual de la cola.
 */
void manejar_emergencia(ferrys *ferry, vehiculos cola[], int *indice_cola);


/**
 * @brief Carga un vehículo en el ferry si hay espacio y la rampa está disponible.
 * 
 * @param ferry Puntero al ferry en el que se cargará el vehículo.
 * @param cola Arreglo de vehículos en espera.
 * @param indice_cola Índice actual de la cola.
 * @param rampa_libre Indica si la rampa está disponible.
 * @param tiempo_carga Tiempo en que la rampa quedará ocupada.
 * @param tiempo_universal Tiempo actual de la simulación.
 */
void cargar_vehiculo_en_ferry(ferrys *ferry,vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga,int tiempo_universal);


/**
 * @brief Expulsa un vehículo normal del ferry para ceder el puesto a una emergencia.
 * @param ferry_actual Puntero al ferry en proceso de carga.
 * @param cola_actual Cola de espera donde se reinsertará el vehículo sacrificado.
 * @param indice_cola Puntero al contador de la cola.
 * @return true si se logró liberar espacio, false si no hay vehículos convencionales a bordo.
 */
bool hacer_espacio_emergencia(ferrys *ferry_actual, vehiculos cola_actual[], int *indice_cola);


/**
 * @brief Inserta un vehículo de emergencia al inicio de la cola de espera.
 * @param cola Arreglo de la cola.
 * @param indice Puntero al contador de la cola.
 * @param nuevo Estructura del vehículo de emergencia.
 */
void insertar_emergencia_cola(vehiculos cola[], int *indice, vehiculos nuevo);


/**
 * @brief Evalúa si se cumplen las condiciones físicas o logísticas para que el ferry zarpe.
 * @return true si el ferry debe salir del muelle inmediatamente.
 */
bool debe_zarpar(ferrys ferry_actual,int vehiculos_en_ferry,int vehiculos_en_cola,int pasajeros_actuales,
    float peso_actual,vehiculos siguiente_en_cola,int tiempo_universal);


/**
 * @brief Controla el proceso de carga de vehículos en los ferrys.
 * 
 * Gestiona la asignación de vehículos, uso de la rampa, prioridades
 * (como emergencias) y actualización de estadísticas del sistema.
 * 
 * @param vector_ferrys Arreglo de ferrys disponibles.
 * @param turno_ferry Índice del ferry actual en operación.
 * @param cola Arreglo de vehículos en espera.
 * @param indice_cola Índice actual de la cola.
 * @param rampa_libre Indica si la rampa está disponible.
 * @param tiempo_carga Tiempo en que la rampa quedará libre.
 * @param tiempo_universal Tiempo actual de la simulación.
 * @param salida Archivo donde se registran los eventos.
 * @param numero_viaje_global Contador global de viajes realizados.
 * @param total_dia_vehiculos Total de vehículos transportados en el día.
 * @param total_dia_pasajeros Total de pasajeros transportados.
 * @param total_dia_ingresos Total de ingresos acumulados.
 * @param frecuencia_tipos Frecuencia de tipos de vehículos transportados.
 */
void procesar_carga_ferry(ferrys vector_ferrys[],int *turno_ferry,vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga, int tiempo_universal,
    FILE *salida,int *numero_viaje_global,int *total_dia_vehiculos,int *total_dia_pasajeros,float *total_dia_ingresos,int frecuencia_tipos[]);


/**
 * @brief Genera el reporte de zarpe, calcula ingresos y actualiza estadísticas globales.
 * @param ferry_actual Ferry que está saliendo.
 * @param salida Puntero al archivo proy1.out.
 * @param numero_viaje ID del viaje actual.
 */
void procesar_salida(ferrys ferry_actual, FILE *salida, int numero_viaje, int *dia_total_vehiculos, int *dia_total_pasajeros, float *dia_total_ingresos, int frec_vehiculos[]);


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

    //Estructuras//
    vehiculos vector_vehiculos[MAX_VEHICULOS];
    vehiculos vector_colaExpress[MAX_CANTIDAD_COLAS];
    vehiculos vector_colaTradicional[MAX_CANTIDAD_COLAS]; //Estaba como vector_colaTradiccional (GR)
    ferrys vector_ferrys[MAX_FERRYS];

    // Apertura y validación de flujos de datos (Entrada/Salida)
    entrada = fopen("proy1.in","r");
    salida  = fopen("proy1.out","w");

    if(entrada == NULL){
        printf("error al abrir el archivo\n");
        return 1;//Angel mamon
    }else{
        printf("Bienvenido al programa: \n");

        //orden de carga de ferrys
        fscanf(entrada,"%i %i %i\n",&orden[0],&orden[1],&orden[2]);

        // Configuración inicial de la flota y lectura del inventario de vehículos
        cargar_ferrys(orden,vector_ferrys);
        cargar_vehiculos(entrada,vector_vehiculos,&cantidad_vehiculos);

        // Normalización de datos: Asegura que el procesamiento sea cronológico
        //Ordena los vehiculos por tiempo de llegada para facilitar la simulacion (GR)
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

            for(int i = 0; i < cantidad_vehiculos; i++){ //Posible mejora con for, probar con while (Saul)
                        
                // Si la hora de llegada del vehiculo coincide con el reloj actual
                if(vector_vehiculos[i].tiempo_llegada == tiempo_universal){

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
                            //Tipo_ferry == 0 (GR)
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


                        procesar_carga_ferry(vector_ferrys,&turno_ferry,vector_colaTradicional,&indice_tradicional,
                            &rampa_libre,&tiempo_carga,tiempo_universal,salida,&numero_viaje_global,&total_dia_vehiculos,
                            &total_dia_pasajeros,&total_dia_ingresos,frecuencia_tipos);
                }else{
                    // =========================================================
                    // LÓGICA SI EL FERRY EN EL MUELLE ES EXPRESS
                    // =========================================================
                        //Llamar funcion para express
              

                        procesar_carga_ferry(vector_ferrys,&turno_ferry,vector_colaExpress,&indice_express,
                            &rampa_libre,&tiempo_carga,tiempo_universal,salida,&numero_viaje_global,&total_dia_vehiculos,
                            &total_dia_pasajeros,&total_dia_ingresos,frecuencia_tipos);

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
            // 2. No hay autos físicamente dentro de los ferrys.
            // 3. Ningún ferry está actualmente navegando (Estado 2).
            // 4. Ya pasó la hora de llegada del último auto del archivo.
            // =========================================================
            if(indice_tradicional == 0 &&
               indice_express == 0 &&
               vector_ferrys[0].vehiculos_actuales == 0 &&
               vector_ferrys[1].vehiculos_actuales == 0 &&
               vector_ferrys[2].vehiculos_actuales == 0 &&
               vector_ferrys[0].estado != 2 && // Ferry no está en viaje
               vector_ferrys[1].estado != 2 &&
               vector_ferrys[2].estado != 2 &&
               !quedan_por_llegar) {
                
                printf("\n--- Simulación finalizada: No hay más operaciones pendientes ---\n");
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
        printf("Mayor frec. vehiculos en espera: %d (a las %02d:%02d)\n", mayor_frecuencia_espera, hora_militar_espera_h, hora_militar_espera_m); //: en hora (GR)

        fprintf(salida, "Estadisticas:\n");
        fprintf(salida, "Total vehiculos transportados: %d\n", total_dia_vehiculos);
        fprintf(salida, "Total pasajeros transportados: %d\n", total_dia_pasajeros);
        fprintf(salida, "Total de ingresos: %.2f BsF.\n", total_dia_ingresos);
        fprintf(salida, "Num. pasajeros no trasladados: %d\n", pasajeros_varados);
        fprintf(salida, "Vehiculo mas frecuente: %s\n", str_tipo_frecuente);
        fprintf(salida, "Mayor frec. vehiculos en espera: %d (a las %02d:%02d)\n", mayor_frecuencia_espera, hora_militar_espera_h, hora_militar_espera_m); //: en hora (GR)

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
 * * Esta función recorre línea por línea el archivo de entrada hasta llegar al final (EOF).
 * Extrae la información de cada vehículo (pasajeros, peso, hora, placa, etc.) según 
 * el formato establecido en el proyecto y la guarda ordenadamente en las posiciones 
 * consecutivas del arreglo de estructuras.
 * * @param entrada Puntero al archivo de texto (previamente abierto en modo lectura).
 * @param vector_vehiculos Arreglo de estructuras tipo `vehiculos` que será llenado con los datos.
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
    char placa[15];
    int tipo_ferry;

    int indice = 0;
    //inicializar contador
    *cantidad_vehiculos = 0;

    //recorrer el archivo hasta fin de archivo 
    while(fscanf(entrada,"%i %i %i %i %i %f %i %s %i",&codigo_vehiculo,
                            &numero_pasajeros_adut,&numero_pasajeros_TCED,&tipo_pasaje_adut,
                            &tipo_pasaje_TCED,&peso,&Hora_llegada,placa,&tipo_ferry) == 9){//!= EOF es inseguro (GR)
        
        


        if(indice >= MAX_VEHICULOS){
            printf("Advertencia: se excedio la capacidad de vehiculos\n");
            break;
        }

        ////Validar tipo de vehiculos
        int tipo = codigo_vehiculo / 100;
        if(tipo < 1 || tipo > 7) continue;//ignorar vehiculo invalido
        vector_vehiculos[indice].tipo = tipo;

        if(tipo == 4){
            //Vehiculo de carga no debe transportar pasajeros
            numero_pasajeros_adut = 0;
            numero_pasajeros_TCED = 0;
        }

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
        

        //Validar tipo de pasaje (VIP 0, Turista 1)
        if(tipo_pasaje_adut != 0 && tipo_pasaje_adut != 1)tipo_pasaje_adut = 1;
            vector_vehiculos[indice].pasaje_adquirido_adut = tipo_pasaje_adut;

        //Validar tipo de pasaje (VIP 0, Turista 1) TCED
        if(tipo_pasaje_TCED != 0 && tipo_pasaje_TCED != 1) tipo_pasaje_TCED = 1;
            vector_vehiculos[indice].pasaje_adquirido_tercera_ed = tipo_pasaje_TCED;
        
        //Validar tipo de ferry
        if(tipo_ferry != 0 && tipo_ferry != 1){
            //si el tipo de ferry es invalido, se asigna express
            tipo_ferry = 0;
        }

        //validar codigo de placa
        if(strlen(placa) < 3 || strlen(placa) > 10){
            printf("Placa invalida\n");
            continue;
        }

        //Validar si tienen placas iguales
        //Se uso while por preferencias del profesor (:/)
        bool duplicado = false;
        int j = 0;
        while(j < indice){
            if(strcmp(vector_vehiculos[j].placa, placa) == 0){
                //Vehiculo duplicado
                duplicado = true;
                break;
            }
            j++;
        }
        if(duplicado) continue;
      

        vector_vehiculos[indice].peso = peso;
         
        if(Hora_llegada < 0 || Hora_llegada > 2359) continue; //Hora invalida, se ignoran vehiculos

        int minutos = Hora_llegada % 100;

        if(minutos >= 60) continue; //minutos invalidos
        if(peso <= 0) continue;
        // GR conversión corregida de peso
        if(vector_vehiculos[indice].tipo == 4){
            //Guarda el peso en toneladas
            vector_vehiculos[indice].peso = peso;
        }else{
            //Convierte kg a toneladas
            vector_vehiculos[indice].peso = peso / 1000.0f;  // la f es para division en float
        }

        vector_vehiculos[indice].tiempo_llegada = convertir_a_minutos(Hora_llegada);
        strcpy(vector_vehiculos[indice].placa,placa);
        vector_vehiculos[indice].tipo_ferry = tipo_ferry;
        
        //acumentar indice_tradicional
        (*cantidad_vehiculos)++;
        indice++;

    }

}



//Ordenar vehiculos
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
 * @brief Verifica el tiempo de viaje de los ferrys y actualiza su estado al regresar al muelle.
 * * Recorre el arreglo de ferrys buscando aquellos que se encuentren actualmente navegando (estado 2).
 * Calcula el tiempo exacto en el que deben regresar al muelle sumando su hora de salida y su tiempo
 * de viaje predeterminado. Si el tiempo actual de la simulación coincide con esa hora de regreso, 
 * el ferry cambia su estado a "Espera" (estado 3) y reinicia su hora de salida a 0, quedando libre 
 * para iniciar un nuevo proceso de carga.
 * * @param vector_ferrys Arreglo que contiene las estructuras de todos los ferrys de la flota.
 * @param tiempo_actual El minuto actual de la simulación (tiempo universal).
 */
void reiniciar_tiempo_de_viaje(ferrys vector_ferrys[],int tiempo_actual){

    //var
    int tiempo_regreso;

    for(int i = 0; i < MAX_FERRYS; i++){
        //validar que el ferry si alla salido
        if((vector_ferrys[i].hora_salida > 0) && (vector_ferrys[i].estado == 2) ){
            //calculamos la hora que tiene que llegar el ferry al muelle
            tiempo_regreso = vector_ferrys[i].hora_salida + vector_ferrys[i].tiempo_de_viaje;

            //validamos si ese tiempo se cumplio 
            if(tiempo_actual >= tiempo_regreso){
                vector_ferrys[i].estado = 3; //entonces cambiamos su estado a espera
                vector_ferrys[i].hora_salida = 0; //reiniciamos la hora de salida a 0
            }
        }
    }      
}

//Libera la rampa del ferry para que accedan los vehiculos
void actualizar_rampa(bool *rampa_libre, int tiempo_universal, int *tiempo_carga){
    if(!(*rampa_libre) && tiempo_universal == *tiempo_carga){
        *rampa_libre = true;
    }
}

//Gestiona la emergencia de vehiculos segun espacio
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

void cargar_vehiculo_en_ferry(ferrys *ferry,vehiculos cola[],int *indice_cola,bool *rampa_libre,int *tiempo_carga,
    int tiempo_universal
){
    ferry->info_vehiculos_actuales[ferry->vehiculos_actuales] = cola[0];

    ferry->vehiculos_actuales++;

    ferry->peso_actual += cola[0].peso;

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

//(GR)
void procesar_carga_ferry(
    ferrys vector_ferrys[],
    int *turno_ferry,
    vehiculos cola[],
    int *indice_cola,
    bool *rampa_libre,
    int *tiempo_carga,
    int tiempo_universal,
    FILE *salida,
    int *numero_viaje_global,
    int *total_dia_vehiculos,
    int *total_dia_pasajeros,
    float *total_dia_ingresos,
    int frecuencia_tipos[]
){

    ferrys *ferry = &vector_ferrys[*turno_ferry];

    // 1. Rampa
    actualizar_rampa(rampa_libre, tiempo_universal, tiempo_carga);

    // 2. Emergencias
    manejar_emergencia(ferry, cola, indice_cola);

    // 3. Operaciones si la rampa está libre
    if(*rampa_libre){

        vehiculos *siguiente = NULL;
        vehiculos vacio = {0};

        if(*indice_cola > 0){
            siguiente = &cola[0];
        }

        // 4. Decidir zarpe
        if(debe_zarpar(
            *ferry,
            ferry->vehiculos_actuales,
            *indice_cola,
            ferry->pasajeros_actuales,
            ferry->peso_actual,
            (siguiente ? *siguiente : vacio),
            tiempo_universal
        )){
            procesar_salida(
                *ferry,
                salida,
                (*numero_viaje_global)++,
                total_dia_vehiculos,
                total_dia_pasajeros,
                total_dia_ingresos,
                frecuencia_tipos
            );

            ferry->hora_salida = tiempo_universal;
            ferry->estado = 2;

            ferry->peso_actual = 0;
            ferry->pasajeros_actuales = 0;
            ferry->vehiculos_actuales = 0;

            *turno_ferry = (*turno_ferry + 1) % MAX_FERRYS;

            *rampa_libre = true;
            *tiempo_carga = 0;
        }
        else if(*indice_cola > 0){

            // 5. Validaciones de capacidad
            bool full_peso = (ferry->peso_actual + cola[0].peso > ferry->peso_maximo);

            bool full_pasajeros =
                (ferry->pasajeros_actuales +
                 cola[0].Num_pasajeros_adultos +
                 cola[0].Num_pasajeros_tercera_edad + 1 >
                 ferry->cap_pasajeros);

            bool full_carros =
                (ferry->vehiculos_actuales >= ferry->cap_max_vehiculos);

            if(full_peso || full_pasajeros || full_carros){

                // Si hay al menos un vehículo, zarpa
                if(ferry->vehiculos_actuales > 0){

                        procesar_salida(
                            *ferry,
                            salida,
                            (*numero_viaje_global)++,
                            total_dia_vehiculos,
                            total_dia_pasajeros,
                            total_dia_ingresos,
                            frecuencia_tipos
                        );

                        ferry->hora_salida = tiempo_universal;
                        ferry->estado = 2;

                        ferry->peso_actual = 0;
                        ferry->pasajeros_actuales = 0;
                        ferry->vehiculos_actuales = 0;

                        *turno_ferry = (*turno_ferry + 1) % MAX_FERRYS;

                        *rampa_libre = true;
                        *tiempo_carga = 0;
                    }

                    return;
                }

                // 6. Cargar vehículo (función modularizada)
                cargar_vehiculo_en_ferry(
                    ferry,
                    cola,
                    indice_cola,
                    rampa_libre,
                    tiempo_carga,
                    tiempo_universal
            );
        }
    }
}

//Las emergencias tienen prioridad, pero el ferry sigue respetando las restricciones físicas de peso y capacidad.
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
 * @brief Desaloja un vehículo convencional del ferry para hacer espacio a una emergencia.
 * * Busca de atrás hacia adelante en el arreglo de vehículos a bordo el último 
 * vehículo que no sea de emergencia (tipo < 5). Al encontrarlo, resta su peso,
 * pasajeros y lo elimina del ferry. Luego, lo inserta en la posición [1] de la 
 * cola de espera (justo detrás del vehículo de emergencia actual) desplazando
 * el resto de los vehículos en la cola.
 * * @param ferry_actual Puntero al ferry que se está cargando actualmente.
 * @param cola_actual Arreglo que representa la cola de vehículos (Tradicional o Express).
 * @param indice_cola Puntero al contador total de vehículos en esa cola.
 * @return true si logró sacar un vehículo normal, false si todos a bordo son emergencias.
 */
bool hacer_espacio_emergencia(ferrys *ferry_actual, vehiculos cola_actual[], int *indice_cola) {
    
    // Recorremos los carros a bordo de atrás hacia adelante
    for (int i = ferry_actual->vehiculos_actuales - 1; i >= 0; i--) {
        int tipo_vehiculo = ferry_actual->info_vehiculos_actuales[i].tipo;
        
        // Si NO es ambulancia (5), bombero (6) o policía (7)
        if (tipo_vehiculo < 5) {
            
            // 1. Guardar una copia del vehículo que vamos a sacar ("sacrificado")
            vehiculos vehiculo_sacrificado = ferry_actual->info_vehiculos_actuales[i];
            
            // 2. Restar sus atributos de los totales del ferry
            ferry_actual->peso_actual -= vehiculo_sacrificado.peso;
            ferry_actual->pasajeros_actuales -= (vehiculo_sacrificado.Num_pasajeros_adultos + 
                                                 vehiculo_sacrificado.Num_pasajeros_tercera_edad + 1); // +1 por el chofer
            
            // 3. Eliminarlo del arreglo del ferry (rodar los que estaban después de él)
            for (int k = i; k < ferry_actual->vehiculos_actuales - 1; k++) {
                ferry_actual->info_vehiculos_actuales[k] = ferry_actual->info_vehiculos_actuales[k + 1];
            }
            ferry_actual->vehiculos_actuales--; // Reducir el contador del barco
            
            // 4. Hacer espacio en la cola (rodando de la posición 1 en adelante)
            // CUIDADO: La posición [0] tiene a la ambulancia, no la podemos tocar.
            for (int j = *indice_cola; j > 1; j--) {
                cola_actual[j] = cola_actual[j - 1];
            }
            
            // 5. Insertar al sacrificado justo detrás de la ambulancia (en [1])
            cola_actual[1] = vehiculo_sacrificado;
            
            // 6. Aumentar el tamaño de la cola porque devolvimos un carro
            if(*indice_cola < MAX_CANTIDAD_COLAS){
                (*indice_cola)++;
            }else{
                printf("Cola llena al devolver vehiculo\n");
            }
            
            // Avisamos que la operación fue un éxito
            return true; 
        }
    }
    
    // Si terminó el ciclo y no retornó true, es porque no encontró carros normales
    return false;
}


//GR
bool debe_zarpar(ferrys ferry_actual,int vehiculos_en_ferry,int vehiculos_en_cola,int pasajeros_actuales, float peso_actual,
    vehiculos siguiente_en_cola, int tiempo_universal){

    // Fin del día
    if(tiempo_universal >= 1439)
        return true;

    // Capacidad máxima de vehículos
    if(vehiculos_en_ferry >= ferry_actual.cap_max_vehiculos)
        return true;

    // Capacidad máxima de pasajeros
    if(pasajeros_actuales >= ferry_actual.cap_pasajeros)
        return true;

    // Si el siguiente vehículo excede el peso
    if(vehiculos_en_cola > 0 &&
       peso_actual + siguiente_en_cola.peso > ferry_actual.peso_maximo)
        return true;

    // Si ya no hay vehículos esperando y se alcanzó el mínimo
    if(vehiculos_en_cola == 0 && vehiculos_en_ferry >= ferry_actual.cap_min_vehiculos) return true;

    
    return false;
}

/**
 * @brief Imprime el reporte del viaje actual y calcula los ingresos generados.
 * Calcula el costo del flete de los vehículos y los pasajes de las personas
 * basándose en el tipo de ferry y la clase de pasaje. Además, acumula las
 * estadísticas globales del día que se imprimen al final.
 *
 * @param ferry_actual Estructura del ferry que acaba de zarpar.
 * @param salida Puntero al archivo donde se escribirá el reporte.
 * @param numero_viaje Contador del número de viaje en el día.
 * @param dia_total_vehiculos Puntero al acumulador de vehículos del día.
 * @param dia_total_pasajeros Puntero al acumulador de pasajeros del día.
 * @param dia_total_ingresos Puntero al acumulador de dinero en BsF.
 * @param frec_vehiculos Arreglo acumulador para saber el vehículo más frecuente.
 */
void procesar_salida(ferrys ferry_actual, FILE *salida, int numero_viaje,
        int *dia_total_vehiculos, int *dia_total_pasajeros, float *dia_total_ingresos, int frec_vehiculos[]) {
    
    float ingreso_viaje = 0.0;
    int tercera_edad_viaje = 0;
    
    for (int i = 0; i < ferry_actual.vehiculos_actuales; i++) {
        vehiculos v = ferry_actual.info_vehiculos_actuales[i];

        // Calcular personas de tercera edad para imprimirlo
        tercera_edad_viaje += v.Num_pasajeros_tercera_edad;
        
        // Sumar al arreglo de frecuencias para las estadisticas finales
        if(v.tipo >= 1 && v.tipo <= 7) {
            frec_vehiculos[v.tipo]++;
        }
        
        // --- CÁLCULO DE INGRESOS ---
        if (ferry_actual.tipo == 1) { 
            // FERRY TRADICIONAL
            if (v.tipo == 1) ingreso_viaje += V_LIVIANO_TRAD;
            else if (v.tipo == 2) ingreso_viaje += V_RUSTICO_TRAD;
            else if (v.tipo == 3) ingreso_viaje += V_MICROBUS_TRAD;
            else if (v.tipo == 4) ingreso_viaje += V_CARGA_TRAD;
            // Vehículos de emergencia viajan gratis (se asume 0 BsF)
            
            // Pasajes de personas

            // cobrar chofer como adulto turista por defecto
            ingreso_viaje += T_ADULTO_TUR_TRAD; //estaba dentro del fi, debe cobrarse SIEMPRE
            //El chofer siempre se cobra como adulto en clase turista por defecto, ya que el enunciado no especifica tipo de pasaje para el
            //casteo necesario (float) (GR)
            if (v.Num_pasajeros_adultos > 0 || v.Num_pasajeros_tercera_edad > 0) { //cambio, verificaba traslada pasajeros == 1
                
                // Adultos
                if (v.pasaje_adquirido_adut == 0) ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_VIP_TRAD;
                else ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_TUR_TRAD;
                
                // Tercera Edad
                if (v.pasaje_adquirido_tercera_ed == 0) ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_VIP_TRAD;
                else ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_TUR_TRAD;

                
            }
            
        } else { 
            // FERRY EXPRESS
            if (v.tipo == 1) ingreso_viaje += V_LIVIANO_EXP;
            else if (v.tipo == 2) ingreso_viaje += V_RUSTICO_EXP;
            else if (v.tipo == 3) ingreso_viaje += V_MICROBUS_EXP;
            else if (v.tipo == 4) ingreso_viaje += V_CARGA_EXP;
            // Vehículos de emergencia viajan gratis (se asume 0 BsF)
            
            
            ingreso_viaje += T_ADULTO_TUR_EXP; //estaba dentro del if, debe cobrarse SIEMPRE
            //El chofer siempre se cobra como adulto en clase turista por defecto, ya que el enunciado no especifica tipo de pasaje para el

            // Pasajes de personas
            if (v.Num_pasajeros_adultos > 0 || v.Num_pasajeros_tercera_edad > 0) { //cambio, verificaba traslada pasajeros == 1

                // Adultos
                if (v.pasaje_adquirido_adut == 0) ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_VIP_EXP;
                else ingreso_viaje += (float)v.Num_pasajeros_adultos * T_ADULTO_TUR_EXP;
                
                // Tercera Edad
                if (v.pasaje_adquirido_tercera_ed == 0) ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_VIP_EXP;
                else ingreso_viaje += (float)v.Num_pasajeros_tercera_edad * T_MAYOR_TUR_EXP;

                
            }
        }
    }

    // --- IMPRESIÓN DEL REPORTE DEL VIAJE ---
    
    // En Pantalla
    printf("\nCarga Nro. %d:\n", numero_viaje);
    printf("Ferry %s\n", ferry_actual.nombre);
    printf("Viaje Nro. %d\n", numero_viaje);
    printf("Num. vehiculos: %d\n", ferry_actual.vehiculos_actuales);
    printf("Num. pasajeros: %d (%d de la tercera edad)\n", ferry_actual.pasajeros_actuales, tercera_edad_viaje);
    printf("Peso: %.2f toneladas\n", ferry_actual.peso_actual);
    printf("Ingreso: %.2f BsF.\n", ingreso_viaje);
    
    // En Archivo
    fprintf(salida, "Carga Nro. %d:\n", numero_viaje);
    fprintf(salida, "Ferry %s\n", ferry_actual.nombre);
    fprintf(salida, "Viaje Nro. %d\n", numero_viaje);
    fprintf(salida, "Num. vehiculos: %d\n", ferry_actual.vehiculos_actuales);
    fprintf(salida, "Num. pasajeros: %d (%d de la tercera edad)\n", ferry_actual.pasajeros_actuales, tercera_edad_viaje);
    fprintf(salida, "Peso: %.2f toneladas\n", ferry_actual.peso_actual);
    fprintf(salida, "Ingreso: %.2f BsF.\n", ingreso_viaje);
    
    // Listado de Vehiculos
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

    // --- ACTUALIZACIÓN DE ESTADÍSTICAS GLOBALES ---
    *dia_total_vehiculos += ferry_actual.vehiculos_actuales;
    *dia_total_pasajeros += ferry_actual.pasajeros_actuales;
    *dia_total_ingresos += ingreso_viaje;
}