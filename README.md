ji3
===

Jeu Inhibition (jeu de rééducation)

Installation
------------
* Dézipper, c'est installé!
* Le répertoire créé 'ji3' est le répertoire racine du jeu.
* Lancer l'exécutable ji3/ji3.exe pour lancer le jeu.

Personnalisation / Création d'un thème
--------------------------------------

Un thème est constitué de :
* un sous-répertoires du répertoire racine (appelons-le T)
* dans ce répertoire T :
    * un fichier avec suffixe .ji3c, qui décrit le thème
    * des fichiers images (.png, .jpg)
    * des fichiers sons (.wav)


Fichier .ji3c:

    description-inhibition: Cliquer sur tous, sauf les animaux qui volent.
    description-attention: Cliquer sur les animaux qui volent.
    items: *.png feuilledechene2.jpeg
    exceptions: Escargot.png feuilledechene2.jpeg
    encoding: latin1 # or utf-8

Les valeurs pour 'encoding' sont celles supportées par QT (http://doc.qt.nokia.com/4.7-snapshot/qtextcodec.html)


Calcul des scores
-----------------

Vitesse moyenne de click : temps moyen entre l'apparition de l'item et le click (en millisecondes). Si le joueur ne clique pas, ce n'est pas compté.

Note globale :
    nombre d'items standards sans faute
    + 3 x nombre d'exceptions sans faute 
    - nombre d'items standards avec faute
    - 3 x nombre d'exceptions avec faute
    x 100 / note maximale

Le note maximale étant :
    nombre d'items standards sans faute
    + 3 x nombre d'exceptions sans faute

(un item étant soit standard, soit exception)

Problème d'encodage des caractères (noms de fichiers)
-----------------------------------------------------

Il est possible de passer l'option --codec à ji3.exe :

    linux/ji3 --codec UTF-8
    linux/ji3 --codec latin1


Structure des fichiers .ji3u
----------------------------
Chaque ligne correspond à un résultat de jeu. Les champs sont séparés par des ;

date (ex: 2012-12-31 19:23:59)
theme (ex: Animaux/volants)
note globale (de 0 à 100)
vitesse de click (en millisecondes)
nombre de clicks corrects sur exceptions
nombre de clicks corrects sur items standards
avec/sans son
mode inhibition/attention/attention divisée
nombre d'exceptions
nombre d'items total
periode

