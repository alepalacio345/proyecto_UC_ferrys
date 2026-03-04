#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//CONSTANTES//
#define MAX_VEHICULOS 100
#define MAX_FERRYS 3
#define MAX_CANTIDAD_COLAS 70

//(GR) Necesario para la logica de la funcion deber_zarpar()
// Capacidades Vehiculares (100%)
#define CAP_LILIA 16
#define CAP_ISABELA 20
#define CAP_MARGARITENA 18

// Capacidades Mínimas (30% redondeado hacia arriba)
#define MIN_LILIA 5 // 16 * 0.3 = 4.8
#define MIN_ISABELA 6 // 20 * 0.3 = 6.0
#define MIN_MARGARITENA 6 // 18 * 0.3 = 5.4
//(GR)

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
void cargar_vehiculos(FILE *entrada, vehiculos vector_vehiculos[],int *cantidad_vehiculos);
void cargar_ferrys(int orden[], ferrys vector_ferrys[]);
bool ferry_esta_lleno(float peso_acumulado, int tipo_ferry);
int convertir_a_minutos(int hora_militar);

//(GR) //Logica de zarpado
bool debe_zarpar(int vehiculos_en_ferry, int vehiculos_en_cola, float peso_actual, ferrys ferry_actual, vehiculos siguiente_en_cola, int pasajeros_actuales, int tiempo_universal);



