#include <Arduino.h>
#define STICK_IDLE 5
#define VRNG (410-deg)


bool cat_common;
bool right_cat;
bool left_cat;
bool turret_roll;
float speed_common;
float speed_right;
float speed_left;
float speed_turret;
bool r_l;
bool ultra_pivot_turn = false;
bool turret_done = false;


/*bool direction[4]
  int speed[4]
  short sticks[4]//lr,vhの順
  double level
  bool r_l
  */

void stickcheck(double sticks[4]){
  sticks[0] = sticks[0]*0.9+(analogRead(A0)-1023/2) * 0.1;
  sticks[1] = sticks[1]*0.9+(analogRead(A1)-1023/2) * 0.1;
  sticks[2] = sticks[2]*0.9+(analogRead(A2)-1023/2) * 0.1;
  sticks[3] = sticks[3]*0.9+(analogRead(A3)-1023/2) * 0.1;



<<<<<<< HEAD
  if(sticks[0]==-500){
    if(analogRead(A0)>600){
      sticks[0]=500;
    }else{
      sticks[0]=-500;
    }
=======
if(sticks[0]==-500){
  if(analogRead(A0)>600){
    sticks[0]=500;
  }else{
    sticks[0]=-500;
  }
}
if(sticks[1]　==　-500){
  if(analogRead(A1)>600){
    sticks[1]=500;
  }else{
    sticks[1]=-500;
>>>>>>> dual-pins
  }
  if(sticks[1]　==　-500){
    if(analogRead(A1)>600){
      sticks[1]=500;
    }else{
      sticks[1]=-500;
    }
  }  



  char i;
  char deg;
  for(i=0;i<=3;i++){
    deg = (sticks[i]>0)? 1 : -1;
    sticks[i] = (abs(sticks[i])<STICK_IDLE) ? 0 : sticks[i]-(STICK_IDLE*deg);
    sticks[i] = constrain(sticks[i], -400, 400);
  }
  
}


void decider(){
  double sticks[4];
  stickcheck(sticks);
    
Serial.print(sticks[0]);
Serial.print(':');
Serial.println(sticks[1]);

  double level;
  if(sticks[1]<0){
    level = sticks[1]*1/400 +1;
    r_l = true;  //左折がtrue
  } else{
    level = -1* sticks[1]* 1/400 +1;
    r_l = false;
  }

  

  if(ultra_pivot_turn == true){
    speed_left = 255*(-1*level+1);
    speed_right = 255*(-1*level+1);
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

    right_cat = cat_common; 
    left_cat = cat_common;//左右のキャタピラの方向を確定

  //    Serial.print(speed_left);
  // Serial.print(':');
  // Serial.println(speed_right);


  }

  speed_turret= sticks[3]*51/80; //Rstick holizontalから基準速度を確定
    if(speed_turret<0){
      speed_turret *= -1;
      turret_roll = false;
    }else{
      turret_roll = true;
    }
}

void send(){
  analogWrite(9,(unsigned int)speed_left);
  analogWrite(10,(unsigned int)speed_right);
  analogWrite(11,(unsigned int)speed_turret);

/*
  int digitalPinValue[3];
  digitalPinValue[0] = (left_cat==true)? HIGH:LOW;
  digitalPinValue[1] = (right_cat==true)? HIGH:LOW;
  //digitalPinValue[2] = (turret_roll)? HIGH:LOW;
*/
  digitalWrite(3,left_cat);
  digitalWrite(4,!left_cat);
  digitalWrite(5,right_cat);
  digitalWrite(6,!right_cat);
  //digitalWrite(5,digitalPinValue[2]);
  
 }

void setup()
{
  for (int i = 3; i <= 10; i++){
    pinMode(i, OUTPUT);
  }
  // digitalWrite(7,LOW);
  // pinMode(A1,OUTPUT);
  // digitalWrite(A1,HIGH);

  digitalWrite(7,HIGH);
  // delay(100);
  // digitalWrite(A1,LOW);
  
  pinMode(11,INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  Serial.begin(9600);
  Serial.println("serialstart");
  analogReference(EXTERNAL);
 
}

void loop()
{
  /*int hit = analogRead(A4);
  if(hit>100){
    waiter();
  }

  int turret_button = digitalRead(12);
  if(turret_button==HIGH && turret_done==false){
    digitalWrite(7,HIGH);
    if(analogRead(A5)>=500){
      digitalWrite(3,LOW);
      digitalWrite(4,LOW);
    }else{
      digitalWrite(3,HIGH);
      digitalWrite(4,HIGH);
    }
    analogWrite(9,255);
    analogWrite(10,255);

    delay(100);

    digitalWrite(9,LOW);
    analogWrite(9,0);
    analogWrite(10,0);
    turret_done = true;
  }
  if(turret_button==LOW)turret_done=false;

  int pivot = digitalRead(13);
  ultra_pivot_turn = (pivot==HIGH)? true:false;*/
  decider();
  send();
  
}

void waiter(){  //waiterを起動すると無限ループで待機し、リセットボタンを押すことでリセットする
  digitalWrite(8,HIGH);
  while(true){
    if(digitalRead(11)==HIGH){
      asm volatile ("  jmp 0"); 
    }
  }
}
