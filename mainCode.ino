#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include<Keypad.h>
#include <RFID.h>

// define constants.

#define KEYPRESSED 1000
//#define BUZZER 0
#define LED 1
#define SS_PIN 10
#define RST_PIN 9
#define PWD_MAX_LENGTH 9 
#define PWD_MIN_LENGTH 5
#define Master_key  "321"
//security
String default_password="123";
int cards[][5] = {
  {5,117,21,219,190},
  {25,171,42,131,27}
};

// Setting the LCD 
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x3f;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE );


// setting up the keypad 
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {0, A1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4,5,6,A2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//rfid settings 

RFID rfid(SS_PIN,RST_PIN);

//setting program. 
bool isDeactivated=true;
bool activateMeRequest=false;
bool letMeInRequest=false;


// cutoms functions 

bool rfid_access(){
 
bool access=false;
if(rfid.isCard()){
if(rfid.readCardSerial()){
for(int x = 0; x < sizeof(cards); x++){
    for(int i = 0; i < sizeof(rfid.serNum); i++ ){
     if(rfid.serNum[i] != cards[x][i]) {
     access = false;
     break;}
     else{
      access = true;
     }
     }
     if(access) break;
     }}
     }

rfid.halt();
return access;
}




void keyPressedSound(){
  tone(LED,KEYPRESSED,200);
  }


void getPassword( char key,String message){}
void reset_screen(String message){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message); 
}

void let_in(){
  
bool goodToGo=false;
bool pass=false;
while(!goodToGo){
goodToGo= verifyPassword(); 
pass=rfid_access(); 
if(goodToGo==true||pass==true){
digitalWrite(LED,LOW);
isDeactivated=true;
goodToGo=true;
pass=true;
  }
  }
  rfid.halt();
 }

bool verifyPassword(){
char key=keypad.getKey();
String temp; 
if(key){

reset_screen("Contracena:");
temp="";
while (key!='*'){
 key=keypad.getKey();
 if(key){
  //keyPressedSound();
  if(key !='*'){      
   lcd.print("*");
   temp+=key;
   if(temp.length()==9){
    temp="";
    reset_screen("PSW INV:");
    delay(500);}}
}}

  if(key=='*'){
    if(temp==default_password){
      temp="";
      return true;
      }
      else{
        temp="";
        reset_screen("Wrong Password"); 
        return false; }
      
      }
}
else{return false;}

  }

void changePassword(){
 keyPressedSound();
 reset_screen("Current PSW:");
char key;
String temp;
  while (key!='*'){
    key=keypad.getKey();
    if(key){
      if(key !='*'){      lcd.print("*");
      temp+=key;
      if(temp.length()==9){
        temp="";
        reset_screen("Current PSW:");
        delay(500);}}
}}

  if(key=='*'){
   lcd.clear();
   lcd.setCursor(0,0);
    if(temp==default_password){
      key="";
      temp="";
      String temp2;
      lcd.print("New PSW:"); 
      lcd.setCursor(0,1);
      while (key!='*'){
        key=keypad.getKey();
        if(key){
          if(key !='*'){          
          lcd.print("*");
          temp+=key;
          if(temp.length()==9){
            temp="";
            reset_screen("New PSW:");;
            delay(500);}}
}}

default_password=temp;
reset_screen("Succesful");}
else{reset_screen("Wrong password"); 
    }
  }
    }

void setup()
{
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print("*****Alarma*****!");
lcd.setCursor(0,1);
delay(500);
lcd.print("LOADING");
lcd.print(".");
delay(200);

SPI.begin();
lcd.print(".");
delay(500);
rfid.init();
lcd.print(".");
delay(200);

pinMode(LED, OUTPUT);
//pinMode(BUZZER, OUTPUT);
digitalWrite(LED, LOW);
lcd.print(".");
delay(200);


  

  pinMode(SLAVE, OUTPUT);
  digitalWrite(SLAVE, HIGH);
  
}
 
 
void loop()
{
 if(isDeactivated){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("**No! Activado**");
    lcd.setCursor(0,1);
    lcd.print("__A:Act  C:PSW__");
    delay(500);

    char key = keypad.getKey();

    if(key){
      lcd.setBacklight(1);
      keyPressedSound(); 
      //keyboard_light(1);
    if(key=='A'){
    keyPressedSound();
    activateMeRequest=true;
    }
   else if(key=='C'){
   keyPressedSound();
   changePassword(); 
   }
   else{
    //WrongKeySound(); 
    } 
 
    }
}

if(activateMeRequest==true){
    

reset_screen("Activating in:");
delay(1000);

bool state= digitalRead(A0); 

if(!state){
  reset_screen("CLOSE ALL DOOR"); 
  delay(500);
  }

 else{
int countDown=9;
 while(countDown>1){
   lcd.setCursor(0,1);
   lcd.print(countDown);
   keyPressedSound(); 
   delay(1000);
   countDown--;
   //keyPressedSound();
}
delay(2000);

keyPressedSound();
delay(500);
keyPressedSound();

reset_screen("Alarm activated");
activateMeRequest=false;
isDeactivated=false;
}}

if (!isDeactivated){
       if(digitalRead(A0)){
     if( rfid_access()||keypad.getKey()=='D'){ 
      reset_screen("Alarm deactivated");
      delay(500); 
      isDeactivated=true;
      }
} 
     else{
        digitalWrite(LED, HIGH); // Send 1KHz sound signal 
        reset_screen("Intrusion");
       let_in();
      }

}

}





