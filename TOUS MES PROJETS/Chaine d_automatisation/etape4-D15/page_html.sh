#!/bin/bash

# Démarrer un conteneur docker pour exécuter les scripts PHP
sudo docker container run -d --name phpToHTML bigpapoo/sae103-wget -c "sleep 99999"

# Copier les fichiers nécessaires dans le conteneur docker
sudo docker cp html.php phpToHTML:/data
sudo docker cp php_to_html.sh phpToHTML:/data
sudo docker cp tableau.csv phpToHTML:/data
sudo docker cp nomp_codep.csv phpToHTML:/data

# Exécuter qui va exécuter le script PHP dans le conteneur docker
sudo docker exec phpToHTML bash php_to_html.sh

# Récupère la page html et la copie dans le répertoire courant
sudo docker cp phpToHTML:/data/index.html ./

# Arrête et supprime le conteneur docker
sudo docker stop phpToHTML
sudo docker rm phpToHTML