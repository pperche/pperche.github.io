#!/bin/bash
nomFichier=$1

# On tri sur le champs 2,3 puis 4 numériquement (-n) et dans l'ordre décroissant (-r)
# Puis sur le champ 1 pour le trier par ordre alphabétique
nouveauTableau=$(sort -t ',' -k 2,2nr -k 3,3nr -k 4,4nr -k 1,1 < $nomFichier)
# On remplace l'ancien tableau par le nouveau
printf "%s\n" "$nouveauTableau" > $nomFichier