#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <id_jugador_eliminado>" << endl;
        return 1;
    }

    int jugador_eliminado = atoi(argv[1]);

    // Amurrarse y reclamar
    cout<<"NOOOOO YAAAA NO YAAAPO, NO VALEEE YAPO, PUCHA OHH. YA CHAOOO\n";
    cout << "¡El jugador " << jugador_eliminado << " se amurra y reclama!\n";
    sleep(2);  // El jugador toma un momento para "reclamar" antes de terminar

    return 0;
}
