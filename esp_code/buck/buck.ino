// https://github.com/Mehmet-Emre-Dogan/smartHome/blob/main/customSonoff/customSonoff.ino
#include <ESP8266WiFi.h>
#include "secrets.h"
#include "pinDefines.h"
#define DEBUG true

//ota
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// webWebSerial
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

// CONSTANTS
#define MIN_ALLOWED_PWM 0 //103
#define MAX_ALLOWED_PWM 921
#define MIN_ALLOWED_SPEED 59
#define MAX_ALLOWED_SPEED 1500

#define CURRENT_COEFF 0.0740
#define CURRENT_IGNORE_THRESH 5 // 0-1023
#define CURR_MEAS_SAMPLE_COU 7

const unsigned long accelInterval = 5; // milliseconds

void ICACHE_RAM_ATTR handleBtn();

// GLOBALS
String header; // Variable to store the HTTP request

unsigned long printInterval = 500;
unsigned long lastPrintTime = 0; // milliseconds

const unsigned long analogReadInterval = 50;
unsigned long lastAnalogReadTime = 0; // milliseconds
int prevReadVal = 0;


bool isMachineEnabled = true;
bool isLeft = true;
bool isLeftPrev = true;

int motorPwmTarget = 0;
int motorPwmCurr = 0;
bool isAccelarating = true;
unsigned long lastAccelTime = 0; // milliseconds

float currentCurrent = 20;
float currentSpeed = 0;

int motorSpeedTarget = 0;
int motorSpeedCurr = 0;



// variables for delay without delay() of barrier control

// print server
AsyncWebServer printServer(8080);

void enableInterrupts(){
  attachInterrupt(digitalPinToInterrupt(BTN), handleBtn, CHANGE);
}

void disableInterrupts(){
  detachInterrupt(digitalPinToInterrupt(BTN) );
}

void handleBtn(){
  disableInterrupts();
  delay(10);

  // button mode
  while(!digitalRead(BTN)) // wait until the btn released
    delay(2);
  /*  
  isMachineEnabled = digitalRead(INDICATOR_LED);
  isMachineEnabled = !isMachineEnabled; // toggle current state

  // isMachineEnabled = !digitalRead(BTN); // switch mode, but remote control will cause problems
  digitalWrite(INDICATOR_LED, isMachineEnabled);*/
  
  enableInterrupts();
}

void setup() {
  pinMode(BLUE_LED, OUTPUT);
  pinMode(INDICATOR_LED, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(CURRENT_SENS_PIN, INPUT);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(INDICATOR_LED, isMachineEnabled);

  // Connect to Wi-Fi network with SSID and password
  WiFi.disconnect(); 
  WiFi.begin(ssid, password);
  WiFi.hostname(deviceName);
  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BLUE_LED, LOW);
    delay(400);
    digitalWrite(BLUE_LED, HIGH);                
    delay(100);                            
  }
  WiFi.config(staticIP, subnet, gateway, dns);
  webserver.begin(); // webserver

  // printserver 192.168.1.x:8080/webserial
  WebSerial.begin(&printServer); 
  WebSerial.msgCallback(recvMsg);
  printServer.begin();
  
  /////////////////////////////OTA////////////////////////////
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //WebSerial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //WebSerial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //WebSerial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //WebSerial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //WebSerial.println("Auth Failed");
      ;
    } else if (error == OTA_BEGIN_ERROR) {
      //WebSerial.println("Begin Failed");
      ;
    } else if (error == OTA_CONNECT_ERROR) {
      //WebSerial.println("Connect Failed");
      ;
    } else if (error == OTA_RECEIVE_ERROR) {
      //WebSerial.println("Receive Failed");
      ;
    } else if (error == OTA_END_ERROR) {
      //WebSerial.println("End Failed");
      ;
    }
  });
  ArduinoOTA.begin();
  ///////////////////////////////////////////////////////////
  
  enableInterrupts();
}

