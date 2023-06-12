#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define SerialMon Serial
#include <AppleMIDI.h>

char ssid[] = "Network Name";
char pass[] = "Network Password";
//doesn't work on 5 Ghz networks

float y_acc;

unsigned long t0 = millis();
int8_t isConnected = 0;

APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();

Adafruit_MPU6050 mpu;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (!mpu.begin()){
      delay(10);
    }
  }  
  
  Serial.println(("Booting"));
  Serial.println("MPU6050 Found!");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(("Establishing connection to WiFi.."));
  }
  
  Serial.println(("Connected to network"));
  Serial.println(WiFi.localIP());
  Serial.println(AppleMIDI.getPort());
  Serial.println(AppleMIDI.getName());
  Serial.println(("OK, now make sure you an rtpMIDI session that is Enabled"));
  Serial.println(("Select and then press the Connect button"));
  Serial.println(("Then open a MIDI listener and monitor incoming notes"));
  Serial.println(("Listen to incoming MIDI commands"));

  MIDI.begin();

  AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
    isConnected++;
    //Serial.println(("Connected to session"), ssrc, name);
  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
    isConnected--;
    //Serial.println(("Disconnected"), ssrc);
  });
  
  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    //Serial.println(("NoteOn"), note);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    //Serial.println(("NoteOff"), note);
  });

    Serial.println(("Sending NoteOn/Off of note 45, every second"));
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
  
}
void loop()
{
  MIDI.read();
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Acceleration X: ");
  float x_acc_temp = a.acceleration.x ;
  float x_acc = mapf(x_acc_temp, -9.5, 10.2, 0, 127);
  if (a.acceleration.z > 0){
      Serial.print(x_acc);
  }
  else {
    Serial.print("out of range");
  }
  
  Serial.print(", Y: ");
  float y_acc_temp = a.acceleration.y ;
  
  if (a.acceleration.y < 2 && a.acceleration.y > -5){
    y_acc = mapf(y_acc_temp, -5, 2, 127, 0);
    MIDI.sendControlChange(1, x_acc, 1);
    Serial.print(y_acc);
  }
  else {
    Serial.print(y_acc);
  }

  Serial.print(", Z: ");
  if (a.acceleration.z > 0){
    Serial.print("1");
  }
  else {
    Serial.print("0");
  }
  Serial.println("");
  
  /*
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
  */
  
  Serial.println("");

}
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
