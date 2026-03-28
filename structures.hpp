#pragma once
#include <string>
#include "constantes.hpp"
#include "wrapper_ncurses.hpp"

using namespace std;

struct Position {
    int y;
    int x;
};

struct Equipement {
    char id;
    string nom;
    Stats stats_equip;
};

struct Liste_id_equip {
    int max_objets = MAX_ITEMS;
    int nb_equip;
    int liste_id[MAX_ITEMS];
};

struct Personnage {
    Position pos;
    string nom;
    Stats stats_perso;
    Liste_id_equip equipement;
    string message_info; // Buffer pour le HUD
};

struct Contrainte {
    Position pos;
    string nom;
    Stats conditions;
    int mode;
    Liste_id_equip oblig;
    Liste_id_equip interdit;
};

struct Monstre {
    char id_char;
    Position pos;
    string nom;
    Stats stats_monstre;
    Liste_id_equip equipement;
    bool actif;
    int ia; // 0: Aléatoire, 1: Traqueur
    Contrainte succes;
    Contrainte defaite;
    int portee;
};

struct Carte {
    string map[CONSOLE_Y_SIZE];
    bool decouverte[CONSOLE_Y_SIZE][80];
};

// Conteneurs statiques
struct Liste_equipement {
    int max_equipement = MAX_ITEMS;
    int nb_equip;
    Equipement liste_equip[MAX_ITEMS];
};

struct liste_monstre {
    int max_monstres = MAX_MONSTRES;
    int nb_monstres;
    Monstre list_monstre[MAX_MONSTRES];
};

struct Liste_contrainte {
    int max_contrainte = MAX_CONTRAINTES;
    int nb_contraintes;
    Contrainte liste_contr[MAX_CONTRAINTES];
};