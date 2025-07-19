# Projet - Comportement de policiers et voleurs dans un graphe

Ce projet a été réalisé durant ma deuxième année d'études universitaires.  
L’objectif était de concevoir les comportements de placement et de déplacement de policiers et de voleurs au sein d’un graphe. Il s’agissait de mettre en pratique les concepts algorithmiques abordés tout au long du semestre, à travers un projet combinant réflexion et analyse.

## Structure du projet

Le projet repose principalement sur trois fichiers :

- **algo.c** : contient les structures de données ainsi que les algorithmes de base nécessaires à notre implémentation.
- **server.py** : gère l’interface graphique et charge les différents plateaux de jeu (fichiers `.txt`) situés dans le dossier `test_file`.
- **game.c** : implémente les mécaniques de jeu, comme le placement et le déplacement des personnages, ainsi que la capture des voleurs.

## Évaluation des performances

Pour évaluer les performances du projet face à différents algorithmes, j’ai utilisé un script Shell nommé **perf.sh**.  
Ce script teste automatiquement l’ensemble des plateaux de jeu contre tous les adversaires disponibles dans le dossier `bin`, pour chaque rôle (gendarmes 👮‍♀️ et voleurs 🥷), et affiche en sortie un pourcentage moyen de réussite selon l’algorithme affronté.
