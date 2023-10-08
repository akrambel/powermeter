#define BLYNK_TEMPLATE_ID "TMPLqbX1yhv2"
#define BLYNK_DEVICE_NAME "SOLOR"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

unsigned long previousMillis = 0;
unsigned long interval = 2000;
float shuntvoltage = 0.00;
float busvoltage = 0.00;
float current = 0.00;
float loadvoltage = 0.00;
float energy = 0.00,  energyCost, energyPrevious, energyDifference;
float energyPrice = 0.9010 ;
float power = 0.00;
float capacity = 0.00;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

  uint32_t currentFrequency;

  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 30);
  display.print("SOLOR");
  display.display(); 
  
  BlynkEdgent.begin();
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }
  }


  Serial.println("SOLOR ...");
}

void loop()
{
  BlynkEdgent.run();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ina219values();
    displaydata();
  }
}

void ina219values()  {

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000) + 0.4;
  power = loadvoltage * current;
  energy = energy + power / 3600; //Wh
  capacity = capacity + current / 1000;
  energyDifference = energy - energyPrevious;
  energyPrevious = energy;
  energyCost = energyCost + (energyPrice / 1000 * energyDifference);

  
  Blynk.virtualWrite(V7, current);
  Blynk.virtualWrite(V5, loadvoltage);
  Blynk.virtualWrite(V4, power);

  
  if (loadvoltage < 1 )loadvoltage = 0;
  if (current < 1 )
  {
    current = 0;
    power = 0;
    energy = 0;
    capacity = 0;
    energyCost=0;
  }
  
  
if (power < 1 )
  {
Blynk.logEvent("power_alert", "Dysfonctionnement dans les panneaux solaires! → Les panneaux sont hors service. / → Les conducteurs du panneau sont mal connectés.");
  }
    
  if (! ina219.begin())
  {
Blynk.logEvent("erreur_capteur", "→ Vérifier la connexion des fils entre le capteur et le microcontrôleur. / → Veuillez vous assurer que le capteur n'est pas endommagé");
  }




/*
 if (current < 300 )
  {
Blynk.logEvent("panneau_1", "→ Le panneau numéro : 1 est hors sérvice");
  }
*/

/*
 if (current < 500 )
  {
Blynk.logEvent("panneau_2", "Le panneau numéro : 2 est hors sérvice");
  }
*/

  

  Serial.print("Tension de bus :   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Tension de derivation : "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Tension de charge:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Courrant:       "); Serial.print(current); Serial.println(" mA");
  Serial.print("Puissance:         "); Serial.print(power); Serial.println(" mW");
  Serial.print("Energie:        "); Serial.print(energy); Serial.println(" Wh");
  Serial.print("Capacité:      "); Serial.print(capacity); Serial.println(" Ah");
  Serial.print("Cout energetique:   "); Serial.print("MAD "); Serial.println(energyCost);
  Serial.println("-------------------------");
}
void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // VOLTAGE
  Blynk.virtualWrite(V0, String(loadvoltage, 2) + String(" V") );
  display.setCursor(0, 5);
  display.print(loadvoltage, 2);
  display.print(" V");
  // CURRENT
  if (current > 1000) {
    Blynk.virtualWrite(V1, String((current / 1000), 2) + String(" A") );
    display.setCursor(60, 5);
    display.print((current / 1000), 2);
    display.println(" A");
    display.setCursor(0, 15);
    display.println("--------------------");
  }
  else
  {
    Blynk.virtualWrite(V1, String(current, 2) + String(" mA"));
    display.setCursor(60, 5);
    display.print(current, 1);
    display.println(" mA");
    display.setCursor(0, 15);
    display.println("--------------------");
  }


  display.setCursor(60, 20);
  display.print("|");
  display.setCursor(60, 24);
  display.print("|");
  display.setCursor(60, 28);
  display.print("|");
  display.setCursor(60, 32);
  display.print("|");
  display.setCursor(60, 36);
  display.print("|");
  display.setCursor(60, 40);
  display.print("|");
  display.setCursor(0, 46);
  display.print("--------------------");

  // POWER
  if (power > 1000) {
    Blynk.virtualWrite(V2, String((power / 1000), 2) + String(" Kw") );
    display.setCursor(0, 24);
    display.print(String((power / 1000), 2));
    display.println(" Kw");
  }
  else {
    Blynk.virtualWrite(V2, String(power, 2) + String(" mW") );
    display.setCursor(0, 24);
    display.print(power, 2);
    display.println(" mW");
  }

  if (energy > 1000) {
    Blynk.virtualWrite(V3, String((energy / 1000), 2) + String(" kWh"));
    display.setCursor(0, 36);
    display.print((energy / 1000), 2);
    display.println(" kWh");
  }
  else {
    Blynk.virtualWrite(V3, String(energy, 2) + String(" Wh"));
    display.setCursor(0, 36);
    display.print(energy, 2);
    display.println(" Wh");
  }

  Blynk.virtualWrite(V6, String("MAD ") + String(energyCost, 5) );
  display.setCursor(10, 54);
  display.print("Coût: MAD ");
  display.println(energyCost, 5);
  display.display();
}