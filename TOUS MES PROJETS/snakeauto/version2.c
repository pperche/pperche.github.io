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

// dimensions du plateau
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40

// position initiale de la tête du serpent
#define X_INITIAL 40
#define Y_INITIAL 20

// nombre de pommes à manger pour gagner
#define NB_POMMES 10

// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 200000

// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE 'O'

// touches de direction ou d'arrêt du jeu
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define STOP 'a'

// caractères pour les éléments du plateau
#define BORDURE '#'
#define ISSUE ' '
#define VIDE ' '
#define POMME '6'

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

void dessinerSerpent(int lesX[], int lesY[]);

void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel);

void gotoxy(int x, int y);

int kbhit();

void disable_echo();
void enable_echo();

char calculerDirectionVersDestination(int lesX[TAILLE], int lesY[TAILLE], int xPomme, int yPomme, tPlateau t);

int snakeCollision(int direction, int lesX[TAILLE], int lesY[TAILLE]);

void PassageTunnel(int xHead, int yHead, int xPomme, int yPomme, int *xDest, int *yDest);

bool peutAllerDansDirection(char direction, int lesX[TAILLE], int lesY[TAILLE], tPlateau t);

bool estPortail(int x, int y, tPlateau t, int *xDest, int *yDest);

bool getDestinationPortail(int x, int y, int *xDest, int *yDest);

// position des pommes
int lesPommesX[NB_POMMES] ={75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] ={8, 39, 2, 2, 5, 39, 33, 38, 35, 2};

