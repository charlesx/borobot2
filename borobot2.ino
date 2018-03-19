// Variable Machine d'etat
#define DEBUG
//#define SLOW 

#define START 1
#define WAIT 2
#define FLAPS 3
#define BLINDATTACK 4
#define DETECTION 5
#define AVANCE 6
#define RECULE 7
#define TOURNE 8

int s_state, s_state_next, s_state_previous;

// Variable Gestion moteur
#define in1Pin 11  /* Moteur 1*/
#define in2Pin 12  /* Moteur 1*/
#define in3Pin 9  /* Moteur 2/Gauche*/
#define in4Pin 10 /* Moteur 2/Gauche*/

int moteur1[3] = {in1Pin, in2Pin},
    moteur2[3] = {in3Pin, in4Pin};


// Variable to calibrate IR capteur
#define capteurIRAvant A2
#define capteurIRArriere A3
int detection = 0; //1 capteur avant ; 2 capteur arriere ; 0 init calue
int status_detection; //flag de detection
// pointeur sur un entier pour acceder aux valeurs du tableau des valeurs de calibration
int *cal;


// LED
const int led1Pin = 7; //LED verte
const int led2Pin = 8; //LED rouge


// Bouton
const int buttonStartPin = 3;     // the number of the pushbutton pin à changer

// Attente
int attente =3000;

// Servo
// ne pas utiliser pin 9 et 10 pwm a cause du timer
// Import biblio gestion servo-moteur
#include <Servo.h>
//creation de l'objet servo
Servo servo;
const int servoPin=4; // VALEUR A VERIFIER


#define VOLETS_LOCKED 45
#define VOLET_OPENED 0


void setup() {  
  #ifdef DEBUG
    Serial.begin(9600 );
  #endif
  // On initialise la machine d'etat 
  s_state=START;

  //Inititalise les LED a OFF
  blinkled();
  delay(1000);
  digitalWrite(led1Pin, 0);
  digitalWrite(led2Pin, 0);

  // Met la LED de detection rouge a ON
  digitalWrite(led2Pin, 1);

  // Servo
  // Lien Pin / Servo
  servo.attach(servoPin);
  // On bloque les volets
  #ifdef DEBUG
  	Serial.println("SETUP : On bloque les volets");
  #endif
  servo.write(VOLETS_LOCKED);

}


void blinkled() {
  digitalWrite(led1Pin, 0);
  digitalWrite(led2Pin, 0);
  delay(1000);
  digitalWrite(led1Pin, 1);
  digitalWrite(led2Pin, 1);
}

void attenteAppuieBouton() {

    int buttonStartState=0;
    #ifdef DEBUG
      Serial.print("START - initialisation:  ");
      Serial.println(buttonStartState);
    #endif
    
    // detection pression bouton Start
    buttonStartState = digitalRead(buttonStartPin);
    //filtrée ici le rebond...
    #ifdef DEBUG
      Serial.print("START - Etat bouton:  ");
      Serial.println(buttonStartState);
    #endif


    if (buttonStartState == 1){
        #ifdef DEBUG
          Serial.println("START - Le bouton start a ete appuye");
        #endif
        s_state_next=WAIT;
    }
    else {
        #ifdef DEBUG
          Serial.println("START - Waiting for Start button to be pressed");
        #endif
        blinkled();
        s_state_next=START;
    }
  
  
}

int * lectureCapteur(){

    //tableau retournant les valeurs lues pour avant et arriere
    static int valeursLu[2];

    // Lecture capteur
    valeursLu[0] = analogRead(capteurIRAvant);
    valeursLu[1] = analogRead(capteurIRArriere);

    #ifdef DEBUG
      //Serial.print("Valeur Capteur Avant");
      //Serial.println("  Valeur Capteur Arriere");
      Serial.print("       ");
      Serial.print(valeursLu[0]);
      Serial.print("       ");
      Serial.println(valeursLu[1]);
    #endif
  
    return valeursLu;
}

int * valeurMoyCal() {
   
    int *valeurLu;
    static int valeurMoy[2];
    
    for(int z=0;z<10;z++){
      valeurLu=lectureCapteur();
      valeurMoy[0] += *(valeurLu);
      valeurMoy[1] += *(valeurLu + 1);
      delay(50);
      }
    valeurMoy[0] = valeurMoy[0] / 10;
    valeurMoy[1] = valeurMoy[1] / 10;

    return valeurMoy;
}

