#include <Arduino.h>

#define LED1_Pin 2
#define LED2_Pin 4

TaskHandle_t Gorev1Handle = NULL;
TaskHandle_t Gorev2Handle = NULL;

void Task1(void *parameter) {
  pinMode(LED1_Pin, OUTPUT);
  for (;;) {
    digitalWrite(LED1_Pin, HIGH);
    Serial.println("Task1: LED1 ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED1_Pin, LOW);
    Serial.println("Task1: LED1 OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.print("Task 1 running on core ");
    Serial.println(xPortGetCoreID());
  }
}

void Task2(void *parameter) {
  pinMode(LED2_Pin, OUTPUT);
  for (;;) {
    digitalWrite(LED2_Pin, HIGH);
    Serial.println("Task2: LED2 ON");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    digitalWrite(LED2_Pin, LOW);
    Serial.println("Task2: LED2 OFF");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    Serial.print("Task 2 running on core ");
    Serial.println(xPortGetCoreID());
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    Task1,             // Task function
    "Task1",           // Task name
    10000,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &Gorev1Handle,      // Task handle
    1                  // Core 1
  );

  xTaskCreatePinnedToCore(
    Task2,            // Task function
    "Task2",          // Task name
    10000,            // Stack size (bytes)
    NULL,             // Parameters     
    1,                // Priority
    &Gorev2Handle,     // Task handle
    0                 // Core 0
  );
}

void loop() {
  // Empty because FreeRTOS scheduler runs the task
}