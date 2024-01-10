# CodeQR
Écrivez votre propre générateur de code QR

Un code QR (abréviation de code à réponse rapide) est une sorte de code-barres bidimensionnel. Ils ont été inventés en 1994 par la société japonaise Denso Wave pour étiqueter les pièces automobiles. Comme les codes-barres, il s’agit d’une image optique lisible par machine qui peut contenir des informations.

Vous les avez probablement vus utilisés pour promouvoir des sites Web sur du matériel de marketing. Comme celui-ci :

Le défi - Construire un générateur de code QR
Dans ce défi, nous allons nous intéresser à la construction d’un générateur de code QR à partir de zéro. Évidemment, ce n’est pas quelque chose que nous ferions pour un système de production, car presque tous les langages de programmation ont une bibliothèque open source qui les générera - utilisez-la ! Cependant, en tant qu’outil d’apprentissage, ils sont assez intéressants, nous pouvons analyser certaines entrées, les analyser, traiter des données binaires, le bit packing, la correction d’erreurs, des algorithmes pour dessiner l’image et calculer les modèles de pénalité et enfin rendre une image à l’écran ou dans un fichier.

Étape zéro
Comme un tableau C, nous sommes indexés à zéro et partons de 0 ! Pour cette étape, votre objectif est de configurer votre environnement pour qu’il puisse commencer à développer et à tester votre solution.

Choisissez l’IDE / l’éditeur de votre choix et le langage de programmation de votre choix.

Si vous voulez vraiment entrer dans l’écriture à partir d’une spécification, les codes QR sont définis dans la norme ISO/IEC 18004 :2015, que vous pouvez trouver sur les sites Web de la plupart des organismes de normalisation.

Étape 1
Dans cette étape, votre objectif est d’analyser les données à encoder pour déterminer comment elles peuvent être encodées. Les codes QR prennent en charge quatre modes d’encodage de texte : numérique, alphanumérique, octet et kanji.

Le mode numérique est pour les chiffres de 0 à 9. Le mode alphanumérique est destiné aux chiffres décimaux de 0 à 9, ainsi qu’aux lettres majuscules et aux symboles $, %, *, +, -, ., /, :, et l’espace. Le mode octet est destiné aux caractères du jeu de caractères ISO-8859-1.

Le mode Kanji est destiné aux caractères codés sur deux octets du jeu de caractères Shift JIS.

Pour déterminer le mode, examinez les données d’entrée et voyez quel est le mode le plus simple qui peut l’encoder. Mon approche consiste à utiliser le développement piloté par les tests (TDD). Je définis un test pour chaque mode et quelques cas invalides puis j’écris une fonction pour déterminer le mode, en m’arrêtant lorsque tous les tests réussissent.

Quelle que soit la façon dont vous l’abordez, une fois que vous êtes satisfait, vous pouvez déterminer correctement le mode, passez à l’étape 2.

Étape 2
Dans cette étape, votre objectif est d’encoder les données sous la forme d’une chaîne de bits. Cet encodage comporte 7 étapes.

2.1 Sélectionnez le niveau de correction d’erreur

Il y a quatre niveaux possibles, les niveaux plus élevés offrent plus de correction d’erreur, mais se traduisent par un code QR plus grand.

Niveau de correction d’erreur	Capacité de correction d’erreurs
L	Récupère 7 % des données
M	Récupère 15 % des données
Q	Récupère 25 % des données
H	Recovers 30% of data
Step 2.2 Determine the Smallest QR Code Version

There are 40 QR Code versions, the first 1 is 21x21 pixels, and they increase in size by 4 pixels up to version 40 at 177x177 pixels. To determine the smallest QR Code version you can look it up in the ISO standard (pages 33-36).

For this challenge we can stick to version 4 and use the following data:

Niveau de correction d’erreur	Mode numérique	Mode alphanumérique	Mode octet	Kanji Mode
L	187	114	78	48
M	149	90	62	38
Q	111	67	46	28
H	82	50	34	21
2.3 Déterminer l’indicateur de mode

Le mode d’encodage est représenté par un indicateur de 4 bits comme suit :

Mode	Indicateur de mode
Numérique	0001
Alphanumérique	0010
Octet	0100
Kanji	1000
2.4 Déterminer l’indicateur de nombre de caractères

Il s’agit d’une chaîne de bits qui représente le nombre de caractères codés dans les données. Pour déterminer sa valeur, comptez le nombre de caractères dans les données d’entrée et convertissez-le en binaire. Ensuite, appuyez sur la chaîne avec la touche gauche si nécessaire pour atteindre le nombre de bits utilisés pour représenter le nombre de caractères pour la version et le mode sélectionnés.

