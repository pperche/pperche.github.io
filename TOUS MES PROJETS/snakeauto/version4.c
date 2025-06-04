#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// taille du serpent
#define TAILLE 10
#define NB_PORTE 4

#define NB_CORRIDOR 8

// dimensions du plateau
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40

// position initiale de la tête du serpent
#define X_INITIAL_SERPENT_1 40
#define Y_INITIAL_SERPENT_1 13

#define X_INITIAL_SERPENT_2 40
#define Y_INITIAL_SERPENT_2 27

// nombre de pommes à manger pour gagner
#define NB_POMMES 10

// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 100000

// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE_SERPENT_1 '1'
#define TETE_SERPENT_2 '2'

// touches de direction ou d'arrêt du jeu
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define IMMOBILE 'i'
#define STOP 'a'
#define VERTICAL 'v'
#define HORIZONTAL 'h'

// caractères pour les éléments du plateau
#define BORDURE '#'
#define ISSUE ' '
#define VIDE ' '
#define POMME '6'

#define COTE_PAVE 5
#define PAVE '#'
#define NOMBRE_PAVE 6

/*
tableau de tableau en constante contenant les coordonnées des portes du plateau,
les coordonnées x et y d'entrée sont sur les 2 premières valeurs
les coordonnées x et y de sortie sont sur les deux dernières
il y a donc 4, et 4 uniques valeurs par tableau
(puisque chaque porte ne peut avoir que 2 valeurs de coordonnées en entrée et de même en sortie, c'est-à-dire 4 valeurs)
*/
const int PORTE_TAB[NB_PORTE][4] = {
{0, HAUTEUR_PLATEAU / 2, LARGEUR_PLATEAU, HAUTEUR_PLATEAU / 2},
{LARGEUR_PLATEAU + 1, HAUTEUR_PLATEAU / 2, 0, HAUTEUR_PLATEAU / 2},
{LARGEUR_PLATEAU / 2, 0, LARGEUR_PLATEAU / 2, HAUTEUR_PLATEAU},
{LARGEUR_PLATEAU / 2, HAUTEUR_PLATEAU + 1, LARGEUR_PLATEAU / 2, 0}};

const int CORRIDOR_TAB[NB_CORRIDOR][4]={
	{3,9,9,3},
	{9,3,3,9},
	{78,9,72,3},
	{72,3,78,9},
	{3,33,8,38},
	{8,38,3,33},
	{78,33,73,38},
	{73,38,78,33}};

// définition d'un type pour le plateau : tPlateau
// Attention, pour que les indices du tableau 2D (qui commencent à 0) coincident
// avec les coordonées à l'écran (qui commencent à 1), on ajoute 1 aux dimensions
// et on neutralise la ligne 0 et la colonne 0 du tableau 2D (elles ne sont jamais
// utilisées)

typedef char tPlateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];

void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);

void ajouterPomme(tPlateau plateau, int nbPomme);

void afficher(int, int, char);
void effacer(int x, int y);

void dessinerSerpent1(int lesX[], int lesY[]);
void dessinerSerpent2(int lesX[], int lesY[]);

void progresser1(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel, bool *corridor);
void progresser2(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel, bool *corridor);

void gotoxy(int x, int y);

int kbhit();

void disable_echo();
void enable_echo();

char choisirManiereDeplacement(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int xDest, int yDest, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);
void choisirManiereDeplacementProgressionPart(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, bool *est_arrive, char direction);

char calculerDirectionVertical(int lesX[TAILLE], int lesY[TAILLE], int xPomme, int yPomme, tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);
char calculerDirectionHorizontal(int lesX[TAILLE], int lesY[TAILLE], int xPomme, int yPomme, tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);

char calculerDirectionElsePart(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);

int snakeCollision(int direction, int lesX[TAILLE], int lesY[TAILLE], int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);

void PassageTunnel(int xHead, int yHead, int xPomme, int yPomme, int *xDest, int *yDest);

bool peutAllerDansDirection(char direction, int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]);

void dessinerPave(tPlateau t);

int genererPos(int min, int max);

int corridorTaken(int xSerpent, int ySerpent);

// position des pommes

