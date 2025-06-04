#!/usr/bin/php
<?php
    # On indique le fichier sur lequel le script s'exécute
    $nomFichier = "tableau.csv";
    # On indique quel séparateur est utilisé dans le fichier
    $separateur = ",";
    # On lit le contenu du fichier et on stocke les lignes dans un tableau de chaînes de caractères
    $lignes = file($nomFichier);
    # On lit le contenu du fichier permettant de faire correspondre les pays avec leur code ISO et on stocke les lignes dans un tableau de chaînes de caractères
    $correspondance = file("nomp_codep.csv");

    # On génère les lignes de base d'un fichier html
    echo "<!DOCTYPE html>\n";
    echo "<html lang=\"fr\">\n";
    echo "<head>\n";
    echo "\t<meta charset=\"UTF-8\">\n";
    echo "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    echo "\t<title>Classement JO Paris 2024</title>\n";
    echo "\t<link rel=\"stylesheet\" href=\"style.css\">";
    echo "</head>\n";
    echo "<body>\n";

    # On ajoute le logo des JO à la page
    echo "\t<img src=\"logo_JO_Paris_2024.png\" alt=\"Image Logo JO Paris 2024\" title=\"Logo Jo 2024\">\n";

    # On créé le tableau du classement, avec son titre
    echo "\t<table>\n";
    echo "\t\t<caption>Classement des pays aux JO de Paris 2024</caption>\n";
    echo "\t\t<tbody>\n";
    
    # On ajoute les lignes du tableau que l'on récupère dans le fichier csv, chaque ligne corresondant à un pays
    foreach ($lignes as $ligne) {
        # On initialise la variable qui va stocker le code ISO du pays
        $ISOcode = "";
        # On divise cahque ligne en un tableau contenant les différentes cellules de la ligne
        $ligne = explode($separateur, rtrim($ligne));
        # On calcule la longueur de la ligne (nombre de cellules)
        $longueur = count($ligne);

        # On parcourt le tableau de correspondance pour trouver le code ISO du pays
        foreach ($correspondance as $codeP) {
            $codeP = explode($separateur, rtrim($codeP));
            if ($codeP[0] == $ligne[1]) {
                $ISOcode = $codeP[1];
            }
            
        }
        
        # On créé une nouvelle ligne dans le code du tableau html
        echo "\t\t\t<tr>\n";
        # On ajoute les cellules de la ligne dans le code du tableau html
        for ($i = 0; $i < $longueur; $i++) {
            # Si la cellule correspond au nom du pays
            if ($i == 1) {
                # Si ce pays n'a pas de code ISO, on génère simplement la ligne sans drapeau
                if ($ISOcode == ""){
                    echo "\t\t\t\t<td>" . $ligne[$i] . "</td>\n";
                }
                # Sinon, on génère la ligne avec le avec l'image du drapeau
                else {
                    # Utiliser cette commande pour les drapeaux normaux
                    echo "\t\t\t\t<td>" . "<img src=\"dossier_drapeaux/$ISOcode.webp\" alt=\"Drapeau\" title=\"Drapeau $ligne[$i]\">" . " $ligne[$i]" . "</td>\n";

                    # Utiliser la suivante pour les drapeaux flottants (mettre en commentaire la commande précédente et décommenter celle ci-dessous)
                    # echo "\t\t\t\t<td>" . "<img src=\"dossier_drapeaux_flottants/$ISOcode.webp\" alt=\"Drapeau\" title=\"Drapeau $ligne[$i]\">" . " $ligne[$i]" . "</td>\n";
                }
                
            }
            # Si la cellule ne correspond pas au nom du pays, on l'ajoute à la ligne directement
            else {
                echo "\t\t\t\t<td>" . $ligne[$i] . "</td>\n";
            }
        }
        # On termine la ligne dans le code du tableau html en ajoutant un saut de ligne
        echo "\t\t\t</tr>\n";
    }

    # On ferme le code du tableau html et on ferme le code da la page html.
    echo "\t\t</tbody>\n";
    echo "\t</table>\n";
    echo "</body>\n";
?>