#pragma once
#include <string>


// 1. NE PAS inclure "lib_projet_ncurses.hpp" ICI !
// C'est ce qui causait les "multiple definition".
#ifndef COLOR_BLACK
#define COLOR_BLACK   0
#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_WHITE   7
#endif

struct PaireCouleur {
    short fg;
    short bg;
};

// Définition de A_BOLD si nécessaire (souvent une valeur binaire)
#ifndef A_BOLD
#define A_BOLD 0x00200000 // Valeur standard pdcurses, sinon essayez 0
#endif
// 2. On définit la constante nous-mêmes
const int CONSOLE_Y_SIZE = 24;
const int CONSOLE_X_SIZE = 80;
const int KEY_DOWN  = 0402;
const int KEY_UP    = 0403;
const int KEY_LEFT  = 0404;
const int KEY_RIGHT = 0405;


// 3. Déclaration anticipée de la structure (ou sa définition si vous l'avez)
struct PaireCouleur; 

// 4. Prototypes (Notez le 'const' ajouté pour correspondre à votre erreur)
bool initialiser_console(const PaireCouleur* couleurs, int nb_couleurs);
void fermer_console();
void couleur_defaut();
void changer_couleur(int c);

void bouger_curseur(int x, int y);
void effacer_console(); // Attention: votre erreur mentionne effacer_console, pas effacer_ecran

void ecrire_char(int x, int y, char c);
void ecrire_string(const std::string& message, int x, int y);

int saisie_bloquante();
int saisie_non_bloquante(); // Ajouté selon vos logs
void attendre_millisecondes(int ms); // Ajouté selon vos logs