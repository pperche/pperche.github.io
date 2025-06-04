import pygame
pygame.init()

from Class_Jeux import *  # Importation des classes de jeu
from import_piece import *  # Importation des pièces d'échecs

fenetre = pygame.display.set_mode((largeur, hauteur))  # Création de la fenêtre de jeu

def get_positions(x, y):

    ligne = y // Case  # Calcul de l'indice de ligne en fonction de la position y de la souris
    colonne = x // Case  # Calcul de l'indice de colonne en fonction de la position x de la souris

    return ligne, colonne

def main():
    run = True  # Booléen pour exécuter la boucle principale du jeu
    game_over = False  # Booléen pour vérifier si le jeu est terminé
    au_tour = blanc  # Variable pour suivre le joueur en cours
    jeu = Jeux(largeur, hauteur, Lignes, Colonnes, Case, fenetre)  # Initialisation du jeu

    while run:
        jeu.update_fenetre()  # Mise à jour de la fenêtre de jeu
        if jeu.jeu_echec():
            game_over = True  # Vérification si l'état actuel du jeu est en échec

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False  # Quitter le jeu si la fenêtre est fermée
                quit()

            if event.type == pygame.MOUSEBUTTONDOWN and not game_over:
                if pygame.mouse.get_pressed()[0]:
                    location = pygame.mouse.get_pos()
                    ligne, colonne = get_positions(location[0], location[1])  # Obtention de la position du clic
                    jeu.selectionner(ligne, colonne)  # Sélection de la pièce sur la case cliquée

main()  # Appel de la fonction principale pour démarrer le jeu
