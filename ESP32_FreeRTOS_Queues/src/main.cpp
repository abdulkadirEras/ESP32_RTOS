#include <Arduino.h>

#define POT_Pin 15 
#define LED_Pin 2   
#define PWM_FREK 5000
#define PWM_COZUNURLUK 12  // 12-bit (0–4095)
#define QUEUE_BOYUT 5

// Kuyruk için handle oluşturuldu
QueueHandle_t potQueue = NULL;

void potGorev(void *parameter) 
{
  for (;;) 
  {
    uint16_t potDeger = analogRead(POT_Pin);  // okuma aralığı 0–4095
    xQueueSend(potQueue, &potDeger, portMAX_DELAY);  // kuyruğa gönder
    Serial.printf("potGorev: gonderilen pot degeri: %u\n", potDeger);
    vTaskDelay(100 / portTICK_PERIOD_MS);  // 100ms
  }
}

void LEDParlaklikGorev(void *parametre) 
{
  for (;;) 
  {
    uint16_t potValue;
    if (xQueueReceive(potQueue, &potValue, portMAX_DELAY)) 
    {
      uint16_t parlaklik = potValue;
      ledcWrite(LED_Pin, parlaklik);
      Serial.printf("LEDParlaklikGorev: Ayarlanan parlaklik: %u\n", parlaklik);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // LED için PWM başlat
  ledcAttach(LED_Pin, PWM_FREK, PWM_COZUNURLUK);

  // Kuyruk oluştur (5 parça, each uint16_t)
  potQueue = xQueueCreate(QUEUE_BOYUT, sizeof(uint16_t));
  if (potQueue == NULL) 
  {
    Serial.println("Kuyruk olusturulamadi!");
    while (1);
  }

  // Gorevleri oluştur
  xTaskCreatePinnedToCore(
    potGorev,
    "potGorev",
    3000,  // Görev yığını
    NULL,
    1,
    NULL,
    1  // Çekirdek 1
  );

  xTaskCreatePinnedToCore(
    LEDParlaklikGorev,
    "LEDParlaklikGorev",
    3000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  
}