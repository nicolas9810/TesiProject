#include <iostream>
#include <cstdlib>
#include <ctime>

float generateRandomValue() {
    // Genera un valore casuale tra 0 e 1
    float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    // Scala il valore nel range -0.20 a 0.20
    return (randomValue * 0.40f) - 0.20f;
}

int main() {
    // Inizializza il generatore di numeri casuali con il seme corrente del tempo
    srand(static_cast<unsigned int>(time(0)));

    // Genera e stampa un valore casuale tra -0.20 e 0.20
    float value = generateRandomValue();
    std::cout << "Random value between -0.20 and 0.20: " << value << std::endl;

    return 0;
}
