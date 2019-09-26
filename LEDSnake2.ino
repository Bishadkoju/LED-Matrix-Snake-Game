#include <LiquidCrystal.h>

byte dataToSend;
long int score=0,highScore=17;
// Pins for the two Shift Registers
int dataPin=8;
int latchPin=9;
int clockPin=10;
int dataPin2=A0;
int latchPin2=A1;
int clockPin2=A2;

int cnt=0;
byte screen[8]={0,0,0,0,0,0,0,0};
int timer1_counter;
boolean disp=false;
// Pins for LCD Display
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = A3, d7 = A4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Initializing the pins for the buttons
int leftPin=4;
int rightPin=5;
int upPin=6;
int downPin=7;
int ePin=13;

int food=30,dirn=1;
int timeRef;
byte j,i,ref; // Initializing Iteration Variables
byte body[64]={27,26,25}; //Initialize snake body
byte sLength=3;// Length of Snake
boolean alive=true;
boolean play=true;




void setup() {
  // put your setup code here, to run once:
pinMode(latchPin,OUTPUT);
pinMode(dataPin,OUTPUT);
pinMode(clockPin,OUTPUT);
pinMode(latchPin2,OUTPUT);
pinMode(dataPin2,OUTPUT);
pinMode(clockPin2,OUTPUT);

pinMode(upPin,INPUT);
pinMode(downPin,INPUT);
pinMode(leftPin,INPUT);
pinMode(rightPin,INPUT);
pinMode(ePin,INPUT);

// Initializing a counter
  noInterrupts();
  TCCR1A=0;
  TCCR1B=0;
  timer1_counter =434286;
  TCNT1=timer1_counter;
  TCCR1B |=(1<<CS12);
  TIMSK1 |=(1<<TOIE1);
  interrupts();
  timeRef=0;

  spawn();
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(" Highscore : "+(String)highScore);
  lcd.setCursor(0,1);
  lcd.print(" Score : "+(String)score);
Serial.begin(9600);
}

// Interrrupt Service Routein
ISR(TIMER1_OVF_vect){
  TCNT1=timer1_counter;
  disp=true; 
}

void loop() {
while(alive && play){
  if(disp){
    snakeUpdate();
    disp=false;
  }
  show();
  buttonInp(); 
}

while(!alive && play){
    show();
    delay(100);
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("  Game Over !!! ");
    if(score>highScore){
      lcd.setCursor(0,1);
      lcd.print(" High Score : "+String(score));
    }else{
      lcd.setCursor(0,1);
      lcd.print(" Score : "+String(score));
      
    }
    
  }
  show();
  buttonInp();  
}

void buttonInp(){
  if(dirn==-1 || dirn==1){
    if(digitalRead(upPin)==HIGH){
      dirn=-8;
    }else if(digitalRead(downPin)==HIGH){
      dirn=8;
    }
  }else if(dirn==-8 || dirn==8){
    if(digitalRead(leftPin)==HIGH){
      dirn=-1;
    }else if(digitalRead(rightPin)==HIGH){
      dirn=1;
    }
  }
}

void placeFood(){
  int a= random(0,64);
  while(isCollide(a)){ // Ensuring that the position of food is not in snake's body
    a=random(0,64);
  }
  food=a;
  turnOn(food);
}


void spawn(){
  turnOn(25);
  turnOn(26);
  turnOn(27);
  placeFood();
  
}
void snakeUpdate(){
  turnOff(body[sLength-1]);// Move the tail

  for(i=sLength-1;i>0;i--){
    body[i]=body[i-1]; // Move body 
  }
  
  // Generating the new postion of head based on snake's movement direction
  if((body[0]%8==0) &&dirn==-1){ // Snake is in left edge and moving left
    body[0]=body[0]+7;
  }else if(((body[0]+1)%8==0)&&dirn==1){ // Snake is in right edge and moving right
    body[0]=body[0]-7;
  }else if((body[0]/8==0)&& dirn==-8){// Snake is in top edge and moving upward
    body[0]=body[0]+56;
  }else if((body[0]/8==7)&& dirn==8){// Snake is in bottom edge and moving downward
    body[0]=body[0]-56;
  }
  else {
    body[0]=body[0]+dirn;
  }
  
 if(isCollide(body[0])){ // Detect the collisiion between the snake's head with its body or food
  if(body[0]==food){// If snake eats food
     sLength++;
     timer1_counter=timer1_counter+1500; // Increasing the snake's speed
     score=score+1; //Adding the score
     //Updating the score on the lcd screen
     lcd.setCursor(0,2);
     lcd.print(" Score : "+(String)score);
     placeFood();
    
    }else{// If snake collides with its own body
      alive=false;
    }
 }
  turnOn(body[0]); // display new head position on led matrix
}

boolean isCollide(int a){
   byte ref=screen[a/8]|(1<<(7-a%8));
  if((screen[a/8]==ref)){
    return true;
  }
  else return false;
}

void turnOn(byte a){
  screen[a/8]=screen[a/8]|(1<<(7-a%8));
  
}

void turnOff(byte a){
  screen[a/8]=screen[a/8]&(~(1<<(7-a%8)));
   
}

void show(){// Display data on LED Matrix
  for(j=0;j<8;j++){
  dataToSend=screen[j];
  digitalWrite(latchPin,LOW);
  shiftOut(dataPin,clockPin,MSBFIRST,dataToSend);
  digitalWrite(latchPin,HIGH);
  digitalWrite(latchPin2,LOW);
  shiftOut(dataPin2,clockPin2,MSBFIRST,~(1<<j));
  digitalWrite(latchPin2,HIGH);
   delay(2);
  
}
  
}
