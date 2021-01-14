#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){//Esta funcion se encarga del printeo de los Bytemaps
    printf("Inodes : ");//Desde esta linea hasta la 13 se encarga de imprimir por pantalla el bytemap de inodos 
    for( int i = 0; i < MAX_INODOS; i++)
        printf("%i",ext_bytemaps->bmap_inodos[i]);
    printf("\n");

    printf("Bloques : ");//Desde esta linea hasta la 18 se encarga de imprimir por pantalla el Bytemap de bloques
    for( int i = 0; i < MAX_BLOQUES_PARTICION; i++)
        printf("%i",ext_bytemaps->bmap_bloques[i]);
    printf("\n");
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){//Esta funcion se encarga de comprobar que el comando introducido existe
    char * pch,*old_pch;
    int numeroChars;

    pch=strchr(strcomando,' ');
    if (pch==NULL)
        pch=strchr(strcomando,'\n');

    if (pch-strcomando != 0){

        strncpy(orden,strcomando,pch-strcomando);
        orden[pch-strcomando] = '\0';// aniadir fin de string

        // calcular siguiente puntero
        old_pch = pch;
        pch=strchr(pch+1,' ');
        if (pch==NULL)  pch=strchr(strcomando,'\n');

    }else{
        return -1;
    }


    if (pch-old_pch != 0 ){
            numeroChars = pch - (old_pch + 1);// calcular numero de caracteres
            strncpy(argumento1,old_pch + 1,numeroChars);
            argumento1[numeroChars] = '\0';// aniadir fin de string

            // calcular siguiente puntero
            old_pch = pch;
            pch=strchr(pch+1,'\n');
    }else{
        strcpy(argumento1,"none");
    }
    if (pch-old_pch != 0 && pch != NULL){
        numeroChars = pch - (old_pch + 1);
        strncpy(argumento2,old_pch + 1,numeroChars);
        argumento2[numeroChars] = '\0';// aniadir fin de string
    }else{
        strcpy(argumento2,"none");
    }

    return 0; // Esto nos permitira salir del bucle de la entrada
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){//Esta funcion se encarga de imprimir por pantalla el contenido del superbloque
    printf("Bloque %d Bytes\n", psup->s_block_size);
    printf("inodos particion = %d\n", psup->s_inodes_count);
    printf("inodes libres = %d\n", psup->s_free_inodes_count);
    printf("Bloques particion = %d\n", psup->s_blocks_count);
    printf("Bloques libres = %d\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos = %d\n", psup->s_first_data_block);
}
void getData(EXT_DATOS *memdatos,int bloque){//Esta funcion se coupa de sacar los datos de los bloques
    for( int i = 0; i < SIZE_BLOQUE; i++)
        printf("%c",memdatos->dato[(SIZE_BLOQUE*(bloque - 4)) + i ]);
}


