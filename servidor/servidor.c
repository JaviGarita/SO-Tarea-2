#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include "../common/estructuras.h"
#include "../common/protocolo.h"
#include "../common/constantes.h"

// Mutex global:
pthread_mutex_t mutexArchivos;

// ID de la cola de mensajes:
int colaMensajes;

// Función ejecutada por cada hilos:
void* atenderCliente(void* arg) {
    int socketCliente = *((int*)arg);

    free(arg);

    Mensaje mensaje;

    //Recibe solicitud del cliente
    int bytesRecibidos = recv(
        socketCliente,
        &mensaje,
        sizeof(Mensaje),
        0
    );

    if(bytesRecibidos <= 0) {
        close(socketCliente);
        pthread_exit(NULL);
    }

    // Enviar solicitud a la cola
    mensaje.tipo = 1;

    if(msgsnd(
        colaMensajes,
        &mensaje,
        sizeof(Mensaje) - sizeof(long),
        0
    ) == -1) {
        perror("Error enviando a cola");
        close(socketCliente);
        pthread_exit(NULL);
    }

    //Respuesta temporal
    Mensaje respuesta;

    respuesta.tipo = 1;
    respuesta.operacion = RESPUESTA_OK;

    strcpy(
        respuesta.datos,
        "Solicitud recibida por el servidor"
    );

    send(
        socketCliente,
        &respuesta,
        sizeof(Mensaje),
        0
    );

    close(socketCliente);
    pthread_exit(NULL);
}

int main()
{
    int servidorSocket;

    struct sockaddr_in servidorAddr;
    struct sockaddr_in clienteAddr;

    socklen_t clienteLen;

    // Inicializar mutex
    pthread_mutex_init(
        &mutexArchivos,
        NULL
    );

    colaMensajes = msgget(
        CLAVE_COLA,
        IPC_CREAT | 0666
    );

    if(colaMensajes == -1)
    {
        perror("Error creando cola");

        return 1;
    }

    servidorSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(servidorSocket < 0)
    {
        perror("Error creando socket");

        return 1;
    }

    servidorAddr.sin_family = AF_INET;
    servidorAddr.sin_addr.s_addr = INADDR_ANY;
    servidorAddr.sin_port = htons(PUERTO);

    if(bind(
        servidorSocket,
        (struct sockaddr*)&servidorAddr,
        sizeof(servidorAddr)
    ) < 0)
    {
        perror("Error en bind");

        close(servidorSocket);

        return 1;
    }

    if(listen(
        servidorSocket,
        10
    ) < 0)
    {
        perror("Error en listen");

        close(servidorSocket);

        return 1;
    }

    printf("\n");
    printf("=================================\n");
    printf(" SERVIDOR DE MATRICULA INICIADO\n");
    printf(" Puerto: %d\n", PUERTO);
    printf("=================================\n");

    while(1)
    {
        clienteLen = sizeof(clienteAddr);

        int* socketCliente =
            malloc(sizeof(int));

        *socketCliente = accept(
            servidorSocket,
            (struct sockaddr*)&clienteAddr,
            &clienteLen
        );

        if(*socketCliente < 0)
        {
            perror("Error en accept");

            free(socketCliente);

            continue;
        }

        printf(
            "\nCliente conectado: %s\n",
            inet_ntoa(clienteAddr.sin_addr)
        );

        pthread_t hilo;

        if(pthread_create(
            &hilo,
            NULL,
            atenderCliente,
            socketCliente
        ) != 0)
        {
            perror("Error creando hilo");

            free(socketCliente);

            continue;
        }

        pthread_detach(hilo);
    }

    close(servidorSocket);

    pthread_mutex_destroy(
        &mutexArchivos
    );

    msgctl(
        colaMensajes,
        IPC_RMID,
        NULL
    );

    return 0;
}