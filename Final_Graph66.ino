
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include "DHT.h"

#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

//웹서버의 포트는 기본 포트인 80번을 사용한다!
ESP8266WebServer server(80);
//웹서버와 웹클라이언트가 뒷구멍으로 주고받는 데이터는 웹소켓에서 81번을 쓴다!
WebSocketsServer webSocket = WebSocketsServer(81);

String response = "\
<html>\
<head>\
<title>WHEATHER</title>\
<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.1.4/Chart.bundle.min.js\"></script>\
<script type=\"text/javascript\" charset=\"utf-8\" src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.1/Chart.min.js\"></script>\
<meta name=\"viewport\" content=\"width=device-width\">\
<meta charset=\"utf-8\">\
<script>\
  var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);\
  connection.onopen = function () {\
     connection.send('Connect ' + new Date());\
  };\ 
  connection.onerror = function (error) {\
     console.log('WebSocket Error ', error);\
  };\
  connection.onmessage = function (e) {\
     console.log('Server: ', e.data);\
     var mydata = JSON.parse(e.data);\
     chart.data.datasets[1].data.shift();\
     chart.data.datasets[1].data.push(mydata.humi);\
     chart.data.datasets[0].data.shift();\
     chart.data.datasets[0].data.push(mydata.temp);\
     chart.update();\
     var temp = (mydata.temp + 50)/100;\
     chart2.data.datasets[0].data[0] = temp;\
     chart2.data.datasets[0].data[1] = 1-temp;\
     var humi = mydata.humi/100;\
     chart3.data.datasets[0].data[0] = humi;\
     chart3.data.datasets[0].data[1] = 1-humi;\
     document.getElementById(\"cap1\").innerHTML = mydata.temp + \"'C\";\
     document.getElementById(\"cap2\").innerHTML = mydata.humi + \"%\";\
     chart2.update();\
     chart3.update();\
  };\
  function send_msg(){\
     connection.send('녹칸다 포에버!!!');\
  }\
</script>\
<style>\
//#intro_bg{\
//width: 100%;\
//height: 714px;\
//}\
body{\
background-color:#eeeeee;\
}\
#btn{\
border: 1px solid blue;\
background-color: rgba(0,0,0,0);\
color: blue;\
padding: 5px;\
border-top-left-radius: 5px;\
border-bottom-left-radius: 5px;\
border-top-right-radius: 5px;\
border-bottom-right-radius: 5px;\
margin-right:-4px;\
height:40px;\
}\
table {\
border-collapse: collapse;\
width:1000px;\
margin-left:auto;\
margin-right:auto;\
}\
table, td, th {\
border: solid 1px #cccccc;\
background-color: white;\
}\
tr {\
height:40px;\
text-align: center;\
}\
h1{\
  font-size: 48px;\
}\
td, th {\
padding: 5px;\
}\
li{\
  margin-left:82;\
}\
ul{\
  list-style:none;\
}\
li{\
  border: 0;\
  float:left;\
}\
.set1 {\
background-color: red;\
color: white;\
text-decoration: underline;\
font-weight: bold;\
}\
#c_head {\
height:30px;\
background-color: #eeeeee;\
}\
#c_width{\
width: 500px;\
}\
#contents1{\
font-size: 20px;\
font-weight: lighter;\
}\
</style>\
</head>\
<body>\
<div id=\"wrap\">\
            <div id=\"intro_bg\">\
                <div id=\"header\" style=\"position:relative; display:flex; width:1280px; margin:auto; height: 86px;\">\
                    <div id=\"searchArea\" style=\"position:relative; width: 300px; height:40px; background: rgba(0, 0, 0, 0, 5); margin-top:24px; \">\
                        <form>\
                            <input type=\"search\" placeholder=\"Search\">\
                            <span>\검색</span>\
                         </form>\
                    </div>\
                    <ul id =\"nav\" style=\"position:relative; line-height: 86px; width:calc(1280px-300px);\">\
                        <li><a href=\"#\">\HOME</a>\</li>\ 
                        <li><a href=\"#\">\ABOUT</a>\</li>\
                        <li><a href=\"#\">\SEARCH</a>\</li>\
                        <li><a href=\"#\">\CONTACT</a>\</li>\
                    </ul>\
                </div>\
                <div id =\"intro_text\" style=\"position:relative; text-align : center; width:1000px; margin:auto;\">\
                    <h1>날씨 정보 DATA</h1>\
                    <h4 id=\"contents1\">esp8266 센서를 이용해 D1R1 보드로 온도와 습도를 출력하였다.</h4>\
                </div>\    
            </div>\
        </div>\
<BR>\
<table>\
<tr id=\"c_head\"><th>온도('C)</th><th>습도(%)</th></tr>\
<tr id=\"c_head\">\
   <th>\
    <div style=\"position:relative;width:250px; height:200px;  margin-left:auto; margin-right:auto;\">\
      <canvas id=\"donut1\" style=\"position:absolute; text-align:center; width:800px;\"></canvas>\
      <div id=\"cap1\" style=\"position:absolute; margin-left:auto; margin-right:auto; top:100px;left:0px;text-align:center;width:100%;font-size:40px;font-family:Arial, sans-serif;\">\
      -'C\
      </div>\
    </div>\
    </th>\
   <th>\
    <div style=\"position:relative;width:250px;  margin-left:auto; margin-right:auto; height:200px;\" width:800px;\">\
      <canvas id=\"donut2\" style=\"position:absolute;  margin-left:auto; margin-right:auto; \"></canvas>\
      <div id=\"cap2\" style=\"position:absolute;top:100px;left:0px; margin-left:auto; margin-right:auto;;width:100%;font-size:40px;font-family:Arial, sans-serif;\">\
      -'C\
      </div>\
    </div>\
    </th>\
</tr><tr><td colspan=2>\
<div style=\"width:800px; text-align:center;\  margin-left:auto; margin-right:auto;\">\
<canvas id=\"line1\" style=\"position:relative; width: 800px; text-align:center;\"></canvas>\
</div>\
</td></tr></table>\
<script>\
var t_value = 25.12;\
var t_color = 'red';\
var num = (t_value + 50)/100; \
var data = { labels: [ 'Temperature' ], datasets: [ { data: [num, 1-num], backgroundColor: [t_color ], hoverBackgroundColor: [t_color] }] }; \
var ctx2 = document.getElementById('donut1').getContext('2d');\
var chart2 = new Chart(ctx2, {\
type: 'doughnut',\
data: data,\
options: { \
   responsive: true, \
   legend: { display: false }, \
   elements: { center: { text: Math.round(num*100), fontStyle: 'Helvetica', sidePadding: 15 } }, \
   maintainAspectRatio : false, \
   cutoutPercentage:70, \
   rotation: 1 * Math.PI,\ 
   circumference: 1 * Math.PI } \
});\
var h_value = 50.12;\
var h_color = 'blue';\
var num2 = h_value/100; \
var data2 = { labels: [ 'Huminity' ], datasets: [ { data: [num2, 1-num2], backgroundColor: [ h_color ], hoverBackgroundColor: [ h_color] }] }; \
var ctx3 = document.getElementById('donut2').getContext('2d');\
var chart3 = new Chart(ctx3, {\
type: 'doughnut',\
data: data2,\
options: { \
   responsive: true, \
   legend: { display: false }, \
   elements: { center: { text: Math.round(num*100), fontStyle: 'Helvetica', sidePadding: 15 } }, \
   maintainAspectRatio : false, \
   cutoutPercentage:70, \
   rotation: 1 * Math.PI,\ 
   circumference: 1 * Math.PI } \
});\
var ctx = document.getElementById('line1').getContext('2d');\
var chart = new Chart(ctx, {\
type: 'line',\
data: {\
labels: ['N-7', 'N-6', 'N-5', 'N-4', 'N-3', 'N-2', 'N-1', 'N'],\
datasets: [\
{\
label: \"온도('C)\",\
backgroundColor: 'transparent',\
borderColor: t_color,\
data: [0, 0, 0, 0, 0, 0, 0, 0]\
},\
{\
label: '습도(%)',\
backgroundColor: 'transparent',\
borderColor: h_color,\
data: [0, 0, 0, 0, 0, 0, 0, 0]\
}\
]\
},\
options: {}\
});\
</script>\
</body>\
</html>";


//클라이언트에서 서버쪽으로 값이 전송되었을때 뭘할거냐?
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            //num = 소켓번호(클라이언트번호)
            //webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
        //메시지 수신부
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);


            break;
    }

}

void setup() {
    //USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);
    dht.begin();
    
    //USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    //자기자신의 IP공유기 ID비밀번호 집어넣는곳!
    WiFiMulti.addAP("mos", "00000000");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    //IP공유기로부터 할당받은 IP주소를 여기서 출력한다!
    USE_SERIAL.println("IP address: ");
    USE_SERIAL.println(WiFi.localIP());
  
    //웹소켓 서버를 연다
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    //윈도우10, 안드로이드 안됨..의미없는 기능
    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
    }

    //웹서버의 index.html
    //웹서버가 클라이언트에게 response해주는 부분!

    server.on("/", []() {
        server.send(200, "text/html", response);
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
}

unsigned long last_10sec = 0;
unsigned int counter = 0;

void loop() {
    unsigned long t = millis();
    webSocket.loop(); //이거 있어야한다!
    server.handleClient(); //이거도 있어야한다!

    //delay(~~~~) 절때 쓰면 안됨!

    //1초간격으로 뭔가 하겠다~
    if((t - last_10sec) > 1000) {
      last_10sec = millis();
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      String msg = "{\"temp\":"+String(t)+",\"humi\":"+String(h)+"}";
      //{"temp":~~,"humi":~~~}
      
      webSocket.broadcastTXT(msg); //모든클라이언트에게 메시지 전송!
        
    }
}
