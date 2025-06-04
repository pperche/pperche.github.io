#!/bin/bash

# On récupère le contenu généré par le script html.php et on le stocke dans la variable html
html=$(php html.php)
# On écrit ensuite le contenu récupéré dans un fichier index.html qui contiendra la page
printf "%s" "$html" > index.html