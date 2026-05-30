
all:
	gcc servidor/servidor.c -o servidor -pthread
	gcc cliente/cliente.c -o cliente