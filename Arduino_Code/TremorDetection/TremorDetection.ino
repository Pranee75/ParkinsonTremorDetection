/*
 * Parkinson's Tremor Detection System - SIMULATED VERSION
 * Arduino Uno/Nano
 * Demonstrates tremor detection using sine wave simulation
 */

#include <SoftwareSerial.h>
#include <math.h>

// HC-05 Bluetooth (D2=RX, D3=TX) 
SoftwareSerial bluetooth(2, 3);

// Tremor detection parameters
float ACCEL_THRESHOLD = 0.5;   // acceleration threshold
float GYRO_THRESHOLD  = 25.0;  // gyro threshold
int TREMOR_COUNT_THRESHOLD = 5;

// Tremor detection variables
int tremor_count = 0;
bool tremor_detected = false;
unsigned long last_tremor_time = 0;

// Data transmission
unsigned long last_data_time = 0;
const int DATA_INTERVAL = 50;

// Status variables
int packet_number = 0;
unsigned long system_start_time = 0;

// Simulated sensor data
float ax, ay, az;
float gx, gy, gz;
float temp_c;

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  Serial.println("=== SIMULATED Parkinson's Tremor Detection System ===");
  Serial.println("Starting tremor simulation...");

  bluetooth.println("TREMOR_SYSTEM_READY");

  system_start_time = millis();
  Serial.println("System ready - sending simulated data via Bluetooth");
}

void loop() {
  unsigned long current_time = millis();

  readMPU6050();        // generates simulated tremor
  processSensorData();  // applies any processing (skipped here)

  detectTremor();       // check for tremor events

  if (current_time - last_data_time >= DATA_INTERVAL) {
    sendBluetoothData();
    last_data_time = current_time;
  }

  if (packet_number % 50 == 0) {
    printDebugInfo();
  }

  delay(10);
}

void readMPU6050() {
  // SIMULATED HAND MOVEMENT
  // Alternates between Normal and Tremor phases for demo
  static float t = 0;
  t += 0.05; // Controls speed of oscillation

  // Use millis() to switch phases every 10 seconds
  bool tremor_phase = ((millis() / 10000) % 2 == 0);

  if (tremor_phase) {
    //  Tremor phase 
    ax = 0.6 * sin(25 * t);
    ay = 0.6 * sin(25 * t + 1.5);
    az = 1.0 + 0.6 * sin(25 * t + 3.0);

    gx = 30 * sin(25 * t);
    gy = 30 * sin(25 * t + 2.0);
    gz = 30 * sin(25 * t + 4.0);
  } else {
    //  Normal phase 
    ax = 0.2 * sin(25 * t);
    ay = 0.2 * sin(25 * t + 1.5);
    az = 1.0 + 0.2 * sin(25 * t + 3.0);

    gx = 10 * sin(25 * t);
    gy = 10 * sin(25 * t + 2.0);
    gz = 10 * sin(25 * t + 4.0);
  }
}


//  PROCESS SENSOR DATA (skip filtering for simulation) 
void processSensorData() {
  temp_c = 36.53; // keep constant for demo
}

//  TREMOR DETECTION 
void detectTremor() {
  float accel_magnitude = sqrt(ax*ax + ay*ay + az*az);
  float gyro_magnitude  = sqrt(gx*gx + gy*gy + gz*gz);

  bool threshold_exceeded = (accel_magnitude > ACCEL_THRESHOLD) &&
                            (gyro_magnitude > GYRO_THRESHOLD);

  if (threshold_exceeded) {
    tremor_count++;
    if (tremor_count >= TREMOR_COUNT_THRESHOLD) {
      if (!tremor_detected) {
        tremor_detected = true;
        last_tremor_time = millis();

        bluetooth.println("TREMOR_ALERT," + String(accel_magnitude, 3) + "," + String(gyro_magnitude, 1));
        Serial.println(">>> TREMOR DETECTED! Accel: " + String(accel_magnitude, 3) + 
                       "g, Gyro: " + String(gyro_magnitude, 1) + "°/s");
      }
    }
  } else {
    tremor_count = 0;
    if (tremor_detected && (millis() - last_tremor_time > 5000)) {
      tremor_detected = false;
      bluetooth.println("TREMOR_ENDED");
      Serial.println(">>> Tremor ended");
    }
  }
}

//  SEND DATA 
void sendBluetoothData() {
  packet_number++;

  String data_packet = String(packet_number) + "," +
                       String(millis()) + "," +
                       String(ax, 3) + "," +
                       String(ay, 3) + "," +
                       String(az, 3) + "," +
                       String(gx, 2) + "," +
                       String(gy, 2) + "," +
                       String(gz, 2) + "," +
                       String(temp_c, 1) + "," +
                       String(tremor_detected ? 1 : 0);

  bluetooth.println(data_packet);
}

//  DEBUG OUTPUT 
void printDebugInfo() {
  unsigned long uptime = (millis() - system_start_time) / 1000;

  Serial.println("=== STATUS ===");
  Serial.println("Uptime: " + String(uptime) + "s | Packets sent: " + String(packet_number));
  Serial.println("Accel: X=" + String(ax, 2) + "g, Y=" + String(ay, 2) + "g, Z=" + String(az, 2) + "g");
  Serial.println("Gyro: X=" + String(gx, 1) + "°/s, Y=" + String(gy, 1) + "°/s, Z=" + String(gz, 1) + "°/s");
  Serial.println("Temperature: " + String(temp_c, 1) + "°C");
  Serial.println("Tremor status: " + String(tremor_detected ? "DETECTED" : "Normal"));
  Serial.println("==============");
}
