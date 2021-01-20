# RewriteFocuser_for_leonardo


## Pré-requis

* Installer VS code qui est l'outil de codage pour le firmware https://code.visualstudio.com/download
* Installer dans VS code le plugin platformio https://platformio.org/install/ide?install=vscode
* Ouvrir un compte EasyEDa pour réaliser les circuits (fihiers json) stockés dans le réperoire Schémas ou les rédessiner à partir des fichiers pdf

## Fichiers de conception du circuit imprimé.
* Ouvrir dans EasyEDA les fichiers json situés dans le répertoire Schémas.
* Les composants à acquérir sont dans le fichier : BOM_test2-2_2021-01-17.csv

## Compilation du firmware
* Ouvrez VSCode dans lequel vous avez installé l'extension platformIO
* clonez le répertoire Github and un répertoire de votre choix : git clone https://github.com/Patrick-81/RewriteFocuser_for_leonardo
* Entrez dans le repertoire RewriteFocuser_for_leonardo ou master_RewriteFocuser_for_leonardo
* Windows: double cliquez sur le fihier platformio.ini ==> le projet devrait se charger dans l'extension platformio de VSCode
* Linux : à partir de VSCode chargez le répertoire (Open Folder). Répondre par la négative à la question posée par la fenêtre popup.
* Linux & windows : 
	* compilez le code en cliquant sur la coche en bas à gauche de la page
	* Branchez votre arduino au port USB du PC, vérifiez le port USB sur lequel vous êtes connectés, si nécessaire modifiez la définition du port dans upload_port dans le fichier platformio.ini
	* téléversez le firmware vers l'arduino en utilsant la flèche "->" en bas à gauche de l'écran.
## c'est fini...
