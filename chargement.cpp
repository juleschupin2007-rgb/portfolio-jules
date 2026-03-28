#include "chargement.hpp"
#include "wrapper_ncurses.hpp"

Carte charger_carte(string nom_fichier) {
    Carte ma_carte;
    fstream flux;
    flux.open(nom_fichier, ios::in);
    string ligne;
    int y = 0;

    if (flux.is_open()) {
        flux >> ligne; 
        while (flux.good() and y < CONSOLE_Y_SIZE) {
            ma_carte.map[y] = ligne;
            flux >> ligne;
            y++;
        }
        flux.close();

        // On cache toute la carte au début (Brouillard)
        for (int i = 0; i < CONSOLE_Y_SIZE; i++) {
            for (int j = 0; j < 80; j++) {
                ma_carte.decouverte[i][j] = false;
            }
        }
    } else {
        ecrire_string("Erreur ouverture : " + nom_fichier, 0, 0);
    }
    return ma_carte;
}

Personnage charger_personnage(string nom_fichier) {
    Personnage p;
    fstream flux;
    flux.open(nom_fichier, ios::in);
    int y = 0;
    if (flux.is_open()) {
        flux >> p.pos.x >> p.pos.y >> p.nom;
        while (y < 6) {
            flux >> p.stats_perso[y];
            y++;
        }
        flux >> p.equipement.max_objets >> p.equipement.nb_equip >> p.equipement.liste_id[0];
        flux.close();
        p.message_info = "";
    }
    return p;
}

Liste_equipement charger_equipement(string nom_fichier) {
    Liste_equipement ma_liste;
    ma_liste.nb_equip = 0; 

    ifstream flux(nom_fichier);
    if (flux.is_open()) {
        char id;
        string nom;
        int s0, s1, s2, s3, s4, s5; 

        // LE SECRET EST ICI : ON TESTE LA LECTURE DANS LE WHILE
        while (ma_liste.nb_equip < MAX_ITEMS && 
               (flux >> id >> nom >> s0 >> s1 >> s2 >> s3 >> s4 >> s5)) { 
            
            Equipement& e = ma_liste.liste_equip[ma_liste.nb_equip];
            e.id = id;
            e.nom = nom;
            e.stats_equip[0] = s0; e.stats_equip[1] = s1;
            e.stats_equip[2] = s2; e.stats_equip[3] = s3;
            e.stats_equip[4] = s4; e.stats_equip[5] = s5;
            
            ma_liste.nb_equip++;
        }
        flux.close();
    }
    return ma_liste;
}

liste_monstre charger_liste_monstre(string nom_fichier) {
    liste_monstre L;
    L.nb_monstres = 0;
    ifstream flux(nom_fichier);
    if (!flux.is_open()) return L;

    flux >> L.nb_monstres;
    // Sécurité bornes
    if (L.nb_monstres > MAX_MONSTRES) L.nb_monstres = MAX_MONSTRES;

    for (int i = 0; i < L.nb_monstres; i++) {
        Monstre &m = L.list_monstre[i];

        // Lecture de base
        if(!(flux >> m.id_char >> m.pos.x >> m.pos.y >> m.nom)) break; // Stop si fichier fini

        for (size_t j = 0; j < NB_STATS; j++) flux >> m.stats_monstre[j];

        // Lecture Equipement
        flux >> m.equipement.nb_equip >> m.equipement.max_objets;
        // Sécurité boucle
        if(m.equipement.nb_equip > MAX_ITEMS) m.equipement.nb_equip = MAX_ITEMS;
        for (int j = 0; j < m.equipement.nb_equip; j++) flux >> m.equipement.liste_id[j];

        flux >> m.actif >> m.ia;

        // ... Le reste (Contraintes) ...
        // AJOUTEZ LA MÊME SECURITE pour chaque boucle "nb_equip" des contraintes
        // Si nb_equip > 20 -> Forcez à 20 sinon crash.
        
        flux >> m.portee;
    }
    return L;
}

void initialiser_monstres_depuis_carte(Carte &carte, liste_monstre &catalogue, liste_monstre &monstres_actifs) {
    monstres_actifs.nb_monstres = 0;

    for (int x = 0; x < CONSOLE_Y_SIZE; x++) {
        for (size_t y = 0; y < carte.map[x].size(); y++) {
            char c = carte.map[x][y];
            if (c >= 'A' && c <= 'Z') {
                // On cherche le modèle dans le catalogue
                int index = -1;
                for (int k = 0; k < catalogue.nb_monstres; k++) {
                    if (catalogue.list_monstre[k].id_char == c) {
                        index = k;
                        break;
                    }
                }
                // Si trouvé, on l'ajoute à la liste active
                if (index != -1 && monstres_actifs.nb_monstres < MAX_MONSTRES) {
                    int id = monstres_actifs.nb_monstres;
                    monstres_actifs.list_monstre[id] = catalogue.list_monstre[index];
                    monstres_actifs.list_monstre[id].pos.x = x;
                    monstres_actifs.list_monstre[id].pos.y = (int)y;
                    monstres_actifs.list_monstre[id].actif = true;
                    monstres_actifs.nb_monstres++;

                    // On efface la lettre de la carte pour éviter les doublons fantômes
                    carte.map[x][y] = '.';
                }
            }
        }
    }
}