int detecter() {
    // On lit les valeurs sur les capteurs Avant et arriere
    //int seuil = 60; //Valeur a 40cm : 60
    int seuil[2];
    int maxarray=sizeof(seuil)/sizeof(*seuil); //pour avoir le nb elment ds le tableau
    for (int i=0;i<maxarray ;i++) {
      seuil[i]=cal[i];
    }
    /*// Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
    int *valeurLu;
    valeurLu=lectureCapteur();*/
    int *valeurMoy;
    valeurMoy=valeurMoyCal();

    detection = 0;
    
    #ifdef DEBUG
      Serial.print("Seuil1: ");
      Serial.print(*(seuil));
      Serial.print("       ");
      Serial.println(*(seuil+1));
      Serial.print("valeurMoy: ");
      Serial.print(*(valeurMoy));
      Serial.print("       ");
      Serial.println(*(valeurMoy+1));
    #endif
    if ( *(valeurMoy) > *(seuil)) {
        detection = 1;
        //Allume LED verte quand detection sinon allume led rouge
        digitalWrite(led1Pin, 1);
        // On etein la LED rouge
        digitalWrite(led1Pin, 0);
     }
     
     if ( *(valeurMoy + 1) > *(seuil+1)) {
        detection = 2;
        //Allume LED verte quand detection sinon allume led rouge
        digitalWrite(led1Pin, 1);
        // On etein la LED rouge
        digitalWrite(led1Pin, 0);
     }

    status_detection=1;
    return detection;
}

int * calibrationIR(){
    // Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
    int *valeurLu;
    static int CalibrationIR[2];

    #ifdef DEBUG
      Serial.print("WAIT - Valeur Capteur Avant");
      Serial.println("  Valeur Capteur Arriere");
    #endif
    for(int z=0;z<10;z++){
      valeurLu=lectureCapteur();
      CalibrationIR[0] += *(valeurLu);
      CalibrationIR[1] += *(valeurLu + 1);
      delay(100);
      }
    CalibrationIR[0] = CalibrationIR[0] / 10;
    CalibrationIR[1] = CalibrationIR[1] / 10;
    CalibrationIR[0] = CalibrationIR[0] + 20;
    CalibrationIR[1] = CalibrationIR[1] + 30;

    #ifdef DEBUG
      Serial.print("WAIT - Calibration capteur Avant  : ");
      Serial.println(CalibrationIR[0]);
      Serial.print("Calibration capteur Arriere: ");
      Serial.println(CalibrationIR[1]);
    #endif

    return CalibrationIR;
}

int * calibrationMaxIR(){
    // Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
    int *valeurLu;
    static int CalibrationIR[2];

    #ifdef DEBUG
      Serial.print("WAIT - Valeur Capteur Avant");
      Serial.println("  Valeur Capteur Arriere");
    #endif
    for(int z=0;z<10;z++){
      valeurLu=lectureCapteur();
      if (*(valeurLu) > CalibrationIR[0]){
        CalibrationIR[0] = *(valeurLu);
      }
      if (*(valeurLu + 1) > CalibrationIR[1]){
        CalibrationIR[1] = *(valeurLu + 1);
      }
      delay(100);
    }

    #ifdef DEBUG
      Serial.print("WAIT - Calibration capteur Avant  : ");
      Serial.println(CalibrationIR[0]);
      Serial.print("Calibration capteur Arriere: ");
      Serial.println(CalibrationIR[1]);
    #endif

    return CalibrationIR;
}


void wait() {
    #ifdef DEBUG
      Serial.println("WAIT - J'attends... et je calibre mes capteurs");
    #endif
    delay(3000);
    //cal=calibrationIR();
    cal=calibrationMaxIR();
    delay(1000);
}

int beDetect=0;
void Robot50HzInterrupt() {
    #ifdef DEBUG
      Serial.println("Interruptions");
    #endif
    beDetect = detecter();
    if(beDetect){
        digitalWrite(led1Pin, HIGH);
    }

}


void ouvertureVolet() {
    #ifdef DEBUG
      Serial.println("FLAP - Ouverture Volets");
    #endif
    servo.write(VOLET_OPENED);
}

