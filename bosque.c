#include<stdio.h>
#define MAX_DIM 100

typedef struct{
    int energia;
    int vidas;
    char posicion;
}Aventurero;

void validarPosicion(Aventurero persona){
    persona.vidas = 3;
    persona.energia = 10;

    if(persona.energia)
}

void recorrer_espiral_matriz(FILE *archivo, char matriz[MAX_DIM][MAX_DIM]) {
    // Definimos los 4 límites de la matriz
    int inicio_fila = 0;
    int fin_fila = MAX_DIM - 1;
    int inicio_col = 0;
    int fin_col = MAX_DIM - 1;

    // El bucle continúa mientras los límites no se crucen
    while (inicio_fila <= fin_fila && inicio_col <= fin_col) {
        
        // 1. Recorrer de Izquierda a Derecha (Fila superior)
        for (int i = inicio_col; i <= fin_col; i++) {
            fscanf(" %c", &matriz[inicio_fila][i]);
        }
        inicio_fila++; // Bajamos el límite superior
        
        // 2. Recorrer de Arriba a Abajo (Columna derecha)
        for (int i = inicio_fila; i <= fin_fila; i++) {
            fscanf(" %c", &matriz[i][fin_col]);
        }
        fin_col--; // Movemos el límite derecho hacia la izquierda
        
        // Verificamos si aún hay filas por recorrer (evita duplicados en matrices no cuadradas)
        if (inicio_fila <= fin_fila) {
            // 3. Recorrer de Derecha a Izquierda (Fila inferior)
            for (int i = fin_col; i >= inicio_col; i--) {
                fscanf(" %c", &matriz[fin_fila][i]);
            }
            fin_fila--; // Subimos el límite inferior
        }
        
        // Verificamos si aún hay columnas por recorrer
        if (inicio_col <= fin_col) {
            // 4. Recorrer de Abajo a Arriba (Columna izquierda)
            for (int i = fin_fila; i >= inicio_fila; i--) {
                fscanf("%c ", &matriz[i][inicio_col]);
            }
            inicio_col++; // Movemos el límite izquierdo hacia la derecha
        }
    }
    printf("\n");
}


int main(){

    FILE *entrada = fopen("bosque.in", "r");
    

    if(entrada == NULL){
        puts("No se pudo abrir el archivo\n");
        fclose(entrada);
        return 1;
    }else{
        //FILE *salidada = fopen("bosque.out", "w");
        int dimension; //N

        fscanf(entrada, "%d", &dimension);
        if(dimension > MAX_DIM){
            puts("Dimension demasiado grande\n");
            return 1;
        }

        char terreno[dimension][dimension];

        recorrer_espiral_matriz(entrada, terreno);

        /*
        for(int i = 0; i < dimension; i++){
            for(int j = 0; j < dimension; j++){
                printf("%c ", terreno[i][j].posicion);
            }
            printf("\n");
        }*/



    }   

    return 0;
}