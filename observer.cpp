#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

using namespace std;

const char* fifo_jugadores = "/tmp/fifo_jugadores";
const char* fifo_observador = "/tmp/fifo_observador";

int main() {
    vector<int> votos;  // Votos acumulados por cada jugador
    int jugadores_inicializados = false;

    while (true) {
        cout << "Esperando votos de los jugadores..." << endl;

        // Abrir el FIFO para leer los votos
        int fifo_fd = open(fifo_jugadores, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Error al abrir el FIFO de jugadores");
            return 1;
        }

        int voto;
        while (read(fifo_fd, &voto, sizeof(voto)) > 0) {
            if (!jugadores_inicializados) {
                // Inicializa el tamaño del vector votos la primera vez
                votos.resize(voto + 1, 0);
                jugadores_inicializados = true;
            }

            // Contabiliza los votos
            votos[voto]++;
        }

        close(fifo_fd);

        // Encontrar el jugador más votado
        int jugador_eliminado = -1;
        int max_votos = 0;
        for (size_t i = 0; i < votos.size(); i++) {
            if (votos[i] > max_votos) {
                max_votos = votos[i];
                jugador_eliminado = i;
            }
        }

        if (jugador_eliminado == -1) {
            cout << "No se recibieron suficientes votos." << endl;
            continue;
        }

        // Enviar el jugador eliminado al proceso de game.cpp
        fifo_fd = open(fifo_observador, O_WRONLY);
        write(fifo_fd, &jugador_eliminado, sizeof(jugador_eliminado));
        close(fifo_fd);

        // Resetear los votos para la próxima ronda
        votos[jugador_eliminado] = 0;
    }

    return 0;
}
