// declaration des librairies

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

//--------------------------------------------------------------------------------

// déclaration des constantes

const int TAILLE = 10;

const int VITESSE = 100000;

const int X = 20;
const int Y = 20;

const char CORPS = 'X';
const char TETE = 'O';

const char STOP = 'a';

// constante de direction
const char HAUT = 'z';
const char GAUCHE = 'q';
const char BAS = 's';
const char DROITE = 'd';

//--------------------------------------------------------------------------------

// déclaration des prototypes
void disableEcho();
void enableEcho();

int kbhit();

void gotoXY(int x, int y);

void afficher(int x, int y, char c);
void effacer(int x, int y);

void dessinerSerpent(int lesX[], int lesY[]);

void droite(int lesX[], int lesY[]);
void gauche(int lesX[], int lesY[]);
void haut(int lesX[], int lesY[]);
void bas(int lesX[], int lesY[]);

void progresser(int lesX[], int lesY[], char direction);

char fVerifTouche(char touche, char ancienneTouche);

int main()
{

   // déclaration des variables

   int lesX[TAILLE];
   int lesY[TAILLE];

   int indice;

   int x, y;

   char touche, verif_touche;

   touche = DROITE;

   x = X;
   y = Y;

   // initialisation des positions du corps du serpent dans les tableaux lesX et lesY
   for (indice = 0; indice < TAILLE; indice++)
   {

      lesX[indice] = x;
      x--;
      lesY[indice] = y;
   }

   // affichage des résultats

   system("clear");
   disableEcho();

   // boucler tant que l'utilisateur n'appuie pas sur 'a'
   while (touche != STOP)
   {

      // effacer la tete du serpent
      effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);
      // faire avancer le serpent
      progresser(lesX, lesY, touche);

      // si l'utilisateur appuie sur une touche
      if (kbhit() == 1)
      {

         // lire la touche appuyee
         verif_touche = getchar();
         // fonction qui verif la touche
         touche = fVerifTouche(verif_touche, touche);
      }

      usleep(VITESSE);
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
void dessinerSerpent(int lesX[], int lesY[])
{

   int indice;

   // dessiner le corps du serpent
   for (indice = 1; indice < TAILLE; indice++)
   {

      afficher(lesX[indice], lesY[indice], CORPS);
   }

   // dessiner la tete du serpent
   afficher(lesX[0], lesY[0], TETE);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers la Droite
void droite(int lesX[], int lesY[])
{

   int indice;

   for (indice = TAILLE - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesX[0] = lesX[0] + 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers la Gauche
void gauche(int lesX[], int lesY[])
{

   int indice;

   for (indice = TAILLE - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesX[0] = lesX[0] - 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers le Haut
void haut(int lesX[], int lesY[])
{

   int indice;

   for (indice = TAILLE - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesY[0] = lesY[0] - 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// Procedure qui permet d'aller vers le Bas
void bas(int lesX[], int lesY[])
{

   int indice;

   for (indice = TAILLE - 1; indice > 0; indice--)
   {

      lesX[indice] = lesX[indice - 1];
      lesY[indice] = lesY[indice - 1];
   }

   lesY[0] = lesY[0] + 1;

   dessinerSerpent(lesX, lesY);
}

//--------------------------------------------------------------------------------

// procedure pour faire avancer le serpent en fonction de la direction en appeler les procedures du dessus
void progresser(int lesX[], int lesY[], char direction)
{

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
}

//--------------------------------------------------------------------------------

// fonction qui test si la touche peut être utilisée ou non et renvoie la touche entrée si la touche respecte les consignes sinon elle renvoie l'ancienne
char fVerifTouche(char touche, char ancienneTouche)
{

   if ((touche == HAUT && ancienneTouche != BAS) || (touche == GAUCHE && ancienneTouche != DROITE) || (touche == BAS && ancienneTouche != HAUT) || (touche == DROITE && ancienneTouche != GAUCHE) || (touche == STOP))
   {

      return touche;
   }

   else
   {

      return ancienneTouche;
   }
}