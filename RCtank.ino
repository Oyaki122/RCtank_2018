#include <Arduino.h>
#include <stdlib.h>

#define STICK_IDLE 10
#define VRNG (410-deg)


bool cat_common;
bool right_cat;
bool left_cat;
bool turret_roll;
short speed_common;
short speed_right;
short speed_left;
short speed_turret;
bool r_l;
bool ultra_pivot_turn = false;


/*bool direction[4]
  int speed[4]
  short sticks[4]//lr,vhの順
  double level
  bool r_l
  */

void stickcheck(short sticks[4]){
  sticks[0] = sticks[0]*0.9+(analogRead(A0)-1023/2) * 0.1;
  sticks[1] = sticks[1]*0.9+(analogRead(A1)-1023/2) * 0.1;
  sticks[2] = sticks[0]*0.9+(analogRead(A2)-1023/2) * 0.1;
  sticks[3] = sticks[1]*0.9+(analogRead(A3)-1023/2) * 0.1;

  byte i;
  char deg;
  for(i=0;i<=3;i++){
    deg = (sticks[i]>0)? 1 : -1;
    sticks[i] = (abs(sticks[i])<STICK_IDLE) ? 0 : sticks[i]-(STICK_IDLE*deg);
    sticks[i] = constrain(sticks[i], VRNG * -1, VRNG);
  }
}


void decider(){
  short sticks[4];
  stickcheck(sticks); 

  short level;
  if(sticks[1]<0){
    level = sticks[1]*1/400 +1;
    r_l = true;  //左折がtrue
  } else{
    level = -1* sticks[1]* 1/400 +1;
    r_l = false;
  }

  if(ultra_pivot_turn){
    speed_left , speed_right = 255*level;
    if(r_l){
      left_cat = false;
      right_cat = true;
    }else{
      left_cat = true;
      right_cat = false;
    }  
  }else{

    speed_common = sticks[0]*51/80; //Lstick verticalから基準速度を確定
    if(speed_common<0){
      speed_common *= -1;
      cat_common = false;
    }else{
      cat_common = true;
    }
    /* double level;  //旋回の基準数値を算出
      bool r_l;
      if(sticks[1]<0){
        level = sticks[1]*1/400 +1;
        r_l = true;  //左折がtrue
      } else{
        level = -1* sticks[1]* 1/400 +1;
        r_l = false;
      }*/

    if(r_l){  //左右別に分けて左右のキャタピタの速度を確定
      speed_left = (level<0.2) ? 0 : speed_common * (1.25*level-0.25);
      speed_right = speed_common;
    }else{
      speed_left = speed_common;
      speed_right = (level<0.2) ? 0 : speed_common * (1.25*level-0.25);
    }

    right_cat,left_cat = cat_common; //左右のキャタピラの方向を確定

    
    speed_turret= sticks[3]*51/80; //Rstick holizontalから基準速度を確定
    if(speed_turret<0){
      speed_turret *= -1;
      turret_roll = false;
    }else{
      turret_roll = true;
    }
  }
}

void send(){
  analogWrite(7,speed_left);
  analogWrite(8,speed_right);
  analogWrite(9,speed_turret);

  int digitalPinValue[3];
  digitalPinValue[0] = (left_cat)? HIGH:LOW;
  digitalPinValue[1] = (right_cat)? HIGH:LOW;
  //digitalPinValue[2] = (turret_roll)? HIGH:LOW;

}

void setup()
{
  // put your setup code here, to run once:
  for (int i = 3; i <= 10; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(11,INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  analogReference(EXTERNAL);
}

void loop()
{
  int hit = analogRead(A4);
  if(hit>100){
    waiter();
  }
  if(digitalRead(12)==HIGH){
    digitalWrite(9,HIGH);
    delay(50);
    digitalWrite(9,LOW);
  }
  ultra_pivot_turn = (digitalRead(13)==HIGH)? true:false;
  decider();
  send();
}

void waiter(){  //waiterを起動すると無限ループで待機し、リセットボタンを押すことでリセットする
  digitalWrite(10,HIGH);
  while(true){
    if(digitalRead(11)==HIGH){
      asm volatile ("  jmp 0"); 
    }
  }
}