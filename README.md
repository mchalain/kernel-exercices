module.1 :
--------
Création d'un module simple avec initialisation et déchargement du module.

module.2 :
--------
Création de 2 modules. mydriver2 appelle une fonction de mydriver1.  
Utilisation de EXPORT_SYMBOL_GPL.

module.2.1 :
----------
Montrer l'importance de la license avec EXPORT_SYMBOL_GPL et MODULE_LICENSE.

chardriver.0 :
------------
Création d'un pilote caractère simple avec les fonctions "open", "read", "write" et "release".  
Faire le parallèle avec les appels systèmes "open", "read", "write" et "close" de l'espace utilisateur.
Faire le lien entre le FS de l'espace utilisateur et le VFS du kernel, avec les notions de majeur et de mineur.  
Montrer la création d'un node sur le FS. Parler du /dev statique

chardriver.1 :
------------
Enregistrement du pilote en tant que device et montrer le résultat dans /sys/class/mydrivers.  
Montrer la création de /dev/mydriver et parler de udev.  
Parler des informations du module : /sys/module/mydriver1 modinfo lsmod
Montrer le problème d'un majeur fixe avec un majeur déjà utilisé. Puis montrer /proc/devices pour le choix du majeur.

chardriver.2 :
------------
Création de plusieurs nodes pour un seul pilote.  
Montrer l'utilisation du mineur dans la fonction "open" pour connaitre le fichier qui a été ouvert. Puis retrouver le fichier ouvert dans "read".

chardriver.3 :
------------
Génération automatique du majeur du pilote par le kernel.  

chardriver.4 :
------------
Passage d'informations entre "open", "read",...  
Parler de la structure "inode" et "file", comme étant une description
du fichier dans le FS du disque dure et dans le VFS.  
Parler des besoins des drivers sur le stockage d'information et la différenciation ente les fichiers.

chardriver.5 :
------------
Version finale de chardriver avec allocation de données.
Parler de l'allocation mémoire dynamique. (*note* :j'ai comme affirmation
qu'il était préferrable de faire de l'allocation statique, parler de l'allocation
statique pour tous les drivers de Linux et de la place occupée,
parler de PreemptRT de l'allocation dynamique seulement au chargement du pilote).

poo.1 :
------
Génération d'une classe abstraite de pilote.  
Parler de la programmation orienté objet de Linux, des 3 couches d'interface
(liaison avec le matériel, liaison entre les modules, liaison avec l'espace utilisateur).  
Rappeler l'exportation de symbole. Parler de la table des symboles,
de la création de liens lors du chargement du pilote. Expliquer l'intéret
de la POO pour minimiser la taille de la table de symboles.

userspace.1 :
-----------
Passage de données entre le noyau et l'espace utilisateur avec "read" et "write".  
Expliquer la gestion de la mémoire entre l'espace utilisateur et le noyau.  
Selon parler des modes "supervisor" et "user" du CPU, des appels systèmes et de
l'intéruption pour passer d'un mode à l'autre.

userspace.2 :
-----------
Utilisation de ioctl pour dialoguer avec le pilote.
Parler des autres fonctions d'accès au pilote ("ioctl" "mmap" "poll") qui existent.  
Parler de la génération des _IO du "magic" et des directions de passage d'arguments.

gpio.1 :
-------
Génération d'un pilote pour Raspberry.  
Montrer la compilation sur target mais aussi la compilation croisée.  
Montrer la modification de KDIR à l'appel de Makefile.  
Parler de nouveau de la gestion de la mémoire, des registres du SOC et de la MMU.  
Expliquer "ioremap", l'utilisation de l'adresse retournée directement ou avec les fonctions "readl" et "writel".

gpio.2 :
------
Réutilisation de userspace.2 pour le passage d'arguments.
