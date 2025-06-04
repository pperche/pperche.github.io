#!/bin/bash
tableau=$1

# On supprime les 3 premières lignes du tableau contenant l'entête et le titre
nouveauTableau=$(tail -n +4 < $tableau)
# On supprime l'ancien tableau
sudo rm $1
# On créé lé nouveau tableau
printf "%s\n" "$nouveauTableau" > $tableau