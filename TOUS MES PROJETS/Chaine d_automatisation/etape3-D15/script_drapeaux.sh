#!/bin/bash

# Démarrer un conteneur docker pour exécuter les scripts de téléchargement des images
sudo docker container run -d --name wgetContainer bigpapoo/sae103-wget -c "sleep 99999"

# Copier les fichiers nécessaires dans ce conteneur
sudo docker cp script_normal_flag.php wgetContainer:/data
sudo docker cp script_waving_flag.php wgetContainer:/data
sudo docker cp codes_pays.txt wgetContainer:/data

# Exécuter les scripts php pour télécharger les images
sudo docker exec wgetContainer php script_normal_flag.php
sudo docker exec wgetContainer php script_waving_flag.php

# Copier le dossiers d'images dans le répertoire courant pour récupérer les drapeaux
sudo docker cp wgetContainer:/data/dossier_drapeaux ./
sudo docker cp wgetContainer:/data/dossier_drapeaux_flottants ./

# Arrêter et supprimer le conteneur docker
sudo docker stop wgetContainer
sudo docker rm wgetContainer