# Notebook

Ce dossier recense les différentes recherches et expériences menés afin de trouver les bonnes pipelines.

## Lecture

Il est conseillé de lire dans l'ordre suivant


### `KMeans.ipynb`

Segmentation non-supervisé sur plusieurs features pour observer si les classes sont facilement ou non segmentable.

Résultat: des classes comme chondre de type I sont très facilement segmentable, mais les autres bcp moins. Peut être qu'en fixant nous mêmes les centroïdes cela peut améliorer les résultats.

### `Spectre5x5.ipynb`

Définit des représentant pour 5 classes (type I, type II, mesostase I, matrix et metals). On calcul un vecteur de caractéristiques comme étant la moyenne pour chaque élement des pixels autour sur une fenêtre 5x5. Ensuite pour chaque pixel de l'image on calcul la similarité cosinus avec les représentants, le pixel est ensuite associé à la classe avec la plus grande similarité.

Résultat: un peu mieux que KMeans, mais qu'un seul représentant par classe (besoin de plus pour avoir meilleur estimation) et certaine classe sont toujours mélangés. Peut être qu'en ajoutant plus de représentant et en ajoutant d'autres features on obtiendra des meilleurs résultats ?

### `Var5x5.ipynb`

On pousse l'analyse précedente, cette fois-ci on prends tous les élements et toutes les classes (type I, type II, ...). Pour chaque classe on extrait environ 8 à 10 points et ensuite on calcul la moyenne, la variance, la variance de l'intensité du gradient et la variance de l'angle du gradient.

Après quelques analyses on se rend compte que la moyenne est quasi suffisante pour discriminer et que les chondres de type II et la matrice sont très corrélées. On teste ensuite différentes combinaisons de features pour la segmentation et on trouve que la moyenne + les gradients marchent le mieux.

Par contre c'est encore un peu bruité, donc peut-être qu'un petit modèle ML pourrait mieux se débrouiller.

### `RandomForest.ipynb`

Ce notebook reprend la même liste de point que dans `Var5x5` et lance une random forest dessus. Les résultats sont globalement pareils. On va donc passer à plus de données et à d'autres modèles de ML.

### `Labels.ipynb`

Ce notebook génère les données d'entraînements et de tests (c'est un split spatial). Les données sont lues depuis des fichiers de labels qui ont était préalablement remplis à la main à l'aide de Ilastik un logiciel d'annotation.

### `Lazy.ipynb`

On entraîne plusieurs modèles de ML sur les labels précédement extrait. On trouve que RandomForest et dérivé marche pas trop mal. Avec une plus fine évaluation on se rend compte qu'il y a beaucoup de bruit pour Carbontes, Type II et Matrices. Probablement parce que overfit, pas assez de données et difficilement distinguable.

De plus les RF sont lentes, il seraient intéressant de passer sur du deep avec des modèles qui marche bien avec peu de données. On peut tenter des CNNs comme ça on à l'information de la localité.

### `PatchCNN.ipynb`

