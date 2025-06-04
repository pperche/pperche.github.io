#!/usr/bin/php
<?php
    # On lit leichier txt qui contient tous les codes des pays et on stocke chaque ligne dans un tableau
    $lines = file("codes_pays.txt"); 

    # On récupère la base du lien
    $baseLien="https://flagcdn.com/w20/"; 
    # On récupère l'extension du drapeau
    $extension=".webp"; 
    # On indique le dossier cible où on va stocker les drapeaux
    $dossierCible="dossier_drapeaux";

    # On créer le dossier cible
    mkdir($dossierCible);

    # On parcourt toutes les lignes du fichier code_pays
    foreach($lines as $codePays){ 
        # On récupère le code pays
        $codePays = rtrim($codePays);
        # On concatène le lien complet de téléchargement du drapeau
        $lien = $baseLien . $codePays . $extension;
        # On concatène la commande de téléchargement et on utilise -P pour mettre les drapeaux dans le dossier cible
        $commande = "wget " . $lien . " -P " . $dossierCible . "\n"; 
        # On exécute la commande
        exec($commande); 
    }
?>