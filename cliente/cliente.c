#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../common/estructuras.h"
#include "../common/protocolo.h"
#include "../common/constantes.h"

void menuPrincipal();
void enviarSolicitud(int operacion);

int main() {
    menuPrincipal();
    return 0;
}

void menuPrincipal() {
    int opcion;

    do {
        printf("\n");
        printf("=================================\n");
        printf(" SISTEMA DE MATRICULA\n");
        printf("=================================\n");
        printf("1. Estudiantes\n");
        printf("2. Profesores\n");
        printf("3. Materias\n");
        printf("4. Matriculas\n");
        printf("5. Salir\n");
        printf("=================================\n");
        printf("Seleccione una opcion: ");

        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                enviarSolicitud(INS_ESTUDIANTE);
                break;

            case 2:
                enviarSolicitud(INS_PROFESOR);
                break;

            case 3:
                enviarSolicitud(INS_MATERIA);
                break;

            case 4:
                enviarSolicitud(INS_MATRICULA);
                break;

            case 5:
                printf("\nSaliendo del sistema...\n");
                break;

            default:
                printf("\nOpcion invalida\n");
        }
    } while(opcion != 5);
}

// Enviar solicitud al servidor
void enviarSolicitud(int operacion)
{
    int socketCliente;

    struct sockaddr_in servidor;

    Mensaje mensaje;
    Mensaje respuesta;

    socketCliente = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(socketCliente < 0){
        perror("Error creando socket");
        return;
    }

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &servidor.sin_addr
    );

    if(connect(
        socketCliente,
        (struct sockaddr*)&servidor,
        sizeof(servidor)
    ) < 0) {
        perror("Error conectando al servidor");
        close(socketCliente);
        return;
    }

    mensaje.tipo = 1;
    mensaje.operacion = operacion;

    strcpy(
        mensaje.datos,
        "Solicitud desde cliente"
    );

    send(
        socketCliente,
        &mensaje,
        sizeof(Mensaje),
        0
    );

    recv(
        socketCliente,
        &respuesta,
        sizeof(Mensaje),
        0
    );

    printf("\nRespuesta servidor:\n");
    printf("%s\n", respuesta.datos);
    close(socketCliente);
}