# borobot2
Code for Borobot2

# BoRoBot - Pin list

* Capteurs ligne blanche
   * Avant   : A1
   * Arriere : A0
* Valeurs seuils des capteurs de ligne blanche
   * arrière sur le blanc,avant sur le noir
      * A0  < 60
      * A1 > 800 
   * arrière sur le noir,avant sur le blanc
      * A0  > 800
      * A1< 60

* Capteurs IR
  * Avant   : A2
  * Arriere : A3
* Valeurs deuils des capteur IR
  * Obstacle à  4  cm -> Valeur : 600
  * Obstacle à  40 cm -> Valeur : 60

* Moteur droite
  * Marche avant   : D11
  * Marche arrière : D12 
* Moteur gauche
  * Marche avant   : D9
  * Marche arrière : D10 

* Leds
  * Led verte : D8
  * Led rouge : D7 

* Servo
  * Volets : D4

* Boutons 
  * Bouton Jaune : D2
  * Bouton Rouge : D3
  * Bouton Noir : On/Off non connecter a l’arduino (coupe-circuit)

