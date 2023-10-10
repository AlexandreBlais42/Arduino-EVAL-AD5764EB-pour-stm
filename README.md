#Module Quad DAC monté autour du *AD5764*

À utiliser avec la carte d’évaluation du DAC AD5764 monté dans le boîtier identifié au P/N de la carte, soit *EVAL-AD5764EB*. Cette carte est pilotée par un Arduino UNO. La résolution est de 0.3 mV.

## Utilisation

Pour l’utiliser, télécharger dans l’Arduino le programme sous le répertoire *embedded*. Par la suite, établir une connection sériel à 155200 baud. Pour commander une tension à l’une des sorties, transmettre la chaîne de caractères suivante:


\<ch\>\<valeur (4 charactères hexadécimal)\>


Où
ch: Le canal utilisé, soit A, B, C, D
valeur: La valeur 16bit en hexadécimal majuscule souhaité. Exemple : 7FFF pour 10V


Exemple:

A7B7B

*Attention*:
1- Le format de la commande est très stricte
2- Il reste beaucoup d’options potentiels à implémenter pour utiliser l’ensemble des possibilités du AD5764.

## Configurations

Les connections entre l’Arduino et la carte d’évaluation sont les suivantes:


| *Pins/fonctions du connecteur *J21* du EVAL-AD5764EB* | *Pins de l'Arduino* |
|-------------------------------------------------------|---------------------|
| 1 / ~SYNC                                             | 10                  |
| 2 / SCLK                                              | 13                  |
| 3 / SDIN                                              | 11                  |
| 4 / SDO                                               | 12                  |
| 5 / ~LDAC                                             | 9                   |
| 6 / ~CLR                                              | 8                   |
| 10 / ~RSTIN                                           | 7                   |
| 11 / BIN~2sCOMP                                       | 6                   |

Où le symbole ~ signifie active bas.


Les jumpers du EVAL-AD5764EB sont configurés de la façon suivante:

| *Jumpers* | *Configuration* |
|-----------|-----------------|
| LK1       | Position B      |
| LK2       | Position C      |
| LK3       | Inséré          |
| LK4       | Position B      |
| LK5       | ?               |
| LK6       | Inséré          |
| LK7       | Position B      |
| LK8       | Position A      |
| LK9       | Enlevé          |
| LK10      | Enlevé          |
| LK11      | Position A      |