int main(){

    //variables de archivos
    FILE *entrada;
    FILE *salida;

    //var normales
    int orden[3];
    int ferry_enMuelle;
    int turno_ferry = 0; //iniciar en el primero y al zarpar incrementar
    char ferry_nombre_turno[20];
    int indice_tradicional = 0;
    int indice_express = 0;
    bool bandera  = true;
    int cantidad_vehiculos;  //var acumuladora
    int tiempo_menor = 9999;     //menor tiempo te llegada de un vehiculo
    int tiempo_universal; //tiempo de todo el dia
    int tiempo_carga; //tiempo de todo el dia

    //structuras//
    vehiculos vector_vehiculos[MAX_VEHICULOS];
    vehiculos vector_colaExpress[MAX_CANTIDAD_COLAS];
    vehiculos vector_colaTradicional[MAX_CANTIDAD_COLAS]; //Estaba como vector_colaTradiccional (GR)
    ferrys vector_ferrys[MAX_FERRYS];

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

        //llamar al procedimiento para cargar los ferrys segun el orden dado en el archivo
        cargar_ferrys(orden,vector_ferrys);

        //llamr a al procedimiento cargar_vehiculo
        cargar_vehiculos(entrada,vector_vehiculos,&cantidad_vehiculos);
        
        //ciclo para obtener la hora en la que llego el primer carro
        for(int i = 0; i < cantidad_vehiculos;i++){
            //encontrar la menor hora
            if(vector_vehiculos[i].tiempo_llegada < tiempo_menor){
                tiempo_menor = vector_vehiculos[i].tiempo_llegada;
            }
        }
        
        float peso_ferry_actual = 0; // Peso del ferry que está en el muelle AHORA
        tiempo_universal = tiempo_menor;

        // (GR) contadores de pasajeros y vehiculos actuales
        int vehiculos_cargados_viaje = 0;
        int pasajeros_actuales = 0;   
        

        // Ciclo que simula el paso del dia minuto a minuto (hasta las 11:59 PM = 1439 mins)
        while(tiempo_universal < 1440){

            // 1. REVISAR LLEGADAS EN EL MINUTO ACTUAL
            for(int i = 0; i < cantidad_vehiculos; i++){
                        
                // Si la hora de llegada del vehiculo coincide con el reloj actual
                if(vector_vehiculos[i].tiempo_llegada == tiempo_universal){
                            
                    // Verificamos a qué cola va
                    if(vector_vehiculos[i].tipo_ferry == 1){ 
                        // Va a la tradicional. Copiamos TODO el struct de un golpe
                        vector_colaTradicional[indice_tradicional] = vector_vehiculos[i];
                        indice_tradicional++; // Solo avanza si metimos un carro
                                
                    } else{
                        //Tipo_ferry == 0 (GR)
                        // Va a la express. Copiamos TODO el struct de un golpe
                        vector_colaExpress[indice_express] = vector_vehiculos[i];
                        indice_express++; // Solo avanza si metimos un carro
                        }
                    }
                }

            //condicional para saber que tipo de ferry esta en el muelle (tradicional o express)
            // Extraemos el tipo de ferry (0 = Express, 1 = Tradicional)
            int tipo_ferry_muelle = vector_ferrys[turno_ferry].tipo;

            if(tipo_ferry_muelle){ 

            // =========================================================
            // LÓGICA SI EL FERRY EN EL MUELLE ES TRADICIONAL
            // =========================================================



                // Paso A: ¿Terminamos de cargar el carro anterior?
                if(!bandera && tiempo_universal == tiempo_carga){
                    // Rodar la cola
                    for(int j = 0; j < indice_tradicional - 1; j++){
                        vector_colaTradicional[j] = vector_colaTradicional[j+1];
                    }
                    indice_tradicional--; // Sacamos un carro
                    bandera = true;       // Liberamos la rampa

                }

                // Paso B: ¿Podemos meter un carro nuevo?
                if(bandera && indice_tradicional > 0){

                //(GR) Logica de zarpado es la misma para ambos               

                if(debe_zarpar(vehiculos_cargados_viaje,
                                indice_tradicional,
                                peso_ferry_actual,
                                vector_ferrys[turno_ferry],
                                vector_colaTradicional[0],
                                pasajeros_actuales, tiempo_universal)){

                        // ZARPA

                        printf("El ferry %s zarpa en el minuto %d con %d vehiculos y %d pasajeros\n",
                        vector_ferrys[turno_ferry].nombre,
                        tiempo_universal,
                        vehiculos_cargados_viaje,
                        pasajeros_actuales);


                        turno_ferry = (turno_ferry + 1) % MAX_FERRYS;
                        peso_ferry_actual = 0;
                        vehiculos_cargados_viaje = 0;
                        pasajeros_actuales = 0;
                        bandera = true;
                    }else{
                        // CARGAR
                        printf("Minuto %d: Cargando vehiculo %s en ferry %s\n",
                        tiempo_universal,
                        vector_colaTradicional[0].placa,
                        vector_ferrys[turno_ferry].nombre);

                        peso_ferry_actual += vector_colaTradicional[0].peso;
                        vehiculos_cargados_viaje++;

                        pasajeros_actuales += vector_colaTradicional[0].Num_pasajeros_adultos + vector_colaTradicional[0].Num_pasajeros_tercera_edad + 1;

                        tiempo_carga = tiempo_universal + 3;
                        bandera = false;
                    }
                }
            

            }else{
            // =========================================================
            // LÓGICA SI EL FERRY EN EL MUELLE ES EXPRESS
            // =========================================================
            
                // Paso A: ¿Terminamos de cargar el carro anterior?
                if(!bandera && tiempo_universal == tiempo_carga){
                    // Rodar la cola Express
                    for(int j = 0; j < indice_express - 1; j++){
                        vector_colaExpress[j] = vector_colaExpress[j+1];
                    }
                    indice_express--; 
                    bandera = true; 
                }


                // Paso B: ¿Podemos meter un carro nuevo?
                if(bandera && indice_express > 0){

                //(GR) Logica de zarpado es la misma para ambos       

                if(debe_zarpar(vehiculos_cargados_viaje,
                            indice_express,
                            peso_ferry_actual,
                            vector_ferrys[turno_ferry],
                            vector_colaExpress[0],
                            pasajeros_actuales, tiempo_universal)){

                    // ZARPA

                    printf("El ferry %s zarpa en el minuto %d con %d vehiculos y %d pasajeros\n",
                    vector_ferrys[turno_ferry].nombre,
                    tiempo_universal,
                    vehiculos_cargados_viaje,
                    pasajeros_actuales);


                    turno_ferry = (turno_ferry + 1) % MAX_FERRYS;
                    peso_ferry_actual = 0;
                    vehiculos_cargados_viaje = 0;
                    pasajeros_actuales = 0;
                    bandera = true;
                }else{
                    // CARGAR

                    printf("Minuto %d: Cargando vehiculo %s en ferry %s\n",
                    tiempo_universal,
                    vector_colaExpress[0].placa,
                    vector_ferrys[turno_ferry].nombre);

                    peso_ferry_actual += vector_colaExpress[0].peso;
                    vehiculos_cargados_viaje++;

                    pasajeros_actuales += vector_colaExpress[0].Num_pasajeros_adultos + vector_colaExpress[0].Num_pasajeros_tercera_edad + 1;

                    tiempo_carga = tiempo_universal + 3;
                    bandera = false;
                }
                }

            }
        

                //EL TIEMPO AVANZA
              tiempo_universal++;
        }

    
    }

    //cerrar archivos 

    fclose(entrada);
    fclose(salida);

    return 0;
}

