// HTML 소스
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";   
String html = R"=====(
<!doctype html>
<html lang="ko">
<head>
<meta charset="utf-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge" />
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=2.0, user-scalable=yes">
<meta name="format-detection" content="telephone=no, address=no, email=no">
<title>Alarm Setting System</title>
<link href="https://design.comin.com/alarm/css/basic.css" rel="stylesheet" type="text/css">
<link href="https://design.comin.com/alarm/css/all.css" rel="stylesheet" type="text/css">
<link href="https://design.comin.com/alarm/css/layout.css" rel="stylesheet" type="text/css">
</head>
<body>  
<div class="wrap">
  <header class="header_area">
    <i class="far fa-stopwatch"></i>
    <p>Alarm Setting System</p>
    <button class="btn_call"><i class="fas fa-volume-up mr10"></i>바로<span>호출</span></button>
  </header>
  <section class="body_area">
      <ul class="active_list">
        <li><input type="checkbox" class="light" id="light01" checked><label for="light01">01</label></li>
        <li><input type="checkbox" class="light" id="light02" checked><label for="light02">02</label></li>
        <li><input type="checkbox" class="light" id="light03" checked><label for="light03">03</label></li>
        <li><input type="checkbox" class="light" id="light04"><label for="light04">04</label></li>
        <li><input type="checkbox" class="light" id="light05"><label for="light05">05</label></li>
        <li><input type="checkbox" class="light" id="light06"><label for="light06">06</label></li>
        <li><input type="checkbox" class="light" id="light07"><label for="light07">07</label></li>
        <li><input type="checkbox" class="light" id="light08"><label for="light08">08</label></li>
        <li><input type="checkbox" class="light" id="light09"><label for="light09">09</label></li>
        <li><input type="checkbox" class="light" id="light10"><label for="light10">10</label></li>
      </ul>
      <div class="set_board">
        <ul class="set_minute">
          <li><input type="radio" name="set_min" id="set_min01" value="05" checked><label for="set_min01">5 min</label></li>
          <li><input type="radio" name="set_min" id="set_min02" value="10"><label for="set_min02">10 min</label></li>
          <li><input type="radio" name="set_min" id="set_min03" value="15"><label for="set_min03">15 min</label></li>
          <li><input type="radio" name="set_min" id="set_min04" value="20"><label for="set_min04">20 min</label></li>
        </ul>
        
        <ul class="set_sound">
          <li><input type="checkbox" class="set_all" id="set_all" value="0"  onclick="javascript:cAll();"><label for="set_all" id="set_all_label">모두선택</label></li>
          <li><input type="checkbox" class="set_sd" id="set_sd01" value="1" ><label for="set_sd01">음성</label></li>
          <li><input type="checkbox" class="set_sd" id="set_sd02" value="2" ><label for="set_sd02">진동</label></li>
          <li><input type="checkbox" class="set_sd" id="set_sd03" value="3" ><label for="set_sd03">소리</label></li>
        </ul>
      </div>
      <div class="send_wrap">
        <input type="submit" class="btn_send" value="전송" onclick="sendData(); return false;" />
      </div>
  </section>
</div>
)=====";
String htmlscript = R"=====(
<script src="https://design.comin.com/alarm/js/jquery.min.js"></script>
<script>
  function cAll() {
    if ($("#set_all").is(':checked')) {
      $(".set_sd").prop("checked", true);
    } else {
      $(".set_sd").prop("checked", false);
    }
  }
</script>
<script>
  function sendData() {
    var min = $('input[name=set_min]:checked').val();
    ajaxLoad('set_min='+min);
  }
  var ajaxRequest = null; 
  if (window.XMLHttpRequest) { 
      ajaxRequest =new XMLHttpRequest(); 
  } else { 
      ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); 
  } 
  function ajaxLoad(ajaxURL) { 
    if(!ajaxRequest){ 
      alert("AJAX is not supported."); 
      return; 
    }   
    ajaxRequest.open("GET",ajaxURL,true); 
    ajaxRequest.onreadystatechange = function() { 
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200) { 
            var ajaxResult = ajaxRequest.responseText; 
        } 
    }; 
    ajaxRequest.send(); 
  } 
</script>
</body>
</html>
)=====";

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <DFPlayerMini_Fast.h>

#if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13); //MP3 PLAY MINI RX, TX
#endif

DFPlayerMini_Fast myMP3;

#ifndef STASSID
#define STASSID "SIIRU-3F"
#define STAPSK  "#comin2131*"
#define LAMP1 4
#define LAMP2 5
#define MOTOR 16
 
#endif
Ticker flipper;
const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

unsigned int t_count,buf_count, count=0;
unsigned char rx_data = 0;

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637Display display(2, 3);