void loop() {
  WiFiClient client = webserver.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    if (DEBUG)  
      WebSerial.println("New Client.");          // print a message out in the WebSerial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //if (DEBUG)  
          //Serial.write(c);                    // print it out the WebSerial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // receive data
            if (header.indexOf("GET /e/run") >= 0){
              isMachineEnabled = 1;
            }
            else if (header.indexOf("GET /e/stop") >= 0){
              isMachineEnabled = 0;
            }
            else if (header.indexOf("GET /sd/?") >= 0) {
                  int index = header.indexOf('?') + 1;
                  String mystr = "";
                  for(; header[index] != '?'; index++){
                    mystr += header[index];
                  }
              int number = mystr.toInt();
              if (number >= MIN_ALLOWED_PWM && number <= MAX_ALLOWED_PWM){
                motorPwmTarget = number; 
                isAccelarating = motorPwmTarget > motorPwmCurr;
              }
            }
            else if (header.indexOf("GET /ss/?") >= 0) {
                  int index = header.indexOf('?') + 1;
                  String mystr = "";
                  for(; header[index] != '?'; index++){
                    mystr += header[index];
                  }
              int number = mystr.toInt();
              if (number >= MIN_ALLOWED_SPEED && number <= MAX_ALLOWED_SPEED){
                motorSpeedTarget = number; 
              }
            }


            if(digitalRead(INDICATOR_LED) != isMachineEnabled){
              digitalWrite(INDICATOR_LED, isMachineEnabled);
           }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("    <head>");
            client.println("<meta http-equiv='content-type' content='text/html; charset=UTF-8'>");
            client.println("<meta charset='utf-8' name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<style >");
            client.println("html {");
            client.println("  font-family: Helvetica, sans-serif;");
            client.println("  color: white;");
            client.println("  background-color:#333;");
            client.println("  justify-content: center;");
            client.println();
            client.println("}");
            client.println();
            client.println(".container{");
            client.println("  justify-content: center;");
            client.println("  display: flex;");
            client.println("  padding: 20px;");
            client.println("}");
            client.println();
            client.println("h2{");
            client.println("  text-align: center;");
            client.println("  /* font-size: 25px; */");
            client.println("}");
            client.println();
            client.println(".box {");
            client.println("  justify-content: center;");
            client.println("  text-align: justify;");
            client.println("  text-justify: auto;");
            client.println("  font-size: large;");
            client.println("  width: 90%;");
            client.println("  background-color: grey;");
            client.println("  padding: 20px;");
            client.println("  padding-top: 5px;");
            client.println("}");
            client.println();
            client.println("code{");
            client.println("  font-size: x-large;");
            client.println("  text-align: center;");
            client.println("  display: block;");
            client.println("  padding-left: auto;");
            client.println("  padding-right: auto;");
            client.println("}");
            client.println();
            client.println(".codeP{");
            client.println("  background-color: #555;");
            client.println("  width: 85%;");
            client.println("  display: flow-root;");
            client.println("  color: #00FF00;");
            client.println("  padding: 5px;");
            client.println("  border-radius: 3px;");
            client.println("  margin-left: auto;");
            client.println("  margin-right: auto;");
            client.println("}");
            client.println();
            client.println(".btn {");
            client.println("        background-color: DodgerBlue;");
            client.println("        border: none;");
            client.println("        color: white;");
            client.println("        padding: 12px 30px;");
            client.println("        cursor: pointer;");
            client.println("        font-size: 20px;");
            client.println("        margin: auto;");
            client.println("        display: inline-block;");
            client.println("        transition: all 0.2s ease-in-out;");
            client.println("        text-decoration: none;");
            client.println("        }");
            client.println();
            client.println("        /* Darker background on mouse-over */");
            client.println("        .btn:hover {");
            client.println("        background-color: RoyalBlue;");
            client.println("        transition: all 0.2s ease-in-out;");
            client.println("        transform: scale(1.1);");
            client.println("        }");
            client.println();
            client.println(".slidecontainer {");
            client.println("    width: 100%;");
            client.println("  }");
            client.println("  ");
            client.println("  .slider {");
            client.println("    /* -webkit-appearance: none; */");
            client.println("    width: 100%;");
            client.println("    height: 15px;");
            client.println("    border-radius: 5px;");
            client.println("    background: #d3d3d3;");
            client.println("    outline: none;");
            client.println("    opacity: 0.7;");
            client.println("    -webkit-transition: .2s;");
            client.println("    transition: opacity .2s;");
            client.println("  }");
            client.println("  ");
            client.println("  .slider:hover {");
            client.println("    opacity: 1;");
            client.println("  }");
            client.println("  ");
            client.println("  .slider::-webkit-slider-thumb {");
            client.println("    /* -webkit-appearance: none; */");
            client.println("    appearance: none;");
            client.println("    width: 25px;");
            client.println("    height: 25px;");
            client.println("    border-radius: 50%;");
            client.println("    background: #049faa;");
            client.println("    cursor: pointer;");
            client.println("  }");
            client.println("  ");
            client.println("  .slider::-moz-range-thumb {");
            client.println("    width: 25px;");
            client.println("    height: 25px;");
            client.println("    border-radius: 50%;");
            client.println("    background: #07788b;");
            client.println("    cursor: pointer;");
            client.println("  }");
            client.println();
            client.println(".footer {");
            client.println("  left: 0;");
            client.println("  bottom: 0;");
            client.println("  width: 100%;");
            client.println("  background-color: #333;");
            client.println("  color: white;");
            client.println("  text-align: center;");
            client.println("}");
            client.println();
            client.println("/* For desktop: */");
            client.println("@media only screen and (min-width: 800px) {");
            client.println("  html {");
            client.println("    max-width: 60%;");
            client.println("    margin: auto;");
            client.println("  }");
            client.println();
            client.println("  .codeP{");
            client.println("  background-color: #555;");
            client.println("  width: 80%;");
            client.println("  display: flow-root;");
            client.println("  color: #00FF00;");
            client.println("  padding: 5px;");
            client.println("  border-radius: 3px;");
            client.println("  margin-left: auto;");
            client.println("  margin-right: auto;");
            client.println("}");
            client.println(".slidecontainer{");
            client.println("    width: 60%;");
            client.println("    margin: auto;");
            client.println("}");
            client.println("}");
            client.println();
            client.println("  </style>");
            client.println("</head>");
            client.println("<body>");
            client.println("    <h1 style='text-align: center;'>Smart Motor Driver</h1>");
            client.println("    <h3 style='text-align: center;'><a style='text-align: center; color: white; text-decoration: none;' href='https://github.com/Mehmet-Emre-Dogan/EE463_2022_TermProject'>Volt Motors Inc.</a></h3>");
            client.println();
            client.println("        <div class='container'>");
            client.println("            <div class='box'>");
            client.println("                <h2>Stats</h2>");
            client.println();
            client.println("                <div class='slidecontainer'>");
            client.println("                    <code>");
            /*
            client.println("                        <p class='codeP' style='font-size: large;'>");
            client.println("                            Speed [rpm]: --- <br>");
            client.println("                            Duty [/1023]: --- <br>");
            client.println("                            Current [A]: --- <br>");
            client.println("                            Temp [??C]: --- <br>");
            client.println("                            Fan Speed [/1023]: --- <br>");
            client.println("                            ");
            client.println("                        </p>");
            */
            client.println("                        <p class='codeP'>Control Loop: Open<br>");
            client.println("                          Direction: Left ");
            client.println("                        </p>");
            client.println("                        ");
            client.println("                        <a href='/ctrl/toggle'><button class='btn'>Toggle Loop</button></a>");
            client.println("                        <a href='http://192.168.1.80:8080/webserial'><button class='btn'>Open Serial</button></a>");
            client.println("                    <code>");
            client.println("                </div>");
            client.println("            </div>");
            client.println("        </div> ");
            client.println();
            client.println("        <div class='container'>");
            client.println("          <div class='box'>");
            client.println("              <h2>Set Direction</h2>");
            client.println("              <div class='slidecontainer'>");
            client.println("                  <code>");
            client.println("                    <h3 class='codeP' style='color:rgb(253, 98, 98); text-decoration: underline;'>WARNING!</h3>");
            client.println("                      <p class='codeP' style='color:rgb(252, 206, 108);'>Stop before changing direction!<br> ");
            client.println("                        Our company is not responsible for the damages due to user errors.");
            client.println("                      </p>");
            client.println("                      <br>");
            client.println("                      ");
            client.println("                      <a href='/dir/left'><button class='btn'>Left</button></a>");
            client.println("                      <a href='/dir/stop'><button class='btn'>Stop</button></a>");
            client.println("                      <a href='/dir/right'><button class='btn'>Right</button></a>");
            client.println("                  <code>");
            client.println("              </div>");
            client.println("          </div>");
            client.println("      </div> ");
            client.println();
            client.println("        <div class='container'>");
            client.println("            <div class='box'>");
            client.println("                <h2>Open Loop</h2>");
            client.println("                ");
            client.println("                <div class='slidecontainer'>");
            client.println("                    <code>");
            client.println("                        <p class='codeP'>Duty [/1023]: <span id='dutyValue'></span></p>");
            client.println("                        <p class='codeP'>Duty [%]: <span id='dutyPercent'></span></p>");
            client.println("                        <br>");
            client.println("                        <input type='range' min='103' max='921' value='" + String(motorPwmTarget) + "' class='slider' id='dutySlider', oninput='displayDuty()'>");
            client.println("                        <br> <br> <br>");
            client.println("                        <span id='saDuty'>");
            client.println("                            <!-- <a id='aDuty' href='#'><button class='btn'>Set Duty</button></a> -->");
            client.println("                        </span>");
            client.println("                    <code>");
            client.println("                </div>");
            client.println("            </div>");
            client.println("        </div> ");
            client.println();
            client.println("        <div class='container'>");
            client.println("            <div class='box'>");
            client.println("                <h2>Close Loop</h2>");
            client.println();
            client.println("                <div class='slidecontainer'>");
            client.println("                    <code>");
            client.println("                        <p class='codeP'>Speed [rpm]: <span id='speedValue'></span></p>");
            client.println("                        <p class='codeP'>Speed [%]: <span id='speedPercent'></span></p>");
            client.println("                        <br>");
            client.println("                        <input type='range' min='60' max='1500' value='50' class='slider' id='speedSlider', oninput='displaySpeed()'>");
            client.println("                        <br> <br> <br>");
            client.println("                        <span id='saSpeed'>");
            client.println("                            <a id='aSpeed' href='#'><button class='btn'>Set Speed</button></a>");
            client.println("                        </span>");
            client.println("                    <code>");
            client.println("                </div>");
            client.println("            </div>");
            client.println("        </div> ");
            client.println();
            client.println("        <br>");
            client.println("        <br>");
            client.println();
            client.println("        <div class='footer'>?? 2022 <br> Web design by medogan <a style='color: #049faa;' href='https://medogan.com'>medogan.com</a></div>");
            client.println("        <br>");
            client.println("        <script>");
            client.println("          function displayDuty(){");
            client.println("            var slider = document.getElementById('dutySlider');");
            client.println("            var dv = document.getElementById('dutyValue');");
            client.println("            var dvpc = document.getElementById('dutyPercent');");
            client.println("            dv.innerHTML = slider.value;");
            client.println("            dvpc.innerHTML = String((Number(slider.value)/1023*100).toFixed(3));");
            client.println();
            client.println("            document.getElementById('saDuty').innerHTML = '<a id=\"aDuty\" href=' + '/sd/?' + slider.value + '?><button class=\"btn\">Set Duty</button></a>';");
            client.println("          }");
            client.println("          function displaySpeed(){");
            client.println("            var slider = document.getElementById('speedSlider');");
            client.println("            var sv = document.getElementById('speedValue');");
            client.println("            var svpc = document.getElementById('speedPercent');");
            client.println("            sv.innerHTML = slider.value;");
            client.println("            svpc.innerHTML = String((Number(slider.value)/1500*100).toFixed(3));");
            client.println();
            client.println("            document.getElementById('saSpeed').innerHTML = '<a id=\"aSpeed\" href=' + '/ss/?' + slider.value + '?><button class=\"btn\">Set Speed</button></a>';");
            client.println("          }");
            client.println("          </script>");
            client.println();
            client.println("        <script>");
            client.println("            displayDuty()");
            client.println("            displaySpeed()");
            client.println("        </script>");
            client.println("</body>");
            client.println("</html>");







            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }

  if (isMachineEnabled){
    drive();
  }
  else{
    shutDown();
  }

  
  if (millis() - lastAnalogReadTime >= analogReadInterval){
    measureCurrentAverage(CURR_MEAS_SAMPLE_COU);
    lastAnalogReadTime = millis();
  } 
  
  printStats();
  ArduinoOTA.handle();
}

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("--> Received Data...");
  int num = atoi( (const char *) data );
  /*
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }*/
  printInterval = num;
  char buf[100];
  sprintf(buf, "--> Print interval is set to %d ms.", num);
  WebSerial.println(buf);
}

