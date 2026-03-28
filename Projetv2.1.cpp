#include <iostream>
#include <string>
#include "lib_projet_ncurses.hpp"
#include <fstream>
using namespace std;

// --- CONSTANTES ---
const size_t MAX_CONTRAINTES = 15;
const size_t MAX_MONSTRES = 15;
const size_t MAX_ITEMS=20;
const size_t nb_stats=6;
using Stats = int[nb_stats];

PaireCouleur couleurs[] = {
    {COLOR_WHITE,COLOR_BLACK},
    {COLOR_BLUE,COLOR_BLACK},
    {COLOR_RED, COLOR_BLACK},
    {COLOR_YELLOW, COLOR_BLACK},
    {COLOR_MAGENTA, COLOR_BLACK},
    {COLOR_WHITE,COLOR_BLACK}
};

const int C_DEFAUT = 1;
const int C_MUR = 6;
const int C_JOUEUR = 2;
const int C_MONSTRE = 3;
const int C_EQUIPEMENT = 4;
const int C_PORTE= 5;

// --- STRUCTURES ---

struct Position {
    int y;
    int x;
};

struct Equipement{
    char id;
    string nom;
    Stats stats_equip;
};

struct Liste_id_equip{
    int max_objets=MAX_ITEMS;
    int nb_equip;
    int liste_id[MAX_ITEMS];
};

struct Personnage{
    Position pos;
    string nom;
    Stats stats_perso;
    Liste_id_equip equipement;
    string message_info;
};

struct Porte {
    Position pos;
    Stats conditions;
    Liste_id_equip oblig;
    Liste_id_equip interdit;
    int id;
};

struct Contrainte{
    Position pos;
    string nom;
    Stats conditions;
    int mode ;  
    Liste_id_equip oblig;
    Liste_id_equip interdit;
};

struct Monstre{
    char id_char;
    Position pos;
    string nom;
    Stats stats_monstre;
    Liste_id_equip equipement;
    bool actif;
    int ia;
    Contrainte succes;
    Contrainte defaite;
    int portee;
};

struct Carte{
  string   map[CONSOLE_Y_SIZE];
  bool decouverte[CONSOLE_Y_SIZE][80];
};

struct Liste_equipement{
    int max_equipement= MAX_ITEMS;
    int nb_equip;
     Equipement liste_equip[MAX_ITEMS];  
};

struct Liste_contrainte{
    int max_contrainte = MAX_CONTRAINTES;
    int nb_contraintes;
    Contrainte liste_contr[MAX_CONTRAINTES];
};

struct liste_monstre{
    int max_monstres = MAX_MONSTRES;
    int nb_monstres;
     Monstre list_monstre[MAX_MONSTRES];
};

