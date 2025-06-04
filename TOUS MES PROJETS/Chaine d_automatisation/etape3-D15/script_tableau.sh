#!/bin/bash

# Convertir le fichier tableau.xlsx en tableau.csv
bash convertir_xlsx_csv.sh tableau.xlsx
# Supprimer le titre et les entêtes de colonnes du tableau
bash suppr_entete_tab.sh tableau.csv
# Trier le tableau dans l'ordre décroissant selon le nombre de médailles de chaque type pour chaque pays, puis selon leur nom
bash trier_tableau.sh tableau.csv


# Démarrer un conteneur docker pour exécuter les scripts php
sudo docker container run --name phpContainer -d bigpapoo/sae103-excel2csv tail -f /dev/null
# Copier les fichiers nécessaires dans ce conteneur
sudo docker cp somme_medailles.php phpContainer:/app
sudo docker cp classement.php phpContainer:/app
sudo docker cp tableau.csv phpContainer:/app


# Exécuter les scripts php pour calculer la somme des médailles et le classement
sudo docker exec phpContainer php somme_medailles.php
sudo docker exec phpContainer php classement.php


# Copier le tableau trié dans le répertoire courant pour récupérer le bon classement
sudo docker cp phpContainer:/app/tableau.csv ./

# Arrêter et supprimer le conteneur docker
sudo docker container stop phpContainer
sudo docker container rm phpContainer