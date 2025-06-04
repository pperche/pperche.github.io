// declaration des librairies

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

//--------------------------------------------------------------------------------

// déclaration des constantes

#define TAILLE 10
#define TAILLE_MAX 20

#define VITESSE 100000

#define X 40
#define Y 20

#define CORPS 'X'
#define TETE 'O'

#define BORDURE '#'
#define PAVE '#'
#define ISSUE ' '
#define VIDE ' '
#define POMME '6'

#define NOMBRE_PAVE 4
#define NB_POMME_MAX 10

#define STOP 'a'

// constante de direction
#define HAUT 'z'
#define GAUCHE 'q'
#define BAS 's'
#define DROITE 'd'

#define LARGEUR 80
#define HAUTEUR 40

#define MIL_LARG (LARGEUR / 2)
#define MIL_HAUT (HAUTEUR / 2)

#define COTE_CARRE 5
#define COEF_VITESSE 0.9

// definition des tableaux pour le serpent et le plateau
typedef int t_serpent[TAILLE_MAX];
typedef char t_plateau[LARGEUR][HAUTEUR];

int tailleSerpent = TAILLE;
float vitesseSerpent = VITESSE;
int nbPommeMangee = 0;

//--------------------------------------------------------------------------------

// déclaration des prototypes
void disableEcho();
void enableEcho();

int kbhit();

void gotoXY(int x, int y);

void afficher(int x, int y, char c);
void effacer(int x, int y);

void dessinerSerpent(t_serpent lesX, t_serpent lesY);

// procedure de déplacement du serpent
void droite(t_serpent lesX, t_serpent lesY);
void gauche(t_serpent lesX, t_serpent lesY);
void haut(t_serpent lesX, t_serpent lesY);
void bas(t_serpent lesX, t_serpent lesY);

void progresser(t_serpent lesX, t_serpent lesY, char direction, bool *collision, t_plateau plateau, bool *manger);

char fVerifTouche(char touche, char ancienne_touche);

void initPlateau(t_plateau plateau);
void dessinerPlateau(t_plateau plateau);

int genererPosx(int min_x, int max_x);
int genererPosy(int min_y, int max_y);
bool dessinerCarre(int x, int y, t_plateau plateau);

void ajouterPomme(int *x, int *y, t_plateau plateau);
//--------------------------------------------------------------------------------

int main()
{

   // declaration des tableaux pour le serpent

   t_serpent lesX;
   t_serpent lesY;

   // declaration des variables pour le plateau

   t_plateau plateau;

   // déclaration des variables

   int indice;

   int x, y;

   char touche, verifTouche;

   bool collision;
   bool manger = false;
   // direction initiale du serpent

   touche = DROITE;

   x = X;
   y = Y;

   collision = false;

   srand(time(NULL));

   // initialisation des positions du corps du serpent dans les plateauleaux lesX et lesY
   for (indice = 0; indice < tailleSerpent; indice++)
   {

      lesX[indice] = x;
      x--;
      lesY[indice] = y;
   }

   disableEcho();

   // initialiser et dessiner le plateau
   initPlateau(plateau);
   dessinerPlateau(plateau);

   // boucler tant que l'utilisateur n'appuie pas sur 'a'
   while (touche != STOP && nbPommeMangee < NB_POMME_MAX)
   {

      // effacer la tete du serpent
      effacer(lesX[tailleSerpent - 1], lesY[tailleSerpent - 1]);
      // faire avancer le serpent
      progresser(lesX, lesY, touche, &collision, plateau, &manger);

      // si une collision est detectee
      if (collision == true)
      {

         touche = STOP;
      }

      // si l'utilisateur appuie sur une touche
      if (kbhit() == 1)
      {

         // lire la touche appuyee
         verifTouche = getchar();
         // fonction qui verif la touche
         touche = fVerifTouche(verifTouche, touche);
      }

      usleep(vitesseSerpent);
   }

   if (nbPommeMangee == NB_POMME_MAX)
   {

      printf("Vous avez gagné!\n");
   }

   else
   {

      printf("Vous avez perdu!\n");
   }

   enableEcho();
   return EXIT_SUCCESS;
}

//--------------------------------------------------------------------------------

void disableEcho()
{
   struct termios tty;

   // Obtenir les attributs du terminal
   if (tcgetattr(STDIN_FILENO, &tty) == -1)
   {
      perror("tcgetattr");
      exit(EXIT_FAILURE);
   }

   // Desactiver le flag ECHO
   tty.c_lflag &= ~ECHO;

   // Appliquer les nouvelles configurations
   if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
   {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
   }
}

//--------------------------------------------------------------------------------

void enableEcho()
{
   struct termios tty;

   // Obtenir les attributs du terminal
   if (tcgetattr(STDIN_FILENO, &tty) == -1)
   {
      perror("tcgetattr");
      exit(EXIT_FAILURE);
   }

   // Reactiver le flag ECHO
   tty.c_lflag |= ECHO;

   // Appliquer les nouvelles configurations
   if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
   {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
   }
}

