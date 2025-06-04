from import_piece import *

from import_piece import *

class Piece:
    def __init__(self, Case,image,couleur,type,ligne,colonne):
        self.Case = Case  # Taille de la case
        self.image = image  # Image représentant la pièce
        self.couleur = couleur  # Couleur de la pièce
        self.ligne = ligne  # Ligne actuelle de la pièce sur le plateau
        self.colonne = colonne  # Colonne actuelle de la pièce sur le plateau
        self.type = type  # Type de la pièce (pion, tour, fou, etc.)
        self.x = 0  # Position X sur l'écran
        self.y = 0  # Position Y sur l'écran
        self.coups_autorises = []  # Liste des coups autorisés pour la pièce
        self.calc_pos()  # Calculer la position de la pièce sur l'écran


    def mouvement_piece(self,ligne,colonne):
        self.ligne = ligne  # Déplacer la pièce à la ligne spécifiée
        self.colonne = colonne  # Déplacer la pièce à la colonne spécifiée
        self.calc_pos()  # Recalculer la position de la pièce

    def calc_pos(self):
        self.x = self.colonne*self.Case  # Calculer la position X sur l'écran en fonction de la colonne
        self.y = self.ligne*self.Case  # Calculer la position Y sur l'écran en fonction de la ligne

    def vide_coups_autorises(self):
        if len(self.coups_autorises) > 0:
            self.coups_autorises = []  # Vider la liste des coups autorisés


class Pion(Piece):
    def __init__(self,Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)
        self.premier_coups = True  # Indique si c'est le premier coup du pion


    def get_coups_autorises(self,ligne,colonne,plateau):

        self.vide_coups_autorises()

        if self.couleur == blanc:
            if ligne-1 >= 0:
                if plateau[ligne-1][colonne] == 0:
                    self.coups_autorises.append((ligne-1,colonne))  # Avancer d'une case

                if self.premier_coups:
                    if plateau[ligne-2][colonne] == 0:
                        self.coups_autorises.append((ligne-2,colonne))  # Avancer de deux cases au premier coup

                if colonne-1 >= 0:
                    if plateau[ligne-1][colonne-1] != 0:
                        piece = plateau[ligne-1][colonne-1]
                        if piece.couleur != self.couleur:
                            self.coups_autorises.append((ligne-1,colonne-1))  # Prendre une pièce en diagonale

            if colonne+1 < len(plateau):
                if plateau[ligne-1][colonne+1] != 0:
                    piece = plateau[ligne-1][colonne+1]
                    if piece.couleur != self.couleur:
                        self.coups_autorises.append((ligne-1,colonne+1))  # Prendre une pièce en diagonale

        if self.couleur == noir:

            if ligne+1 < len(plateau):
                if plateau[ligne+1][colonne] == 0:
                    self.coups_autorises.append((ligne+1,colonne))  # Avancer d'une case

                if self.premier_coups:
                    if plateau[ligne+1][colonne] == 0 and plateau[ligne+2][colonne] == 0:
                        self.coups_autorises.append((ligne+2,colonne))  # Avancer de deux cases au premier coup

                if colonne-1 >= 0:
                    if plateau[ligne+1][colonne-1] != 0:
                        piece = plateau[ligne+1][colonne-1]
                        if piece.couleur != self.couleur:
                            self.coups_autorises.append((ligne+1,colonne-1))  # Prendre une pièce en diagonale

            if ligne+1 < len(plateau) and colonne+1 < len(plateau[0]):
                if plateau[ligne+1][colonne+1] != 0:
                    piece = plateau[ligne+1][colonne+1]
                    if piece.couleur != self.couleur:
                        self.coups_autorises.append((ligne+1,colonne+1))  # Prendre une pièce en diagonale

        return self.coups_autorises


class Tour(Piece):
    def __init__(self, Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)

    def get_coups_autorises(self,ligne,colonne,plateau):
        self.vide_coups_autorises()
        # Vérification des mouvements verticaux
        for i in range(ligne+1, 8):
            if plateau[i][colonne] == 0:
                self.coups_autorises.append((i,colonne))  # Avancer d'une case
            else:
                if plateau[i][colonne].couleur != self.couleur:
                    self.coups_autorises.append((i,colonne))  # Prendre une pièce adverse
                    break
                else:
                    break

        for j in range(ligne-1,-1,-1):
            if plateau[j][colonne] == 0:
                self.coups_autorises.append((j,colonne))  # Avancer d'une case
            else:
                if plateau[j][colonne].couleur != self.couleur:
                    self.coups_autorises.append((j,colonne))  # Prendre une pièce adverse
                    break
                else:
                    break

        # Vérification des mouvements horizontaux
        for i in range(colonne+1, 8):
            if plateau[ligne][i] == 0:
                self.coups_autorises.append((ligne,i))  # Avancer d'une case
            else:
                if plateau[ligne][i].couleur != self.couleur:
                    self.coups_autorises.append((ligne,i))  # Prendre une pièce adverse
                    break
                else:
                    break

        for i in range(colonne-1, -1,-1):
            if plateau[ligne][i] == 0:
                self.coups_autorises.append((ligne,i))  # Avancer d'une case
            else:
                if plateau[ligne][i].couleur != self.couleur:
                    self.coups_autorises.append((ligne,i))  # Prendre une pièce adverse
                    break
                else:
                    break

        return self.coups_autorises