void flip() {
  if(t_count && ++buf_count >= 10)//타임 data 수신 자료 1s마다 처리
  {
    buf_count = 0;  
    --t_count;  
    display.showNumberDec(t_count, false);
    
  }
  
  switch( rx_data ){
    case 1:       //5분 타임 수신
      t_count = 60*min+1;
      rx_data =0;
      myMP3.play(1);
      digitalWrite(LAMP1, 0);
      digitalWrite(LAMP2, 0);
      digitalWrite(MOTOR, 0);
      break;
      
    case 2:    //10분 타임 수신
      t_count = 60*10+1;
      rx_data =0;
      myMP3.play(1);
      digitalWrite(LAMP1, 0);
      digitalWrite(LAMP2, 0);
      digitalWrite(MOTOR, 0);
      break;
      
    case 3:   //램프 점멸
      count++;
      myMP3.play(2);
      if(count % 2){
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 1);
      }
      else{
        digitalWrite(LAMP1, 1); 
        digitalWrite(LAMP2, 0);   
      }
      if(count > (7*5)) //램프 점등 횟수
      {
        count = 0;
        rx_data= 0;
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 0);
      }
      break;   
      
    case 4:   //진동 모터 동작
      count++;
      myMP3.play(2);
      if(count % 5) digitalWrite(MOTOR, 0);
      else digitalWrite(MOTOR, 1);    
      if(count > (7*5)) //모터 반복 7회
      {
        count = 0;
        rx_data= 0;
        t_count =0;//음식 제공 완료이므로 타임0으로 표시
        digitalWrite(MOTOR, 0);
      }
      break;   
            
    case 5:   //램프, 진동모터 동작
      count++;
      myMP3.play(2);
      if(count % 2){
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 1);
      }
      else
      {
        digitalWrite(LAMP1, 1);
        digitalWrite(LAMP2, 0);
      }
 
      if(count % 5) digitalWrite(MOTOR, 0);
      else digitalWrite(MOTOR, 1);
    
      if(count > (7*5)) //모터 반복 7회
      {
        count = 0;
        rx_data= 0;
        t_count =0;//음식 제공 완료이므로 타임0으로 표시
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 0);
        digitalWrite(MOTOR, 0);
      }
      break;
    case 6:   //램프, 진동모터 , 타이머 0
      count++;
      myMP3.play(2);
      if(count % 2){
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 1);
      }
      else{
        digitalWrite(LAMP1, 1);
        digitalWrite(LAMP2, 0);
      }
 
      if(count % 5) digitalWrite(MOTOR, 0);
      else digitalWrite(MOTOR, 1);
    
      if(count > (7*5)) //모터 반복 7회
      {
        count = 0;
        rx_data= 0;
        t_count =0;//음식 제공 완료이므로 타임0으로 표시
        display.showNumberDec(t_count, false);
        digitalWrite(LAMP1, 0);
        digitalWrite(LAMP2, 0);
        digitalWrite(MOTOR, 0);
      }
      break;
  //default:
  }
}

void setup() {
  Serial.begin(9600);
  
  #if !defined(UBRR1H)
    mySerial.begin(9600);
    myMP3.begin(mySerial, true);
  #else
    Serial1.begin(9600);
    myMP3.begin(Serial1, true);
  #endif
  
  Serial.println("Setting volume 15");
  myMP3.volume(15);

  pinMode(LAMP1, OUTPUT);           //LAMP1
  pinMode(LAMP2, OUTPUT);           //LAMP2
  pinMode(MOTOR, OUTPUT);           //MOTOR

  flipper.attach(0.1, flip);  //100ms타이머
  display.setBrightness(2);   //fnd 밝기
  display.showNumberDec(0, false);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() { 

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // 요청 값 처리
  if ( req.length() != 25) { 
    // 시간 설정
    if ( req.indexOf("set_min") > 0 ) { 
      String req_min = req.substring(13,15);
      int min = req_min.toInt();
    } 
  }

  // Match the request
  int val;
  if (req.indexOf(F("/gpio/1")) != -1)      rx_data = 1;
  else if (req.indexOf(F("/gpio/2")) != -1) rx_data = 2;
  else if (req.indexOf(F("/gpio/3")) != -1) rx_data = 3;
  else if (req.indexOf(F("/gpio/4")) != -1) rx_data = 4;
  else if (req.indexOf(F("/gpio/5")) != -1) rx_data = 5;
  else if (req.indexOf(F("/gpio/6")) != -1) rx_data = 6;
  else {
    Serial.println(F("invalid request"));
    val = digitalRead(LED_BUILTIN);
  }

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print( header ); 
  client.print( html );
  client.print( htmlscript );
//   client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
//   client.print((val) ? F("high") : F("low"));
//   client.print(F("<br><br>Click <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/1'>111111</a> to switch LED GPIO on, or <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/2'>222222</a> to switch LED GPIO off.</html>"));
  
//   client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
//   client.print((val) ? F("high") : F("low"));
//   client.print(F("<br><br>Click <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/3'>333333</a> to switch LED GPIO on, or <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/4'>444444</a> to switch LED GPIO off.</html>"));
  
//   client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
//   client.print((val) ? F("high") : F("low"));
//   client.print(F("<br><br>Click <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/5'>555555</a> to switch LED GPIO on, or <a href='http://"));
//   client.print(WiFi.localIP());
//   client.print(F("/gpio/6'>666666</a> to switch LED GPIO off.</html>"));

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}