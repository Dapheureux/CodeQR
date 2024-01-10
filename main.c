#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int is_black;
}
QRModule;

//structure pour representer le code QR
typedef struct{
    int version;
    QRModule** modules;
}QRCode;

//Fonction pour initialiser un code QR
QRCode* initQRCode(int version){
    QRCode* code = malloc(sizeof(QRCode));
    code->version = version;
    code->modules = malloc(sizeof(QRModule*)*version*version);
    for(int i = 0; i < version*version; i++){
        code->modules[i] = malloc(sizeof(QRModule));
    }
    return code;
} 

// fonction pour liberer  la memoire allouée pour le code QR
void freeQRCode(QRCode* code){
    for(int i = 0; i < code->version*code->version; i++){
        free(code->modules[i]);
    }
    free(code->modules);
    free(code);
}
// Fonction pour afficher le code QR
void printQRCode(QRCode* code){
    for (int i=0; i<code->version; i++){
        for (int j=0; j<code->version; j++){
            printf("%c ", code->modules[i*code->version+j]->is_black);
            }
        printf("\n");
        }
    }

// Fonction principale pour générer un code QR
void generateQRCode(char* data){
    //TODO:
}

int main(){
    char* data_to_encode[] = "Hello World";
    generateQRCode(data_to_encode);
    return 0;
}
