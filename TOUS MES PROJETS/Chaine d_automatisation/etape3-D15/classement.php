#!/usr/bin/php
<?php
    # On indique le fichier sur lequel le script s'exécute
    $nomFichier = "tableau.csv"; 
    # On lit le contenu du fichier et on stocke les lignes dans un tableau de chaînes de caractères
    $lignes = file($nomFichier);
    # On indique quel séparateur est utilisé dans le fichier
    $separateur = ",";
    # On initialise le nouveau tableau qui va contenir les lignes modifiées
    $nouveauTableau = "";
    # Initialisation de la variable $cle qui va nous permettre de savoir la position de la ligne dans le tableau
    $cle = 0;
    # Initialisation du classement qui va nous permettre de savoir le numéro de la ligne dans le classement
    $classement = 1;

    # On parcours le tableau "lignes" et on récupère chaque ligne du fichier
    foreach ($lignes as $ligne){
        # On supprime les retours chariot et les sauts de ligne
        $ligne = rtrim($ligne); 
        # On initialise la place du pays à la valeur de la variable classement
        $place = $classement; 

        # Si la ligne possède une ligne qui la précède
        if ($cle > 1) {
            # On stock l'ancienne ligne pour ensuite la comparer
            $oldLigneTab = explode($separateur, $lignes[$cle - 1]); 
            $ligneTab = explode($separateur, $ligne);

            # On test si la ligne précécente a le même nombre de médailles que la ligne actuelle si c'est le cas alors le classement = "-"
            if ($oldLigneTab[1] == $ligneTab[1] && $oldLigneTab[2] == $ligneTab[2] && $oldLigneTab[3] == $ligneTab[3] && $oldLigneTab[4] == $ligneTab[4]){
                $place = "-";
            }
        }

        # On construit la nouvelle ligne avec le numéro de la place et la ligne actuelle
        $nouvelleLigne = $place . $separateur . $ligne . "\n"; 
        # On ajoute la nouvelle ligne au nouveau tableau
        $nouveauTableau = $nouveauTableau.$nouvelleLigne;

        # On augmente la variable $cle pour passer à la ligne suivante
        $cle += 1;
        # On augmente la variable $classement pour passer au numéro suivant dans le classement
        $classement += 1;
    }
    # Remplace l'ancien tableau par le nouveau tableau 
    file_put_contents($nomFichier, $nouveauTableau);

?>