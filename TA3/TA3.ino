#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void TaskHeatFan( void *pvParameters );
void TaskSet( void *pvParameters );
void TaskSensor( void *pvParameters );

#define Fan 3
#define Heater 4

#define setUp 5
#define setDown 6

#define LM A1

int LMValue = 0, Temperature = 0, setTemp = 30;

void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect.
  }

  pinMode(Heater, OUTPUT);
  pinMode(Fan, OUTPUT);  
  pinMode(setUp, INPUT);
  pinMode(setDown, INPUT);
  
  pinMode(LM, INPUT);

  xTaskCreate(TaskHeatFan, "AutoHeatCool", 128, NULL, 1, NULL);
  xTaskCreate(TaskSet, "Set Temperature", 128, NULL, 2, NULL);
  xTaskCreate(TaskSensor, "SensorRead", 128, NULL, 3, NULL);

  Serial.println("+++Start Program+++");
  
}

void loop()
{ // Kosong

}


void TaskHeatFan(void *pvParameters) 
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
        
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
    vTaskDelay(150/portTICK_PERIOD_MS);  // ten tick delay (150ms) in between reads for stability
  }
}

void TaskSensor(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  for (;;)
  {
    LMValue = analogRead(LM);
    Temperature = (LMValue * 4.88)/10;
    Serial.print("Temperature :"); Serial.print(Temperature); Serial.println(" °C"); 

    lcd.setCursor(0, 0);lcd.print("Auto-Termostat"); 
    lcd.setCursor(0, 1);lcd.print("Temp: "); 
    lcd.setCursor(6, 1);lcd.print(Temperature); 
    lcd.setCursor(9, 1);lcd.print("Set: "); 
    lcd.setCursor(14, 1);lcd.print(setTemp);

    vTaskDelay(100/portTICK_PERIOD_MS);  // ten tick delay (100ms) in between reads for stability
  }
}
