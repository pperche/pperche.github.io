//declaration des librairies


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


//déclaration des constantes


const int TAILLE = 10;
const char CORPS = 'X';
const char TETE = 'O';
const char STOP = 'a';


int kbhit(){
   // la fonction retourne :
   // 1 si un caractere est present
   // 0 si pas de caractere present
   
   int unCaractere=0;
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

   if(ch != EOF){
      ungetc(ch, stdin);
      unCaractere=1;
   }
   return unCaractere;
}


//procedure pour afficher le curseur sur une position donnee
void gotoXY(int x, int y) {
   
      printf("\033[%d;%df", y, x);


   }


//procedure pour afficher un caractere sur une position donnee
void afficher(int x, int y, char c){


      //appel de la procedure gotoXY pour positionner le curseur
      gotoXY(x,y);
      printf("%c",c);
      gotoXY(1,1);

   }


//procedure pour effacer un caractere sur une position donnee
void effacer(int x, int y){


      //appel de la procedure gotoXY pour positionner le curseur
      gotoXY(x,y);
      printf(" ");
      gotoXY(1,1);

   }


//procedure pour dessiner le serpent sur l'ecran
void dessinerSerpent(int lesX[],int lesY[]){


   int indice;


   // dessiner le corps du serpent
   for ( indice = 1; indice < TAILLE; indice++ ){


      afficher(lesX[indice],lesY[indice],CORPS);


   }


   // dessiner la tete du serpent
   afficher(lesX[0],lesY[0],TETE);
   
}


//procedure pour faire avancer le serpent
void progresser(int lesX[],int lesY[]){


   int indice;
   
   //ajouter 1 au positionnement de chaque partie du corps du serpent
   for ( indice = 0; indice < TAILLE; indice++ ){


      lesX[indice] = lesX[indice]+1;
   }
   dessinerSerpent(lesX,lesY);


}


int main(){


   //déclaration des variables


   int lesX[TAILLE];
   int lesY[TAILLE];


   int indice;
   
   int x,y;


   char touche;


   x = 0;
   y = 0;


   touche = ' ';


   //test pour les positions de la tete du serpent pour assurer qu'il ne sort pas de l'ecran
   while ( ( (x < 1) || (x > 40) ) || ( (y < 1) || (y > 40) ) ) {


      if ( (x < 1) || (x > 40) ) {


         printf( "Saisissez une valeur pour pour la postion de la tete en x : \n" );
         scanf("%d",&x);


      }

      else {


         printf( "Saisissez une valeur pour pour la postion de la tete en y : \n" );
         scanf("%d",&y);


      }
   }


   //initialisation des positions du corps du serpent dans les tableaux lesX et lesY
   for ( indice = 0; indice < TAILLE; indice++ ){


      lesX[indice] = x;
      x--;
      lesY[indice] = y;
   }


   // affichage des résultats




   system("clear");

   //boucler tant que l'utilisateur n'appuie pas sur 'a'
   while (touche != STOP){



      //effacer la tete du serpent
      effacer(lesX[TAILLE-1],lesY[TAILLE-1]);
      //faire avancer le serpent
      progresser(lesX,lesY);

      //si l'utilisateur appuie sur une touche
      if (kbhit() == 1){
         
         //lire la touche appuyee
         touche = getchar();


      }

      usleep(1000000);
   }


   return EXIT_SUCCESS;




}