void drive(){
  calcRamp();
  adjustMotorOL();
}

void adjustMotorOL(){
  if (isLeft == isLeftPrev){
    writeMotor();
  }
  else{
    ;//TODO
  }

}

void writeMotor(){
  analogWrite(MOTOR_PIN, motorPwmCurr);
}

void calcRamp(){
  if (millis() - lastAccelTime >=  accelInterval){
    if (isAccelarating && motorPwmCurr < motorPwmTarget)
      motorPwmCurr++;
    else if (!isAccelarating && motorPwmCurr > motorPwmTarget)
      motorPwmCurr--;
    lastAccelTime = millis();
  }
  
}

int measureCurrentRaw(){
  int output = 0;
  output = (512 - analogRead(CURRENT_SENS_PIN));
  if (output < 0)
    output *= -1;
  output -= CURRENT_IGNORE_THRESH; // Threshold
  if(output < 0)
    output = 0;
  if(output > 512)
    output = 512;
  return output;
}

void measureCurrentAverage(int sampleCount){
  int averageOutput = 0;
  for(int i = 0; i < sampleCount; i++){
    averageOutput += measureCurrentRaw();
  }
  averageOutput = averageOutput/sampleCount;
  
  // map(value, fromLow, fromHigh, toLow, toHigh)
  
  currentCurrent = ( (float) averageOutput) * CURRENT_COEFF; // Sensivity, i.e. multiplier
}

void printStats(){
  if (millis() - lastPrintTime >=  printInterval){
    char timestr[40];
    sprintf(timestr,"t: %u", (millis()/100) );
    WebSerial.print(timestr);
    WebSerial.print(" motorPwmCurr:");
    WebSerial.print(motorPwmCurr);
    WebSerial.print(" currentCurr:");
    WebSerial.print(currentCurrent);
    WebSerial.print(" spdCurr:");
    WebSerial.print(currentSpeed);
    WebSerial.println();

    lastPrintTime = millis();
}
}

void shutDown(){
;
}
