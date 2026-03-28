#include "moteur_jeu.hpp"
#include "outils.hpp"
#include "wrapper_ncurses.hpp"
#include <string>

// --- FONCTIONS INTERNES (Privées) ---

// Vérifie si une entité peut aller sur une case (Murs, Limites, Collisions)
bool est_position_valide(int x, int y, Carte &carte, Personnage &perso, liste_monstre &monstres, int id_ignorer) {
    if (x < 0 || x >= CONSOLE_Y_SIZE || y < 0 || y >= 80) return false;
    if (carte.map[x][y] == '#') return false;
    if (x == perso.pos.x && y == perso.pos.y) return false;

    // Vérification de collision avec d'autres monstres vivants
    for (int j = 0; j < monstres.nb_monstres; j++) {
        if (j != id_ignorer && monstres.list_monstre[j].actif) {
            if (monstres.list_monstre[j].pos.x == x && monstres.list_monstre[j].pos.y == y) {
                return false;
            }
        }
    }
    return true;
}

// Algorithme de Raycasting (Lancer de rayon) pour la vision
bool verifier_ligne_vue(int x1, int y1, int x2, int y2, Carte &carte) {
    int dx = valeur_absolue(x2 - x1);
    int dy = valeur_absolue(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x1 < 0 || x1 >= CONSOLE_Y_SIZE || y1 < 0 || y1 >= 80) return false;
        if (x1 == x2 and y1 == y2) return true;
        if (carte.map[x1][y1] == '#') return false; // Bloqué par un mur

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}


// --- FONCTIONS PUBLIQUES ---

void calculer_champ_vision(Personnage &perso, Carte &ma_carte) {
    int rayon = perso.stats_perso[0]; // TODO: Vérifier l'index correspondant à la Vision
    if (rayon < 1) rayon = 1;

    for (int i = -rayon; i <= rayon; i++) {
        for (int j = -rayon; j <= rayon; j++) {
            int cx = perso.pos.x + i;
            int cy = perso.pos.y + j;

            if (cx >= 0 && cx < CONSOLE_Y_SIZE && cy >= 0 && cy < 80) {
                // Formule du cercle pour une vision naturelle
                if ((i * i) + (j * j) <= (rayon * rayon)) {
                    if (verifier_ligne_vue(perso.pos.x, perso.pos.y, cx, cy, ma_carte)) {
                        ma_carte.decouverte[cx][cy] = true;
                    }
                }
            }
        }
    }
}

void tour_joueur(int touche, Carte &carte, Personnage &perso, Liste_equipement &items, liste_monstre &monstres) {
    perso.message_info = ""; // Réinitialisation du HUD
    int nx = perso.pos.x;
    int ny = perso.pos.y;

    if (touche == KEY_UP) nx--;
    else if (touche == KEY_DOWN) nx++;
    else if (touche == KEY_LEFT) ny--;
    else if (touche == KEY_RIGHT) ny++;

    char tuile = carte.map[nx][ny];

    if (tuile == '#') return; // Mur

    // Détection d'un monstre sur la case cible
    int id_cible = -1;
    for (int i = 0; i < monstres.nb_monstres; i++) {
        if (monstres.list_monstre[i].actif && 
            monstres.list_monstre[i].pos.x == nx && 
            monstres.list_monstre[i].pos.y == ny) {
            id_cible = i;
            break;
        }
    }

    if (id_cible != -1) {
        
    } else {
        // Ramassage d'objet
        if (tuile >= 'a' && tuile <= 'z') {
            for(int i=0; i<items.nb_equip; i++) {
                if(items.liste_equip[i].id == tuile) {
                    // Application des bonus
                    for(int k=0; k<NB_STATS; k++) perso.stats_perso[k] += items.liste_equip[i].stats_equip[k];
                    perso.message_info = "Trouve : " + items.liste_equip[i].nom;
                    carte.map[nx][ny] = '.'; // L'objet est consommé
                    break;
                }
            }
        }
        // Déplacement effectif
        perso.pos.x = nx;
        perso.pos.y = ny;
    }
}

void tour_monstres(liste_monstre &monstres, Carte &carte, Personnage &perso) {
    for (int i = 0; i < monstres.nb_monstres; i++) {
        Monstre &m = monstres.list_monstre[i];
        if (!m.actif) continue;

        int nx = m.pos.x;
        int ny = m.pos.y;
        
        int dist_x = perso.pos.x - m.pos.x;
        int dist_y = perso.pos.y - m.pos.y;
        int dist_totale = abs(dist_x) + abs(dist_y); // Utilisez abs() standard ou votre valeur_absolue()

        // --- IA TRAQUEUR ---
        if (m.ia == 1 && dist_totale <= m.portee) {
            // Rapprochement prioritaire
            if (abs(dist_x) > abs(dist_y)) nx += (dist_x > 0) ? 1 : -1;
            else ny += (dist_y > 0) ? 1 : -1;

            // Contournement si bloqué
            if (!est_position_valide(nx, ny, carte, perso, monstres, i)) {
                nx = m.pos.x; ny = m.pos.y; // Reset
                // Essai sur l'autre axe
                if (abs(dist_x) > abs(dist_y)) { 
                    if(dist_y != 0) ny += (dist_y > 0) ? 1 : -1; 
                } else { 
                    if(dist_x != 0) nx += (dist_x > 0) ? 1 : -1; 
                }
            }
        } 
        
        // --- IA ALEATOIRE (Si pas bougé ou IA passive) ---
        if (nx == m.pos.x && ny == m.pos.y) {
            int dir = rand() % 4; // Utilisez rand() ou votre generateur
            if(dir==0) nx--; else if(dir==1) nx++;
            else if(dir==2) ny--; else if(dir==3) ny++;
        }

        // --- RESOLUTION ACTION (C'est ICI que ça manquait !) ---
        
        // 1. Si le monstre veut aller sur le joueur -> ATTAQUE
        if (nx == perso.pos.x && ny == perso.pos.y) {
            // combat(perso, m); // Décommentez quand vous aurez le combat
            // Pour l'instant, on peut juste ne pas bouger
        }
        // 2. Sinon, si la case est valide -> DEPLACEMENT
        else if (est_position_valide(nx, ny, carte, perso, monstres, i)) {
            m.pos.x = nx;
            m.pos.y = ny;
        }
    }
}