class Fou(Piece):
    def __init__(self, Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)

    def get_coups_autorises(self,ligne,colonne,plateau):
        self.vide_coups_autorises()
        # Vérification des mouvements en diagonale
        # Diagonale haut-droite
        ligne_i = ligne+1
        colonne_i = colonne+1
        while ligne_i <= 7 and colonne_i <= 7:
            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i,colonne_i))  # Avancer d'une case
                ligne_i += 1
                colonne_i += 1
            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i,colonne_i))  # Prendre une pièce adverse
                    break
                break

        # Diagonale haut-gauche
        ligne_i = ligne-1
        colonne_i = colonne-1
        while ligne_i >= 0 and colonne_i >= 0:
            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i,colonne_i))  # Avancer d'une case
                ligne_i -= 1
                colonne_i -= 1
            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i,colonne_i))  # Prendre une pièce adverse
                    break
                break

        # Diagonale bas-droite
        ligne_i = ligne-1
        colonne_i = colonne+1
        while ligne_i >= 0 and colonne_i <= 7:
            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i, colonne_i))  # Avancer d'une case
                ligne_i -= 1
                colonne_i += 1
            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i, colonne_i))  # Prendre une pièce adverse
                    break
                break

        # Diagonale bas-gauche
        ligne_i = ligne+1
        colonne_i = colonne-1
        while ligne_i <= 7 and colonne_i >= 0:
            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i, colonne_i))  # Avancer d'une case
                ligne_i += 1
                colonne_i -= 1
            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i, colonne_i))  # Prendre une pièce adverse
                    break
                break

        return self.coups_autorises


class Cavalier(Piece):
    def __init__(self, Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)

    def get_coups_autorises(self,ligne,colonne,plateau):
        self.vide_coups_autorises()

        # Vérification des mouvements en L pour le cavalier
        if ligne-2 >= 0 and colonne+1 < len(plateau):
            if plateau[ligne-2][colonne+1] == 0 or plateau[ligne-2][colonne+1].couleur != self.couleur:
                self.coups_autorises.append((ligne-2,colonne+1))

        if ligne-1 >= 0 and colonne+2 < len(plateau):
            if plateau[ligne-1][colonne+2] == 0 or plateau[ligne-1][colonne+2].couleur != self.couleur:
                self.coups_autorises.append((ligne-1,colonne+2))

        if ligne+1 < len(plateau) and colonne+2 < len(plateau):
            if plateau[ligne+1][colonne+2] == 0 or plateau[ligne+1][colonne+2].couleur != self.couleur:
                self.coups_autorises.append((ligne+1,colonne+2))

        if ligne+2 < len(plateau) and colonne+1 < len(plateau):
            if plateau[ligne+2][colonne+1] == 0 or plateau[ligne+2][colonne+1].couleur != self.couleur:
                self.coups_autorises.append((ligne+2,colonne+1))

        if ligne+2 < len(plateau) and colonne-1 >= 0:
            if plateau[ligne+2][colonne-1] == 0 or plateau[ligne+2][colonne-1].couleur != self.couleur:
                self.coups_autorises.append((ligne+2,colonne-1))

        if ligne+1 < len(plateau) and colonne-2 >= 0:
            if plateau[ligne+1][colonne-2] == 0 or plateau[ligne+1][colonne-2].couleur != self.couleur:
                self.coups_autorises.append((ligne+1,colonne-2))


        if ligne-1 >= 0 and colonne-2 >= 0:
            if plateau[ligne-1][colonne-2] == 0 or plateau[ligne-1][colonne-2].couleur != self.couleur:
                self.coups_autorises.append((ligne-1,colonne-2))

        if ligne-2 >= 0 and colonne-1 >= 0:
            if plateau[ligne-2][colonne-1] == 0 or plateau[ligne-2][colonne-1].couleur != self.couleur:
                self.coups_autorises.append((ligne-2,colonne-1))


        return self.coups_autorises


