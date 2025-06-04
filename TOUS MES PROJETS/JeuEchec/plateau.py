from Class_Piece import *
class nouveau_plateau:
    def __init__(self, largeur, hauteur, Lignes, Colonnes, Case, fenetre):
        self.largeur = largeur  # Largeur du plateau
        self.hauteur = hauteur  # Hauteur du plateau
        self.Case = Case  # Taille de chaque case
        self.Gameplateau = self.largeur // 2  # Milieu du plateau (non utilisé dans le code actuel)
        self.fenetre = fenetre  # Fenêtre du jeu
        self.Lignes = Lignes  # Nombre de lignes du plateau
        self.Colonnes = Colonnes  # Nombre de colonnes du plateau
        self.plateau = []  # Plateau de jeu représenté sous forme de liste de listes
        self.creer_plateau()  # Création initiale du plateau avec les pièces

    # Crée et initialise le plateau avec les pièces
    def creer_plateau(self):
        for ligne in range(self.Lignes):
            self.plateau.append([0 for i in range(self.Colonnes)])  # Ajoute une ligne vide au plateau

            for colonne in range(self.Colonnes):
                if ligne == 1:
                    self.plateau[ligne][colonne] = Pion(self.Case, Pion_noir, noir, "Pion", ligne, colonne)  # Place les pions noirs
                if ligne == 6:
                    self.plateau[ligne][colonne] = Pion(self.Case, Pion_blanc, blanc, "Pion", ligne, colonne)  # Place les pions blancs

                if ligne == 0:
                    if colonne == 0 or colonne == 7:
                        self.plateau[ligne][colonne] = Tour(self.Case, Tour_noir, noir, "Tour", ligne, colonne)  # Place les tours noires
                    if colonne == 1 or colonne == 6:
                        self.plateau[ligne][colonne] = Cavalier(self.Case, Cavalier_noir, noir, "Cavalier", ligne, colonne)  # Place les cavaliers noirs
                    if colonne == 2 or colonne == 5:
                        self.plateau[ligne][colonne] = Fou(self.Case, Fou_noir, noir, "Fou", ligne, colonne)  # Place les fous noirs
                    if colonne == 3:
                        self.plateau[ligne][colonne] = Dame(self.Case, Dame_noir, noir, "Dame", ligne, colonne)  # Place la dame noire
                    if colonne == 4:
                        self.plateau[ligne][colonne] = Roi(self.Case, Roi_noir, noir, "Roi", ligne, colonne)  # Place le roi noir

                if ligne == 7:
                    if colonne == 0 or colonne == 7:
                        self.plateau[ligne][colonne] = Tour(self.Case, Tour_blanc, blanc, "Tour", ligne, colonne)  # Place les tours blanches
                    if colonne == 1 or colonne == 6:
                        self.plateau[ligne][colonne] = Cavalier(self.Case, Cavalier_blanc, blanc, "Cavalier", ligne, colonne)  # Place les cavaliers blancs
                    if colonne == 2 or colonne == 5:
                        self.plateau[ligne][colonne] = Fou(self.Case, Fou_blanc, blanc, "Fou", ligne, colonne)  # Place les fous blancs
                    if colonne == 3:
                        self.plateau[ligne][colonne] = Dame(self.Case, Dame_blanc, blanc, "Dame", ligne, colonne)  # Place la dame blanche
                    if colonne == 4:
                        self.plateau[ligne][colonne] = Roi(self.Case, Roi_blanc, blanc, "Roi", ligne, colonne)  # Place le roi blanc

    # Récupère une pièce à une position donnée
    def get_piece(self, ligne, colonne):
        return self.plateau[ligne][colonne]

    # Déplace une pièce vers une nouvelle position
    def move(self, piece, ligne, colonne):
        self.plateau[piece.ligne][piece.colonne], self.plateau[ligne][colonne] = self.plateau[ligne][colonne], self.plateau[piece.ligne][piece.colonne]
        piece.mouvement_piece(ligne, colonne)  # Met à jour la position de la pièce

        if piece.type == "Pion":
            if piece.premier_coups:
                piece.premier_coups = False  # Met à jour l'état du pion après son premier mouvement

    # Dessine le plateau de jeu
    def dessine_plateau(self):
        self.fenetre.fill(vert)  # Remplit la fenêtre avec la couleur bleue

        for ligne in range(self.Lignes):
            for colonne in range(ligne % 2, self.Colonnes, 2):
                pygame.draw.rect(self.fenetre, blanc, (self.Case * (ligne), self.Case * (colonne), self.Case, self.Case))  # Dessine les cases blanches

    # Dessine une pièce sur la fenêtre
    def dessine_piece(self, piece, fenetre):
        fenetre.blit(piece.image, (piece.x, piece.y))

    # Dessine toutes les pièces sur le plateau
    def dessine_Pieces(self):
        for ligne in range(self.Lignes):
            for colonne in range(self.Colonnes):
                if self.plateau[ligne][colonne] != 0:
                    self.dessine_piece(self.plateau[ligne][colonne], self.fenetre)
