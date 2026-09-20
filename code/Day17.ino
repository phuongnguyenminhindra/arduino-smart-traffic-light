#define LED_RED 5
#define LED_YELLOW 4
#define LED_GREEN 3
#define LED_PED_RED 7
#define LED_PED_GREEN 8
#define BUTTON 2
#define BUZZER 6
#define LDR_PIN A0
int lightThresholdLow = 300;  // dưới 300 là trời tối
int lightThresholdHigh = 500; // trên 500 là trời sáng (để chống nháy)

bool isDark = false;

enum State { STATE_RED, STATE_YELLOW, STATE_GREEN };
enum Mode { MODE_NORMAL, MODE_NIGHT, MODE_EMERGENCY };

State currentState;
Mode currentMode = MODE_NORMAL;

struct Timer { unsigned long start; unsigned long interval; };
Timer stateTimer, beepTimer, holdTimer;
Timer autoLightTimer;

bool pedestrianRequest = false;
bool lastButtonRaw = HIGH;// lần bấm nút cuối(lưu lại cho lần sau so sánh)
bool isHolding = false;// cờ giữ nút bấm

// Timer helpers
void Timer_Start(Timer *t, unsigned long ms){ t->start=millis(); t->interval=ms; }
bool Timer_Expired(Timer *t){ return millis() - t->start >= t->interval; }

void Update_LEDs_Normal(){
  digitalWrite(LED_RED, currentState==STATE_RED);
  digitalWrite(LED_YELLOW, currentState==STATE_YELLOW);
  digitalWrite(LED_GREEN, currentState==STATE_GREEN);
  if(currentState==STATE_RED){
    digitalWrite(LED_PED_RED, LOW);
    digitalWrite(LED_PED_GREEN, HIGH);
  } else {
    digitalWrite(LED_PED_RED, HIGH);
    digitalWrite(LED_PED_GREEN, LOW);
  }
}

void ChangeMode() {
  bool raw = digitalRead(BUTTON);

  if (raw == LOW && lastButtonRaw == HIGH) { // vua moi bam xuong
    Timer_Start(&holdTimer, 0);
    isHolding = true;
  }

  if (raw == LOW && isHolding) {//nếu vẫn đang giữ
    unsigned long held = millis() - holdTimer.start;//tính thời gian giữ
    if (held >= 5000 && currentMode != MODE_EMERGENCY) { // hơn 5s
      currentMode = MODE_EMERGENCY;
      Timer_Start(&stateTimer, 0);
      Serial.println(">> CHE DO CAP CUU!");
      isHolding = false; // tranh lap lai
    }
  }

  if (raw == HIGH && lastButtonRaw == LOW) { // vua moi tha ra - DONG NAY BAN DANG THIEU
    if (isHolding) {
      unsigned long held = millis() - holdTimer.start;
      
      

   if (held >= 50 && held < 3000) { // bam nhanh
        if (currentMode == MODE_NORMAL) {
          pedestrianRequest = true;
          Serial.println("-> Xin qua duong");
        } 
        else if (currentMode == MODE_EMERGENCY) {
          // Chi cho phep thoat khoi CAP CUU
          currentMode = MODE_NORMAL;
          currentState = STATE_RED;
          Timer_Start(&stateTimer, 5000);
          noTone(BUZZER);
          Serial.println(">> TRO VE NORMAL (tu CAP CUU)");
        }
        // Neu dang MODE_NIGHT thi bam khong lam gi ca -> chi bien tro moi thoat duoc
      }
      isHolding = false;
    }
  }
  lastButtonRaw = raw;
}
void AutoLight_Check() {
  int light = analogRead(LDR_PIN);
  
  // Trời tối dần
  if (light < lightThresholdLow) {// giả lập ánh sáng đo được nhỏ hơn mức đặt ra
    if (!isDark) {// nếu trời sáng
      Timer_Start(&autoLightTimer, 2000); // phải tối liên tục 2s mới đổi
      isDark = true;
    }
    if (Timer_Expired(&autoLightTimer) && currentMode == MODE_NORMAL) {
      currentMode = MODE_NIGHT;
      currentState = STATE_YELLOW;
      Timer_Start(&stateTimer, 500);
      Serial.println(">> Troi toi -> TU DONG SANG DEM");
    }
  } 
  // Trời sáng dần
  else if (light > lightThresholdHigh) {// nếu cao hơn mức đặt ra
    if (isDark) {// nếu trời tối
      Timer_Start(&autoLightTimer, 2000);//đảm bảo sáng đủ 2s tránh nhiễu
      isDark = false;
    }
    if (Timer_Expired(&autoLightTimer) && currentMode == MODE_NIGHT) {
      currentMode = MODE_NORMAL;
      currentState = STATE_RED;
      Timer_Start(&stateTimer, 5000);
      Serial.println(">> Troi sang -> VE NGAY");
    }
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(LED_RED,OUTPUT); pinMode(LED_YELLOW,OUTPUT); pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_PED_RED,OUTPUT); pinMode(LED_PED_GREEN,OUTPUT);
  pinMode(BUTTON,INPUT_PULLUP); pinMode(BUZZER,OUTPUT);
  currentState=STATE_RED;
  Timer_Start(&stateTimer,5000); Timer_Start(&beepTimer,400);
  Timer_Start(&autoLightTimer, 0);
  Serial.println("Day 17");
}

void loop(){
  ChangeMode();
  AutoLight_Check();

  // --- CHE DO BAN DEM ---
  if(currentMode == MODE_NIGHT){
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PED_RED, LOW);
    digitalWrite(LED_PED_GREEN, LOW);
    noTone(BUZZER);
    if(Timer_Expired(&stateTimer)){
      digitalWrite(LED_YELLOW, !digitalRead(LED_YELLOW));
      Timer_Start(&stateTimer, 500);
    }
    return;//kết thục tránh chạy code dưới normal
  }

  // --- CHE DO CAP CUU ---
  if(currentMode == MODE_EMERGENCY){
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PED_GREEN, LOW);
    if(Timer_Expired(&stateTimer)){
      bool on = digitalRead(LED_RED);
      digitalWrite(LED_RED, !on);
      digitalWrite(LED_PED_RED, !on);
      if(!on) tone(BUZZER, 900, 150); else noTone(BUZZER);
      Timer_Start(&stateTimer, 300);
    }
    return;// kết thúc lần lặp này sang lần lặp tới tránh chạy code phía dưới
  }

  // --- CHE DO NORMAL (nhu Day 15) ---
  if(Timer_Expired(&stateTimer)){//đảm bảo hết thời gian từng trạng thái thì đổi chế độ
    if(currentState==STATE_RED){ currentState=STATE_GREEN; Timer_Start(&stateTimer,3000); }
    else if(currentState==STATE_GREEN){ currentState=STATE_YELLOW; Timer_Start(&stateTimer,2000); }
    else {
      currentState=STATE_RED;
      if(pedestrianRequest){ Timer_Start(&stateTimer,8000); pedestrianRequest=false; }
      else Timer_Start(&stateTimer,5000);
    }
  }

  if(currentState==STATE_RED && stateTimer.interval==8000){
    if(Timer_Expired(&beepTimer)){ 
      static bool b=false; b=!b; 
      if(b) tone(BUZZER,1200,200); else noTone(BUZZER);
      Timer_Start(&beepTimer,400); 
    }
  } else if (currentMode==MODE_NORMAL) { noTone(BUZZER); }// dòng này đảm bảo còi chỉ kêu đúng 8s(phòng thủ vì bài này không cần lắm)

  Update_LEDs_Normal();
}
