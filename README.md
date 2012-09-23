ji3
===

Jeu Inhibition (jeu de rééducation)

Installation
------------

    * [Télécharger](http://fhoerni.free.fr/ji3/download.html)
    * Dézipper, c'est installé !
    * Lancer ji3.bat pour lancer le jeu.


Raccourcis clavier
-------------------
Sur l'écran de contrôle (le premier écran) :

    * Ctrl-Q pour quitter

Sur les autres (le déroulement et le graphe) :

    * q : abandonner et revenir à l'écran de contrôle
    * f : plein écran / réduire

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
    items: *.png feuilledechene2.jpg
    exceptions: Escargot.png feuilledechene2.jpg
    encoding: latin1 # or utf-8
    force-sound: on # 'on' or 'off'
    force-type: inhibition # inhibition or attention
    force-exception: 10 # %
    force-speed: 1000 # milliseconds
    force-number: 75


Les valeurs pour 'encoding' sont celles supportées par QT (http://doc.qt.nokia.com/4.7-snapshot/qtextcodec.html)


Calcul des scores
-----------------

Vitesse moyenne de click : temps moyen entre l'apparition de l'item et le click (en millisecondes). Si le joueur ne clique pas, ce n'est pas compté.

Note globale :

    (   nombre d'items standards sans faute
      + 3 × nombre d'exceptions sans faute 
      - nombre d'items standards avec faute
      - 3 × nombre d'exceptions avec faute
    ) × 100 / note maximale

Le note maximale étant :

    nombre d'items standards sans faute
    + 3 × nombre d'exceptions sans faute

(un item étant soit standard, soit exception)

Problème d'encodage des caractères (noms de fichiers)
-----------------------------------------------------

Il est possible de passer l'option --codec à ji3.exe (dans ji3.bat), afin de régler d'éventuels problèmes concernant les accents dans les noms des fichiers .ji3u :

    linux/ji3 --codec UTF-8
    linux/ji3 --codec latin1


Structure des fichiers .ji3u
----------------------------
Chaque ligne correspond à un résultat de jeu. Les champs sont séparés par des ;

* date (ex: 2012-12-31 19:23:59)
* theme (ex: Animaux/volants)
* note globale (de 0 à 100)
* vitesse de click (en millisecondes)
* nombre de clicks corrects sur exceptions
* nombre de clicks corrects sur items standards
* avec/sans son
* mode inhibition/attention/attention divisée
* nombre d'exceptions
* nombre d'items total
* periode

Organisation des fichiers
------------------------
    ji3/
    |-- DLL          (contient le programme)
    |-- Data         (contient les fichiers des personnes)
    `-- Themes
        |-- Couleurs
        |-- Instruments
        |-- Test
        `-- Autres...


Développeurs
------------

* Pré-requis :
    - SDL-1.2, SDL_mixer-1.2
    - Qt v4.7
* Cross-Compiler pour Windows (à partir d'un Linux)
    - copier qmake.conf -> /usr/share/qt4/mkspecs/mingw32-g++/qmake.conf
    - qmake -spec mingw32-g++ && make
* Compiler pour Linux
    - qmake && make

Licence 
-------
Le code source de ce programme est sous licence [GPL v3](http://fhoerni.free.fr/ji3/LICENSE), qui confère les libertés suivantes :

* liberté d'exécuter le logiciel, pour n'importe quel usage
* liberté d'étudier le fonctionnement d'un programme et de l'adapter à ses besoins
* liberté de redistribuer des copies
* liberté de faire bénéficier à la communauté des versions modifiées






