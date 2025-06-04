import pygame

largeur, hauteur = 760,760
Lignes, Colonnes = 8,8
Case = largeur//Lignes

noir = (0,0,0)
blanc = (255,255,255)
bleu = (75, 115, 153)
bleu_2 = (64, 99, 131)
vert = (78, 92, 73)
vert_2 = (103,121,97)


Pion_blanc = pygame.image.load("pion_blanc.png")
red_pion_blanc = (Case,Case)
Pion_blanc = pygame.transform.scale(Pion_blanc,red_pion_blanc)

Pion_noir = pygame.image.load("pion_noir.png")
red_pion_noir = (Case,Case)
Pion_noir = pygame.transform.scale(Pion_noir,red_pion_noir)


Cavalier_blanc = pygame.image.load("cavalier_blanc.png")
red_cavalier_blanc = (Case,Case)
Cavalier_blanc = pygame.transform.scale(Cavalier_blanc,red_cavalier_blanc)

Cavalier_noir = pygame.image.load("cavalier_noir.png")
red_cavalier_noir = (Case,Case)
Cavalier_noir = pygame.transform.scale(Cavalier_noir,red_cavalier_noir)


Fou_blanc = pygame.image.load("fou_blanc.png")
red_fou_blanc = (Case,Case)
Fou_blanc = pygame.transform.scale(Fou_blanc,red_fou_blanc)

Fou_noir = pygame.image.load("fou_noir.png")
red_fou_noir = (Case,Case)
Fou_noir = pygame.transform.scale(Fou_noir,red_fou_noir)


Tour_blanc = pygame.image.load("tour_blanc.png")
red_tour_blanc = (Case,Case)
Tour_blanc = pygame.transform.scale(Tour_blanc,red_tour_blanc)

Tour_noir = pygame.image.load("tour_noir.png")
red_tour_noir = (Case,Case)
Tour_noir = pygame.transform.scale(Tour_noir,red_tour_noir)


Roi_blanc = pygame.image.load("roi_blanc.png")
red_roi_blanc = (Case,Case)
Roi_blanc = pygame.transform.scale(Roi_blanc,red_roi_blanc)

Dame_blanc = pygame.image.load("dame_blanc.png")
red_dame_blanc = (Case,Case)
Dame_blanc = pygame.transform.scale(Dame_blanc,red_dame_blanc)


Roi_noir = pygame.image.load("roi_noir.png")
red_roi_noir = (Case,Case)
Roi_noir = pygame.transform.scale(Roi_noir,red_roi_noir)

Dame_noir = pygame.image.load("dame_noir.png")
red_dame_noir = (Case,Case)
Dame_noir = pygame.transform.scale(Dame_noir,red_dame_noir)