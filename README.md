# Conception d'un module
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

# Les modules de périphériques en mode caractère

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

# Architecture orienté objet du kernel

poo.0 :
-----
Génération d'une classe abstraite de pilote.  
Parler de la programmation orienté objet de Linux, des 3 couches d'interface
(liaison avec le matériel, liaison entre les modules, liaison avec l'espace utilisateur).  
Rappeler l'exportation de symbole. Parler de la table des symboles,
de la création de liens lors du chargement du pilote. Expliquer l'intéret
de la POO pour minimiser la taille de la table de symboles.

poo.1 :
-----
Utilisation d'une classe pour la création d'un node dans /dev.
Parler de la réutilisation de la classe pour différents drivers, qui eux
n'auront pas à gérer toute la partie accès au "user space".
Informer qu'il est rare de fabriquer une classe pour l'accès en "user space",
en général cela a déjà été fait. Une exception "spi esclave" n'a pas d'abstraction,
pour son accès en "user space".

poo.2 :
------
Exploitation d'une classe pour minimiser le code du driver. Résultat d'exercice.

poo.misc :
--------
Présentation de la classe miscdevice. Elle remplace notre classe my_class.
Rappeler qu'une classe n'est pas spécifique à l'accès depuis le "user space",
mais elles perment de dialoguer avec tous les modules sans augmenter le nombre
de fonctions exportées.
Parler d'autres classe pouvant être utile "dmaengine", "usb", "pci"...

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
Parler de la génération des IO du "magic" et des directions de passage d'arguments.

# Accès au matériel
## Gestion de la mémoire
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

gpio.3 :
------
Utilisation de l'API "gpio".
Montrer que l'architecture de Linux n'est pas toujours homogène.
Ici il n'y a pas de concepte objet pour la gestion des GPIO.
Montrer la configuration des GPIO en entrée ou en sortie.
Montrer l'utilisation de ioctl avec des passages d'argument.
Parler des bonnes pratiques sur l'utilisation de ioctl.

gpio.4 :
------
Utilisation en entrée d'un GPIO, et utilisation de read au lieu de ioctl.
Montrer que l'utilisation de ioctl nécessite le développement d'une
application spécifique, alors qu'une utilisation basique d'un périphérique
en mode caractère, doit rester simple.

## Gestion des interruptions
irq.1 :
-----
Mise en place d'un handler sur l'IT d'un GPIO.

irq.2 :
-----
Mise en place d'une synchronisation entre l'interruption et le traitement.
Parler des moyens de synchronisation comme les mutex et semaphore. Ceux-ci
peuvent être problématiques en tant que synchro et ne doivent être utiliser
qu'en protection de bloc de code.
Montrer l'utilisation des wait_queue pour la synchronisation.
Montrer l'interaction de l'interruption et de la sortie de /dev/mydriver.
En effet Ctrl+C ne sort que lorsque une interruption est déclencher.

# Kernel multitache
irq.3 :
-----
Mise en place de protection de bloc de code avec spin_lock.
Montrer la protection de bloc de code avec les spin_lock.
Expliquer les différences entre spin_lock/raw_spin_lock/sem.
Parler de la préemption et du nouveau PreemptRT.

irq.4 :
-----
Mise en place d'un Bottom-Half pour le traitement en tasklet.
Montrer de la tache ksoftirq du kernel, des listes des tasklet et hi_tasklet.
Parler de la priorité de traitement des tasklet dans l'ordonnanceur.
Parler des traitements faits dans le Bottom-Half.

irq.5 :
-----
Passage du Bottom-Half en kthread indépendant.
Montrer les avantages des kthread et des tasklet.
Utiliser "top" pour montrer l'utilisation du CPU dans les 2 cas.
Reparler du PreemptRT et de l'avantage du kthread pour la préemption.

# Device Tree
platform.1 :
----------
Utilisation de l'API platform_device.
Montrer depuis poo.misc la séparation du code et de la configuration du driver.
Parler des différentes structures nécessaires:

 *    struct platform_device,
 *    struct platform_driver,
 *    struct device,

platform.2 :
----------
Mise en place de ressource d'un device.
Montrer la liste des ressources disponibles. Expliquer que MEM et IO
sont vérifiés au niveau de la valeur de "start" et une mauvaise valeur
peut provoquer une erreur.
Parler de l'allocation dynamique par device avec les fonctions devm_* qui
associent une zone mémoire ou une irq à une structure "device", puis seront
libérés lors de la destruction de cette dernière.

platform.3 :
----------
Transformation du module mydevice1.ko en overlay mydevice1.dtbo
Parler des informations utiles dans mydevice1.ko et dans le code assembleur.
Montrer que la partie ".text" est inutile d'où la génération d'un objet
data d'où le dtbo.
Parler de l'origine du format dts.
Faire le lien entre le nom du node dans le dts et le nom du driver. Le
nom du node et le "compatible" doivent être identique.
Faire attention que l'utilisation de vmalloc pour allouer miscdevice provoque
un crash.

platform.4 :
----------
Utilisation de la table compatibilité entre driver et device.
Parler que fait de devoir avoir un node de device ayant le même nom que
le driver ne permet pas d'avoir plusieurs devices utilisant le même driver
en même temps.
Montrer le nouveau nom du node dans les overlays.
Parler des rêgles de nommage dans les "compatible".
