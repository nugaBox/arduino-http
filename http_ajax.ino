/*

* Sketch: ESP8266_LED_CONTROL_AJAX_02 * Intended to be run on an ESP8266 */  
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

#include <ESP8266WiFi.h>   // change these values to match your network 
char ssid[] = "SIIRU-4F"; // your network SSID (name) 
char pass[] = "#comin2131*"; // your network password   
WiFiServer server(80);     
String request = ""; 
int LED_Pin = D3;     

void setup() { 
  // 초기 세팅
  pinMode(LED_Pin, OUTPUT);   
  Serial.begin(9600); 
  Serial.println(); 
  Serial.println("Serial started at 9600"); 
  Serial.println("ESP8266_LED_CONTROL_AJAX_02"); 
  Serial.println();   // Connect to a WiFi network 
  Serial.print(F("Connecting to ")); 
  Serial.println(ssid); 

  // Wi-Fi 연결
  WiFi.begin(ssid, pass);   
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); delay(500); 
  }  
  Serial.println(""); 
  Serial.println(F("[CONNECTED]")); 
  Serial.print("[IP "); 
  Serial.print(WiFi.localIP()); 
  Serial.println("]");   // start a server 
  
  // 웹서버 시작 
  server.begin(); 
  Serial.println("Server started");   
}
    
void loop() {
  // 서버 작동 확인
  WiFiClient client = server.available(); 
  if (!client) { 
    return; 
  }

  // 요청 값 처리
  req = client.readStringUntil('\r');
  
  // 브라우저의 favicon.ico 요청 제외
  if ( req.length() != 25) { 
    Serial.print("request: "); 
    Serial.println(req);
    // 시간 설정
    if ( req.indexOf("set_min") > 0 ) { 
      String req_min = req.substring(13,15);
      int min = req_min.toInt();
      Serial.print("SET_MIN : ");
      Serial.print(min);
      Serial.println();
      digitalWrite(LED_Pin, HIGH);
      delay(min*1000); 
      digitalWrite(LED_Pin, LOW); 
    } 
    // Request 값 없을 때
    else { 
      boolean pinStatus = digitalRead(LED_Pin); 
      Serial.println("NO REQUEST");
      digitalWrite(LED_Pin, HIGH);
      delay(1000); 
      digitalWrite(LED_Pin, LOW); 

      // HTML 소스 로드
      client.flush(); 
      client.print( header ); 
      client.print( html );
      client.print( htmlscript );
      delay(5); 
    }     
  }
}