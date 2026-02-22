#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//CONSTANTES//
#define MAX_VEHICULOS 100
#define MAX_FERRYS 3
#define MAX_CANTIDAD_COLAS 70

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
int convertir_a_minutos(int hora_militar);

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

    //variables acumuladoras de peso del los ferrys
    float peso_actual_del_ferry_Isabela = 0;
    float peso_actual_del_ferry_Margariteña = 0 ;
    float peso_actual_del_ferry_Lilia = 0;



    //structuras//
    vehiculos vector_vehiculos[MAX_VEHICULOS];
    vehiculos vector_colaExpress[MAX_CANTIDAD_COLAS];
    vehiculos vector_colaTradiccional[MAX_CANTIDAD_COLAS];
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
        
        ferry_enMuelle = orden[turno_ferry];
        tiempo_universal = tiempo_menor;
        // Ciclo que simula el paso del dia minuto a minuto (hasta las 11:59 PM = 1439 mins)
        while(tiempo_universal < 1440){

            // 1. REVISAR LLEGADAS EN EL MINUTO ACTUAL
            for(int i = 0; i < cantidad_vehiculos; i++){
                        
                // Si la hora de llegada del vehiculo coincide con el reloj actual
                if(vector_vehiculos[i].tiempo_llegada == tiempo_universal){
                            
                    // Verificamos a qué cola va
                    if(vector_vehiculos[i].tipo_ferry == 1){ 
                        // Va a la tradicional. Copiamos TODO el struct de un golpe
                        vector_colaTradiccional[indice_tradicional] = vector_vehiculos[i];
                        indice_tradicional++; // Solo avanza si metimos un carro
                                
                    } else if (vector_vehiculos[i].tipo_ferry == 0) {
                        // Va a la express. Copiamos TODO el struct de un golpe
                        vector_colaExpress[indice_express] = vector_vehiculos[i];
                        indice_express++; // Solo avanza si metimos un carro
                        }
                    }
                }

            //condicional para saber que tipo de ferry esta en el muelle (tradicional o express)
            if((ferry_enMuelle == 2) || (ferry_enMuelle == 1)){

                // validar que se cumpla el tiempo de carga
                if(!bandera && tiempo_universal == tiempo_carga){
                    // ¡Carga exitosa!  
                    //Aquí usas tu ciclo for para rodar la cola hacia adelante
                    for(int j = 0; j < indice_tradicional - 1; j++){
                        vector_colaTradiccional[j] = vector_colaTradiccional[j+1];
                    }
                    indice_tradicional--; // Le restamos 1 porque sacamos un carro
                            
                    //Liberamos el muelle para que pueda entrar otro
                    bandera = true; 
                }

                // Si el muelle está libre (bandera true) y hay carros esperando en la cola
                if(bandera && indice_tradicional > 0){
                    //condicional para saber en cual de los dos ferrys estamos cargando
                    //MARGARITEÑO
                    if(ferry_enMuelle == 3){
                            
                        // Revisamos si el primer carro de la cola cabe en el ferry
                        if(!ferry_esta_lleno(peso_actual_del_ferry_Margariteña, 1) ) { 
                            // ¡Sí cabe! Empezamos a cargarlo
                            // (Aquí puedes llamar a tu función para copiar el carro al vector_ferry)
                            tiempo_carga = tiempo_universal + 3; // Bloqueamos por 3 mins
                            bandera = false;                     // El muelle ahora está ocupado
                        } else {
                            // EL FERRY ESTÁ LLENO (o se cumplió la regla del 30%)
                            // Aquí iría la lógica para que el ferry zarpe 
                            // y pongas el siguiente ferry de orden[] en el muelle.
                        }
                    //SI NO ES EL ISABELA
                    }else{
                        peso_actual_del_ferry_Isabela = 0; //cambiar despues
                        // Revisamos si el primer carro de la cola cabe en el ferry
                        if(!ferry_esta_lleno(peso_actual_del_ferry_Isabela, 1)) { 
                            // ¡Sí cabe! Empezamos a cargarlo
                            // (Aquí puedes llamar a tu función para copiar el carro al vector_ferry)
                            tiempo_carga = tiempo_universal + 3; // Bloqueamos por 3 mins
                            bandera = false;                     // El muelle ahora está ocupado
                        } else {
                            // EL FERRY ESTÁ LLENO (o se cumplió la regla del 30%)
                            // Aquí iría la lógica para que el ferry zarpe 
                            // y pongas el siguiente ferry de orden[] en el muelle.
                        }
                    }
                    //LILIA EXPRESS
                    }else{
                        // validar que se cumpla el tiempo de carga
                        if(!bandera && tiempo_universal == tiempo_carga){
                            // ¡Carga exitosa! 
                            //Aquí usas tu ciclo for para rodar la cola hacia adelante
                            for(int j = 0; j < indice_express - 1; j++){
                                vector_colaExpress[j] = vector_colaExpress[j+1];
                            }
                            indice_express--; // Le restamos 1 porque sacamos un carro
                                
                            //Liberamos el muelle para que pueda entrar otro
                            bandera = true; 
                        }

                        // Si el muelle está libre (bandera true) y hay carros esperando en la cola
                    if(bandera && indice_express > 0){

                        peso_actual_del_ferry_Lilia = 0; //cambiar despues
                            
                        // Revisamos si el primer carro de la cola cabe en el ferry
                        if(!ferry_esta_lleno(peso_actual_del_ferry_Lilia,0) ) { 
                            // ¡Sí cabe! Empezamos a cargarlo
                            // (Aquí puedes llamar a tu función para copiar el carro al vector_ferry)
                            tiempo_carga = tiempo_universal + 3; // Bloqueamos por 3 mins
                            bandera = false;                     // El muelle ahora está ocupado
                        } else {
                            // EL FERRY ESTÁ LLENO (o se cumplió la regla del 30%)
                            // Aquí iría la lógica para que el ferry zarpe 
                            // y pongas el siguiente ferry de orden[] en el muelle.
                        }
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

/**
 * @brief Verifica si el ferry actual ya alcanzó su límite de peso.
 * * @param peso_acumulado El peso total de los vehículos que ya están dentro del ferry.
 * @param tipo_ferry 0 para el Express (max 60T), 1 para Tradicional (max 80T).
 * @return true si ya no le cabe más peso, false si aún tiene espacio.
 */
bool ferry_esta_lleno(float peso_acumulado, int tipo_ferry) {
    //var
    bool bandera = false; //asuminos directamente que no hay espacio

    if (tipo_ferry == 0) {
        // Es un ferry Express
        if (peso_acumulado >= 60.0) {
            bandera = true;
        }
    } else if (tipo_ferry == 1) {
        // Es un ferry Tradicional
        if (peso_acumulado >= 80.0) {
            bandera = true;
        }
    }
    
    return bandera; // Si no entró en los if anteriores, es porque aún hay espacio
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
    //inicializar contador
    *cantidad_vehiculos = 0;

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