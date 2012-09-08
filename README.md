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
    description: Images d'animaux. Exceptions : animaux qui rampent.
    items: *.png feuilledechene2.jpeg
    exceptions: Escargot.png feuilledechene2.jpeg
    encoding: latin1 # or utf-8

Les valeurs pour 'encoding' sont celles supportées par QT (http://doc.qt.nokia.com/4.7-snapshot/qtextcodec.html)

