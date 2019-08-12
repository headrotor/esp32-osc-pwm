#include <ArduinoOSC.h>
#include "secrets.h"
// from https://github.com/hideakitai/ArduinoOSC
// install via library manager


#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C   u8g2(U8G2_R0, 16, 15, 4);


// WiFi stuff in secrets.h
//const char* ssid = "TP-LINK_9BBF8F";
//const char* pwd = "hahanope";

const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
OscWiFi osc;
const char* host = "192.168.1.123";
//const char* host = "192.168.1.101";
const int recv_port = 10000;
const int send_port = 12000;


float f1 = 0.;
float f2 = 0.;
float f3 = 0.;
float f4 = 0.;
float f5 = 0.5;

int freq = 5000;
int res = 8;
int pwm_max = (0x01 << 8) - 1;


// push 1, 2 increments and decrements this
int incr0 = 8;
// push 3, 4 increments and decrements this
int incr1 = 8;

#define INCR0_MAX 16
#define INCR1_MAX 16



void onOscReceived(OscMessage& m)
{
  Serial.print("callback : ");
  Serial.print(m.ip()); Serial.print(" ");
  Serial.print(m.port()); Serial.print(" ");
  Serial.print(m.size()); Serial.print(" ");
  Serial.print(m.address()); Serial.print(" ");
  Serial.print(m.arg<int>(0)); Serial.print(" ");
  Serial.print(m.arg<float>(1)); Serial.print(" ");
  Serial.print(m.arg<String>(2)); Serial.println();
}

void setup()
{
  Serial.begin(115200);

  // WiFi stuff
  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());


  // LCD display
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);


  // ArduinoOSC
  osc.begin(recv_port);


  osc.subscribe("/1/fader1", [](OscMessage & m)
  {
    //Serial.print("fader1 : ");
    //Serial.println(m.arg<float>(0));
    f1 = m.arg<float>(0);
    change_pwm();
    update_display();
  });
  osc.subscribe("/1/fader2", [](OscMessage & m)
  {
    //Serial.print("fader2 : ");
    //Serial.println(m.arg<float>(0));
    f2 = m.arg<float>(0);
    change_pwm();
    update_display();
  });

  osc.subscribe("/1/fader5", [](OscMessage & m)
  {
    Serial.print("fader5 : ");
    Serial.println(m.arg<float>(0));
    f5 = m.arg<float>(0);
    setup_PWM();
    update_display();
  });


  osc.subscribe("/2/push1", [](OscMessage & m)
  {
    if ( m.arg<float>(0) > 0.5) {
      Serial.print("push1 : ");
      Serial.println(m.arg<float>(0));
      incr0 = constrain(incr0 + 1, 0, INCR0_MAX);
    }
    setup_PWM();
    update_display();
  });
  osc.subscribe("/2/push2", [](OscMessage & m)
  {

    if ( m.arg<float>(0) > 0.5) {
      Serial.print("push2 : ");
      Serial.println(m.arg<float>(0));
      incr0 = constrain(incr0 - 1 , 0, INCR0_MAX);
    }
    setup_PWM();
    update_display();
  });

  osc.subscribe("/2/push3", [](OscMessage & m)
  {
    if ( m.arg<float>(0) > 0.5) {
      Serial.print("push3 : ");
      Serial.println(m.arg<float>(0));
      incr1 = constrain(incr1 + 1, 0, INCR1_MAX);
    }
    update_display();
  });
  osc.subscribe("/2/push4", [](OscMessage & m)
  {
    if ( m.arg<float>(0) > 0.5) {
      Serial.print("push4 : ");
      Serial.println(m.arg<float>(0));
      incr1 = constrain(incr1 - 1 , 0, INCR1_MAX);
    }
    update_display();
  });


  //    osc.subscribe("/wildcard/*/test", [](OscMessage & m)
  //osc.subscribe("/*", [](OscMessage& m)

  /*
    {
      Serial.print("wildcard : ");
      Serial.print(m.ip()); Serial.print(" ");
      Serial.print(m.port()); Serial.print(" ");
      Serial.print(m.size()); Serial.print(" ");
      Serial.print(m.address()); Serial.print(" ");
      Serial.print(m.arg<int>(0)); Serial.println();

    });



    osc.subscribe("/need/reply", [](OscMessage & m)
    {
      Serial.println("/need/reply");

      int i = 12;
      float f = 34.56F;
      double d = 78.987;
      String s = "hello";
      bool b = true;

      osc.send(host, send_port, "/send", i, f, d, s, b);
    });


  */
}


void setup_PWM() {


  // set up PWM, changing resolution and frequency if necessary
  freq = int(f5 * 10000);
  res = incr0;
  pwm_max = (0x01 << res) - 1;

  ledcSetup(0, freq, res);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(5, 0);


  ledcSetup(1, freq, res);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(2, 1);
  change_pwm();
}



void change_pwm() {
  ledcWrite(0, int(f1 * pwm_max));
  ledcWrite(1, int(f2 * pwm_max));

}


void update_display() {
  char outs[16];


  u8g2.setCursor(0, 8);
  u8g2.print("addr: ");
  u8g2.print(WiFi.localIP());
  snprintf(outs, 16, "f1: %4d%", int(100 * f1));
  u8g2.drawStr(0, 8 + 8, outs  );
  snprintf(outs, 16, "f1: %4d%", int(100 * f2));
  u8g2.drawStr(0, 8 + 2 * 8, outs );
  u8g2.setCursor(0, 8 + 3 * 8);
  u8g2.print("res: ");
  u8g2.print(res);
  u8g2.setCursor(0, 8 + 4 * 8);
  u8g2.print("freq: ");
  u8g2.print(freq);
  u8g2.sendBuffer();
}

void loop()
{
  osc.parse(); // should be called
}