//--------------------------------------------------------------------------------

int kbhit()
{
   // la fonction retourne :
   // 1 si un caractere est present
   // 0 si pas de caractere present

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

   if (ch != EOF)
   {
      ungetc(ch, stdin);
      unCaractere = 1;
   }
   return unCaractere;
}

//--------------------------------------------------------------------------------

// procedure pour afficher le curseur sur une position donnee
void gotoXY(int x, int y)
{

   printf("\033[%d;%df", y, x);
}

//--------------------------------------------------------------------------------

// procedure pour afficher un caractere sur une position donnee
void afficher(int x, int y, char c)
{

   // appel de la procedure gotoXY pour positionner le curseur
   gotoXY(x, y);
   printf("%c", c);
   gotoXY(1, 1);
}

//--------------------------------------------------------------------------------

// procedure pour effacer un caractere sur une position donnee
void effacer(int x, int y)
{

   // appel de la procedure gotoXY pour positionner le curseur
   gotoXY(x, y);
   printf(" ");
   gotoXY(1, 1);
}

//--------------------------------------------------------------------------------

// procedure pour dessiner le serpent sur l'ecran
void dessinerSerpent(t_serpent lesX, t_serpent lesY)
{

   int indice;

   // dessiner la tete du serpent
   afficher(lesX[0], lesY[0], TETE);

   // dessiner le corps du serpent
   for (indice = 1; indice < tailleSerpent; indice++)
   {

      afficher(lesX[indice], lesY[indice], CORPS);
   }
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers la Droite
void droite(t_serpent lesX, t_serpent lesY)
{

   int indice;

   for (indice = tailleSerpent - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesX[0] = lesX[0] + 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers la Gauche
void gauche(t_serpent lesX, t_serpent lesY)
{

   int indice;

   for (indice = tailleSerpent - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesX[0] = lesX[0] - 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers le Haut
void haut(t_serpent lesX, t_serpent lesY)
{

   int indice;

   for (indice = tailleSerpent - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesY[0] = lesY[0] - 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers le Bas
void bas(t_serpent lesX, t_serpent lesY)
{

   int indice;

   for (indice = tailleSerpent - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesY[0] = lesY[0] + 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// procedure pour faire avancer le serpent en fonction de la direction en appeler les procedures du dessus
void progresser(t_serpent lesX, t_serpent lesY, char direction, bool *collision, t_plateau plateau, bool *manger)
{

   int x_pomme, y_pomme;

   if (direction == DROITE)
   {

      droite(lesX, lesY);
   }

   else if (direction == GAUCHE)
   {

      gauche(lesX, lesY);
   }

   else if (direction == HAUT)
   {

      haut(lesX, lesY);
   }

   else if (direction == BAS)
   {

      bas(lesX, lesY);
   }

   // si il prend l'issue haut il apparait à l'issue du bas

   if (lesX[0] == MIL_LARG - 1 && lesY[0] == 0)
   {

      effacer(lesX[0], lesY[0]);
      lesX[0] = MIL_LARG - 1;
      lesY[0] = HAUTEUR;
   }

   // si il prend l'issue bas il apparait à l'issue du haut

   else if (lesX[0] == MIL_LARG - 1 && lesY[0] == HAUTEUR +1)
   {

      effacer(lesX[0], lesY[0]);
      lesX[0] = MIL_LARG - 1;
      lesY[0] = 1;
   }

   // si il prend l'issue droite il apparait à l'issue de gauche

   else if (lesX[0] == LARGEUR && lesY[0] == MIL_HAUT)
   {

      effacer(lesX[0], lesY[0]);
      lesX[0] = 0;
      lesY[0] = MIL_HAUT;
   }

   // si il prend l'issue gauche il apparait à l'issue de droite

   else if (lesX[0] == 0 && lesY[0] == MIL_HAUT)
   {

      effacer(lesX[0], lesY[0]);
      lesX[0] = LARGEUR - 1;
      lesY[0] = MIL_HAUT;
   }

   
   // test si le serpent touche le bordure
   if (plateau[lesX[0] - 1][lesY[0] - 1] == BORDURE)
   {

      *collision = true;
   }

   // test si le serpent touche une partie du corps
   for (int i = 1; i < tailleSerpent; i++)
   {

      if ((lesX[0] == lesX[i]) && (lesY[0] == lesY[i]))
      {

         *collision = true;
      }
   }


   if (plateau[lesX[0] - 1][lesY[0] - 1] == POMME)
   {

      *manger = true;
      plateau[lesX[0] - 1][lesY[0] - 1] = VIDE;
      ajouterPomme(&x_pomme, &y_pomme, plateau);
      tailleSerpent++;
      vitesseSerpent = vitesseSerpent * COEF_VITESSE;
      nbPommeMangee ++;
   }
}

//--------------------------------------------------------------------------------

// fonction qui test si la touche peut être utilisée ou non et renvoie la touche entrée si la touche respecte les consignes sinon elle renvoie l'ancienne
char fVerifTouche(char touche, char ancienne_touche)
{

   if ((touche == HAUT && ancienne_touche != BAS) || (touche == GAUCHE && ancienne_touche != DROITE) || (touche == BAS && ancienne_touche != HAUT) || (touche == DROITE && ancienne_touche != GAUCHE) || (touche == STOP))
   {

      return touche;
   }

   else
   {

      return ancienne_touche;
   }
}

//--------------------------------------------------------------------------------

void initPlateau(t_plateau plateau)
{

   int col, row, x, y, x_pomme, y_pomme;

   system("clear");

   // initialisation du plateau de vide
   for (col = 1; col < LARGEUR; col++)
   {

      for (row = 1; row < HAUTEUR; row++)
      {

         plateau[col][row] = VIDE;
      }
   }

   // initialisation des bordures
   for (row = 1; row < HAUTEUR; row++)
   {

      plateau[0][row] = BORDURE;
      plateau[LARGEUR - 1][row] = BORDURE;
   }

   for (col = 1; col < LARGEUR; col++)
   {

      plateau[col][0] = BORDURE;
      plateau[col][HAUTEUR - 1] = BORDURE;
   }

   // issue du haut
   plateau[MIL_LARG - 2][0] = ISSUE;

   // issue du bas
   plateau[MIL_LARG - 2][HAUTEUR - 1] = ISSUE;

   // issue de gauche
   plateau[0][MIL_HAUT - 1] = ISSUE;

   // issue de droite
   plateau[LARGEUR - 1][MIL_HAUT - 1] = ISSUE;

   for (int i = 0; i < NOMBRE_PAVE; i++)
   {

      x = genererPosx(COTE_CARRE, LARGEUR - (COTE_CARRE + 1));
      y = genererPosy(COTE_CARRE, HAUTEUR - (COTE_CARRE + 1));

      while ((x < (X + (COTE_CARRE - 1)) && x > X - (TAILLE + (COTE_CARRE + 1))) && (y > Y - (COTE_CARRE + 2) && y < Y + (COTE_CARRE - 2))) // Creer une zone pour que le serpent ne puisse pas entrer dans une partie du corps du serpent
      {

         x = genererPosx(3, LARGEUR - (COTE_CARRE + 1)); // Genere une nouvelle position tant que le carre est dans une zone du serpent et qui permet de creer un carre qui ne colle pas les bordures
         y = genererPosy(3, HAUTEUR - (COTE_CARRE + 1));
      }

      dessinerCarre(x, y, plateau); // Dessine le carre sur le plateau
   }
   
   ajouterPomme(&x_pomme, &y_pomme, plateau); // Ajoute la première pomme sur le plateau

}

//--------------------------------------------------------------------------------

int genererPosx(int min_x, int max_x)
{

   int x;

   // génération d'une position aléatoire entre min_x et max_x inclusifs

   x = rand() % (max_x - min_x + 1) + min_x;

   return x;
}

//--------------------------------------------------------------------------------

int genererPosy(int min_y, int max_y)
{

   int y;

   // génération d'une position aléatoire entre min_y et max_y inclusifs

   y = rand() % (max_y - min_y + 1) + min_y;

   return y;
}

//--------------------------------------------------------------------------------

void dessinerPlateau(t_plateau plateau)
{

   int col, row;

   for (col = 0; col < LARGEUR; col++)
   {
      for (row = 0; row < HAUTEUR; row++)
      {

         afficher(col + 1, row + 1, plateau[col][row]);
      }

      printf("\n");
   }
}

//--------------------------------------------------------------------------------

bool dessinerCarre(int x, int y, t_plateau plateau)
{

   for (int i = 0; i < COTE_CARRE; i++)
   {

      for (int j = 0; j < COTE_CARRE; j++)
      {

         plateau[x + i - 1][y + j - 1] = PAVE; // Dessine le carre sur le plateau

         afficher(i + x, j + y, PAVE);
      }
      printf("\n");
   }

   return false;
}

//--------------------------------------------------------------------------------

void ajouterPomme(int *x, int *y, t_plateau plateau)
{

   *x = genererPosx(COTE_CARRE, LARGEUR - (COTE_CARRE + 1)); // Genere une nouvelle position
   *y = genererPosy(COTE_CARRE, HAUTEUR - (COTE_CARRE + 1)); // Genere une nouvelle position

   while (plateau[*x - 1][*y - 1] != VIDE) // Si la position est déjà occupée par un carre ou une bordure
   { 

      *x = genererPosx(COTE_CARRE, LARGEUR - (COTE_CARRE + 1)); // Genere une nouvelle position
      *y = genererPosy(COTE_CARRE, HAUTEUR - (COTE_CARRE + 1)); // Genere une nouvelle position
   }

   plateau[*x - 1][*y - 1] = POMME; // Dessine la pomme sur le plateau
   afficher(*x, *y, POMME);

}