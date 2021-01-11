#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){
    printf("Inodes : ");
    for( int i = 0; i < MAX_INODOS; i++)
        printf("%c",ext_bytemaps->bmap_inodos[i]);
    printf("\n");

    printf("Bloques : ");
    for( int i = 0; i < MAX_BLOQUES_PARTICION; i++)
        printf("%c",ext_bytemaps->bmap_bloques[i]);
    printf("\n");
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){
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


    if (pch-old_pch != 0){
            numeroChars = pch - (old_pch + 1);// calcular numero de caracteres
            strncpy(argumento1,old_pch + 1,numeroChars);
            argumento1[numeroChars] = '\0';// aniadir fin de string

            // calcular siguiente puntero
            old_pch = pch;
            pch=strchr(pch+1,'\n');
    }else{
        strcpy(argumento1,"none");
    }
    if (pch != NULL){
        numeroChars = pch - (old_pch + 1);
        strncpy(argumento2,old_pch + 1,numeroChars);
        argumento2[numeroChars] = '\0';// aniadir fin de string
    }else{
        strcpy(argumento2,"none");
    }

    return 0; // this will exit the input loop
}
