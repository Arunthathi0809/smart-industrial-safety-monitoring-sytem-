#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3Ou20jyYs"
#define BLYNK_TEMPLATE_NAME "Industrial monitoring system"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Blynk credentials
char auth[] = "1m7dl5s6PclIrUBuBn7J8dFCiGVysZJR";
char ssid[] = "Redmi 11 Prime 5G";
char pass[] = "thenamuthamurugan";

// Sensor pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ2_PIN 34

// Buzzer
#define BUZZER 25

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Blynk timer
BlynkTimer timer;

void sendSensor()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasADC = analogRead(MQ2_PIN);

  // Convert ADC to approximate PPM
  int gasPPM = map(gasADC, 0, 4095, 0, 1000);

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Sensor Error");
    return;
  }

  // -------- Serial Monitor --------
  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Gas PPM: ");
  Serial.println(gasPPM);

  Serial.println("---------------------");

  // -------- Send to Blynk --------
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, gasPPM);
  Blynk.virtualWrite(V4, humidity);

  // -------- LCD Display --------
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature,1);
  lcd.print("C ");

  lcd.setCursor(8,0);
  lcd.print("H:");
  lcd.print(humidity,1);
  lcd.print("% ");

  lcd.setCursor(0,1);
  lcd.print("Gas:");
  lcd.print(gasPPM);
  lcd.print("    "); // clear old digits

  // -------- Alarm Logic --------
  if(gasPPM > 800 || temperature > 50)
  {
    tone(BUZZER, 1500);   // High danger
  }
  else if(gasPPM > 500 || temperature > 35)
  {
    tone(BUZZER, 800);    // Warning
  }
  else
  {
    noTone(BUZZER);       // Safe
  }
}

void setup()
{
  Serial.begin(115200);

  Wire.begin(21,22);

  lcd.init();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);

  dht.begin();

  // Startup message
  lcd.setCursor(0,0);
  lcd.print("Industrial IoT");
  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  delay(2000);
  lcd.clear();

  // Read sensors every 3 seconds
  timer.setInterval(3000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}