#pragma once
#include "wrapper_ncurses.hpp" // Nécessaire pour les couleurs

// Dimensions et limites
const size_t MAX_CONTRAINTES = 15;
const size_t MAX_MONSTRES = 15;
const size_t MAX_ITEMS = 20;
const size_t NB_STATS = 6;

// Codes couleurs ncurses
const int C_DEFAUT = 0;
const int C_MUR = 6;
const int C_JOUEUR = 4;
const int C_MONSTRE = 2;
const int C_EQUIPEMENT = 1;
const int C_PORTE = 5;

// Raccourci de type
using Stats = int[NB_STATS];