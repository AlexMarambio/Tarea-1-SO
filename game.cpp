#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

using namespace std;

const char* fifo_jugadores = "/tmp/fifo_jugadores";
const char* fifo_observador = "/tmp/fifo_observador";

void jugador(int id, int players_amount, int fifo_fd) {
    srand(time(NULL) + id);  // Semilla diferente para cada jugador
    while (true) {  // Cada jugador vota en cada ronda
        int vote = rand() % players_amount;  // Elige un jugador al azar
        cout << "Jugador " << id << " vota por el jugador " << vote << endl;
        write(fifo_fd, &vote, sizeof(vote));  // Envía el voto al observador
        sleep(2);  // Simular la espera de la siguiente ronda
    }
}

int main() {
    // Crear archivos FIFO si no existen
    mkfifo(fifo_jugadores, 0666);  // Para que los jugadores envíen los votos al observador
    mkfifo(fifo_observador, 0666);  // Para que el observador envíe el jugador eliminado al padre

    int players_amount = 0;
    cout << "Seleccione la cantidad de jugadores: ";
    cin >> players_amount;

    if (players_amount < 2) {
        cout << "No cumple con la cantidad mínima de 2 jugadores." << endl;
        return 1;
    }

    // Compilar y ejecutar el observador en segundo plano
    if (system("g++ -o observer observer.cpp") != 0) {
        cerr << "Error al compilar observer.cpp" << endl;
        return 1;
    }
    if (system("./observer &") == -1) {
        cerr << "Error al ejecutar el observador" << endl;
        return 1;
    }

    // Compilar el programa de amurro
    if (system("g++ -o amurro amurro.cpp") != 0) {
        cerr << "Error al compilar amurro.cpp" << endl;
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
            int fifo_fd = open(fifo_jugadores, O_WRONLY);
            if (fifo_fd == -1) {
                perror("Error al abrir el FIFO de jugadores");
                exit(1);
            }
            jugador(i, players_amount, fifo_fd);
            close(fifo_fd);
            exit(0);
        }
    }

    // Proceso principal (padre)
    int ronda = 1;
    while (players_amount > 1) {
        cout << "\nComienza la ronda " << ronda << " con " << players_amount << " jugadores." << endl;

        // Leer el resultado del observador
        int fifo_fd = open(fifo_observador, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error al abrir el FIFO del observador");
            return 1;
        }

        int jugador_eliminado;
        read(fifo_fd, &jugador_eliminado, sizeof(jugador_eliminado));
        close(fifo_fd);

        cout << "El jugador " << jugador_eliminado << " ha sido eliminado." << endl;

        // Ejecutar el programa amurro
        pid = fork();
        if (pid == 0) {
            execlp("./amurro", "amurro", to_string(jugador_eliminado).c_str(), NULL);
            perror("Error ejecutando amurro");
            exit(1);
        }

        wait(NULL);  // Esperar que el jugador eliminado termine

        // Reducir la cantidad de jugadores
        players_amount--;
        ronda++;
    }

    cout << "¡El jugador final ha ganado el juego!" << endl;

    // Eliminar los FIFOs
    unlink(fifo_jugadores);
    unlink(fifo_observador);

    return 0;
}