class Dame(Piece):
    def __init__(self, Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)

    def get_coups_autorises(self,ligne,colonne,plateau):
        self.vide_coups_autorises()

        # Vérification des mouvements en diagonale (comme le Fou)
        ligne_i = ligne+1
        colonne_i = colonne+1
        while ligne_i <= 7 and colonne_i <= 7:
            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i,colonne_i))
                ligne_i += 1
                colonne_i += 1

            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i,colonne_i))
                    break
                break

        ligne_i = ligne-1
        colonne_i = colonne-1
        while ligne_i >= 0 and colonne_i >= 0:

            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i,colonne_i))
                ligne_i -= 1
                colonne_i -= 1

            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i,colonne_i))
                    break
                break



        ligne_i = ligne-1
        colonne_i = colonne+1
        while ligne_i >= 0 and colonne_i <= 7:


            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i, colonne_i))
                ligne_i -= 1
                colonne_i += 1
            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur:
                    self.coups_autorises.append((ligne_i, colonne_i))
                    break

                break

        ligne_i = ligne+1
        colonne_i = colonne-1
        while ligne_i <= 7 and colonne_i >= 0:

            if plateau[ligne_i][colonne_i] == 0:
                self.coups_autorises.append((ligne_i, colonne_i))
                ligne_i += 1
                colonne_i -= 1

            else:
                if plateau[ligne_i][colonne_i].couleur != self.couleur :
                    self.coups_autorises.append((ligne_i, colonne_i))
                    break

                break

        # Vérification des mouvements verticaux et horizontaux (comme la Tour)            
        for i in range(ligne+1, 8):

            if plateau[i][colonne] == 0:

                self.coups_autorises.append((i,colonne))
            else:
                if plateau[i][colonne].couleur != self.couleur:

                    self.coups_autorises.append((i,colonne))
                    break
                else:
                    break

        for j in range(ligne-1,-1,-1):

            if plateau[j][colonne] == 0:

                self.coups_autorises.append((j,colonne))

            else:
                if plateau[j][colonne].couleur != self.couleur:

                    self.coups_autorises.append((j,colonne))
                    break

                else:

                    break

        for i in range(colonne+1, 8):
            if plateau[ligne][i] == 0:
                self.coups_autorises.append((ligne,i))

            else:
                if plateau[ligne][i].couleur != self.couleur:
                    self.coups_autorises.append((ligne,i))
                    break

                else:
                    break

        for i in range(colonne-1, -1,-1):

            if plateau[ligne][i] == 0:
                self.coups_autorises.append((ligne,i))

            else:
                if plateau[ligne][i].couleur != self.couleur:
                    self.coups_autorises.append((ligne,i))
                    break

                else:
                    break

        return self.coups_autorises



class Roi(Piece):
    def __init__(self,Case, image,couleur,type,ligne,colonne):
        super().__init__(Case, image,couleur,type,ligne,colonne)

    # Vérification des mouvements du roi dans toutes les directions (haut, bas, gauche, droite et diagonales)
    def get_coups_autorises(self,ligne,colonne,plateau):
        self.vide_coups_autorises()

        if ligne-1 >= 0:
            if plateau[ligne-1][colonne] == 0 or plateau[ligne-1][colonne].couleur != self.couleur:
                self.coups_autorises.append((ligne-1,colonne))

        if ligne-1 >= 0 and colonne+1 < len(plateau):
            if plateau[ligne-1][colonne+1] == 0 or plateau[ligne-1][colonne+1].couleur != self.couleur:
                self.coups_autorises.append((ligne-1,colonne+1))

        if colonne+1 < len(plateau):
            if plateau[ligne][colonne+1] == 0 or plateau[ligne][colonne+1].couleur != self.couleur:
                self.coups_autorises.append((ligne,colonne+1))

        if ligne+1 < len(plateau) and colonne+1 < len(plateau):
            if plateau[ligne+1][colonne+1] == 0 or plateau[ligne+1][colonne+1].couleur != self.couleur:
                self.coups_autorises.append((ligne+1,colonne+1))

        if ligne+1 < len(plateau):
            if plateau[ligne+1][colonne] == 0 or plateau[ligne+1][colonne].couleur != self.couleur:
                self.coups_autorises.append((ligne+1,colonne))

        if ligne+1 < len(plateau) and colonne-1 >= 0:
            if plateau[ligne+1][colonne-1] == 0 or plateau[ligne+1][colonne-1].couleur != self.couleur:
                self.coups_autorises.append((ligne+1,colonne-1))

        if colonne-1 >= 0:
            if plateau[ligne][colonne-1] == 0 or plateau[ligne][colonne-1].couleur != self.couleur:
                self.coups_autorises.append((ligne,colonne-1))

        if ligne-1 >= 0 and colonne-1 >= 0:
            if plateau[ligne-1][colonne-1] == 0 or plateau[ligne-1][colonne-1].couleur != self.couleur:
                self.coups_autorises.append((ligne-1, colonne-1))


        return self.coups_autorises
