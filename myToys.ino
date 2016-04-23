#include <Servo.h>
#define TURN_TIME 1700
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

const int PIN=11;
#define NUMPIXELS 60

Servo myservo; 
Servo doorServo;

const int buttonPin = 2;              //white button to run the central servo
const int yellowButton = 3;
const int blueButton = 4;
const int softHand = 5;
const int locationHome = 6;
const int locationForest = 7;
const int doorServoPin = 8; 
const int servoPin = 15;
const int yellowLed = 12;
const int blueLed = 13;
const int speakerPin = 14;

//automatic door
const int stopSpeed = 94;
const int revolutionSpeed = 96;

boolean revolution = false;
int buttonState = HIGH;
int pos = 0;                      //position of the automatic door servo
int yellowButtonState = HIGH;
int blueButtonState = HIGH;
int softHandState = HIGH;
int locationHomeState = HIGH;
int locationForestState = HIGH;
boolean isHome = false;
boolean isForest = false;
int array[] = {0,1};
int index = 0;
boolean clickStatus = false;
boolean isStarted = false; // delete later

int length = 15; // the number of notes
char notes[] = "ccggaagffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup(){
  Serial.begin(9600);
  myservo.attach(servoPin);
  doorServo.attach(doorServoPin);
 // myservo.write (0, 100, true);
 doorServo.write(80);            //reset door angle
 myservo.write(stopSpeed);     //reset machine state
 pinMode(buttonPin, INPUT_PULLUP);
 pinMode(yellowButton, INPUT_PULLUP);
 pinMode(blueButton, INPUT_PULLUP);
 pinMode(softHand, INPUT_PULLUP);
 pinMode(yellowLed, OUTPUT);
 pinMode(blueLed, OUTPUT);
 pinMode(locationHome, INPUT_PULLUP);
 pinMode(locationForest, INPUT_PULLUP);
 pinMode(speakerPin, OUTPUT);
 digitalWrite(yellowLed, HIGH);
 digitalWrite(blueLed, LOW);
  strip.begin();
  strip.show();
// ledCounter = 0;
}

//detect which button get clicked
int getColorClicked(){
  if(yellowButtonState == 0) {return 0;}
  else if(blueButtonState == 0) {return 1;}
  return -1;
}

//to check button sequence
boolean compare(int yellowButtonState, int blueButtonState, int com){
  switch(com){
    case 0:
    return yellowButtonState == 0;
    case 1:
    return blueButtonState == 0;
    default:
    return false;
  }}

//detect the button to trigger the sharing function
void judge(){
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    revolution = true;
  }else{
    buttonState = false;
} 
}

//open toy dropping door 
void openDoor(){
   for(pos = 80; pos >=1; pos-=1 )
   {
     doorServo.write(pos);
     delay(20);
   }
}

//close toy dropping door
void closeDoor(){
  for (pos = 0; pos <=80; pos+=1)
  {
     doorServo.write(pos);
     delay(20);
  }
}

//run the sharing function
void run(){
  if (revolution==true)  //trigger of machine revolution
 {
   myservo.write(revolutionSpeed);
   delay(TURN_TIME);
   myservo.write(stopSpeed);
   delay(500);
   openDoor();
   delay(3000);
   closeDoor();
   revolution=false;
 }
}

void playTone(int tone, int duration) {
  for (long k = 0; k < duration * 1000L; k += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int j = 0; j < 8; j++) {
    if (names[j] == note) {
      playTone(tones[j], duration);
    }
  }
}

void playMelody(){
for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    // pause between notes
    delay(tempo / 2); 
    noTone(speakerPin);
    }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
     strip.show();
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


void softHandPress(){       //reaction when softhand button is pressed
  softHandState = digitalRead(softHand);
//  Serial.println(softHandState);
  if(softHandState == LOW){
   Serial.println(softHandState);
   Serial.println("soft hand button is pressed, play music");
   myservo.write(93);  
   rainbow(20);
   myservo.write(94);
   playMelody();
   myservo.write(94);   //91
   colorWipe(strip.Color(0, 0, 0),10);
   myservo.write(stopSpeed);
   delay(3000);
 }
}

void bunnyInteraction(){
  locationHomeState = digitalRead(locationHome);
  locationForestState = digitalRead(locationForest);
  if(locationHomeState == 0){
    isHome = true;
    Serial.println("isHome");
  }
  if(locationForestState == 0){
    isForest = true;
    Serial.println("isForest");
  }
  if(isForest == true && isHome == true){
    Serial.println("bunny interaction done. Music!");
    myservo.write(94);  //93
    colorWipe(strip.Color(255, 0, 0), 50);
    colorWipe(strip.Color(0, 255, 0), 50);
    colorWipe(strip.Color(0, 0, 255), 50);
   // myservo.write(93);
    playMelody();
 //   myservo.write(91);
    colorWipe(strip.Color(0, 0, 0),10);
    myservo.write(stopSpeed);
//    myservo.write(stopSpeed);
    isHome = false;
    isForest = false;
    delay(3000);
  }
}

void beginning(){
  
if(yellowButtonState == LOW || blueButtonState == LOW){

   Serial.println(getColorClicked());
 //  clickStatus = true;
   if(compare(yellowButtonState, blueButtonState, array[index])){
     if(index == 0){
       Serial.println("start!");
       digitalWrite(yellowLed, LOW);
       digitalWrite(blueLed, HIGH);
     }
     index++;
     if(index >= sizeof(array)/2){
       Serial.println("success!");
       digitalWrite(blueLed, LOW);
       openDoor();
       delay(3000);
       closeDoor();
       clickStatus = true;
  }
}else if(compare(yellowButtonState, blueButtonState, array[0]) && index == 1 || index ==0){
 // Serial.println("start! What?");
}else{
     index =0;
     Serial.println("wrong");
}
  }
}

void loop(){
  
  yellowButtonState = digitalRead(yellowButton);
  blueButtonState = digitalRead(blueButton); 
  
  if(clickStatus==false){
    beginning();
}
   if(clickStatus == true){
   judge();
   softHandPress();
   bunnyInteraction();
   run();
 }
}


