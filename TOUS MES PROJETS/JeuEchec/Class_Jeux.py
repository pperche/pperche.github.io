from plateau import *
class Jeux:
    def __init__(self, largeur, hauteur, Lignes, Colonnes, Case, fenetre):
        self.fenetre = fenetre  # Fenêtre du jeu
        self.plateau = nouveau_plateau(largeur, hauteur, Lignes, Colonnes, Case, fenetre)  # Initialisation du plateau
        self.Case = Case  # Taille de chaque case
        self.selectionne = None  # Pièce actuellement sélectionnée
        self.au_tour = blanc  # Couleur du joueur dont c'est le tour
        self.coups_autorises = []  # Liste des coups autorisés pour la pièce sélectionnée
        self.pieces_noires_restantes = 16  # Nombre de pièces noires restantes
        self.pieces_blanches_restantes = 16  # Nombre de pièces blanches restantes

    # Met à jour l'affichage de la fenêtre
    def update_fenetre(self):
        self.plateau.dessine_plateau()  # Dessine le plateau
        self.plateau.dessine_Pieces()  # Dessine toutes les pièces sur le plateau
        self.dessine_coups_autorises()  # Dessine les coups autorisés
        pygame.display.update()  # Met à jour l'affichage


    # Vérifie si le jeu est terminé (échec et mat)
    def jeu_echec(self):
        if self.pieces_noires_restantes == 0:
            print("Les blancs ont gagnés")
            return True

        if self.pieces_blanches_restantes == 0:
            print("Les noirs ont gagnés")
            return True

        if self.echec_et_mat(self.plateau):
            if self.au_tour == blanc:
                print("Les noirs ont gagnés")
                return True
            else:
                print("Les blancs ont gagnés")
                return True

    # Récupère les coups des pièces ennemies
    def coups_ennemies(self, piece, plateau):
        coups_ennemies = []
        for i in range(len(plateau)):
            for j in range(len(plateau[i])):
                if plateau[i][j] != 0:
                    if plateau[i][j].couleur != piece.couleur:
                        moves = plateau[i][j].get_coups_autorises(i, j, plateau)
                        for move in moves:
                            coups_ennemies.append(move)
        return coups_ennemies

    # Obtient la position du roi
    def get_Roi_pos(self, plateau):
        for i in range(len(plateau)):
            for j in range(len(plateau)):
                if plateau[i][j] != 0:
                    if plateau[i][j].type == "Roi" and plateau[i][j].couleur == self.au_tour:
                        return (i, j)

    # Simule un mouvement pour vérifier s'il est légal
    def simulate_move(self, piece, ligne, colonne):
        piece_ligne, piece_colonne = piece.ligne, piece.colonne
        print("piece ligne, colonne", piece_ligne, piece_colonne)
        print(ligne, colonne)
        save_piece = self.plateau.plateau[ligne][colonne]
        if self.plateau.plateau[ligne][colonne] != 0:
            self.plateau.plateau[ligne][colonne] = 0

        self.plateau.plateau[piece.ligne][piece.colonne], self.plateau.plateau[ligne][colonne] = self.plateau.plateau[ligne][colonne], self.plateau.plateau[piece.ligne][piece.colonne]

        Roi_pos = self.get_Roi_pos(self.plateau.plateau)
        if Roi_pos in self.coups_ennemies(piece, self.plateau.plateau):
            piece.ligne, piece.colonne = piece_ligne, piece_colonne
            self.plateau.plateau[piece_ligne][piece_colonne] = piece
            self.plateau.plateau[ligne][colonne] = save_piece
            return False

        piece.ligne, piece.colonne = piece_ligne, piece_colonne
        self.plateau.plateau[piece_ligne][piece_colonne] = piece
        self.plateau.plateau[ligne][colonne] = save_piece
        return True

    # Récupère les coups possibles pour les pièces du joueur actuel
    def coups_possibles(self, plateau):
        coups_possibles = []
        for i in range(len(plateau)):
            for j in range(len(plateau[i])):
                if plateau[i][j] != 0:
                    if plateau[i][j].couleur == self.au_tour and plateau[i][j].type != "Roi":
                        moves = plateau[i][j].get_coups_autorises(i, j, plateau)
                        for move in moves:
                            coups_possibles.append(move)

        return coups_possibles

    # Vérifie si le roi est en échec et mat
    def echec_et_mat(self, plateau):
        Roi_pos = self.get_Roi_pos(plateau.plateau)
        get_Roi = plateau.get_piece(Roi_pos[0], Roi_pos[1])
        Roi_coups_autorises = set(get_Roi.get_coups_autorises(Roi_pos[0], Roi_pos[1], plateau.plateau))
        coups_ennemies_set = set(self.coups_ennemies(get_Roi, plateau.plateau))
        Roi_moves = Roi_coups_autorises - coups_ennemies_set
        set1 = Roi_coups_autorises.intersection(coups_ennemies_set)
        coups_possibles_to_def = set1.intersection(self.coups_possibles(plateau.plateau))
        if len(Roi_moves) == 0 and len(Roi_coups_autorises) != 0 and len(coups_possibles_to_def) == 0:
            return True

        return False

    # Change le tour du joueur
    def change_au_tour(self):
        if self.au_tour == blanc:
            self.au_tour = noir
        else:
            self.au_tour = blanc

    # Sélectionne une pièce pour le mouvement
    def selectionner(self, ligne, colonne):
        if self.selectionne:
            move = self._move(ligne, colonne)
            if not move:
                self.selectionne = None
                self.selectionner(ligne, colonne)

        piece = self.plateau.get_piece(ligne, colonne)
        if piece != 0 and self.au_tour == piece.couleur:
            self.selectionne = piece
            self.coups_autorises = piece.get_coups_autorises(ligne, colonne, self.plateau.plateau)
            print("self coups_autorises", self.coups_autorises)
            print("new coups_autorises", self.coups_autorises)

    # Déplace une pièce vers une nouvelle position
    def _move(self, ligne, colonne):
        piece = self.plateau.get_piece(ligne, colonne)
        print("self selectionne", self.selectionne.type)
        if self.selectionne and (ligne, colonne) in self.coups_autorises:
            if piece == 0 or piece.couleur != self.selectionne.couleur:
                print(self.simulate_move(self.selectionne, ligne, colonne))
                if self.simulate_move(self.selectionne, ligne, colonne):
                    self.remove(self.plateau.plateau, piece, ligne, colonne)
                    self.plateau.move(self.selectionne, ligne, colonne)
                    if self.selectionne.type == "Pion":
                        self.promotion(ligne, colonne)
                    self.change_au_tour()
                    print("au_tour", self.au_tour)
                    self.coups_autorises = []
                    self.selectionne = None
                    return True
                return False
        return False

    # Retire une pièce du plateau
    def remove(self, plateau, piece, ligne, colonne):
        if piece != 0:
            plateau[ligne][colonne] = 0
            if piece.couleur == blanc:
                self.pieces_blanches_restantes -= 1
            else:
                self.pieces_noires_restantes -= 1
        print("pieces_blanches_restantes : ", self.pieces_blanches_restantes)
        print("pieces_noires_restantes : ", self.pieces_noires_restantes)

    # Dessine les coups autorisés sur le plateau
    def dessine_coups_autorises(self):
        if len(self.coups_autorises) > 0:
            for pos in self.coups_autorises:
                ligne, colonne = pos[0], pos[1]
                pygame.draw.circle(self.fenetre, vert_2, (colonne * self.Case + self.Case // 2, ligne * self.Case + self.Case // 2), self.Case // 8)

    # Récupère le plateau actuel
    def get_plateau(self):
        return self.plateau

    # Gère la promotion d'un pion en dame
    def promotion(self, ligne, colonne):
        piece = self.plateau.get_piece(ligne, colonne)
        if piece is not None and piece.type == "Pion":
            if (piece.couleur == blanc and ligne == 0) or (piece.couleur == noir and ligne == 7):
                self.plateau.plateau[ligne][colonne] = Dame(self.Case, Dame_blanc, blanc, "Dame", ligne, colonne)
