#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#define MAX_BUFFER 512

typedef struct {
    char cedula[20];
    char nombre[100];
    char direccion[100];
    char telefono[20];
} Estudiante;

typedef struct {
    char cedula[20];
    char nombre[100];
    char direccion[100];
    char telefono[20];
    char gradoAcademico[50];
} Profesor;

typedef struct {
    char codigo[20];
    char descripcion[100];
} Materia;

typedef struct {
    char codigoMatricula[20];
    char cedulaEstudiante[20];
    char cedulaProfesor[20];
    char grupo[20];
    char nrc[20];
    char codigoMateria[20];
    char horario[50];
} Matricula;

typedef struct {
    long tipo;
    int operacion;
    char datos[MAX_BUFFER];
} Mensaje;

#endif