Pour les versions 1 à 9, le nombre de bits est :

Mode	Nombre de bits
Numérique	10
Alphanumérique	9
Octet	8
Kanji	8
2.5 Encoder les données en utilisant le mode sélectionné

À l’aide des valeurs déterminées précédemment, construisez la chaîne de bits qui représente le mode, la longueur et les données. Pour les données qui seraient :HELLO CC WORLD

Mode: 0010

Nombre de caractères : 000001110

Données: 01100001011011110001101000101110001000101000110011101001000101001101110111110

2.6 Tamponner au besoin

Une fois l’étape précédente terminée, vous devez déterminer la chaîne de bits complète à utiliser. Il s’agit de la combinaison du mode, du nombre de caractères et des chaînes de bits de données. La spécification exige que la chaîne de bits remplisse toute la capacité du code QR, vous devrez donc remplir la chaîne de bits si ce n’est pas le cas.

Pour connaître la capacité, vous devez vous référer au tableau 9 à la page 38 de la spécification ISO. Pour la version 4 c’est :

Niveau de correction d’erreur	Nombre total de mots de code de données pour cette version et niveau EC
L	80
M	64
Q	48
H	36
Multipliez le nombre de mots de code par 8 pour obtenir le nombre de bits. Si votre chaîne de bits est plus courte, ajoutez-la en tant que terminaison. S’il n’y a pas assez de place, raccourcissez la terminaison pour qu’elle s’adapte.0000

Après l’ajout de la terminaison, d’autres zéros de remplissage doivent être ajoutés si la chaîne de bits ne se termine pas par un octet complet. Après cela, si les données sont toujours plus courtes que la capacité de données du code QR, elles doivent être remplies avec l’alternance d’octets et de .0xEC0x11

Étape 3
Dans cette étape, votre objectif est d’implémenter la correction d’erreur. Ceci est basé sur la correction d’erreur de Reed-Solomon et est couvert dans la spécification ISO.

Pour implémenter complètement la correction d’erreur, vous devez diviser le message en groupes 1 et 2, ainsi qu’en blocs au sein du groupe. Pour la version 4, il n’y a pas de groupe 2 et le nombre de blocs est le suivant :

Niveau de correction d’erreur	Nombre de blocs de correction d’erreur	Mots de code de correction d’erreur par bloc	Mots de code de données par bloc
L	1	20	80
M	2	18	32
Q	2	26	29
H	4	16	9
Ainsi, au niveau de correction d’erreur L, l’ensemble du message serait dans un bloc, pour M, le message serait divisé en deux blocs de 32 mots de code de données.

Ensuite, il suffit 😇 de calculer les mots de code de correction d’erreur pour chaque bloc et de structurer le message avec eux. Le détail de cette étape est trop long pour que je puisse l’intégrer, il est décrit dans les sections 7.5.2 et 7.6 de la spécification - à partir de la page 44.

Il y a aussi un excellent tutoriel ici.

Étape 4
Dans cette étape, votre objectif est de commencer à créer l’image du code QR. Il y a plusieurs étapes, définies dans la spécification à la section 7.7 (page 46) et suivantes. Essentiellement, les étapes sont les suivantes :

Créez un tableau de la bonne taille, c’est-à-dire que pour la version 1, il serait de 21x21.
Placez les motifs de recherche en haut à gauche, en haut à droite et en bas à gauche.
Ajoutez les séparateurs autour d’eux.
Ajoutez le motif d’alignement.
Ajoutez le modèle de synchronisation.
Ajoutez le module sombre.
Réservez les zones d’informations sur le format et la version.
Place the data bits, skipping the areas affected by steps 1-7 of this list.
Again there’s an excellent tutorial here.

Step 5
In this step your goal is to determine which data mask should be applied. The data mask changes which entries in the array are dark and which are light. The purpose of this is to modify the QR code to make it as easy for a QR code reader to scan as possible.

Applying the data mask is described in the specification Section 7.8 on page 50. You can also study this tutorial.

Step 6
In this step your goal is to create the format and version information encoding and place them in the correct location. This is detailed in section 7.9 and 7.10 of the specification. It’s also explained in this tutorial.

Step 7
In this step your goal is to render the QR Code, adding the quiet zone (section 9.1 of the specification) as an image and save it to file.

Essentially for this step you want add a quiet zone of 4 array elements all around the data array, these would all be set to zero. When you render the QR code, elements in the array that are set to 1 will be rendered as black squares and zeros as white squares.

How large you make each square when you render it will depend on your target image size.

As a final test, see if you can reproduce this QR code:

