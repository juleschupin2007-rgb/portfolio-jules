#include "affichage.hpp"
#include "wrapper_ncurses.hpp"

// Définition des couleurs
PaireCouleur g_couleurs[] = {
    {COLOR_WHITE, COLOR_BLACK}, {COLOR_BLUE, COLOR_BLACK},
    {COLOR_RED, COLOR_BLACK}, {COLOR_YELLOW, COLOR_BLACK},
    {COLOR_MAGENTA, COLOR_BLACK}, {COLOR_WHITE, COLOR_BLACK}
};

void initialiser_affichage() {
    initialiser_console(g_couleurs, 6);
}

void afficher_carte(Carte &main_carte) {
    for (int x = 0; x < CONSOLE_Y_SIZE; x++) {
        for (size_t k = 0; k < size(main_carte.map[x]); k++) {
            
            if (main_carte.decouverte[x][k] == false) {
                changer_couleur(C_DEFAUT);
                ecrire_char(k, x, ' ');
            } else {
                char s = main_carte.map[x][k];
                int coul = C_DEFAUT;
                
                if (s == '@') coul = C_JOUEUR;
                else if (s >= 'a' && s <= 'z') coul = C_EQUIPEMENT;
                else if (s >= 'A' && s <= 'Z') coul = C_MONSTRE;
                else if (s == '#') coul = C_MUR;
                else if (s >= '0' && s <= '9') coul = C_PORTE;

                changer_couleur(coul | A_BOLD);
                ecrire_char(k, x, s);
            }
        }
    }
}

void afficher_hud(Personnage &perso){
    changer_couleur(C_JOUEUR);
    string ligne1 = "Nom: " + perso.nom + " | Pos: (" + to_string(perso.pos.x) + "," + to_string(perso.pos.y) + ")";
    string ligne3 = "Force: " + to_string(perso.stats_perso[3]) +
                    " | Butin: " + to_string(perso.stats_perso[2]) +
                    " | Peur: " + to_string(perso.stats_perso[4])+" | Discretion: " +to_string(perso.stats_perso[1]);
    changer_couleur(C_DEFAUT);
    ecrire_string("                                                                ", 0, 23);

    if (perso.message_info != "") {
        changer_couleur(C_EQUIPEMENT);
        ecrire_string("                                                                                ",0,23);
        ecrire_string(  perso.message_info, 0, 23);
    }
    else{
        ecrire_string(ligne1+ligne3,0,23);
    }
}


void mise_a_jour_rendu(Carte &main_carte, Personnage &perso, liste_monstre &monstres) {
    afficher_carte(main_carte);

    // Dessin des monstres vivants par-dessus la carte
    for(int i = 0; i < monstres.nb_monstres; i++) {
        Monstre &m = monstres.list_monstre[i];
        if(m.actif && main_carte.decouverte[m.pos.x][m.pos.y]) {
             changer_couleur(C_MONSTRE | A_BOLD);
             ecrire_char(m.pos.y, m.pos.x, m.id_char);
        }
    }

    // Dessin du joueur en dernier
    changer_couleur(C_JOUEUR);
    ecrire_char(perso.pos.y, perso.pos.x, '@');
}