# Testbench-controller
Code and libraries used to control IPL's testbench. Architecture built around the Teensy 4.1 containing pressure, temperature, flow sensors and load cells, actuators and solenoid valves.
Currently focusing on builing the bricks that will compose the final code, to test segment by segment.

------------SOUS PROGRAMMES------------------

- Ethernet UDP communication: transmission et lecture de message, récuperation de paramètres par Ethernet fait 

- TVC: controller la position des vérins avec le potentiomètre numérique TPL0202
TODO
	- définir la position neutre des vérins (moteur à plat)
	- definir des listes de valeurs pour chaque pattern
	- creer une fonction qui reproduit les patterns
	- à integrer dans la fonction test à terme

- sensors: lire, vérifier, réagir et transmettre les valeurs des capteurs
TODO:
	- faire lire les valeurs aux fréquences choisies -- attendre de connaitre la frequence d'envoi
	- ajouter l'envoi des données par ethernet + construction de la byte frame
	- ajouter la sauvegarde des données sur la carte SD
	- ajouter la vérification des données reçues et l'envoi de warnings/errors et vent si necessaire

- emergency state : mettre le banc en sécurité tout en lisant les capteurs en cas d'erreur majeure

- valves: ajouter l'envoi de confirmations de reception de commande (Ethernet) fait

- pressurization: implementer une pressurisation bang-bang sur le réservoir choisi à une valeur donnée (automatiquement déterminer les marges)

- test: test (reception de message abort) et test preparation (vannes, vérins, pressurisation puis confirmation)
TODO:
	- maybe faire une fonction data differente lors des tests
	- pas besoin de prendre en consideration l'eau si test du battleship

- idle: lien entre les états et réponses aux commandes
hello
