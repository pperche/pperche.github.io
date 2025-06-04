#!/bin/bash

# Exécuter le script qui ajoute la colonne de proportion des médailles pour chaque pays
bash tableau_part_medailles.sh

# Exécuter le script qui crée la page HTML avec le tableau et les images
bash page_html.sh

# Exécuter le script qui convertit la page HTML en PDF
bash html_to_pdf.sh