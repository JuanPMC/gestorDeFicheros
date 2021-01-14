    #include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps); //d
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2); //d
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);//d
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);//d
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);//d
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,char *nombreantiguo, char *nombrenuevo);//d
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

void getData(EXT_DATOS *memdatos,int bloque);


int main(){
	 char *comando[LONGITUD_COMANDO];
	 char *orden[LONGITUD_COMANDO];
	 char *argumento1[LONGITUD_COMANDO];
	 char *argumento2[LONGITUD_COMANDO];

	 int i,j;
	 unsigned long int m;
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;

     // Lectura del fichero completo de una sola vez
     //...

     fent = fopen("particion.bin","r+b");
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);


     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);

     // Buce de tratamiento de comandos
     for (;;){
		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
            if (strcmp(orden,"dir")==0) {
                Directorio(&directorio,&ext_blq_inodos);
                continue;
            }
            if (strcmp(orden,"info")==0) {
                LeeSuperBloque(&ext_superblock);
                continue;
            }
            if (strcmp(orden,"rename")==0) {
                if(Renombrar(&directorio,&ext_blq_inodos,argumento1,argumento2))
                    printf("%s renombrado a %s\n",argumento1,argumento2);
                else
                    printf("el fichero %s ya existe\n",argumento2);
                continue;
            }
            if (strcmp(orden,"remove")==0) {
                Borrar(&directorio,&ext_blq_inodos,&ext_bytemaps,&ext_superblock,argumento1,fent);
                continue;
            }
            if (strcmp(orden,"copy")==0) {
                Copiar(&directorio,&ext_blq_inodos,&ext_bytemaps,&ext_superblock,&memdatos,argumento1,argumento2,fent);
                continue;
            }
            if (strcmp(orden,"imprimir")==0) {
                Imprimir(&directorio,&ext_blq_inodos,&memdatos,argumento1);
                continue;
            }
            Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
            GrabarByteMaps(&ext_bytemaps,fent);
            GrabarSuperBloque(&ext_superblock,fent);
 /*           if (grabardatos)
                GrabarDatos(&memdatos,fent);
            grabardatos = 0;
       */     //Si el comando es salir se habrán escrito todos los metadatos
            //faltan los datos y cerrar

            if (strcmp(orden,"salir")==0){
                GrabarDatos(&memdatos,fent);
                fclose(fent);
                return 0;
            }
     }
}