// --- OUTIL ALEATOIRE MAISON (Remplacement de rand() sans librairie) ---
int mon_rand() {
    static unsigned long int next = 1;
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

// --- FONCTIONS DE CHARGEMENT ---

Carte charger_carte (string nom_fichier_carte){
    Carte ma_carte;
    fstream flux;
    flux.open(nom_fichier_carte,ios::in);
    string ligne;
    int y = 0;
    if (flux.is_open()){
        flux >> ligne;
        while (flux.good() and y<CONSOLE_Y_SIZE){  
            ma_carte.map[y] = ligne;
            flux >> ligne;
            y+=1;
        }
        flux.close();
        for(int i = 0; i < CONSOLE_Y_SIZE; i++) {
            for(int j = 0; j < 80; j++) {
                ma_carte.decouverte[i][j] = false;
            }
        }
        return ma_carte;
    }
    else{
        ecrire_string("Impossible d'ouvrir le fichier "+ nom_fichier_carte,0,0);
        return ma_carte;
    }
}

Personnage charger_personnage (string nom_fichier_personnage){
    Personnage mon_personnage;
    fstream flux;
    flux.open(nom_fichier_personnage, ios::in);
    int y=0;
    if (flux.is_open()){
        flux >> mon_personnage.pos.x;
        flux >> mon_personnage.pos.y;
        flux >> mon_personnage.nom;
        while (y<6){
            flux >> mon_personnage.stats_perso[y];
            y=y+1;
        }
        flux >> mon_personnage.equipement.max_objets;
        flux >> mon_personnage.equipement.nb_equip;
        flux >> mon_personnage.equipement.liste_id[0];
        flux.close();
        mon_personnage.message_info = "";
        return mon_personnage;
    }
    else{
        ecrire_string("Impossible d'ouvrir le fichier "+ nom_fichier_personnage,0,0);
        return mon_personnage;
    }
}

Liste_equipement charger_equipement(string nom_fichier_equipement){
    Liste_equipement mes_equipements;
    mes_equipements.nb_equip = 0;
    fstream flux;
    flux.open(nom_fichier_equipement, ios::in);
    if (!flux.is_open()){
        ecrire_string("Impossible d'ouvrir le fichier "+ nom_fichier_equipement,0,0);
        return mes_equipements;
    }
    char id_char;
    string nom;
    Equipement eq;
    while (flux >> id_char >> nom) {
        eq.id=id_char;
        eq.nom = nom;
        for (size_t i = 0; i < nb_stats; i++){
            flux >> eq.stats_equip[i];
        }
        if (mes_equipements.nb_equip < mes_equipements.max_equipement){
            mes_equipements.liste_equip[mes_equipements.nb_equip] = eq;
            mes_equipements.nb_equip++;
        }
    }
    flux.close();
    return mes_equipements;
}

liste_monstre charger_liste_monstre(string nom_fichier) {
    liste_monstre L;
    L.nb_monstres = 0;
    ifstream flux(nom_fichier);
    if (!flux.is_open()) return L;

    flux >> L.nb_monstres;
    if (L.nb_monstres > L.max_monstres) L.nb_monstres = L.max_monstres;

    for (int i = 0; i < L.nb_monstres; i++) {
        Monstre &m = L.list_monstre[i];
    
        flux >> m.id_char;              
        flux >> m.pos.x >> m.pos.y;    
        flux >> m.nom;                  
        
        for (size_t j= 0; j < nb_stats; j++) flux >> m.stats_monstre[j];
        
        flux >> m.equipement.nb_equip >> m.equipement.max_objets;
        for (int j = 0; j < m.equipement.nb_equip; j++) flux >> m.equipement.liste_id[j];
            
        flux >> m.actif >> m.ia;
        
        flux >> m.succes.nom >> m.succes.pos.x >> m.succes.pos.y;
        for (size_t j = 0; j < nb_stats; j++) flux >> m.succes.conditions[j];
        flux >> m.succes.mode >> m.succes.oblig.nb_equip;
        for (int j = 0; j < m.succes.oblig.nb_equip; j++) flux >> m.succes.oblig.liste_id[j];
        flux >> m.succes.interdit.nb_equip;
        for (int j = 0; j < m.succes.interdit.nb_equip; j++) flux >> m.succes.interdit.liste_id[j];
        flux >> m.defaite.nom >> m.defaite.pos.x >> m.defaite.pos.y;
        for (size_t j = 0; j < nb_stats; j++) flux >> m.defaite.conditions[j];
        flux >> m.defaite.mode >> m.defaite.oblig.nb_equip;
        for (int j = 0; j < m.defaite.oblig.nb_equip; j++) flux >> m.defaite.oblig.liste_id[j];
        flux >> m.defaite.interdit.nb_equip;
        for (int j = 0; j < m.defaite.interdit.nb_equip; j++) flux >> m.defaite.interdit.liste_id[j];
        flux >> m.portee;
    }
    return L;
}

Liste_contrainte charger_liste_contrainte(string nom_fichier) {
    Liste_contrainte L;
    ifstream flux(nom_fichier);
    if (!flux.is_open()) return L;
    flux >> L.nb_contraintes;
    if (L.nb_contraintes > L.max_contrainte) L.nb_contraintes = L.max_contrainte;
    for (int i = 0; i < L.nb_contraintes; i++) {
        Contrainte &c = L.liste_contr[i];
        flux >> c.nom;
        flux >> c.pos.x >> c.pos.y;
        for (size_t j = 0; j < nb_stats; j++) flux >> c.conditions[j];
        flux >> c.mode;
        flux >> c.oblig.nb_equip;
        for (int j = 0; j < c.oblig.nb_equip; j++)
            flux >> c.oblig.liste_id[j];
        flux >> c.interdit.nb_equip;
        for (int j = 0; j < c.interdit.nb_equip; j++)
            flux >> c.interdit.liste_id[j];
    }
    return L;
}

// --- AFFICHAGE ET VISION ---

void afficher_carte(Carte main_carte){
    for (int x= 0; x<CONSOLE_Y_SIZE; x++){
        for (size_t k=0; k<size(main_carte.map[x]); k++){
            
            if (main_carte.decouverte[x][k] == false) {
                changer_couleur(C_DEFAUT);
                ecrire_char(k, x, ' ');
            }
            else {
                char symbole = main_carte.map[x][k];

                if (symbole == '@'){
                    changer_couleur(C_JOUEUR | A_BOLD);
                    ecrire_char(k, x, symbole);
                }
                else if (symbole >= 'a' and symbole <= 'z'){
                    changer_couleur(C_EQUIPEMENT | A_BOLD);
                    ecrire_char(k, x, symbole);
                }
                else if (symbole >= 'A' and symbole <= 'Z'){
                    changer_couleur(C_MONSTRE | A_BOLD);
                    ecrire_char(k, x, symbole);
                }
                else if (symbole >= '0' and symbole <= '9'){
                    changer_couleur(C_PORTE | A_BOLD);
                    ecrire_char(k, x, symbole);
                }
                else if (symbole == '#'){
                    changer_couleur(C_MUR | A_BOLD);
                    ecrire_char(k, x, symbole);
                }
                else{
                    changer_couleur(C_DEFAUT);
                    ecrire_char(k, x, symbole);
                }
            }
        }
    }
}

bool ligne_de_vue_libre(int x1, int y1, int x2, int y2, Carte &ma_carte) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; 
    int sy = (y1 < y2) ? 1 : -1; 
    int err = dx - dy;

    while (true) {
        if (x1 < 0 || x1 >= CONSOLE_Y_SIZE || y1 < 0 || y1 >= 80) {
            return false;
        }
        if (x1 == x2 and y1 == y2) return true;

        if (ma_carte.map[x1][y1] == '#') {
            return false;
        }

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void mettre_a_jour_vision(Personnage &perso, Carte &ma_carte) {
    int rayon = perso.stats_perso[0];
    if (rayon < 1) rayon = 1;

    int px = perso.pos.x;
    int py = perso.pos.y;

    for (int i = -rayon; i <= rayon; i++) {
        for (int j = -rayon; j <= rayon; j++) {
            
            int cible_x = px + i;
            int cible_y = py + j;

            if (cible_x >= 0 and cible_x < CONSOLE_Y_SIZE and cible_y >= 0 and cible_y < 80) {
                bool dans_le_cercle = ((i * i) + (j * j) <= (rayon * rayon));

                if (dans_le_cercle) {
                    if (ligne_de_vue_libre(px, py, cible_x, cible_y, ma_carte)) {
                        ma_carte.decouverte[cible_x][cible_y] = true;
                    }
                }
            }
        }
    }
}

// --- LOGIQUE JEU ---

bool gerer_equipement(char case_cible, int x, int y, Carte &main_carte, Personnage &perso, const Liste_equipement &equipements) {
    int i= 0;
    while (i < equipements.nb_equip ) {
        if (equipements.liste_equip[i].id == case_cible) {
            for (int k = 0; k < 6; k++) {
                perso.stats_perso[k] += equipements.liste_equip[i].stats_equip[k];
            }
             int pos = perso.equipement.nb_equip;
             perso.equipement.liste_id[pos] = (int)case_cible;
             perso.equipement.nb_equip=perso.equipement.nb_equip+1;
             string noms_stats[6] = {"Vision ", "Discretion ", "Butin ", "Force ", "Peur ", "Crochetage "};
             string bonus_msg = "";
             for(int k=0; k<6; k++) {
                int val = equipements.liste_equip[i].stats_equip[k];
                if (val != 0) {
                    bonus_msg += " " + noms_stats[k] + (val > 0 ? "+" : "") + to_string(val);
                }
            }
            perso.message_info = perso.nom+ " Obtient: " + equipements.liste_equip[i].nom + " (" + bonus_msg + " )";
             main_carte.map[x][y] = '.';
            return true;
        }
    i++;
    }
    return false;
}

void deplacement(int key, Carte &main_carte, Personnage &perso, Liste_equipement liste_items){
    perso.message_info = "";
    int next_x = perso.pos.x;
    int next_y = perso.pos.y;
    
    // 1. Calcul de la destination théorique
    if (key == KEY_UP)    next_x = perso.pos.x - 1;
    if (key == KEY_DOWN)  next_x = perso.pos.x + 1;
    if (key == KEY_LEFT)  next_y = perso.pos.y - 1;
    if (key == KEY_RIGHT) next_y = perso.pos.y + 1;

    char case_cible = main_carte.map[next_x][next_y];

    // 2. Vérification collision MUR
    if (case_cible == '#') {
        // C'est un mur : ON NE FAIT RIEN.
        // On remet next_x/y à la position actuelle pour ne pas bouger
        next_x = perso.pos.x;
        next_y = perso.pos.y;
    }
    else {
        // C'est libre (ou un objet), on peut avancer
        
        // Gestion Objet
        if (case_cible >= 'a' and case_cible <= 'z') {
            gerer_equipement(case_cible, next_x, next_y, main_carte, perso, liste_items);
        }

        // Mise à jour graphique (effacer ancienne position)
        changer_couleur(C_DEFAUT);
        ecrire_char(perso.pos.y, perso.pos.x, '.'); // On efface la trace visuelle

        // Mise à jour données
        perso.pos.x = next_x;
        perso.pos.y = next_y;
    }

    // 3. Mise à jour finale (Vision + Affichage)
    mettre_a_jour_vision(perso, main_carte);
    afficher_carte(main_carte);
    changer_couleur(C_JOUEUR);
    ecrire_char(perso.pos.y, perso.pos.x, '@');
}

bool est_case_libre(int x, int y, Carte &main_carte, Personnage &perso, liste_monstre &tous_les_monstres, int index_monstre_actuel) {
    // 1. Limites de la carte
    if (x < 0 || x >= CONSOLE_Y_SIZE || y < 0 || y >= 80) return false;

    // 2. Mur
    if (main_carte.map[x][y] == '#') return false;

    // 3. Joueur
    if (x == perso.pos.x && y == perso.pos.y) return false;

    // 4. Autres monstres (Collision entre eux)
    for (int j = 0; j < tous_les_monstres.nb_monstres; j++) {
        if (j != index_monstre_actuel && tous_les_monstres.list_monstre[j].actif) {
            if (tous_les_monstres.list_monstre[j].pos.x == x && 
                tous_les_monstres.list_monstre[j].pos.y == y) {
                return false;
            }
        }
    }
    
    return true; // C'est tout bon, on peut y aller
}

// FONCTION DEPLACEMENT MONSTRES (AVEC RANDOM MAISON)
void deplacement_monstres(liste_monstre &monstres, Carte &main_carte, Personnage &perso) {
    
    for(int i = 0; i < monstres.nb_monstres; i++) {
        Monstre &m = monstres.list_monstre[i]; // Raccourci pour lire plus facilement
        if(!m.actif) continue;

        int mx = m.pos.x;
        int my = m.pos.y;
        int next_mx = mx;
        int next_my = my;
        bool a_bouge = false;

        // --- INTELLIGENCE ARTIFICIELLE ---

        // Calcul de la distance avec le joueur (Distance de Manhattan)
        int dist_x = perso.pos.x - mx;
        int dist_y = perso.pos.y - my;
        int distance_totale = abs(dist_x) + abs(dist_y);

        // CAS IA = 1 : LE TRAQUEUR
        if (m.ia == 1 && distance_totale <= m.portee) {
            // Le monstre essaie de réduire l'écart le plus grand (X ou Y)
            
            // Essai 1 : Approche prioritaire
            if (abs(dist_x) > abs(dist_y)) {
                // On bouge en X
                next_mx += (dist_x > 0) ? 1 : -1;
            } else {
                // On bouge en Y
                next_my += (dist_y > 0) ? 1 : -1;
            }

            // Vérification : Si le chemin idéal est bloqué, on essaie l'autre axe (contournement simple)
            if (!est_case_libre(next_mx, next_my, main_carte, perso, monstres, i)) {
                // Reset
                next_mx = mx; 
                next_my = my;
                
                // On tente l'axe secondaire
                if (abs(dist_x) > abs(dist_y)) {
                     // Si on avait tenté X, on tente Y (si utile)
                     if (dist_y != 0) next_my += (dist_y > 0) ? 1 : -1;
                } else {
                     // Si on avait tenté Y, on tente X
                     if (dist_x != 0) next_mx += (dist_x > 0) ? 1 : -1;
                }
            }
        }
        
        // CAS PAR DEFAUT (IA = 0) OU SI LE TRAQUEUR EST LOIN : MOUVEMENT ALEATOIRE
        // Si next_mx/my n'ont pas changé (pas d'IA ou IA bloquée/trop loin), on fait du random
        if (next_mx == mx && next_my == my) {
            int direction = mon_rand() % 4; 
            if(direction == 0) next_mx--;
            else if(direction == 1) next_mx++;
            else if(direction == 2) next_my--;
            else if(direction == 3) next_my++;
        }

        // --- APPLICATION DU MOUVEMENT ---
        if (est_case_libre(next_mx, next_my, main_carte, perso, monstres, i)) {
            m.pos.x = next_mx;
            m.pos.y = next_my;
        }
    }

    // --- AFFICHAGE (Identique à avant) ---
    afficher_carte(main_carte); 

    for(int i = 0; i < monstres.nb_monstres; i++) {
        if(monstres.list_monstre[i].actif) {
            if (main_carte.decouverte[monstres.list_monstre[i].pos.x][monstres.list_monstre[i].pos.y]) {
                changer_couleur(C_MONSTRE | A_BOLD);
                ecrire_char(monstres.list_monstre[i].pos.y, monstres.list_monstre[i].pos.x, monstres.list_monstre[i].id_char);
            }
        }
    }
    changer_couleur(C_JOUEUR);
    ecrire_char(perso.pos.y, perso.pos.x, '@');
}

void generer_monstres_depuis_carte(Carte &carte, liste_monstre &bestiaire, liste_monstre &monstres_actifs) {
    monstres_actifs.nb_monstres = 0; // On part d'une liste vide

    // On parcourt toute la carte
    for(int x = 0; x < CONSOLE_Y_SIZE; x++) { // x est la ligne (vertical)
        for(size_t y = 0; y < carte.map[x].size(); y++) { // y est la colonne (horizontal)
            
            char c = carte.map[x][y];

            // Si c'est une lettre majuscule (potentiellement un monstre)
            if (c >= 'A' && c <= 'Z') {
                
                // 1. On cherche ce caractère dans le bestiaire (monstres.txt)
                size_t index_modele = -1;
                for(int k = 0; k < bestiaire.nb_monstres; k++) {
                    if (bestiaire.list_monstre[k].id_char == c) {
                        index_modele = k;
                        break;
                    }
                }

                // 2. Si on a trouvé le modèle et qu'il reste de la place
                if (index_modele != -1 && monstres_actifs.nb_monstres < MAX_MONSTRES) {
                    // On copie le modèle dans la liste des monstres actifs
                    int id_nouveau = monstres_actifs.nb_monstres;
                    monstres_actifs.list_monstre[id_nouveau] = bestiaire.list_monstre[index_modele];
                    
                    // IMPORTANT : On donne la position trouvée sur la carte
                    monstres_actifs.list_monstre[id_nouveau].pos.x = x; 
                    monstres_actifs.list_monstre[id_nouveau].pos.y = (int)y;
                    
                    // On active le monstre
                    monstres_actifs.list_monstre[id_nouveau].actif = true;
                    
                    monstres_actifs.nb_monstres++; // On a un monstre de plus !
                    
                    // 3. On efface la lettre de la carte pour qu'elle devienne du sol
                    carte.map[x][y] = '.'; 
                }
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

int main(){
    initialiser_console( couleurs, 6);
    Carte test = charger_carte("level.txt");
    Personnage perso = charger_personnage("personnage.txt");
    Liste_equipement mes_equip = charger_equipement("equipement.txt");
    
    // 1. On charge le CATALOGUE (les définitions des monstres)
    liste_monstre bestiaire = charger_liste_monstre("monstres.txt");
    
    // 2. On prépare la liste des monstres VIVANTS (vide pour l'instant)
    liste_monstre mes_monstres; 
    
    // 3. MAGIE : On scanne la carte pour créer les monstres
    generer_monstres_depuis_carte(test, bestiaire, mes_monstres);
    
    mettre_a_jour_vision(perso, test);
    afficher_carte(test);
    
    // Affichage initial
    for(int i=0; i<mes_monstres.nb_monstres; i++) {
        // ... (ton code d'affichage initial reste identique, il utilisera mes_monstres rempli auto)
         if(mes_monstres.list_monstre[i].actif && test.decouverte[mes_monstres.list_monstre[i].pos.x][mes_monstres.list_monstre[i].pos.y]) {
             changer_couleur(C_MONSTRE | A_BOLD);
             ecrire_char(mes_monstres.list_monstre[i].pos.y, mes_monstres.list_monstre[i].pos.x, mes_monstres.list_monstre[i].id_char);
        }
    }

    test.map[perso.pos.x][perso.pos.y] = '.';
    changer_couleur(C_JOUEUR);
    ecrire_char(perso.pos.y, perso.pos.x, '@');
    
    bool jeu_en_cours = true;
    int touche;
    while(jeu_en_cours){
        afficher_hud(perso);
        touche = saisie_bloquante();
        if (touche=='a'){
            jeu_en_cours=false;
            fermer_console();
        }
        else{
            deplacement(touche,test,perso,mes_equip);
            deplacement_monstres(mes_monstres, test, perso);
        }
    }
}