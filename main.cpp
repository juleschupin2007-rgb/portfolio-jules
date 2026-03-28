#include "chargement.hpp"
#include "affichage.hpp"
#include "moteur_jeu.hpp"
#include "wrapper_ncurses.hpp"

int main() {
    initialiser_affichage();

    // 1. Chargement des données
    Carte niveau = charger_carte("level.txt");
    Personnage joueur = charger_personnage("personnage.txt");
    Liste_equipement items = charger_equipement("equipement.txt");
    liste_monstre catalogue = charger_liste_monstre("monstres.txt");
    
    // 2. Initialisation des monstres
    liste_monstre monstres_actifs; 
    initialiser_monstres_depuis_carte(niveau, catalogue, monstres_actifs);

    // 3. Premier affichage
    calculer_champ_vision(joueur, niveau);
    
    // On efface le joueur de la carte statique s'il y est dessiné
    niveau.map[joueur.pos.x][joueur.pos.y] = '.';
    
    mise_a_jour_rendu(niveau, joueur, monstres_actifs);

    // 4. Boucle de jeu
    bool en_cours = true;
    while (en_cours) {
        afficher_hud(joueur);
        
        int touche = saisie_bloquante();

        if (touche == 'a') {
            en_cours = false;
        } else {
            tour_joueur(touche, niveau, joueur, items, monstres_actifs);
            tour_monstres(monstres_actifs, niveau, joueur);
            
            calculer_champ_vision(joueur, niveau);
            mise_a_jour_rendu(niveau, joueur, monstres_actifs);

        }
    }

    fermer_console();
    return 0;
}