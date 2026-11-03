// 쟈철 × 아두이노: 버튼 누르면 화면에 알람이 뜨고, 화면 상태에 따라 LED·부저가 반응한다.
// 브라우저(크롬·엣지)에서 설정 > '아두이노 연결'을 누르고 이 보드의 포트를 고르면 된다. 속도 9600.
//
// 아두이노 → 화면:  "ALARM\n"            (버튼 눌림 → 알람 모달 열림)
// 화면 → 아두이노:  "MOOD:free\n"        (여유)   초록 LED
//                   "MOOD:prep\n"        (준비)   노랑 LED
//                   "MOOD:go\n"          (나가)   노랑 LED 깜빡
//                   "MOOD:late\n"        (늦음)   빨강 LED
//                   "MOOD:doomed\n"      (왕창 늦음) 빨강 LED 깜빡 + 부저
//                   "ALARM:<mood>\n"     (알람 떴을 때) 부저 삑삑
//
// 배선: 버튼 2번 핀 ↔ GND (내부 풀업), LED 초록 9 / 노랑 10 / 빨강 11 (저항 220Ω), 부저 8번.

const int BTN = 2, BUZZ = 8, GREEN = 9, YELLOW = 10, RED = 11;
String mood = "free", line = "";
bool lastBtn = HIGH;
unsigned long lastPress = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(BUZZ, OUTPUT);
  pinMode(GREEN, OUTPUT); pinMode(YELLOW, OUTPUT); pinMode(RED, OUTPUT);
}

void beep(int times) {
  for (int i = 0; i < times; i++) { tone(BUZZ, 880, 90); delay(130); }
}

void showMood() {
  bool blink = (millis() / 400) % 2;
  digitalWrite(GREEN,  mood == "free");
  digitalWrite(YELLOW, mood == "prep" || (mood == "go" && blink));
  digitalWrite(RED,    mood == "late" || (mood == "doomed" && blink));
}

void loop() {
  // 버튼: 눌린 순간 한 번만 (떨림 방지 250ms)
  bool b = digitalRead(BTN);
  if (b == LOW && lastBtn == HIGH && millis() - lastPress > 250) {
    Serial.println("ALARM");
    lastPress = millis();
  }
  lastBtn = b;

  // 화면에서 온 줄 읽기
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      line.trim();
      if (line.startsWith("MOOD:")) {
        mood = line.substring(5);
        if (mood == "doomed") beep(3);
      } else if (line.startsWith("ALARM:")) {
        beep(mood == "late" || mood == "doomed" ? 3 : 1);
      }
      line = "";
    } else line += c;
  }
  showMood();
}
