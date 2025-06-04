#!/bin/bash

# Récupérer le nom du fichier à convertir
nomFichier=$1
# Le nom du nouveau fichier est le même que l'ancien auquel on retire l'extension
nomNouveauFichier=$(echo $nomFichier | cut -d "." -f 1)

# Démarrer un conteneur docker permettant d'exécuter la commande de conversion
sudo docker container run --name excel2csv -d bigpapoo/sae103-excel2csv tail -f /dev/null

# Copier le fichier Excel vers le conteneur docker
sudo docker cp $nomFichier excel2csv:/app

# Exécuter la comande de conversion
sudo docker exec excel2csv ssconvert $nomFichier $nomNouveauFichier.csv

# Copier le nouveau fichier CSV vers le répertoire courant
sudo docker cp excel2csv:/app/$nomNouveauFichier.csv ./

# Arrêter et supprimer le conteneur
sudo docker container stop excel2csv
sudo docker container rm excel2csv