#!/bin/bash

# Démarrer un conteneur docker pour exécuter le script permettant la génération d'un PDF à partir d'une page HTML
sudo docker container run -d --name HTMLtoPdf bigpapoo/sae103-html2pdf sleep "99999"

# Copier les fichier nécessaires dans le conteneur docker
sudo docker cp index.html HTMLtoPdf:/data
sudo docker cp dossier_drapeaux HTMLtoPdf:/data
sudo docker cp dossier_drapeaux_flottants HTMLtoPdf:/data
sudo docker cp logo_JO_Paris_2024.png HTMLtoPdf:/data
sudo docker cp style.css HTMLtoPdf:/data

# Exécuter le script de génération du PDF
sudo docker exec HTMLtoPdf weasyprint /data/index.html /data/index.pdf

# Récupérer le PDF et le copier dans le répertoir courant
sudo docker cp HTMLtoPdf:/data/index.pdf ./

# Arrêter et supprimer le conteneur docker
sudo docker stop HTMLtoPdf
sudo docker rm HTMLtoPdf