#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void jugador(int id, int players_amount, int pipe_fd) {
    srand(time(NULL) + id);  // Semilla diferente para cada jugador
    int vote = rand() % players_amount;  // Elige un jugador al azar
    cout<<"voto realizado\n";
    write(pipe_fd, &vote, sizeof(vote));  // Envía el voto al observador
    exit(0);  // El jugador termina después de votar
}

int main() {
    int players_amount = 0;
    cout << "Seleccione la cantidad de jugadores\n";
    cin >> players_amount;

    if (players_amount < 2) {
        cout << "No cumple con la cantidad mínima mayor a 2 jugadores." << endl;
        return 1;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {  // Crear el pipe
        perror("Error creando el pipe");
        return 1;
    }

    // Crear procesos hijos (jugadores)
    pid_t pid;
    for (int i = 0; i < players_amount; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        }

        if (pid == 0) {  // Código del proceso hijo (jugador)
            cout<<getpid()<<endl;
            close(pipe_fd[0]);  // Cierra el lado de lectura del pipe en el hijo
            jugador(i, players_amount, pipe_fd[1]);
        }
    }

    // Proceso principal (padre) solo anuncia la ronda
    close(pipe_fd[1]);  // Cierra el lado de escritura del pipe en el padre
    cout << "Comienza la ronda..." << endl;

    // Llama al proceso observador
    pid = fork();
    if (pid == 0) {
        // Ejecuto el observador
        execlp("./observer", "observer", to_string(players_amount).c_str(), NULL);
        // Si execlp falla
        perror("Error ejecutando el observador");
        exit(1);
    }

    // Espera a que el observador termine
    wait(NULL);

    // Esperar a que todos los hijos (jugadores) terminen
    for (int i = 0; i < players_amount; i++) {
        wait(NULL);
    }

    return 0;
}
