#!/bin/bash

# Démarrer un conteneur docker pour exécuter les scripts PHP
sudo docker container run -d --name TableauPHP bigpapoo/sae103-wget -c "sleep 99999"

# Copier les fichiers nécessaires dans le conteneur docker
sudo docker cp part_medailles.php TableauPHP:/data
sudo docker cp tableau.csv TableauPHP:/data

# Exécuter les scripts PHP dans le conteneur docker
sudo docker exec TableauPHP php part_medailles.php

# Copier le résultat du tableau dans le répertoire courant
sudo docker cp TableauPHP:/data/tableau.csv ./

# Arrêter et supprimer le conteneur docker
sudo docker stop TableauPHP
sudo docker rm TableauPHP