//Bibliotecas
#include <MIDI.h>

//Declaração de Portas
#define LedR  8
#define LedG  9
#define LedB  10
#define pedal_01 14
#define pedal_02 15
#define pedal_03 16

//Iniciar MIDI
MIDI_CREATE_DEFAULT_INSTANCE();

//Variaveis Configuração
byte midiCh = 1;
int sensibilidade = 180;
const int numPads = 8;
const int pads[numPads] = {A0, A1, A2, A3, A4, A5, A6, A7};
byte note[numPads] = {36,37,38,39,40,41,42,43}; //Notas que serão disparadas

const int padsLED[numPads] = {2, 3, 4, 5, 6, 7, 22, 23};



//Variaveis Controle
int escritaLED[numPads] = {0};
unsigned long tempoAbertura = 0, tempoEsperaAbertura = 7000;
unsigned long ledTempo[numPads] = {0}, ledMudar = 5, manterON = 100;
unsigned long ledTempoMudar = 0, ledEsperaMudar = 3000;
bool ledON[numPads] = {0};
unsigned long ledTempoON[numPads+1] = {0}; //por algum motivo estranho a posição 0 nao armazenava o valor
int deslocamento = 0, deslocTem = 0;
bool contP_01 = false, contP_02 = false, contP_03 = false;
bool contPA_01 = false, contPA_02 = false, contPA_03 = false;

int padCS[numPads] = {0}; //Estado atual do Pad
int padPS[numPads] = {0}; //Estado anterior do Pad
bool statusP, abertura = 1;
int redPri = 1, greenPri = 0, bluePri = 0, redSec = 0, greenSec = 0, blueSec = 1;
char ledPri = 'r';

void setup() {
  MIDI.begin();
  Serial.begin(31250);
  
  //Definindo I/O's
  for (int i = 0; i < numPads; i++) {
    pinMode(pads[i], INPUT);
    pinMode(padsLED[i], OUTPUT);
  }
    
  //LEDs
  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(LedB, OUTPUT);  
  pinMode(pedal_01, INPUT);
  pinMode(pedal_02, INPUT);
  pinMode(pedal_03, INPUT);
  ledTempo[numPads] = {millis()};  
  //Note OFF em todos os pads
  for (int i = 0; i < numPads; i++) {
    MIDI.sendNoteOn(note[i], 0, midiCh);
  }  

  aberturaLed();  
}

void loop() {
  pedais();
  for(int i=0; i<numPads;i++){  
    if(analogRead(pads[i])>sensibilidade){
      statusP = HIGH;
      ledON[i] = 1;
      ledTempoON[i+1] = millis();
    }else{
      statusP = LOW;       
    }
    padCS[i] = statusP;
    controleLEDs(i);
  }
  padCS[numPads] = !padCS[numPads];
  
  midiSend();
  //muda as cores dos leds de forma aleatoria  
  if((millis() - ledTempoMudar) > ledEsperaMudar){
    mudarCorLed();
  }
}

void  aberturaLed(){
  corLed(0, 0, 1);
  digitalWrite(padsLED[0], HIGH);
  digitalWrite(padsLED[5], HIGH);
  delay(500);  
  corLed(0, 1, 0);
  digitalWrite(padsLED[1], HIGH);
  digitalWrite(padsLED[4], HIGH);
  delay(500);  
  corLed(1, 0, 0);
  digitalWrite(padsLED[2], HIGH);
  digitalWrite(padsLED[3], HIGH);
  delay(500);  
}

void midiSend(){
  for(int i = 0; i < numPads; i++){
    if(padCS[i] != padPS[i]){
      if(padCS[i] == HIGH){
        //MIDI.sendNoteOn(note+i, 127, midiCh); // envia NoteOn(nota, velocity, canal midi)
        MIDI.sendNoteOn((note[i]+(numPads*(deslocamento+deslocTem))), 127, midiCh); // use esta se quiser uma escala personalizada
        padPS[i] = padCS[i];
      }else{
        //MIDI.sendNoteOn(note+i, 0, midiCh); // use esta se quiser uma escala cromatica
        MIDI.sendNoteOn((note[i]+(numPads*(deslocamento+deslocTem))), 0, midiCh); // use esta se quiser uma escala personalizada
        padPS[i] = padCS[i];
      }
    }
  }
}

void controleLEDs(int i){     
    if(ledON[i]){
      if((millis()-ledTempoON[i+1]) < manterON){ 
        escritaLED[i] = 1;
      }else{
        if(escritaLED[i]){
          escritaLED[i] = 0;
        }
        ledON[i] = 0;
      }
    }
    ledDestaque(i);   
}

void ledDestaque(int i){
  if((millis()- ledTempo[i]) < ledMudar){
    for(int i = 0;i<numPads;i++){
      digitalWrite(padsLED[i], escritaLED[i]);
    }
    corLed(redPri,greenPri,bluePri);
  }else{
    for(int i = 0;i<numPads;i++){
      digitalWrite(padsLED[i], !escritaLED[i]);
    }
    corLed(redSec,greenSec,blueSec);
  }
  if((millis() - ledTempo[i]) >= (2*ledMudar)){
    ledTempo[i] = millis();
  }
}

void corLed(int r, int g, int b){
  digitalWrite(LedR, !r);
  digitalWrite(LedG, !g);
  digitalWrite(LedB, !b);
}

void mudarCorLed(){
  ledTempoMudar = millis();
  ledEsperaMudar = random(3000,5000);  
  int cor = random(1,4); 
  //Cor Primaria 
  switch (cor) {
    case 1:
      ledPri = 'r';
      redPri    = 1;
      greenPri  = 0;
      bluePri   = 0;
      break;
    case 2:
      ledPri = 'g';
      redPri    = 0;
      greenPri  = 1;
      bluePri   = 0;
      break;
    case 3:
      ledPri = 'b';
      redPri    = 0;
      greenPri  = 0;
      bluePri   = 1;
      break;
    default:
      break;
    
  }
  //Cor Fundo
  cor = random(1,4);  
  switch (cor) {
    case 1:
      if(ledPri != 'r'){
        redSec    = 1;
        greenSec  = 0;
        blueSec   = 0;
      }else{
        redSec    = 0;
        greenSec  = 1;
        blueSec   = 0;        
      }
      break;
    case 2:
      if(ledPri != 'g'){
        redSec    = 0;
        greenSec  = 1;
        blueSec   = 0;
      }else{
        redSec    = 0;
        greenSec  = 0;
        blueSec   = 1;         
      }
      break;
    case 3:
      if(ledPri != 'b'){
        redSec    = 0;
        greenSec  = 0;
        blueSec   = 1;
      }else{
        redSec    = 1;
        greenSec  = 0;
        blueSec   = 0;        
      }
      break;
    default:
      break;
  }    
}

void pedais(){
  contP_01 = digitalRead(pedal_01);
  contP_02 = digitalRead(pedal_02);
  contP_03 = digitalRead(pedal_03);
  if(contP_01 && !contPA_01){
    if(deslocamento <= 6){
      deslocamento++;
    }
  }
  contPA_01 = contP_01;
  if(contP_02 && !contPA_02){
    if(deslocamento >0){
      deslocamento--;
    }
  }
  contPA_02 = contP_02;
  if(contP_03){
    deslocTem = 1;
  }else{    
    deslocTem = 0;
  }
}