// Gestion Moteur
void setMotor(int motorG, int motorD) {

  //speed = map(speed, 0, 100, 0, 255);

if (motorG > 127) {
      digitalWrite(moteur1[0], LOW);
      digitalWrite(moteur1[1], HIGH);
      }
else if  (motorG < 127) {
      digitalWrite(moteur1[0], HIGH);
      digitalWrite(moteur1[1], LOW);
      }
else {
      digitalWrite(moteur1[0], LOW);
      digitalWrite(moteur1[1], LOW);
      }

if (motorD > 127) {
      digitalWrite(moteur2[0], LOW);
      digitalWrite(moteur2[1], HIGH);
      }
else if  (motorD < 127) {
      digitalWrite(moteur2[0], HIGH);
      digitalWrite(moteur2[1], LOW);
      }
else {
      digitalWrite(moteur2[0], LOW);
      digitalWrite(moteur2[1], LOW);
      }
}

void tourneDroite() {
    #ifdef DEBUG
      Serial.println("TOURNE A DROITE ");
    #endif
    setMotor(0,255);  
}

void avance() {
    #ifdef DEBUG
      Serial.println("AVANCE : on avance");
    #endif
    setMotor(126,126); 
}


// A TESTER
void recule() {
    #ifdef DEBUG
      Serial.println("RECULE : on recule");
    #endif
    setMotor(0,0); 
}


void detectionDojo() {
    //bord_circuit_analog_front = analogRead(capteurLF);// Black > 600 White < 180
    //bord_circuit_analog_rear = analogRead(capteurLR); // Black > 600 White < 180

    //int const white_limit=180;
    //if (bord_circuit_analog_front < white_limit || bord_circuit_analog_rear < white_limit) {
    //  dojo_limit=1;
    //}
}

void loop() {
  switch (s_state){
  #ifdef DEBUG
    Serial.println("");
    Serial.print("State:");
    Serial.println(s_state);
    Serial.print("Next State:");
    Serial.println(s_state_next);
  #endif

    case START:
      #ifdef DEBUG
        Serial.println("DEPART");
      #endif
      attenteAppuieBouton();
      //s_state_next is set in the function
      break;

    case WAIT:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef DEBUG
        Serial.println("WAIT");
      #endif
      wait();
      s_state_next=FLAPS;
      break;

    case FLAPS:
      #ifdef SLOW
        delay(attente);
      #endif
       #ifdef DEBUG
        Serial.println("FLAPS");
      #endif
      ouvertureVolet();
      s_state_next=BLINDATTACK;
      break;

    case BLINDATTACK:
      #ifdef SLOW
        delay(attente);
      #endif
       #ifdef DEBUG
        Serial.println("BEGIN - BLINDATTACK");
       #endif
      tourneDroite();
      avance();  // pendant 300ms a coder
       #ifdef DEBUG
        Serial.println("END - BLINDATTACK");
       #endif
      s_state_next=DETECTION;
      break;

    case DETECTION:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef DEBUG
        Serial.println("DETECTION");
      #endif
      // On eteint LED verte et on alume LED rouge
      digitalWrite(led1Pin, 0);
      digitalWrite(led1Pin, 1);
        
      int detect;

      detect=detecter(); // lecture catpteur + tourne sur place droite ou gauche en fonction du catpeur qui detect
      #ifdef DEBUG
        Serial.print("DETECTION - Valeur de detect:");
        Serial.println(detect);
      #endif
      if (detect == 2 ) {
        s_state_next=AVANCE;
      }
      else if (detect == 1) {
        s_state_next=RECULE;
        //s_state_next=TOURNE;
      } 
      else {
        s_state_next=TOURNE;
      }
      break;

     case AVANCE:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef DEBUG
        Serial.println("AVANCE");
      #endif
      avance(); // lecture capteur + moteur
      s_state_next=DETECTION;     
      break;

     case RECULE:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef DEBUG
        Serial.println("RECULE");
      #endif
      recule(); // lecture capteur + moteur
      s_state_next=DETECTION;     
      break;

     case TOURNE:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef DEBUG
        Serial.println("TOURNE");
      #endif
      tourneDroite();
      s_state_next=DETECTION;  
      break;
  }
 s_state = s_state_next;
}

/*

Appuie boutton
Attends 5s
volet ouvert
attaque aveugle (tourne 90 avance 300ms)
  => detection on avance
  => pas detection on tourne
on tourne jusqua detection max 180° (capteur avant capteur arriere)
tant que detection 
=> on avance
Plus detection on tourne

*/