//FUNCIONES//

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
    vectores_auxiliar[0].tipo = 0; 

    // Ferry ID 2 (Tradicional)
    strcpy(vectores_auxiliar[1].nombre, "La Isabela");
    vectores_auxiliar[1].tiempo_de_viaje = 65;
    vectores_auxiliar[1].tipo = 1;

    // Ferry ID 3 (Tradicional)
    strcpy(vectores_auxiliar[2].nombre, "La Margariteña");
    vectores_auxiliar[2].tiempo_de_viaje = 65;
    vectores_auxiliar[2].tipo = 1;

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

//Elimine funcion ferry_esta_lleno() porque debe_zarpar() hace lo mismo (GR)

// (GR) VERSION CORREGIDA
/*

La funcion determina si el ferry que está actualmente en el muelle debe salir en ese minuto de la 
simulación, evaluando varias reglas operativas: primero verifica que haya alcanzado al menos el 30% mínimo de su 
capacidad de vehículos (salvo que sea el final del día, donde puede salir forzado), luego revisa si alcanzó la 
capacidad máxima de vehículos, la capacidad máxima de pasajeros, si ya no quedan vehículos en cola, o si el 
siguiente vehículo en espera haría que se exceda el peso máximo permitido; si se cumple cualquiera de esas 
condiciones después de alcanzar el mínimo, la función retorna true y el ferry zarpa, de lo contrario retorna false
y continúa cargando vehículos

NOTA: Elimine la funcion esta_lleno() porque esta, asi eso tambien y verifica condiciones


*/
bool debe_zarpar(int vehiculos_en_ferry,
                 int vehiculos_en_cola,
                 float peso_actual,
                 ferrys ferry_actual,
                 vehiculos siguiente_en_cola,
                 int pasajeros_actuales, int tiempo_universal)
{
    int cap_max = 0;
    int cap_min = 0;
    int cap_pasajeros = 0;
    float peso_max = (ferry_actual.tipo == 0) ? 60.0 : 80.0;

    // Asignar capacidades según ferry
    if (strcmp(ferry_actual.nombre, "Lilia Concepcion") == 0) {
        cap_max = 16;
        cap_min = 5;
        cap_pasajeros = 50;
    } 
    else if (strcmp(ferry_actual.nombre, "La Isabela") == 0) {
        cap_max = 20;
        cap_min = 6;
        cap_pasajeros = 70;
    } 
    else { // La Margariteña
        cap_max = 18;
        cap_min = 6;
        cap_pasajeros = 60;
    }

    // REGLA 0: Si no ha llegado al 30%, NO puede zarpar
    if (vehiculos_en_ferry < cap_min) {

    // Si es el final del día, zarpa igual
    if (tiempo_universal >= 1439)
        return true;

    return false;
    }

    // REGLA 1: Capacidad máxima de vehículos
    if (vehiculos_en_ferry >= cap_max)
        return true;

    // REGLA 2: Capacidad máxima de pasajeros
    if (pasajeros_actuales >= cap_pasajeros)
        return true;

    // REGLA 3: No hay más vehículos en cola
    if (vehiculos_en_cola == 0)
        return true;

    // REGLA 4: El siguiente vehículo excede el peso máximo
    if (peso_actual + siguiente_en_cola.peso > peso_max)
        return true;

    return false;
}//(GR)


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
    int tipo_vehiculo;
    int origen_vehiculo;
    int posee_pasajeros;
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
    while(fscanf(entrada,"%i %i %i %i %i %f %i %s %i\n",&codigo_vehiculo,
                            &numero_pasajeros_adut,&numero_pasajeros_TCED,&tipo_pasaje_adut,
                            &tipo_pasaje_TCED,&peso,&Hora_llegada,placa,&tipo_ferry) != EOF){
        //llenar vectores
        vector_vehiculos[indice].tipo = codigo_vehiculo / 100;
        vector_vehiculos[indice].procedencia = (codigo_vehiculo / 10) % 10;
        vector_vehiculos[indice].traslada_pasajeros = codigo_vehiculo % 10;
        vector_vehiculos[indice].Num_pasajeros_adultos = numero_pasajeros_adut;
        vector_vehiculos[indice].Num_pasajeros_tercera_edad = numero_pasajeros_TCED;
        vector_vehiculos[indice].pasaje_adquirido_adut = tipo_pasaje_adut;
        vector_vehiculos[indice].pasaje_adquirido_tercera_ed = tipo_pasaje_TCED;
        vector_vehiculos[indice].peso = peso;
         
    
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
        
        //acumentar indice_tradicional
        (*cantidad_vehiculos)++;
        indice++;

    }
}