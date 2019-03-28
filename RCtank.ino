byte vibe = 6;
byte sound = 10;
byte respawn = A2;
unsigned byte LIFE = 3;
byte Speed = 0;
byte UD, LR;
bool old_IR;
bool IR;          //赤外線受信
bool shot;       //発射入力


void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);   //コントローラー上下
  pinMode(A1, INPUT);   //コントローラー左右
  pinMode(A2, INPUT_PULLUP);  //ライフ回復
  pinMode(7, INPUT);   //発射入力
  pinMode(13, OUTPUT);  //ライフ1
  pinMode(12, OUTPUT);  //ライフ2
  pinMode(11, OUTPUT);  //ライフ3
  pinMode(10, OUTPUT);  //音 10
  pinMode(6, OUTPUT);   //バイブレーション 6

  pinMode(8, INPUT_PULLUP);   //赤外線受信
  pinMode(9, OUTPUT);         //赤外線送信

  pinMode(5, OUTPUT);         // IN1(左モーター)
  pinMode(4, OUTPUT);         // IN2

  pinMode(3, OUTPUT);         // IN1(3)（右モーター〕
  pinMode(2, OUTPUT);         // IN2(4)

}

void loop() {
  noTone(sound);                            //初期設定
  IR = digitalRead(8);
  old_IR = IR;
  shot = digitalRead(7);

  Serial.print("IR= ");
  Serial.print(IR);
  Serial.print(":");
  Serial.print("old_IR= ");
  Serial.print(old_IR);
  Serial.print(":");
  Serial.print("LIFE= ");
  Serial.print(LIFE);
  Serial.print(":");
  Serial.print("shot= ");
  Serial.print(shot);
  Serial.print(":");
  Serial.print("UD= ");
  Serial.print(UD);
  Serial.print(":");
  Serial.print("LR= ");
  Serial.print(LR);
  Serial.print(":");
  Serial.print("Speed= ");
  Serial.println(Speed);





  switch (LIFE) {                           //ライフゲージ設定
    case 3:
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      digitalWrite(11, LOW);
      break;
    case 2:
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      digitalWrite(11, HIGH);
      break;
    case 1:
      digitalWrite(13, LOW);
      digitalWrite(12, HIGH);
      digitalWrite(11, HIGH);
      break;
  }


  if (IR == 0) {                                  //受信状態
    Serial.println("IR received");
    if (IR == 0 && old_IR == 0) {
      LIFE--;
      digitalWrite(vibe, HIGH);                     //バイブ
      tone(sound, 300, 1000);                       //警告音
      delay(1000);
      digitalWrite(vibe, LOW);
    }
  }

  else if (IR == 1) {
    if (LIFE <= 0) {
      if (digitalRead(respawn) == 0) {               //ライフ回復
        LIFE = 3;
        tone(sound, 1200, 800);
        delay(800);
      }
      
      else {
        digitalWrite(5, LOW);
        digitalWrite(4, LOW);
        digitalWrite(3, LOW);
        digitalWrite(2, LOW);

        digitalWrite(13, HIGH);
        digitalWrite(12, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(vibe, HIGH);
        tone(sound, 987);
        delay(750);

        digitalWrite(13, LOW);
        digitalWrite(12, LOW);
        digitalWrite(11, LOW);
        digitalWrite(vibe, LOW);
        tone(sound, 783);
        delay(750);
      }
    }



    else if (LIFE >= 1) {
      UD = (pulseIn(A0, HIGH, 2000) + 10) *  8 / 1000;
      LR = (pulseIn(A1, HIGH, 2000) + 10) *  8 / 1000;
      if (UD == 0) UD = (analogRead(A0) <= 100) ? 0 : 8;
      if (LR == 0) LR = (analogRead(A1) <= 100) ? 0 : 8;
      digitalWrite(vibe, LOW);

      if ((UD == 4) && (4 < LR)) {         //中心,ブレーキ
        digitalWrite(5, HIGH);
        digitalWrite(4, HIGH);
        digitalWrite(3, HIGH);
        digitalWrite(2, HIGH);
      }

      else if ((UD > 4) && (LR == 4)) {           //上
        Speed = abs(UD - 4) * 32 + 127;
        digitalWrite(5, LOW);
        analogWrite(4, Speed);
        digitalWrite(5, LOW);
        analogWrite(2, Speed);
      }

      else if ((UD < 4) && (LR == 4)) {      //下
        Speed = abs(UD - 4) * 32 + 127;
        analogWrite(5, Speed);
        digitalWrite(5, LOW);
        analogWrite(3, Speed);
        digitalWrite(5, LOW);
      }

      else if ((UD == 4) && (4 < LR)) {       //右
        digitalWrite(5, LOW);
        digitalWrite(4, HIGH);
        digitalWrite(3, HIGH);
        digitalWrite(2, LOW);
      }

      else if ((UD == 4) && (4 > LR)) {       //左
        digitalWrite(5, HIGH);
        digitalWrite(4, LOW);
        digitalWrite(3, LOW);
        digitalWrite(2, HIGH);
      }

      else if ((4 < UD) && (4 < LR)) {  //第一象限
        Speed = 255 - abs(LR - 4) * 40;
        digitalWrite(5, LOW);
        digitalWrite(4, HIGH);
        digitalWrite(3, LOW);
        analogWrite(2, Speed);
      }

      else if ((4 < UD) && (LR < 4)) {  //第二象限
        Speed = 255 - abs(LR - 4) * 40;
        digitalWrite(5, LOW);
        analogWrite(6, Speed);
        digitalWrite(3, LOW);
        digitalWrite(2, HIGH);
      }

      else if ((UD < 4) && (LR < 4)) {  //第三象限
        Speed = 255 - abs(LR - 4) * 40;
        analogWrite(5, Speed);
        digitalWrite(4, LOW);
        digitalWrite(3, HIGH);
        digitalWrite(2, LOW);
      }

      else if ((UD < 4) && (4 < LR)) {  //第四象限
        Speed = 255 - abs(LR - 4) * 40;
        digitalWrite(5,  HIGH);
        digitalWrite(4, LOW);
        analogWrite(9, Speed);
        digitalWrite(2, LOW);
      }




      if (shot == 0) {
        tone(9, 38000, 500);        //赤外線出力
        digitalWrite(13, HIGH);
        digitalWrite(12, HIGH);
        digitalWrite(11, HIGH);

        digitalWrite(5, LOW);           //ノックバック
        digitalWrite(4, LOW);           //ノックバック
        digitalWrite(3, LOW);           //ノックバック
        digitalWrite(2, LOW);           //ノックバック
        delay(150);

        digitalWrite(5, HIGH);           //ノックバック
        digitalWrite(4, HIGH);           //ノックバック
        digitalWrite(3, HIGH);           //ノックバック
        digitalWrite(2, HIGH);           //ノックバック
        delay(650);

        digitalWrite(vibe, HIGH);           //バイブ 1q
        digitalWrite(11, LOW);        //ライフLED１
        tone(sound, 1200, 700);           //音１
        delay(700);
        digitalWrite(vibe, LOW);           //バイブ
        delay(100);

        digitalWrite(vibe, HIGH);           //バイブ
        digitalWrite(12, LOW);        //ライフLED２
        tone(sound, 1200, 700);           //音２
        delay(700);
        digitalWrite(vibe, LOW);           //バイブ
        delay(100);

        digitalWrite(vibe, HIGH);           //バイブ
        digitalWrite(13, LOW);        //ライフLED３
        tone(sound, 1800, 600);           //音３
        delay(700);
        digitalWrite(vibe, LOW);           //バイブ

      }
    }
  }
}
