#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void TaskHeatFan( void *pvParameters );
void TaskSet( void *pvParameters );
void TaskLampu( void *pvParameters );
void TaskBTN( void *pvParameters );
void TaskSensor( void *pvParameters );

#define Lamp 6
#define Fan 3
#define Heater 4

#define setUp 9
#define setDown 10

#define pinAuto 8
#define pinManual 12
#define pinSwitch 11
#define LDR A0
#define LM A1

bool swhManual = false;
bool btnAuto = true; 
bool btnManual = false;

bool modeChanged = false;

int LDRValue = 0, LMValue = 0, Temperature = 0, setTemp = 30;

void setup() {
  lcd.begin(16,2);
  lcd.backlight();
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect.
  }

  pinMode(Heater, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(Lamp, OUTPUT);
  
  pinMode(setUp, INPUT);
  pinMode(setDown, INPUT);
  pinMode(pinAuto, INPUT);
  pinMode(pinManual, INPUT);
  pinMode(pinSwitch, INPUT);
  pinMode(LDR, INPUT);
  pinMode(LM, INPUT);

  xTaskCreate(TaskHeatFan, "AutoHeatCool", 128, NULL, 1, NULL);
  xTaskCreate(TaskSet, "Set Temperature", 128, NULL, 2, NULL);
  xTaskCreate(TaskLampu, "AutoLamp", 128, NULL, 3, NULL);
  xTaskCreate(TaskBTN, "buttonAuto", 128, NULL, 4, NULL);
  xTaskCreate(TaskSensor, "SensorRead", 128, NULL, 5, NULL);

  Serial.println("+++Start Program+++");
  
}

void loop()
{ // Kosong

}

void TaskLampu(void *pvParameters) 
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    if (btnAuto == true) {
      lcd.setCursor(0, 0);
      lcd.print("Auto");

      if (LDRValue > 100) {
        lcd.setCursor(0, 1);
        lcd.print("Lampu Hidup");
        digitalWrite(Lamp, HIGH); // turn the Lamp on
      }
      else {
        lcd.setCursor(0, 1);
        lcd.print("Lampu Mati");
        digitalWrite(Lamp, LOW); // turn the Lamp off
      }
      
      if (modeChanged) {
        lcd.clear(); // Clear the entire screen
        modeChanged = false;
      }
    }
    else if (btnManual == true) {
      lcd.setCursor(0, 0);
      lcd.print("Manual");
      
      if (swhManual == true) {
        lcd.setCursor(0, 1);
        lcd.print("Lampu Hidup");
        digitalWrite(Lamp, HIGH); // turn the Lamp on
      }
      else {
        lcd.setCursor(0, 1);
        lcd.print("Lampu Mati");
        digitalWrite(Lamp, LOW); // turn the Lamp off
      }
      
      if (!modeChanged) {
        lcd.clear(); // Clear the entire screen
        modeChanged = true;
      }
    }
  }
}

void TaskHeatFan(void *pvParameters) 
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    lcd.setCursor(12, 0);
    lcd.print(Temperature);
    
    if (Temperature == setTemp){
      digitalWrite(Fan, LOW);
      digitalWrite(Heater, LOW);
    }    
    if (Temperature < setTemp){
      digitalWrite(Heater, HIGH);
      digitalWrite(Fan, LOW);
    }
    if (Temperature > setTemp){
      digitalWrite(Fan, HIGH);
      digitalWrite(Heater, LOW);
    }
    vTaskDelay(150/portTICK_PERIOD_MS);  // ten tick delay (150ms) in between reads for stability
    
  }
}

void TaskSet(void *pvParameters) 
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    if (digitalRead(setUp) == HIGH){
      setTemp++;
    }
    if (digitalRead(setDown) == HIGH){
      setTemp--;
    }
    lcd.setCursor(12, 1);
    lcd.print(setTemp);
    
    vTaskDelay(150/portTICK_PERIOD_MS);  // ten tick delay (150ms) in between reads for stability
  }
}

void TaskBTN(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;)
  {
    if(digitalRead(pinAuto) == HIGH){
       btnAuto = true;
       btnManual = false;
    }
    if(digitalRead(pinManual) == HIGH){
       btnAuto = false;
       btnManual = true;
    }
    if(digitalRead(pinSwitch) == HIGH){
      swhManual = true;
    }
    if(digitalRead(pinSwitch) != HIGH){
      swhManual = false;
    }
   
    //    Serial.print("Switch :");
    //    Serial.println(swhManual);
    vTaskDelay(150/portTICK_PERIOD_MS);  // ten tick delay (150ms) in between reads for stability
  }
}


void TaskSensor(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  for (;;)
  {
    LDRValue = analogRead(LDR);
    LMValue = analogRead(LM);
    Temperature = (LMValue * 4.88)/10;
//    if(btnAuto == true){
    Serial.print("LDR :"); Serial.print(LDRValue); Serial.print("Temperature :"); Serial.print(Temperature); Serial.println(" °C"); 
//    }
    vTaskDelay(100/portTICK_PERIOD_MS);  // ten tick delay (100ms) in between reads for stability
  }
}