int lesPommesX[NB_POMMES] = {40, 75, 78, 2, 9, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = {20, 38, 2, 2, 5, 38, 32, 38, 32, 2};

int lesPavesX[NOMBRE_PAVE] = { 4, 73, 4, 73, 38, 38};
int lesPavesY[NOMBRE_PAVE] = { 4, 4, 33, 33, 14, 22};

int main(){

	// 2 tableaux contenant les positions des éléments qui constituent le serpent
	int lesXSerpent1[TAILLE];
	int lesYSerpent1[TAILLE];

	int lesXSerpent2[TAILLE];
	int lesYSerpent2[TAILLE];

	int xDest1;
	int xDest2;
	int yDest1;
	int yDest2;

	// nombre de déplacements
	int nbDeplacementsSerpent1 = 0;
	int nbDeplacementsSerpent2 = 0;

	// représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
	char touche;

	// direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
	char directionSerpent1, directionSerpent2;

	// le plateau de jeu
	tPlateau lePlateau;


	bool collision = false;
	bool gagne = false;

	bool pommeMangeeSerpent1 = false;
	bool pommeMangeeSerpent2 = false;

	bool tunnel1 = false;
	bool tunnel2 = false;

	bool isInCorridorSnake1 = false;
	bool isInCorridorSnake2 = false;

	int corridorIndexSnake1 = -1; // l'indice d'un tableau commence obligatoirement à 0, -1 ne pointe donc aucun élément
	int corridorIndexSnake2 = -1; // l'indice d'un tableau commence obligatoirement à 0, -1 ne pointe donc aucun élément

	// compteur de pommes mangées
	int pommeIndicateurSerpent1 = 0;
	int pommeIndicateurSerpent2 = 0;
	int pommeIndicateurTotal = 0;

	// initialisation de la position du serpent : positionnement de la
	// tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche
	for (int i = 0; i < TAILLE; i++){
		lesXSerpent1[i] = X_INITIAL_SERPENT_1 - i;
		lesYSerpent1[i] = Y_INITIAL_SERPENT_1;
	}

	for (int i = 0; i < TAILLE; i++){
		lesXSerpent2[i] = X_INITIAL_SERPENT_2 - i;
		lesYSerpent2[i] = Y_INITIAL_SERPENT_2;
	}

	// mise en place du plateau
	initPlateau(lePlateau);
	system("clear");
	dessinerPlateau(lePlateau);

	srand(time(NULL));
	ajouterPomme(lePlateau, pommeIndicateurTotal);

	// initialisation : le serpent se dirige vers la DROITE
	dessinerSerpent1(lesXSerpent1, lesYSerpent1);
	dessinerSerpent2(lesXSerpent2, lesYSerpent2);
	disable_echo();
	directionSerpent1 = DROITE;
	directionSerpent2 = DROITE;
	touche = DROITE;

	PassageTunnel(lesXSerpent1[0], lesYSerpent1[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest1, &yDest1);
	PassageTunnel(lesXSerpent2[0], lesYSerpent2[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest2, &yDest2);

	char deplacementWaySerpent1 = choisirManiereDeplacement(lesXSerpent1, lesYSerpent1, lePlateau, xDest1, yDest1, lesXSerpent2, lesYSerpent2);
	char deplacementWaySerpent2 = choisirManiereDeplacement(lesXSerpent2, lesYSerpent2, lePlateau, xDest2, yDest2, lesXSerpent1, lesYSerpent1);

	// boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
	// si toutes les pommes sont mangées
	do{
		if (deplacementWaySerpent1 == VERTICAL){
			directionSerpent1 = calculerDirectionVertical(lesXSerpent1, lesYSerpent1, xDest1, yDest1, lePlateau, lesXSerpent2, lesYSerpent2);
		}
		
		else {
			directionSerpent1 = calculerDirectionHorizontal(lesXSerpent1, lesYSerpent1, xDest1, yDest1, lePlateau, lesXSerpent2, lesYSerpent2);
		}

		if (deplacementWaySerpent2 == VERTICAL){
			directionSerpent2 = calculerDirectionVertical(lesXSerpent2, lesYSerpent2, xDest2, yDest2, lePlateau, lesXSerpent1, lesYSerpent1);
		}
		
		else {
			directionSerpent2 = calculerDirectionHorizontal(lesXSerpent2, lesYSerpent2, xDest2, yDest2, lePlateau, lesXSerpent1, lesYSerpent1);
		}




		if (directionSerpent1 != IMMOBILE){
			progresser1(lesXSerpent1, lesYSerpent1, directionSerpent1, lePlateau, &collision, &pommeMangeeSerpent1, &tunnel1, &isInCorridorSnake1);
			nbDeplacementsSerpent1++;
		} 
		else if (directionSerpent1 == IMMOBILE){
			nbDeplacementsSerpent1++;
		}

		if (directionSerpent2 != IMMOBILE){
			progresser2(lesXSerpent2, lesYSerpent2, directionSerpent2, lePlateau, &collision, &pommeMangeeSerpent2, &tunnel2, &isInCorridorSnake2);
			nbDeplacementsSerpent2++;
		}
		else if (directionSerpent2 == IMMOBILE){
			nbDeplacementsSerpent2++;
		}



		if(isInCorridorSnake1 && corridorIndexSnake1 == -1){
			corridorIndexSnake1 = corridorTaken(lesXSerpent1[0], lesYSerpent1[0]);
		} 
		else if (!isInCorridorSnake1 && corridorIndexSnake1 != -1){
			corridorIndexSnake1 = -1;
			PassageTunnel(lesXSerpent1[0], lesYSerpent1[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest1, &yDest1);
			deplacementWaySerpent1 = choisirManiereDeplacement(lesXSerpent1, lesYSerpent1, lePlateau, xDest1, yDest1, lesXSerpent2, lesYSerpent2);
		}

		if(isInCorridorSnake2 && corridorIndexSnake2 == -1){
			corridorIndexSnake2 = corridorTaken(lesXSerpent2[0], lesYSerpent2[0]);
		} 
		else if (!isInCorridorSnake2 && corridorIndexSnake2 != -1){
			corridorIndexSnake2 = -1;
			PassageTunnel(lesXSerpent2[0], lesYSerpent2[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest2, &yDest2);
			deplacementWaySerpent2 = choisirManiereDeplacement(lesXSerpent2, lesYSerpent2, lePlateau, xDest2, yDest2, lesXSerpent1, lesYSerpent1);
		}




		if (!isInCorridorSnake2 && !isInCorridorSnake1){

			corridorIndexSnake1 = -1;
			corridorIndexSnake2 = -1;
		}
 
		if (tunnel1){
			xDest1 = lesPommesX[pommeIndicateurTotal];
			yDest1 = lesPommesY[pommeIndicateurTotal];
			tunnel1 = false;
			isInCorridorSnake1 = false;
			deplacementWaySerpent1 = choisirManiereDeplacement(lesXSerpent1, lesYSerpent1, lePlateau, xDest1, yDest1, lesXSerpent2, lesYSerpent2);
		}




		if (tunnel2){
			xDest2 = lesPommesX[pommeIndicateurTotal];
			yDest2 = lesPommesY[pommeIndicateurTotal];
			tunnel2 = false;
			isInCorridorSnake2 = false;
			deplacementWaySerpent2 = choisirManiereDeplacement(lesXSerpent2, lesYSerpent2, lePlateau, xDest2, yDest2, lesXSerpent1, lesYSerpent1);
		}
	

		if (pommeMangeeSerpent1 || pommeMangeeSerpent2){
			pommeIndicateurTotal++;
			gagne = (pommeIndicateurTotal == NB_POMMES);

			if (!gagne){
				ajouterPomme(lePlateau, pommeIndicateurTotal);

				if (pommeMangeeSerpent1){
					pommeIndicateurSerpent1++;
					pommeMangeeSerpent1 = false;
				}

				else if (pommeMangeeSerpent2){
					pommeIndicateurSerpent2++;
					pommeMangeeSerpent2 = false;								
				}

				if (isInCorridorSnake1)
				{
					xDest1 = CORRIDOR_TAB[corridorIndexSnake1][2];
					yDest1 = CORRIDOR_TAB[corridorIndexSnake1][3];
				} else {
					PassageTunnel(lesXSerpent1[0], lesYSerpent1[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest1, &yDest1);
					deplacementWaySerpent1 = choisirManiereDeplacement(lesXSerpent1, lesYSerpent1, lePlateau, xDest1, yDest1, lesXSerpent2, lesYSerpent2);
				}

				if (isInCorridorSnake2)
				{
					xDest2 = CORRIDOR_TAB[corridorIndexSnake2][2];
					yDest2 = CORRIDOR_TAB[corridorIndexSnake2][3];
				} else {
					PassageTunnel(lesXSerpent2[0], lesYSerpent2[0], lesPommesX[pommeIndicateurTotal], lesPommesY[pommeIndicateurTotal], &xDest2, &yDest2);
					deplacementWaySerpent2 = choisirManiereDeplacement(lesXSerpent2, lesYSerpent2, lePlateau, xDest2, yDest2, lesXSerpent1, lesYSerpent1);
				}

				
				

			}
			else{

				if (pommeMangeeSerpent1){
					pommeIndicateurSerpent1++;
				}

				else if (pommeMangeeSerpent2){
					pommeIndicateurSerpent2++;							
				}

			}
		}

		gotoxy(LARGEUR_PLATEAU +5, HAUTEUR_PLATEAU/2);
		printf("s1 : (%d;%d), way : (%c)", xDest1, yDest1, deplacementWaySerpent1);
		gotoxy(LARGEUR_PLATEAU +5, HAUTEUR_PLATEAU/2 + 1);
		printf("s2 : (%d;%d), way : (%c)", xDest2, yDest2, deplacementWaySerpent2);

		gotoxy(LARGEUR_PLATEAU +5, HAUTEUR_PLATEAU/2 +3);
		printf("s1 : (%d), corridor : (%d)", isInCorridorSnake1, corridorIndexSnake1);
		gotoxy(LARGEUR_PLATEAU +5, HAUTEUR_PLATEAU/2 + 4);
		printf("s2 : (%d), corridor : (%d)", isInCorridorSnake2, corridorIndexSnake2);


		if (!gagne){

			if (!collision){
				usleep(ATTENTE);

				if (kbhit() == 1){
					touche = getchar();
				}
			}
		}

	} while (touche != STOP && !collision && !gagne);

	enable_echo();

	gotoxy(1, HAUTEUR_PLATEAU + 1);

	// affiche le nombre de déplacements
	printf("Le serpent 1 a fait %d déplacements\n", nbDeplacementsSerpent1);
	printf("Le serpent 2 a fait %d déplacements\n", nbDeplacementsSerpent2);

	printf("Le serpent 1 a mangé %d pommes\n", pommeIndicateurSerpent1);
	printf("Le serpent 2 a mangé %d pommes\n", pommeIndicateurSerpent2);

	return EXIT_SUCCESS;
}

/************************************************/
/*		FONCTIONS ET PROCEDURES DU JEU 			*/
/************************************************/

void initPlateau(tPlateau plateau){

	// initialisation du plateau avec des espaces
	for (int i = 1; i <= LARGEUR_PLATEAU; i++){

		for (int j = 1; j <= HAUTEUR_PLATEAU; j++){
			plateau[i][j] = VIDE;
		}
	}

	// Mise en place la bordure autour du plateau
	// première ligne
	for (int i = 1; i <= LARGEUR_PLATEAU; i++){
		plateau[i][1] = BORDURE;
	}

	// lignes intermédiaires
	for (int j = 1; j <= HAUTEUR_PLATEAU; j++){
		plateau[1][j] = BORDURE;
		plateau[LARGEUR_PLATEAU][j] = BORDURE;
	}

	// dernière ligne
	for (int i = 1; i <= LARGEUR_PLATEAU; i++){
		plateau[i][HAUTEUR_PLATEAU] = BORDURE;
	}

	// issue du haut
	plateau[LARGEUR_PLATEAU / 2][1] = ISSUE;

	// issue du bas
	plateau[LARGEUR_PLATEAU / 2][HAUTEUR_PLATEAU] = ISSUE;

	// issue de gauche
	plateau[1][HAUTEUR_PLATEAU / 2] = ISSUE;

	// issue de droite
	plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU / 2] = ISSUE;


    dessinerPave(plateau); // Dessine les paves sur le plateau
	
}

void dessinerPlateau(tPlateau plateau){

	// affiche eà l'écran le contenu du tableau 2D représentant le plateau
	for (int i = 1; i <= LARGEUR_PLATEAU; i++){

		for (int j = 1; j <= HAUTEUR_PLATEAU; j++){
			afficher(i, j, plateau[i][j]);
		}
	}
}

void ajouterPomme(tPlateau plateau, int nbPomme){

	// génère aléatoirement la position d'une pomme,
	// vérifie que ça correspond à une case vide
	// du plateau puis l'ajoute au plateau et l'affiche
	afficher(lesPommesX[nbPomme], lesPommesY[nbPomme], POMME);
	plateau[lesPommesX[nbPomme]][lesPommesY[nbPomme]] = POMME;
}

void afficher(int x, int y, char car){

	gotoxy(x, y);
	printf("%c", car);
	gotoxy(1, 1);
}

void effacer(int x, int y){

	gotoxy(x, y);
	printf(" ");
	gotoxy(1, 1);
}

void dessinerSerpent1(int lesX[], int lesY[]){

	// affiche les anneaux puis la tête
	for (int i = 1; i < TAILLE; i++){
		afficher(lesX[i], lesY[i], CORPS);
	}

	afficher(lesX[0], lesY[0], TETE_SERPENT_1);
}

void dessinerSerpent2(int lesX[], int lesY[]){

	// affiche les anneaux puis la tête
	for (int i = 1; i < TAILLE; i++){
		afficher(lesX[i], lesY[i], CORPS);
	}

	afficher(lesX[0], lesY[0], TETE_SERPENT_2);
}

void progresser1(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel, bool *corridor){

	// efface le dernier élément avant d'actualiser la position de tous les
	// élémentds du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure
	effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

	for (int i = TAILLE - 1; i > 0; i--){
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}

	// faire progresser la tete dans la nouvelle direction
	switch (direction){
	case HAUT:
		lesY[0] = lesY[0] - 1;
		break;
	case BAS:
		lesY[0] = lesY[0] + 1;
		break;
	case DROITE:
		lesX[0] = lesX[0] + 1;
		break;
	case GAUCHE:
		lesX[0] = lesX[0] - 1;
		break;
	}

	*pomme = false;

	// détection d'une "collision" avec une pomme
	if (plateau[lesX[0]][lesY[0]] == POMME){
		*pomme = true;
		// la pomme disparait du plateau
		plateau[lesX[0]][lesY[0]] = VIDE;
	}

	// détection d'une collision avec la bordure
	if (plateau[lesX[0]][lesY[0]] == BORDURE){
		*collision = true;
	}

	// issue gauche qui téléporte à droite
	if (lesX[0] <= 0 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = LARGEUR_PLATEAU;
		*tunnel = true;
	}

	// issue droite qui téléporte à gauche
	else if (lesX[0] >= LARGEUR_PLATEAU + 1 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = 1;
		*tunnel = true;
	}

	// issue haut qui téléporte en bas
	else if (lesY[0] <= 0 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = HAUTEUR_PLATEAU;
		*tunnel = true;
	}

	// issue bas qui téléporte en haut
	else if (lesY[0] >= HAUTEUR_PLATEAU + 1 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = 1;
		*tunnel = true;
	}

	if ((((plateau[lesX[0]][lesY[0] - 1] == BORDURE || plateau[lesX[0]][lesY[0] - 1] == PAVE || plateau[lesX[0]][lesY[0] - 2] == BORDURE || plateau[lesX[0]][lesY[0] - 2] == PAVE) && (plateau[lesX[0]][lesY[0] + 1] == BORDURE || plateau[lesX[0]][lesY[0] + 1] == PAVE || plateau[lesX[0]][lesY[0] + 2] == BORDURE || plateau[lesX[0]][lesY[0] + 2] == PAVE)) || 
		((plateau[lesX[0] - 1][lesY[0]] == BORDURE || plateau[lesX[0] - 1][lesY[0]] == PAVE || plateau[lesX[0] - 2][lesY[0]] == BORDURE || plateau[lesX[0] - 2][lesY[0]] == PAVE) && (plateau[lesX[0] + 1][lesY[0]] == BORDURE || plateau[lesX[0] + 1][lesY[0]] == PAVE || plateau[lesX[0] + 2][lesY[0]] == BORDURE || plateau[lesX[0] + 2][lesY[0]] == PAVE)))
		&& !(*tunnel)){
		*corridor = true;
	} else if ((plateau[lesX[0]][lesY[0] - 1] == VIDE && plateau[lesX[0]][lesY[0] - 2] == VIDE && plateau[lesX[0]][lesY[0] + 1] == VIDE && plateau[lesX[0]][lesY[0] + 2] == VIDE) ||
		(plateau[lesX[0] - 1][lesY[0]] == VIDE && plateau[lesX[0] - 2][lesY[0]] == VIDE && plateau[lesX[0] + 1][lesY[0]] == VIDE && plateau[lesX[0] + 2][lesY[0]] == VIDE)){
		*corridor = false;
	}

	dessinerSerpent1(lesX, lesY);
}

void progresser2(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel, bool *corridor){

	// efface le dernier élément avant d'actualiser la position de tous les
	// élémentds du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure
	effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

	for (int i = TAILLE - 1; i > 0; i--){
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}

	// faire progresser la tete dans la nouvelle direction
	switch (direction){
	case HAUT:
		lesY[0] = lesY[0] - 1;
		break;
	case BAS:
		lesY[0] = lesY[0] + 1;
		break;
	case DROITE:
		lesX[0] = lesX[0] + 1;
		break;
	case GAUCHE:
		lesX[0] = lesX[0] - 1;
		break;
	}

	*pomme = false;

	// détection d'une "collision" avec une pomme
	if (plateau[lesX[0]][lesY[0]] == POMME){
		*pomme = true;
		// la pomme disparait du plateau
		plateau[lesX[0]][lesY[0]] = VIDE;
	}

	// détection d'une collision avec la bordure
	if (plateau[lesX[0]][lesY[0]] == BORDURE){
		*collision = true;
	}

	// issue gauche qui téléporte à droite
	if (lesX[0] <= 0 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = LARGEUR_PLATEAU;
		*tunnel = true;
	}

	// issue droite qui téléporte à gauche
	else if (lesX[0] >= LARGEUR_PLATEAU + 1 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = 1;
		*tunnel = true;
	}

	// issue haut qui téléporte en bas
	else if (lesY[0] <= 0 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = HAUTEUR_PLATEAU;
		*tunnel = true;
	}

	// issue bas qui téléporte en haut
	else if (lesY[0] >= HAUTEUR_PLATEAU + 1 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = 1;
		*tunnel = true;
	}

	if ((((plateau[lesX[0]][lesY[0] - 1] == BORDURE || plateau[lesX[0]][lesY[0] - 1] == PAVE || plateau[lesX[0]][lesY[0] - 2] == BORDURE || plateau[lesX[0]][lesY[0] - 2] == PAVE) && (plateau[lesX[0]][lesY[0] + 1] == BORDURE || plateau[lesX[0]][lesY[0] + 1] == PAVE || plateau[lesX[0]][lesY[0] + 2] == BORDURE || plateau[lesX[0]][lesY[0] + 2] == PAVE)) || 
		((plateau[lesX[0] - 1][lesY[0]] == BORDURE || plateau[lesX[0] - 1][lesY[0]] == PAVE || plateau[lesX[0] - 2][lesY[0]] == BORDURE || plateau[lesX[0] - 2][lesY[0]] == PAVE) && (plateau[lesX[0] + 1][lesY[0]] == BORDURE || plateau[lesX[0] + 1][lesY[0]] == PAVE || plateau[lesX[0] + 2][lesY[0]] == BORDURE || plateau[lesX[0] + 2][lesY[0]] == PAVE)))
		&& !(*tunnel)){
		*corridor = true;
	} else if ((plateau[lesX[0]][lesY[0] - 1] == VIDE && plateau[lesX[0]][lesY[0] - 2] == VIDE && plateau[lesX[0]][lesY[0] + 1] == VIDE && plateau[lesX[0]][lesY[0] + 2] == VIDE) ||
		(plateau[lesX[0] - 1][lesY[0]] == VIDE && plateau[lesX[0] - 2][lesY[0]] == VIDE && plateau[lesX[0] + 1][lesY[0]] == VIDE && plateau[lesX[0] + 2][lesY[0]] == VIDE)){
		*corridor = false;
	}

	dessinerSerpent2(lesX, lesY);
}

/************************************************/
/*				 FONCTIONS UTILITAIRES 			*/
/************************************************/

void gotoxy(int x, int y){

	printf("\033[%d;%df", y, x);
}

int kbhit(){

	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent

	int unCaractere = 0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF){
		ungetc(ch, stdin);
		unCaractere = 1;
	}

	return unCaractere;
}

// Fonction pour désactiver l'echo
void disable_echo(){

	struct termios tty;

	// Obtenir les attributs du terminal
	if (tcgetattr(STDIN_FILENO, &tty) == -1){
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}

	// Désactiver le flag ECHO
	tty.c_lflag &= ~ECHO;

	// Appliquer les nouvelles configurations
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1){
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}

// Fonction pour réactiver l'echo
void enable_echo(){

	struct termios tty;

	// Obtenir les attributs du terminal
	if (tcgetattr(STDIN_FILENO, &tty) == -1){

		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}

	// Réactiver le flag ECHO
	tty.c_lflag |= ECHO;

	// Appliquer les nouvelles configurations
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1){
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}
}


char choisirManiereDeplacement(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int xDest, int yDest, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){
	/*
	fonction de calcul du chemin le plus court, simule le déplacement du serpent,
	teste le déplacement du serpent en X puis en Y, et en Y puis en X
	retourne un char définissant la manière de déplacmeent
	*/

	bool est_arrive; // booléen définissant si le serpent est arrivé à destination
	char maniere;
	
	// var contenant la diretion pour les deux chemin testés
	char directionVertical;
	char directionHorizontal;

	bool arriveVertical = false;
	bool arriveHorizontal = false;

	//tableaux des positions pour les deux chemins testés
	int lesXVerticaux[TAILLE];
	int lesXHorizontaux[TAILLE];
	int lesYVerticaux[TAILLE];
	int lesYHorizontaux[TAILLE];

	for (int i = 0; i < TAILLE; i++){
		lesXHorizontaux[i] = lesX[i]; 
		lesXVerticaux[i] = lesX[i];
		lesYHorizontaux[i] = lesY[i];
		lesYVerticaux[i] = lesY[i];
	}

	while (arriveVertical == false && arriveHorizontal == false){


		//on simule la mise à jour la direction du serpent			
		directionHorizontal = calculerDirectionHorizontal(lesXHorizontaux, lesYHorizontaux, xDest, yDest, t, otherSnakeX, otherSnakeY);
		directionVertical = calculerDirectionVertical(lesXVerticaux, lesYVerticaux, xDest, yDest, t, otherSnakeX, otherSnakeY);

		//on simule la mise à jour de ses tableaux de positions, on vérifie si le premier n'est pas arrivé à destination avant de mettre à jour l'autre simulation
		choisirManiereDeplacementProgressionPart(lesXVerticaux, lesYVerticaux, t, &est_arrive, directionVertical);
		if (est_arrive == true){
			arriveVertical = true;
			est_arrive = false;
		} else {
			choisirManiereDeplacementProgressionPart(lesXHorizontaux, lesYHorizontaux, t, &est_arrive, directionHorizontal);
			if (est_arrive == true){
				arriveHorizontal = true;
				est_arrive = false;
			}
		}	
	}
	// if (arriveHorizontal == true){
	// 	maniere = HORIZONTAL;
	// } else {
	// 	maniere = VERTICAL;
	// }

	if (arriveVertical == true){
		maniere = VERTICAL;
	} else {
		maniere = HORIZONTAL;
	}

	for(int i =0; i < TAILLE; i++){
			effacer(lesX[i], lesY[i]);
		}

	return maniere;
}

void choisirManiereDeplacementProgressionPart(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, bool *est_arrive, char direction){

	for (int i = TAILLE - 1; i > 0; i--){
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}

	// faire progresser la tete dans la nouvelle direction
	switch (direction){
	case HAUT:
		lesY[0] = lesY[0] - 1;
		break;
	case BAS:
		lesY[0] = lesY[0] + 1;
		break;
	case DROITE:
		lesX[0] = lesX[0] + 1;
		break;
	case GAUCHE:
		lesX[0] = lesX[0] - 1;
		break;
	}

	*est_arrive = false;

	// détection d'une "collision" avec une pomme

	if (t[lesX[0]][lesY[0]] == POMME){
		*est_arrive = true;
	}

	// issue gauche qui téléporte à droite
	if (lesX[0] <= 0 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = LARGEUR_PLATEAU;
		*est_arrive = true;
	}

	// issue droite qui téléporte à gauche
	else if (lesX[0] >= LARGEUR_PLATEAU + 1 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = 1;
		*est_arrive = true;
	}

	// issue haut qui téléporte en bas
	else if (lesY[0] <= 0 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = HAUTEUR_PLATEAU;
		*est_arrive = true;
	}

	// issue bas qui téléporte en haut
	else if (lesY[0] >= HAUTEUR_PLATEAU + 1 && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = 1;
		*est_arrive = true;
	}

}


char calculerDirectionVertical(int lesX[TAILLE], int lesY[TAILLE], int xDest, int yDest, tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){
	/*
	fonction qui réalise le déplacement du serpent en Y, puis en X
	*/

	char direction;

	if (lesY[0] < yDest && peutAllerDansDirection(BAS, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = BAS; //Si le serpent peut aller en bas, choisir la direction BAS
	}

	else if (lesY[0] > yDest && peutAllerDansDirection(HAUT, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = HAUT; //Si le serpent peut aller en haut, choisir la direction HAUT
	}

	else if (lesX[0] < xDest && peutAllerDansDirection(DROITE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = DROITE; //Si le serpent peut aller à droite, choisir la direction DROITE
	}

	else if (lesX[0] > xDest && peutAllerDansDirection(GAUCHE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = GAUCHE; //Si le serpent peut aller à gauche, choisir la direction GAUCHE
	}

	//Si aucune direction directe n'est possible, on teste les directions restantes jusqu'à ce que l'une soit valide
	else{

		direction = calculerDirectionElsePart(lesX, lesY, t, otherSnakeX, otherSnakeY);
	}
	return direction;
}


char calculerDirectionHorizontal(int lesX[TAILLE], int lesY[TAILLE], int xDest, int yDest, tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){
	/*
	fonction qui réalise le déplacement du serpent en X, puis en Y
	*/

	char direction;

	if (lesX[0] < xDest && peutAllerDansDirection(DROITE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = DROITE; //Si le serpent peut aller à droite, choisir la direction DROITE
	}

	else if (lesX[0] > xDest && peutAllerDansDirection(GAUCHE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = GAUCHE; //Si le serpent peut aller à gauche, choisir la direction GAUCHE
	}

	else if (lesY[0] < yDest && peutAllerDansDirection(BAS, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = BAS; //Si le serpent peut aller en bas, choisir la direction BAS
	}

	else if (lesY[0] > yDest && peutAllerDansDirection(HAUT, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = HAUT; //Si le serpent peut aller en haut, choisir la direction HAUT
	}

	//Si aucune direction directe n'est possible, on teste les directions restantes jusqu'à ce que l'une soit valide
	else{

		direction = calculerDirectionElsePart(lesX, lesY, t, otherSnakeX, otherSnakeY);
	}
	return direction;
}


char calculerDirectionElsePart(int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){
	/*
	fonction de recherche d'une issue que le serpent peut emprunter dans le cas où le chemin optimal n'est pas empruntable
	*/

	char direction;

	if (peutAllerDansDirection(BAS, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = BAS;
	}

	else if (peutAllerDansDirection(GAUCHE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = GAUCHE;
	}

	else if (peutAllerDansDirection(HAUT, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = HAUT;
	}

	else if (peutAllerDansDirection(DROITE, lesX, lesY, t, otherSnakeX, otherSnakeY)){
		direction = DROITE;
	}

	else {
		direction = IMMOBILE;
	}
	
	return direction;
}


int snakeCollision(int direction, int lesX[TAILLE], int lesY[TAILLE], int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){

	int yHead = lesY[0];
	int xHead = lesX[0];

	int collision = 0;

	int index = 0;

	switch (direction){
	case HAUT:
		yHead = lesY[0] - 1;
		break;
	case BAS:
		yHead = lesY[0] + 1;
		break;
	case DROITE:
		xHead = lesX[0] + 1;
		break;
	case GAUCHE:
		xHead = lesX[0] - 1;
		break;
	}

	// Parcourir le corps du serpent pour vérifier s'il y a une collision
	while (index < TAILLE && !collision){

	 	// Si la nouvelle position de la tête correspond à une position du corps
		if (index > 0 && (xHead == lesX[index]) && (yHead == lesY[index])){
	 		collision = 1; // Il y a une collision
	 	}
	 	if ((xHead == otherSnakeX[index] && yHead == otherSnakeY[index])){
	 		collision = 1; // Il y a une collision
	 	}

	 	index++;
	}

	return collision;
}

void PassageTunnel(int xHead, int yHead, int xPomme, int yPomme, int *xDest, int *yDest){

	/*
		Calcul des distances entre le serpent, la pomme et les tunnels, vise à vérifier si la prise d'un tunnel n'est pas plus courte qu'un
		simple déplacement jusqu'à la pomme, pour cela on :
		-calcule la distance entre le serpent et la pomme (serpPommeDist)
		-pour chaque tunnel on somme la distance entre le serpent et le tunnel, et entre le tunnel et la pomme (SerpTunnelPommeDist)
		-si serpPommeDist > SerpTunnelPommeDist : on donne comme destination le tunnel et on affecte à serpPommeDist la valeur de SerpTunnelPommeDist
		-pour s'assurer que le tunnel vérifié après a bien un chemin encore plus court ou non
	 */

	// variables intermédiaires de calcul de la distance entre le serpent et la pomme en empruntant un tunnel
	int xDistSerpTunnel, xDistTunnelPomme, yDistSerpTunnel, yDistTunnelPomme;

	// calcul de la distance entre le serpent et la pomme
	int xDefaultDest = xHead - xPomme;
	*xDest = xPomme;

	int yDefaultDest = yHead - yPomme;
	*yDest = yPomme;

	// mise en valeur absolue des distances
	if (xDefaultDest < 0){
		xDefaultDest = (-1) * xDefaultDest;
	}

	if (yDefaultDest < 0){
		yDefaultDest = (-1) * yDefaultDest;
	}

	int serpPommeDist = xDefaultDest + yDefaultDest; // distance entre le serpent et la pomme
	int SerpTunnelPommeDist = 0;					 // distance entre le serpent et la pomme en empruntant un tunnel

	for (int i = 0; i < NB_PORTE; i++){
		xDistSerpTunnel = xHead - PORTE_TAB[i][0];
		xDistTunnelPomme = xPomme - PORTE_TAB[i][2];

		// mise en valeur absolue des distances
		if (xDistSerpTunnel < 0){
			xDistSerpTunnel = (-1) * xDistSerpTunnel;
		}

		if (xDistTunnelPomme < 0){
			xDistTunnelPomme = (-1) * xDistTunnelPomme;
		}

		yDistSerpTunnel = yHead - PORTE_TAB[i][1];
		yDistTunnelPomme = yPomme - PORTE_TAB[i][3];

		// mise en valeur absolue des distances
		if (yDistSerpTunnel < 0){
			yDistSerpTunnel = (-1) * yDistSerpTunnel;
		}

		if (yDistTunnelPomme < 0){
			yDistTunnelPomme = (-1) * yDistTunnelPomme;
		}

		SerpTunnelPommeDist = xDistSerpTunnel + xDistTunnelPomme + yDistSerpTunnel + yDistTunnelPomme;

		// on vérifie si la distance avec le tunnel et plus courte que le déplacement actuel
		if (serpPommeDist > SerpTunnelPommeDist){
			*xDest = PORTE_TAB[i][0];
			*yDest = PORTE_TAB[i][1];
			serpPommeDist = SerpTunnelPommeDist;
		}
	}
}

//Cette fonction vérifie si un mouvement dans une direction donnée est possible pour le serpent
bool peutAllerDansDirection(char direction, int lesX[TAILLE], int lesY[TAILLE], tPlateau t, int otherSnakeX[TAILLE], int otherSnakeY[TAILLE]){

	//Vérifie si la case en dessous de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision avec le serpent dans cette direction.
	if (direction == BAS){
		return (t[lesX[0]][lesY[0] + 1] != BORDURE && !snakeCollision(BAS, lesX, lesY, otherSnakeX, otherSnakeY) && t[lesX[0]][lesY[0] + 1] != PAVE);
	}

	//Vérifie si la case au-dessus de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == HAUT){
		return (t[lesX[0]][lesY[0] - 1] != BORDURE && !snakeCollision(HAUT, lesX, lesY, otherSnakeX, otherSnakeY) && t[lesX[0]][lesY[0] - 1] != PAVE );
	}

	//Vérifie si la case à droite de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == DROITE){
		return (t[lesX[0] + 1][lesY[0]] != BORDURE && !snakeCollision(DROITE, lesX, lesY, otherSnakeX, otherSnakeY) && t[lesX[0] + 1][lesY[0]] != PAVE );
	}

	//Vérifie si la case à gauche de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == GAUCHE){
		return (t[lesX[0] - 1][lesY[0]] != BORDURE && !snakeCollision(GAUCHE, lesX, lesY, otherSnakeX, otherSnakeY) && t[lesX[0] - 1][lesY[0]] != PAVE);
	}

	return false;
}

void dessinerPave(tPlateau t) {
    // Dessiner chaque pavé en fonction des coordonnées définies
    for (int indicePave = 0; indicePave < NOMBRE_PAVE; indicePave++) {
        int xPave = lesPavesX[indicePave];
        int yPave = lesPavesY[indicePave];

        // Dessiner un pavé carré de taille COTE_PAVE x COTE_PAVE
        for (int i = 0; i < COTE_PAVE; i++) {
            for (int j = 0; j < COTE_PAVE; j++) {
                int x = xPave + i;
                int y = yPave + j;
				t[x][y] = PAVE;  
			}
        }
    }
}

int corridorTaken(int xSerpent, int ySerpent){
	int xs, ys;
	int index = 0;
	int distance = 0; //distance cumulée du serpent et du serpent 2 par rapport à chaque entrée de couloir
	int keptIndex = 0;
	int distanceMemory = HAUTEUR_PLATEAU + LARGEUR_PLATEAU;
	for(int i = 0; i < NB_CORRIDOR; i++){
		xs = xSerpent - CORRIDOR_TAB[i][0];

		if (xs < 0){
			xs *= (-1);
		}

		ys = ySerpent - CORRIDOR_TAB[i][1];

		if (ys < 0){
			ys *= (-1);
		}


		distance = xs + ys;

		if (distance < distanceMemory && distance < 20){
			distanceMemory = distance;
			keptIndex = index;
		}

		index++;
	
	}
	return keptIndex;

}