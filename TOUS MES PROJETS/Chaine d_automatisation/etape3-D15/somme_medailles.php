#!/usr/bin/php
<?php
    # On indique le fichier sur lequel le script s'exécute
    $nomTableau = "tableau.csv";
    # On indique quel séparateur est utilisé dans le fichier
    $separateur = ",";
    # On lit le contenu du fichier et on stocke les lignes dans un tableau de chaînes de caractères
    $lignes = file($nomTableau);
    # On initialise le nouveau tableau qui va contenir les lignes modifiées
    $nouveauTableau = "";

    # On parcourt les lignes du tableau et pour chaque ligne :
    foreach ($lignes as $ligne){
        # On supprime les retours chariot et les sauts de ligne
        $ligne = rtrim($ligne);
        # On divise cahque ligne en un tableau contenant les différentes cellules de la ligne
        $champs = explode($separateur, $ligne);

        # On récupère le nombre de médailles pour chaque métal
        $or = $champs[1]; 
        $argent = $champs[2];
        $bronze = $champs[3];

        # On fait la somme de toutes les medailles en ayant préalablement modifié le type
        $somme = (int)$or + (int)$argent + (int)$bronze; 
        # On asemble la nouvelle ligne du tableau
        $nouvelle_ligne = $ligne.$separateur.$somme."\n";
        # On ajoute la nouvelle ligne dans le nouveau tableau
        $nouveauTableau = $nouveauTableau.$nouvelle_ligne;
    }
    # Enfin, on remplace l'ancien tableau par le nouveau tableau 
    file_put_contents($nomTableau, $nouveauTableau);

?>