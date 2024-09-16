#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <cantidad_de_jugadores>" << endl;
        return 1;
    }
    cout<<"corriendo observer"<<argc<<endl<<argv<<"observando skrr"<<endl;
    int players_amount = atoi(argv[1]);

    // Leer votos de los jugadores
    int votos[players_amount];
    int contador_votos[players_amount] = {0};
    int pipe_fd = 0;  // El pipe que conecta con el padre

    for (int i = 0; i < players_amount; i++) {
        int voto;
        read(pipe_fd, &voto, sizeof(voto));  // Leer el voto del pipe
        contador_votos[voto]++;
    }

    // Encontrar al jugador más votado
    int jugador_eliminado = 0;
    int max_votos = 0;
    for (int i = 0; i < players_amount; i++) {
        if (contador_votos[i] > max_votos) {
            max_votos = contador_votos[i];
            jugador_eliminado = i;
        }
    }

    // Anunciar el jugador eliminado
    cout << "El jugador " << jugador_eliminado << " ha sido eliminado." << endl;

    // Ejecutar el archivo de amurrar para el jugador eliminado
    pid_t pid_amurra = fork();
    if (pid_amurra == 0) {
        // El jugador eliminado ejecuta el archivo "amurrar"
        execlp("./amurro", "amurro", to_string(jugador_eliminado).c_str(), NULL);
        // Si execlp falla
        perror("Error ejecutando el proceso de amurrar");
        exit(1);
    }

    // Esperar a que el proceso de amurrar termine
    waitpid(pid_amurra, NULL, 0);

    return 0;
}
