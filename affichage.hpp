#pragma once
#include "structures.hpp"

void initialiser_affichage();
void afficher_carte(Carte &main_carte);
void afficher_hud(Personnage &perso);
void mise_a_jour_rendu(Carte &main_carte, Personnage &perso, liste_monstre &monstres);