int main(){

	// 2 tableaux contenant les positions des éléments qui constituent le serpent
	int lesX[TAILLE];
	int lesY[TAILLE];

	int xDest;
	int yDest;

	// nombre de déplacements
	int nbDeplacements = 0;

	// représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
	char touche;

	// direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
	char direction;

	// le plateau de jeu
	tPlateau lePlateau;

	bool collision = false;
	bool gagne = false;
	bool pommeMangee = false;
	bool tunnel = false;

	// compteur de pommes mangées
	int pommeIndicateur = 0;

	// initialisation de la position du serpent : positionnement de la
	// tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche
	for (int i = 0; i < TAILLE; i++){
		lesX[i] = X_INITIAL - i;
		lesY[i] = Y_INITIAL;
	}

	// mise en place du plateau
	initPlateau(lePlateau);
	system("clear");
	dessinerPlateau(lePlateau);

	srand(time(NULL));
	ajouterPomme(lePlateau, pommeIndicateur);

	// initialisation : le serpent se dirige vers la DROITE
	dessinerSerpent(lesX, lesY);
	disable_echo();
	direction = DROITE;
	touche = DROITE;

	PassageTunnel(lesX[0], lesY[0], lesPommesX[pommeIndicateur], lesPommesY[pommeIndicateur], &xDest, &yDest);

	// début du calcul du temps CPU
	clock_t begin = clock();

	// boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
	// si toutes les pommes sont mangées
	do{

		direction = calculerDirectionVersDestination(lesX, lesY, xDest, yDest, lePlateau);

		progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee, &tunnel);

		if (tunnel){
			xDest = lesPommesX[pommeIndicateur];
			yDest = lesPommesY[pommeIndicateur];
			tunnel = false;
		}

		nbDeplacements++;

		if (pommeMangee){
			pommeIndicateur++;
			gagne = (pommeIndicateur == NB_POMMES);

			if (!gagne){
				ajouterPomme(lePlateau, pommeIndicateur);
				pommeMangee = false;
				PassageTunnel(lesX[0], lesY[0], lesPommesX[pommeIndicateur], lesPommesY[pommeIndicateur], &xDest, &yDest);
			}
		}

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

	// affiche le temps CPU
	clock_t end = clock();
	double tmpsCPU = ((end - begin) * 1.0) / CLOCKS_PER_SEC;
	printf("Temps CPU = %.3f secondes\n", tmpsCPU);

	// affiche le nombre de déplacements
	printf("Vous avez fait %d déplacements\n", nbDeplacements);

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

void dessinerSerpent(int lesX[], int lesY[]){

	// affiche les anneaux puis la tête
	for (int i = 1; i < TAILLE; i++){
		afficher(lesX[i], lesY[i], CORPS);
	}

	afficher(lesX[0], lesY[0], TETE);
}

void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme, bool *tunnel){

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
	else if (plateau[lesX[0]][lesY[0]] == BORDURE){
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

	dessinerSerpent(lesX, lesY);
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

char calculerDirectionVersDestination(int lesX[TAILLE], int lesY[TAILLE], int xDest, int yDest, tPlateau t){

	char direction;
	int xPortalDest, yPortalDest;

	if (lesY[0] < yDest && peutAllerDansDirection(BAS, lesX, lesY, t)){
		direction = BAS; //Si le serpent peut aller en bas, choisir la direction BAS
	}

	else if (lesY[0] > yDest && peutAllerDansDirection(HAUT, lesX, lesY, t)){
		direction = HAUT; //Si le serpent peut aller en haut, choisir la direction HAUT
	}

	else if (lesX[0] < xDest && peutAllerDansDirection(DROITE, lesX, lesY, t)){
		direction = DROITE; //Si le serpent peut aller à droite, choisir la direction DROITE
	}

	else if (lesX[0] > xDest && peutAllerDansDirection(GAUCHE, lesX, lesY, t)){
		direction = GAUCHE; //Si le serpent peut aller à gauche, choisir la direction GAUCHE
	}

	//Si aucune direction directe n'est possible, on teste les directions restantes jusqu'à ce que l'une soit valide
	else{

		if (peutAllerDansDirection(BAS, lesX, lesY, t)){
			direction = BAS;
		}

		else if (peutAllerDansDirection(GAUCHE, lesX, lesY, t)){
			direction = GAUCHE;
		}

		else if (peutAllerDansDirection(HAUT, lesX, lesY, t)){
			direction = HAUT;
		}

		else if (peutAllerDansDirection(DROITE, lesX, lesY, t)){
			direction = DROITE;
		}
	}

	// Vérifier si un portail est accessible dans la direction choisie
	if (direction == BAS && estPortail(lesX[0], lesY[0] + 1, t, &xPortalDest, &yPortalDest)){

		if (xPortalDest == xDest && yPortalDest == yDest){
			return BAS; // Prendre le portail
		}
	}

	else if (direction == HAUT && estPortail(lesX[0], lesY[0] - 1, t, &xPortalDest, &yPortalDest)){

		if (xPortalDest == xDest && yPortalDest == yDest){
			return HAUT; // Prendre le portail
		}
	}

	else if (direction == DROITE && estPortail(lesX[0] + 1, lesY[0], t, &xPortalDest, &yPortalDest)){

		if (xPortalDest == xDest && yPortalDest == yDest){
			return DROITE; // Prendre le portail
		}
	}

	else if (direction == GAUCHE && estPortail(lesX[0] - 1, lesY[0], t, &xPortalDest, &yPortalDest)){

		if (xPortalDest == xDest && yPortalDest == yDest){
			return GAUCHE; // Prendre le portail
		}
	}

	return direction;
}

int snakeCollision(int direction, int lesX[TAILLE], int lesY[TAILLE]){

	int yHead = lesY[0];
	int xHead = lesX[0];

	int collision = 0;

	int index = 1;

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
		if ((xHead == lesX[index]) && (yHead == lesY[index])){
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
bool peutAllerDansDirection(char direction, int lesX[TAILLE], int lesY[TAILLE], tPlateau t){

	//Vérifie si la case en dessous de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision avec le serpent dans cette direction.
	if (direction == BAS){
		return (t[lesX[0]][lesY[0] + 1] != BORDURE && !snakeCollision(BAS, lesX, lesY));
	}

	//Vérifie si la case au-dessus de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == HAUT){
		return (t[lesX[0]][lesY[0] - 1] != BORDURE && !snakeCollision(HAUT, lesX, lesY));
	}

	//Vérifie si la case à droite de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == DROITE){
		return (t[lesX[0] + 1][lesY[0]] != BORDURE && !snakeCollision(DROITE, lesX, lesY));
	}

	//Vérifie si la case à gauche de la tête du serpent n'est pas une BORDURE et s'il n'y a pas de collision.
	else if (direction == GAUCHE){
		return (t[lesX[0] - 1][lesY[0]] != BORDURE && !snakeCollision(GAUCHE, lesX, lesY));
	}

	return false;
}

// Fonction pour trouver le portail à une position
bool getDestinationPortail(int x, int y, int *xDest, int *yDest){

	// On Parcourt tous les portails du tableau en constante PORTE_TAB
	for (int i = 0; i < NB_PORTE; i++){

		//Si la postion correspond à un portail alors on récupère les coordonnées de destination
		if (PORTE_TAB[i][0] == x && PORTE_TAB[i][1] == y){
			*xDest = PORTE_TAB[i][2];
			*yDest = PORTE_TAB[i][3];
			return true;
		}
	}

	return false; // Ce n'est pas un portail
}

//Fonction qui vérifie si la valeur à la position (x, y) sur le plateau est égale à ISSUE.
bool estPortail(int x, int y, tPlateau t, int *xDest, int *yDest){
	
	//Si oui, cela signifie que c'est un portail.
	if (t[x][y] == ISSUE){
		return getDestinationPortail(x, y, xDest, yDest);
	}

	return false;
}