import json

# Ouvrir le fichier JSON et initialiser le programme. A la fin du programme il se referme automatiquement
with open("BD_decheteries-ecopoints-nantes-metropole-tonnages.json", "r", encoding="utf-8") as fichier_d:
    data = json.load(fichier_d)

def choix_lieu():
    # Extraire le nom de toute les déchèteries qu'une seule fois
    noms_decheteries = []
    for objet in data:
        nom = objet["decheterie_ecopoint"]
        if nom not in noms_decheteries:
            noms_decheteries.append(nom)
    # Afficher les noms
    for i in range(len(noms_decheteries)):
        print(i," - ",noms_decheteries[i])
    # Le choix d'un lieu par l'utilisateur   
    choix_decheterie = int(input("Entrez le numéro de la déchèterie ou de l'écopoint dont vous voulez plus d'informations : "))
    verif = False
    while verif != True :
        if choix_decheterie >= len(noms_decheteries)-1 or choix_decheterie < 0:
            choix_decheterie = int(input("Entrez un autre numéro de déchèterie ou de l'écopoint dont vous voulez plus d'informations car le nombre précédent est soit trop grand ou trop petit : "))
        else :
            verif = True
    print("Vous avez choisi la/l'",noms_decheteries[choix_decheterie])
    return(noms_decheteries[choix_decheterie])

def choix_dechets():
    # Présence des même déchets dans toute les déchèteries de la base de donnée même avec une valeur nul
    dechets = ["bois","carton","papier","dechets_dangereux","deee","amiante","batteries","cartouche_encre","extincteurs","huile","pneumatiques","dechets_verts","ferraille","gravats","mobilier","reemploi","textiles","tout_venant","verre"]
    # Afficher les dechets présent dans les déchèteries
    for i in range(len(dechets)):
        print(i," - ",dechets[i])
    # Le choix d'un dechet par l'utilisateur   
    choix_dechet = int(input("Entrez le numéro du dechet dont vous voulez plus d'informations : "))
    verif = False
    while verif != True :
        if choix_dechet >= len(dechets)-1 or choix_dechet < 0:
            choix_dechet = int(input("Entrez un autre numéro de dechet dont vous voulez plus d'informations car le nombre précédent est soit trop grand ou trop petit : "))
        else :
            verif = True
    print("Vous avez choisi le/l'/les",dechets[choix_dechet])
    return(dechets[choix_dechet])

def ordre():
    # Initialisation des valeurs
    lieu = str(choix_lieu())
    dechet = str(choix_dechets())
    annee = []
    tonnage = []
    # Prise des données dans la base en fonction des choix de l'utilisateur
    for objet in data:
        info = objet["decheterie_ecopoint"]
        if info == lieu:
            annee.append(int(objet["annee"]))
            tonnage.append(float(objet[dechet]))
    # Tri les années dans l'ordre croissant et qui garde le bon tonnage pour chaque année
    annee_ord = []
    tonnage_ord = []
    # Recherche de l'indice du plus petit nombre
    while annee != []:
        mini = min(annee) # On cherche l'indice de la plus petite date 
        mini_i = 0
        for i in range(len(annee)):
            if annee[i] == mini :
                mini_i = i
        # On supprime les valeurs de leur liste pour les remettre dans le bon ordre dans une autre 
        elt_sup = annee.pop(mini_i) 
        annee_ord.append(elt_sup)
        elt_sup = tonnage.pop(mini_i)
        tonnage_ord.append(elt_sup)
    # On retourne les listes triées
    return annee_ord, tonnage_ord

# Pour faire fonctionner le prog tu utile la fonction : ordre()
# Plus qu'a faire le graphique et le diapo

ordre()


      
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
