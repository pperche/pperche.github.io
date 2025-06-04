import numpy as np
import matplotlib.pyplot as plt
from math import sqrt
from random import randint

def genereGroupe(nbPoint, ecartMaxPoint):
    '''
    génère un groupe de points aléatoires autour d'un point initial dans un espace 2D.
    '''
    premierPoint = randint(-30, 30), randint(-30, 30)
    points = [(premierPoint[0] + randint(-ecartMaxPoint, ecartMaxPoint),
               premierPoint[1] + randint(-ecartMaxPoint, ecartMaxPoint))
              for i in range(nbPoint)]
    return points

def calculDistanceEuclidienne(pointA, pointB):
    return round(sqrt((pointA[0] - pointB[0]) ** 2 + (pointA[1] - pointB[1]) ** 2), 2)

def trouve_centre_point(distances_totales):
    rep=[]
    for i in range(len(distances_totales)):
        x=0
        y=0
        c=0
        if (len(distances_totales[i]) !=0):
            for j in range(len(distances_totales[i])):
                x+=distances_totales[i][j][0][0]
                y+=distances_totales[i][j][0][1]
                c+=1
        if(x !=0 and y != 0):
            x=round(x/c,2)
            y=round(y/c,2)
            rep.append((x,y))
        
    return rep

def KMeans(k):
    points = []
    groupe = {}
    # on génère entre 3 et 10 groupes
    for i in range(1, randint(3, 10)):
        nbPoint = randint(2, 25)
        groupe[f"g{i}"] = genereGroupe(nbPoint, 5)
        points.extend(groupe[f"g{i}"])
    
    n_clusters = randint(2, 10)
    centroids = [points[i] for i in np.random.choice(len(points), n_clusters, replace=False)]

    #
    for i in range(k):
        distances_totales = [[] for i in range(len(centroids))]

        for point in points:
            distances = [calculDistanceEuclidienne(point, centroid) for centroid in centroids]
            min_index = distances.index(min(distances))
            distances_totales[min_index].append((point, min(distances)))
        
        new_centroids = trouve_centre_point(distances_totales)
        if new_centroids == centroids:
            break
        centroids = new_centroids
    
    return points, centroids, distances_totales

def afficher_clusters(k):
    points, centroids, distances_totales = KMeans(k)
    couleurs = ['blue', 'green', 'orange', 'purple', 'brown', 'pink', 'yellow', 'cyan', 'magenta', 'gray']
    plt.figure(figsize=(8, 6))

    for i, cluster_points in enumerate(distances_totales):
        cluster_points = [p[0] for p in cluster_points]  # Extraire les coordonnées
        if cluster_points:
            cluster_points = np.array(cluster_points)
            plt.scatter(cluster_points[:, 0], cluster_points[:, 1], color=couleurs[i % len(couleurs)], s=60, alpha=0.6, label=f'Cluster {i+1}')
    
    centroids = np.array(centroids)
    plt.scatter(centroids[:, 0], centroids[:, 1], color='black', marker='o', s=40, label='Centroïdes')
    plt.title('Représentation K-Means')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend(loc="upper left", bbox_to_anchor=(1, 1))
    plt.show()


k=10
afficher_clusters(k)