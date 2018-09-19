#include <Arduino.h>
#define STICK_IDLE 5
#define VRNG (410-deg)



bool cat_common;
bool cats[3]; //left-0,right-1,turret-2
float speeds[3];
float speed_common;
bool r_l;
bool ultra_pivot_turn = false;
bool turret_done = false;
double sticks[4];

/*bool direction[4]
  int speed[4]
  short sticks[4]//lr,vhの順
  double level
  bool r_l
  */

void stickcheck(){
	sticks[0] = sticks[0]*0.9+(analogRead(A0)-1023/2) * 0.1;
	sticks[1] = sticks[1]*0.9+(analogRead(A1)-1023/2) * 0.1;
	sticks[2] = sticks[2]*0.9+(analogRead(A2)-1023/2) * 0.1;

 	Serial.println(analogRead(A0));
	char i;
	char deg;
	for(i=0;i<3;i++){
		deg = (sticks[i]>0)? 1 : -1;
		sticks[i] = (abs(sticks[i])<STICK_IDLE) ? 0 : sticks[i]-(STICK_IDLE*deg);
		
		sticks[i] = constrain(sticks[i], -400, 400);
	}
   
	 
}


void decider(){
	
	stickcheck();
	
	// Serial.print(sticks[0]);
	// Serial.print(':');
	// Serial.println(sticks[1]);


	double level;
	if(sticks[1]<0){
		level = sticks[1]*1/400 +1;
		r_l = true;  //左折がtrue
	} else{
		level = -1* sticks[1]* 1/400 +1;
		r_l = false;
	}

  

	if(ultra_pivot_turn == true){
		speeds[0] = 255*(-1*level+1);
		speeds[1] = 255*(-1*level+1);
		
		if(r_l){
			cats[0] = false;
			cats[1] = true;
		}else{
			cats[0] = true;
			cats[1] = false;
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
			speeds[0] = (level<0.2) ? 0 : speed_common * (1.25*level-0.25);
			speeds[1] = speed_common;
		}else{
			speeds[0] = speed_common;
			speeds[1] = (level<0.2) ? 0 : speed_common * (1.25*level-0.25);
		}

		cats[0] = cat_common; 
		cats[1] = cat_common;//左右のキャタピラの方向を確定

	//    Serial.print(speeds[0]);
	// Serial.print(':');
	// Serial.println(speeds[1]);
  	}
  	speeds[2]= sticks[2]*51/80; //Rstick holizontalから基準速度を確定

	if(speeds[2]<0){
	  	speeds[2] *= -1;
	  	cats[2] = false;
	}else{
	  	cats[2] = true;
	}
}

void send(){
	char pins[6]={3,5,6,9,10,11};
	for(char i=0;i<3;i++){
		
		if(cats[i]){
			analogWrite(pins[2*i],(byte)speeds[i]);
			digitalWrite(pins[2*i+1],LOW);
			
		}else{
			digitalWrite(pins[2*i],LOW);
			analogWrite(pins[2*i+1],(byte)speeds[i]);
		}
	}
	
}

void setup(){
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
