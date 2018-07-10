#include <Arduino.h>
#include <stdlib.h>

#define STICK_IDLE 10
#define VRNG (410-deg)

bool cat_common;
bool right_cat;
bool left_cat;
int speed_common;
int speed_right;
int speed_left;
short stick1[2];
short stick2[2];
double level;  
bool r_l;
unsigned long timer[2];
bool timer_flag[2];
bool ultra_pivot_turn = false;


void stickcheck(){
  stick1[0] = stick1[0]*0.9+(analogRead(A0)-1023/2) * 0.1;
  stick1[1] = stick1[1]*0.9+(analogRead(A1)-1023/2) * 0.1;
  stick2[0] = stick2[0]*0.9+(analogRead(A2)-1023/2) * 0.1;
  stick2[1] = stick2[1]*0.9+(analogRead(A3)-1023/2) * 0.1;

  for(int i = 0;i<=1;i++){
    char deg = (stick1[i]>0)? 1 : -1;
    stick1[i] = (abs(stick1[i])<STICK_IDLE) ? 0 : stick1[i]-(STICK_IDLE*deg);
    stick1[i] = constrain(stick1[i], VRNG * -1, VRNG);
  }
  for(int j = 0;j<=1;j++){
    char deg = (stick1[j]>0)? 1 : -1;
    stick2[j] = (abs(stick2[j])<STICK_IDLE) ? 0 : stick2[j]-(STICK_IDLE*deg);
    stick2[j] = constrain(stick2[j], VRNG * -1, VRNG);
  }


  if(stick1[1]<0){
    level = stick1[1]*1/400 +1;
    r_l = true;  //左折がtrue
  } else{
    level = -1* stick1[1]* 1/400 +1;
    r_l = false;
  }
}


int decide_cat(){

  if(ultra_pivot_turn){
    speed_left , speed_right = 255*level;
    if(r_l){
      left_cat = false;
      right_cat = true;
    }else{
      left_cat = true;
      right_cat = false;
    }
    if(level==0){
      ultra_pivot_turn = false;
      for(int k=0;k<=2;k++){
        timer[k] = 0;
        timer_flag[k] = false;
      }
    }
    
  }else{

    speed_common = stick1[0]*51/80; //Lstick verticalから基準速度を確定
    if(speed_common<0){
      speed_common *= -1;
      cat_common = false;
    }else{
      cat_common = true;
    }
    /* double level;  //旋回の基準数値を算出
      bool r_l;
      if(stick1[1]<0){
        level = stick1[1]*1/400 +1;
        r_l = true;  //左折がtrue
      } else{
        level = -1* stick1[1]* 1/400 +1;
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

    if(level<0.1 && !timer_flag[0]){ //超信地旋回用判定1
      timer[0] = millis();
      timer_flag[0] = true;
    }
    if(level<0.1 && timer_flag[0] && timer_flag[1] && millis()-timer[1]<100){ //超信地旋回用判定3 , 超信地旋回操作
      ultra_pivot_turn = true;
    }
    
    if(level>0.9 && timer_flag[0] && timer[0]-millis()<100){ //超信地旋回用判定2
      timer[1]=millis();
      timer_flag[1] = true;
    }
  }
}

void send(){
  analogWrite(7,speed_left);
  analogWrite(8,speed_right);

  int digitalPinValue[3];
  digitalPinValue[0] = (left_cat)? HIGH:LOW;
  digitalPinValue[1] = (right_cat)? HIGH:LOW;
  //digitalPinValue[2] = (turret_roll)? HIGH:LOW;

}

void setup()
{
  // put your setup code here, to run once:
  for (int i = 3; i <= 9; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  analogReference(EXTERNAL);
}

void loop()
{
  int left_v = analogRead(A1);
  int left_h = analogRead(A2);
  int right_v = analogRead(A3);
  int right_h = analogRead(A4);


  stickcheck();
  decide_cat();
  send();
}
