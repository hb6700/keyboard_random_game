#define SERIAL_RATE (9600)
#define SERIAL_DELAY (50)
#define PORT_RAND (A1)
#define MAX_TYPE_CHAR (26 + 26 + 10 + 5)

int nTotalScore = 0;          //합산 스코어
int nTotalInputTime = 0;      //총 입력 시간(ms)

//init(타자 초기화)
void initRand(){
  int nVolt = analogRead(PORT_RAND);   // analogRead() @ A0 port: 0(0V)~1023(5V)<->  digitalWrite()
  randomSeed(nVolt);    //난수 초기화
}

// 타자 초기화
void initGame(){
  Serial.begin(SERIAL_RATE);
  initRand();
  nTotalScore = 0;
}

//String methods
//Serial → String
String getSerialInput() {
  String str;                       //length() == 0
  while (Serial.available() > 0)    //Data input
  {
    char c = Serial.read();
    str += c;
    delay(SERIAL_DELAY);
  }
  return str;
}

//Scanf : Serial → String
String scanStr() {
  while (Serial.available() <= 0);      //wait
  return getSerialInput();
}

//Scanf : Serial → String → int
int scanInt() {
  String str = scanStr();
  return str.toInt();
}

void clearSerialInput() {
  while (Serial.available() > 0)    //Data input
  {
    Serial.read();
    delay(SERIAL_DELAY);
  }
}

//Random Sentence(난수 문장)
char toRandChar(int nRand){
  if (nRand <= 25) return (char)('a' + nRand);
  else if (nRand <= 51) return (char)('A' + nRand - 26);
  else if (nRand <= 61) return (char)('0' + nRand - 52);
  else if (nRand == 62) return '.';
  else if (nRand == 63) return ',';
  else if (nRand == 64) return '?';
  else if (nRand == 65) return '!';
  //else if (nRand == 66) return ' ';
  else return ' ';
}

//난수 문장 발생기
String makeRandSentence(int nRandSize, int mode){
  String sRand;
  //반복문
  for (int i = 0 ; i < nRandSize ; i++){ 
    int nRand = 0;
    if(mode == 1) //전체
    {
      nRand = random(MAX_TYPE_CHAR);
    }
    else  //영문만
    {
      nRand = random(MAX_TYPE_CHAR)%52;    //
    }
    char ch = toRandChar(nRand);      //int > char
    sRand += ch;
  }
  return sRand;
}

//Typing Methods
//사용자 입력, 입력시간 측정
String getUserInput(int& nInputTime) {
  nInputTime = 0;
  clearSerialInput();
  int nStartTime = millis();
  String sInput = scanStr();
  int nStopTime = millis();
  nInputTime = nStopTime - nStartTime;    //in msec
  return sInput;
}

//return score
int compareSentences(String sRand, String sInput) {
  //문장 비교
  int nScore = 0;
  int nRandLen = sRand.length();
  int nInputLen = sInput.length();
  int nMinLen = (nRandLen < nInputLen) ? nRandLen : nInputLen;
  for (int i = 0 ; i < nMinLen ; i++) {
    if (sRand[i] == sInput[i]) nScore++;
    else nScore--;
  }
  return nScore - abs(nRandLen - nInputLen);    //abs : |x|
}

//nTotalScore, nTotalInputTime
void updateStat(int nUserScore, int nInputTime) {
  //타자 통계
  nTotalScore += nUserScore;
  nTotalInputTime += nInputTime;
}

//nTotalScore, nTotalInputTime
void printStat() {
  //통계 출력
  double typeSpeed = nTotalScore/(double)(nTotalInputTime*1e-3)*60;
  Serial.println("전체 스코어 = " + String(nTotalScore));
  Serial.println("평균 분당 타자수 = " + String(typeSpeed));
}

void setup() {
  // put your setup code here, to run once:
  initGame();
}

void loop() {
  // put your main code here, to run repeatedly:
  int nInputTime = 0;
  int nInputLevel = 0;
  String i = "1";
  Serial.println("영문자 연습을 원하시면 E를 입력하세요 ");
  Serial.print("모드입력 : ");
  String sMode = getUserInput(nInputTime);    //모드 입력받기
  String sRand = ""; 
  Serial.println(sMode);
  if(sMode == "E"){
    Serial.println("영어만 연습합니다.");
  }
  else{
    Serial.println("영문자 숫자 특수문자가 등장합니다.");
  }
  Serial.print("[3:상, 2:중, 1:하] 난이도를 입력하세요 : ");
  String sLevel = getUserInput(nInputLevel);  //난이도 입력받기
  Serial.println(sLevel);
  int nRandSize;
  if(sLevel == "1") {
    nRandSize = 10;
  }
  else if(sLevel == "2") {
    nRandSize = 20;
  }
  else if(sLevel == "3") {
    nRandSize = 30;
  }

  while(i == "1"){
    if(sMode == "E"){  
      sRand = makeRandSentence(nRandSize, 0); 
    }
    else{
      sRand = makeRandSentence(nRandSize, 1);    //난수문장 생성 부분
    }
    Serial.println(sRand);
    String sInput = getUserInput(nInputTime);
    Serial.println(sInput);
    Serial.println("입력 시간 = " + String(nInputTime*1e-3) + " sec");
    long nUserScore = compareSentences(sRand, sInput);
    Serial.println("현재 스코어 = " + String(nUserScore));
    updateStat(nUserScore, nInputTime);
    printStat();
    Serial.println("===========================");
    Serial.print("계속 하시려면 1번, 종료하려면 2번 : ");
    i = getUserInput(nInputTime);   
    Serial.println(i);
  }
  Serial.println("타자연습을 종료합니다");
  Serial.println("===========================");
}
