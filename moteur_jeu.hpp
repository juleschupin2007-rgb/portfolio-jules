#pragma once
#include "structures.hpp"

void calculer_champ_vision(Personnage &perso, Carte &ma_carte);
void tour_joueur(int touche, Carte &carte, Personnage &perso, Liste_equipement &items, liste_monstre &monstres);
void tour_monstres(liste_monstre &monstres, Carte &carte, Personnage &perso);