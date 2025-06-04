from math import *
from random import *
import matplotlib.pyplot as plt


def genereGroupe(nbPoint, ecartMaxPoint,dim):
    # premier point de référence au centre 
    premierPoint = tuple(randint(-30, 30) for _ in range(dim))

    #génère un groupe avec un espace maximum entre les points et le centre = à ecartMaxPoint dans une dimension dim
    for j in range(len(premierPoint)):    
        points = [tuple(premierPoint[j] + randint(-ecartMaxPoint,ecartMaxPoint) for _ in range(dim)) for _ in range(nbPoint)]
    return points


# Distance de Manhattan (p = 1)
def calculDistanceManhattan(pointA, pointB):
    return sum(abs(a - b) for a, b in zip(pointA, pointB))

# Distance Euclidienne (p = 2)
def calculDistanceEuclidienne(pointA, pointB):
    return round(sqrt(sum((a - b) ** 2 for a, b in zip(pointA, pointB))), 2)

# Distance de Chebyshev (p → ∞)
def calculDistanceChebyshev(pointA, pointB):
    return max(abs(a - b) for a, b in zip(pointA, pointB))

def classification(lst):
    """
    classification simple à l'aide du mode (la valeur la plus fréquente)
    """
    rep={}
    for val in lst:# Parcourt la liste pour compter les occurrences
        if val not in rep:
            rep[val]=1
        else:
            rep[val]+=1

    maxi=0
    eti_maxi=None
    # recherche le max
    for cle in rep:
        if rep[cle]>maxi:
            maxi=rep[cle]
            eti_maxi=cle
    return eti_maxi

def regression(lst):
    moyennes={}
    res={}
    for val in lst:
        if val[0] not in moyennes:
            moyennes[val[0]]=[val[1]]
        else:
            moyennes[val[0]].append(val[1])
    for cle in moyennes.keys():
        moy=0
        for val in moyennes[cle]:
            moy+=val
        moy = moy/len(moyennes[cle])
        res[cle]=moy

    mini = float('inf')
    clef = None

    for cle, val in res.items():
        if val < mini: 
            mini = val
            clef = cle

    return clef

def representation(l,nbEtiquette):

    for i in range(nbEtiquette):
        for x, y in donnee[l[i][0]]:
            plt.scatter(x, y, c=l[i][1], marker=l[i][2], label="List "+ l[i][0] if (x, y) == donnee[l[i][0]][0] else "")

    
    plt.scatter(pt_dep[0], pt_dep[1], c="green", marker="o", label="Random Point")

    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)
    plt.grid(color='gray', linestyle='--', linewidth=0.5)
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.legend()
    plt.title("Représentation 2D du KNN")
    plt.show()

def representation3D(l,nbEtiquette):

    fig = plt.figure()
    ax = fig.add_subplot(111,projection='3d')

    for i in range(nbEtiquette):
        for x, y, z in donnee[l[i][0]]:
            ax.scatter(x, y, z,  c=l[i][1], marker=l[i][2], label="List "+ l[i][0] if (x, y, z) == donnee[l[i][0]][0] else "")

    
    ax.scatter(pt_dep[0], pt_dep[1], pt_dep[2], c="green", marker="o", label="Random Point")

    ax.set_xlabel("X-axis")
    ax.set_ylabel("Y-axis")
    ax.set_zlabel("Z-axis")
    plt.title("Représentation 3D du KNN")
    plt.legend()
    plt.show()

def KNN(k,dim,pt_dep,donnee,l,nbEtiquette):

    distance = []
    kpremier = []
    etiquette = []

    for cle in donnee.keys():
            for data in donnee[cle]:
                if (dim == 1) :
                    distance.append((cle,calculDistanceManhattan(pt_dep,data)))
                elif (dim == 2) :
                    distance.append((cle,calculDistanceEuclidienne(pt_dep,data)))
                elif dim >= 3 :
                    distance.append((cle,calculDistanceChebyshev(pt_dep,data)))
    distance.sort(key=lambda x:x[1])

    for i in range(k):
        kpremier.append(distance[i])
        etiquette.append(kpremier[i][0])
        
        
    print("Pour faire une Régression -> entrez (R) ou une Classification -> entrez (C) ")
    choix = input()

    while (choix !="R" and choix !="C"):   

        print("Votre choix ne correspond pas aux propositions ")
        print("Pour faire une Régression -> entrez (R) ou une Classification -> entrez (C) ")
        choix = input()

    if(choix == "R"):
        res=regression(kpremier)
    elif(choix == "C"):
        res=classification(etiquette)

    if dim==2:
        representation(l,nbEtiquette)
    elif dim==3:
        representation3D(l,nbEtiquette)
    else:
        print("La dimenssion ne permet pas d'affiche une représentationd des points")

    resultat = ("Le point centrale", pt_dep, "appartient au groupe", res)

    return resultat
# -------------------------------------------------------------------------------------- #
# choix de la dimension
dim=2

#données pour le graph matplotlib
l= [("A","red","+"),
("B","blue","*"),
("C","orange","."),
("D","purple","^"),
("E","brown","1")]

#génération du point qu'on cherche a classer
pt_dep = tuple(randint(-30, 30) for _ in range(dim))


nbEtiquette = randint(2,5)
donnee={}
for i in range(nbEtiquette):
    donnee[l[i][0]]=genereGroupe(50,10,dim)
print(KNN(20,dim,pt_dep,donnee,l,nbEtiquette))