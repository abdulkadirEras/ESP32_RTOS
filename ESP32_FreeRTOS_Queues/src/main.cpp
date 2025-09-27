#include <Arduino.h>

#define POT_Pin 15 
#define LED_Pin 2   
#define PWM_FREK 5000
#define PWM_COZUNURLUK 12  // 12-bit (0–4095)
#define QUEUE_BOYUT 5

// Kuyruk için handle oluşturuldu
QueueHandle_t potQueue = NULL;

void potTask(void *parameter) {
  for (;;) {
    uint16_t potValue = analogRead(POT_Pin);  // Read 0–4095
    xQueueSend(potQueue, &potValue, portMAX_DELAY);  // Send to queue
    Serial.printf("potTask: Sent pot value %u\n", potValue);
    vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms
  }
}

void LEDBrightnessTask(void *parameter) {
  for (;;) {
    uint16_t potValue;
    if (xQueueReceive(potQueue, &potValue, portMAX_DELAY)) {
      uint16_t brightness = potValue;
      ledcWrite(LED_Pin, brightness);
      Serial.printf("LEDBrightnessTask: Set brightness %u\n", brightness);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup PWM for LED
  ledcAttach(LED_Pin, PWM_FREK, PWM_COZUNURLUK);

  // Create queue (5 items, each uint16_t)
  potQueue = xQueueCreate(QUEUE_BOYUT, sizeof(uint16_t));
  if (potQueue == NULL) {
    Serial.println("Failed to create queue!");
    while (1);
  }

  // Create tasks
  xTaskCreatePinnedToCore(
    potTask,
    "potTask",
    3000,  // Task stack
    NULL,
    1,
    NULL,
    1  // Core 1
  );

  xTaskCreatePinnedToCore(
    LEDBrightnessTask,
    "LEDBrightnessTask",
    3000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  // Empty
}