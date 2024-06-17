# Projet de Seam Carving

### 1) Objectifs du projet
L'objectif principal du projet était de proposer une application de Seam Carving, une technique de réduction de la taille d'images qui n'implique aucune déformation et se contente juste de retirer les pixels jugés les moins utiles. Tous les pixels de l'image sont attribuées une valeur d'énergie qui représente leur niveau de dispendabilité (plus l'énergie est haute, mois le pixel est dispensable). Cette énergie est ensuite propagée en partant d'un des côtés.

Par exemple, si l'on choisit comme côté de départ le haut de l'image, les pixels de la première rangée sont inchangés, et l'énergie propagée des pixels suivants est calculée comme l'énergie du pixel plus le minimum de l'énergie des pixels directement au-dessus, au-dessus à gauche et au-dessus à droite (voir schéma suivant) :

![image](https://github.com/aethiiis/SeamCarving/assets/137384414/e9aec378-b840-414e-ad99-46b75b59a52c)
(Source : wikipedia.org/wiki/Seam_carving)

Une fois cette procédure effectuée pour tous les pixels, il suffit d'inspecter la rangée la plus inférieure des pixels (dans le cas d'un départ du côté haut) pour trouver les pixels présentant les minimums énergétiques et de remonter le "fil" des pixels : on a trouvé le "fil" de pixel le moins important d'un point de vue énergétique de toute l'image (voir schéma suivant). On peut donc procéder à son effacement pour obtenir une image un pixel moins large. 

![image](https://github.com/aethiiis/SeamCarving/assets/137384414/efcbb4ba-6b73-4b25-8bea-39c4909afaba)
(Source : wikipedia.org/wiki/Seam_carving)

Le second objectif du projet était de proposer une interface graphique permettant de visionner les changements effectués sur l'image et de visualiser les calculs opérés. Cette interface graphique devait être accompagnée de quelques éléments, comme une glissière permettant de choisir le niveau de Seam Carving souhaité, un bouton sauvegarder et un boutton pour passer d'un Seam Carving horizontal à vertical et vice-versa.

Le dernier objectif du projet était de réussir à compiler le programme en WebAssembly afin qu'il puisse être hébergé sur un serveur web local.

### 2) Circonstances du projet

L'immense partie du temps consommé pour le projet l'a été pour l'algorithme de Seam Carving en lui-même, alors que l'interface graphique, bien qu'incomplète, s'est avérée bien moins chronophage. Durant la majeure partie du projet, j'ai essayé de travaillé avec les objets Qt pour l'algorithme de Seam Carving en lui-même, notamment QImage, mais il s'est avéré que passer par une bibliothèque comme opencv était beaucoup plus efficace, rapide et simple.

A partir de vendredi matin, je n'ai plus eu accès à un ordinateur capable de faire fonctionner Qt. En effet, ce projet a été réalisé sur un ordinateur prêté par le département Informatique que j'ai dû rendre vendredi pour les 1ère années en cours introductif. Bien que l'objectif principal ait été réalisé, je n'ai pas pu compléter les objectifs secondaires pour cette raison. Je vous serais reconnaissant de bien vouloir prendre en compte cette situation exceptionnelle.

### 3) Contenu du projet

- L'algorithme de Seam Carving est fonctionnel.
- L'affichage des 3 premières images réprésentatives des étapes de l'algorithme est fonctionnel.
- L'affichage de la dernère image qui est celle obtenue après avoir retirer les différents "fils" n'est pas encore fonctionnel car cette image est déformée pour remplir la totalité du quart de la fenêtre.
- Il manque les différents boutons et éléments graphiques pour paramétrer l'algorithme.
- La partie de compilation en WebAssembly n'a pas été traitée.

### 4) Comment utiliser le programme

1) Modifier la variable 'objectif' pour choisir le nombre de "fils" à retirer.
2) Modifier le booléen 'vertical' pour choisir si le Seam Carving doit être vertical ou horizontal.
3) Modifier le chemin de l'image à réduire (ligne 93) et le chemin d'enregistrement de l'image produite (ligne 391).
4) Exécuter.

### 5) Pistes d'améliorations

- Régler le problème d'affichage de l'image produite (court terme).
- Rajouter les éléments d'interface graphique, comme des boutons ou la glissière de niveau de Seam Carving (moyen terme).
- Compiler en WebAssembly, ce qui nécessite des modifications au fichier CMakeLists.txt à minima (long terme).
