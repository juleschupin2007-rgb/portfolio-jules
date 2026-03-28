#include "outils.hpp"

int generer_aleatoire() {
    // Implémentation d'un générateur congruentiel linéaire simple
    // pour éviter la dépendance à <cstdlib>
    static unsigned long int next = 1;
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

int valeur_absolue(int valeur) {
    return (valeur < 0) ? -valeur : valeur;
}