int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){//Leva a cabo la funcion de buscar el fichero 
    
    //printf("%d",inodos->blq_inodos[directorio->dir_inodo].i_nbloque[0]);
    for(int i = 0; i < MAX_FICHEROS; i++)
        if(strcmp(directorio[i].dir_nfich,nombre) == 0){
            return inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[0];
        }
    return 0;
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){//Se encarga de ostarte una lista con todos los ficheros existentes
    for(int i = 0; i < MAX_FICHEROS; i++)
        if ( directorio[i].dir_inodo != NULL_INODO){//Con este if nos encargamos de imprimir aquellos directorios que son existentes
            char* nombre= directorio[i].dir_nfich;
            int tamanio = inodos->blq_inodos[directorio[i].dir_inodo].size_fichero;
            int inode = directorio[i].dir_inodo;
            unsigned short int *bloques = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
            printf("%s",nombre);
            int espacios = 20-strlen(nombre);
            for ( int i = 0; i < espacios;i++)//En este for vamos imprimiendo el tamaño de los ficheros
                printf(" ");
            printf("Tamanio: %d",tamanio);

            espacios = 15-floor(log10(abs(tamanio))) + 1;
            if ( tamanio == 0 ) espacios = 16;

            for ( int i = 0; i < espacios;i++)//A continuacion imprimimos los bloques corresponidentes al fichero
                printf(" ");
            printf("Bloques: ");
            for (int i = 0; i < MAX_NUMS_BLOQUE_INODO;i++)
                if ( bloques[i] != NULL_BLOQUE )
                    printf("%i ", bloques[i]);
            printf("\n");
        }
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,char *nombreantiguo, char *nombrenuevo){//Con esta funcion nos encargamos del comando de renombrar 
    int noesta = 1;
    for(int i = 0; i < MAX_FICHEROS; i++){//Con este for y el if que le continua nos aseguramos que el nuevo nombre no este en ningun fichero
        if(strcmp(directorio[i].dir_nfich,nombrenuevo) == 0){
            noesta=0;
        }
    }
    if(noesta){//A partir de aqui buscamos el fichero con el nombre antiguo y cargamos el nuevo nombre en su lugar
        for(int j = 0; j < MAX_FICHEROS; j++){
            if(strcmp(directorio[j].dir_nfich,nombreantiguo) == 0){
                strcpy(directorio[j].dir_nfich,nombrenuevo);
                return 1;
            }
        }
    }
    return 0;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,char *nombre,  FILE *fich){//Se encarga de borrar el fichero
    for(int i = 0; i < MAX_INODOS; i++){
        // encontrar fichero
        if(strcmp(directorio[i].dir_nfich,nombre) ==0){
            // encontrar bloques
            unsigned short int *bloques = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
            // liberar los bloques
            for( int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++){
                if(bloques[j] != NULL_BLOQUE){
                    ext_bytemaps->bmap_bloques[bloques[j]] = 0;
                    bloques[j] = NULL_BLOQUE; // resetear inode
                    (ext_superblock->s_free_blocks_count)++;
                }
            }
            inodos->blq_inodos[directorio[i].dir_inodo].size_fichero = 0;// resetear inode
            // liberar el resto de linode
            ext_bytemaps->bmap_inodos[directorio[i].dir_inodo] = 0;
            (ext_superblock->s_free_inodes_count)++;
            // liberar entrada a dir
            directorio[i].dir_inodo = NULL_INODO;
        }
    }
    return 1;
}
//En la funcion que prosigue nos encargamos de copiar los datos de un fichero y cargarlos en otro
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich){
    EXT_ENTRADA_DIR *ficheroNuevo;
    EXT_SIMPLE_INODE *inode;
    EXT_SIMPLE_INODE *inodeOriginal;
    // encontrar fichero original
    for(int i = 0; i < MAX_FICHEROS; i++)
        if(strcmp(directorio[i].dir_nfich,nombreorigen) == 0){
            inodeOriginal = &(inodos->blq_inodos[directorio[i].dir_inodo]);
            break;
        }
    // buscar primera entrada libre de directorio
    for(int i = 0; i < MAX_FICHEROS; i++)
        if ( directorio[i].dir_inodo == NULL_INODO){
            ficheroNuevo = &directorio[i];
            break;
        }
    strcpy((ficheroNuevo->dir_nfich),nombredestino);

    for(int i = 0; i < MAX_INODOS; i++)
        if( ext_bytemaps->bmap_inodos[i] == 0){
            inode = &(inodos->blq_inodos[i]);
            ficheroNuevo->dir_inodo = i;
            ext_bytemaps->bmap_inodos[i] = 1;
            break;
        }
    inode->size_fichero = inodeOriginal->size_fichero;
    ext_superblock->s_free_inodes_count--;
    // copiar los bloques
    for( int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++){
        if(inodeOriginal->i_nbloque[j] != NULL_BLOQUE){
            for (int i = ext_superblock->s_first_data_block; i < MAX_BLOQUES_DATOS; i++){
                if(ext_bytemaps->bmap_bloques[i] == 0){
                    inode->i_nbloque[j] = i;
                    char *testing = &(memdatos->dato[inodeOriginal->i_nbloque[j] * SIZE_BLOQUE]);
                    memcpy(&(memdatos->dato[(i - 4)*SIZE_BLOQUE]),&(memdatos->dato[(-4 + inodeOriginal->i_nbloque[j]) * SIZE_BLOQUE]),SIZE_BLOQUE);
                    ext_superblock->s_free_blocks_count--;
                    ext_bytemaps->bmap_bloques[i] = 1;
                    break;
                }
            }
        }
    }
    return 1;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_DATOS *memdatos, char *nombre){//Lleva a cabo el printeo de los daots de un fichero
   unsigned short int *bloques;
   for(int i = 0; i < MAX_FICHEROS; i++){
        if(strcmp(directorio[i].dir_nfich,nombre) ==0){
            bloques = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
        }
    }
    for( int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++){
        if(bloques[j] != NULL_BLOQUE){
            getData(memdatos,bloques[j]);
        }
    }
    printf("\n");
}

void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich){//Grabamos los inodos y el directorio en el fichero
    // abrimos el fichero
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    rewind(fich);
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

    memcpy(&datosfich[3],directorio, SIZE_BLOQUE);
    memcpy(&datosfich[2],inodos, SIZE_BLOQUE);

    rewind(fich);
    fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich){//Grabamos los Bytemaps en el fichero
    // abrimos el fichero
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    rewind(fich);
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

    memcpy(&datosfich[1],ext_bytemaps, SIZE_BLOQUE);

    rewind(fich);
    fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich){//Graba los datos del superbloque  en el fichero
    // abrimos el fichero
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    rewind(fich);
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

    memcpy(&datosfich[0],ext_superblock, SIZE_BLOQUE);

    rewind(fich);
    fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich){//Graba todos los datos de los ficheros en el fichero
    // abrimos el fichero
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    rewind(fich);
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

    memcpy(&datosfich[4],memdatos,MAX_BLOQUES_DATOS*SIZE_BLOQUE);

    rewind(fich);
    fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

