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
    # On initialise le total des médailles à 0
    $totalMedailles = 0;

    # On parcours le tableau et on récupère chaque ligne du fichier
    foreach ($lignes as $ligne){ 
        # On divise cahque ligne en un tableau contenant les différentes cellules de la ligne
        $champs = explode($separateur, $ligne);

        # On prend la somme de toutes les medailles d'un pays et on l'ajoute au total
        $somme = $champs[5]; 
        $totalMedailles += $somme;
    }

    # On parcours le tableau "lignes" et on récupère chaque ligne du fichier
    foreach ($lignes as $ligne){ 
        # Supprime les retours chariot et les sauts de ligne
        $ligne = rtrim($ligne); 
        # On divise cahque ligne en un tableau contenant les différentes cellules de la ligne
        $champs = explode($separateur, $ligne);

        # On récupère le nombre total de médailles pour un pays
        $medailles = $champs[5];

        # On calcule le rapport entre le nombre de médailles du pays et le nombre total de médailles
        $partMedailles = (($medailles / $totalMedailles) * 100);
        # On arrondi ce rapport pour limiter la longueur de la valeur
        $partMedailles = round($partMedailles, 1);

        # On met à jour la ligne avec la valeur du pourcentage de médailles pour ce pays
        $nouvelleLigne = $ligne.$separateur.$partMedailles."%\n";
        # On ajoute chaque nouvelle ligne dans un nouveau tableau
        $nouveauTableau = $nouveauTableau.$nouvelleLigne;
    }
    # On remplace l'ancien tableau par le nouveau tableau 
    file_put_contents($nomTableau, $nouveauTableau); 
?>