#pragma once
#include <string>
#include <fstream>
#include "structures.hpp"

Carte charger_carte(string nom_fichier);
Personnage charger_personnage(string nom_fichier);
Liste_equipement charger_equipement(string nom_fichier);
liste_monstre charger_liste_monstre(string nom_fichier);
Liste_contrainte charger_liste_contrainte(string nom_fichier);

// Initialise les entités dynamiques (monstres) basées sur la carte statique
void initialiser_monstres_depuis_carte(Carte &carte, liste_monstre &catalogue, liste_monstre &monstres_actifs);