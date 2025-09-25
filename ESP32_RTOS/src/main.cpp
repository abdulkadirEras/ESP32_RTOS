#include <Arduino.h>


#define LED_Pin 2
#define Buton_Pin 23

TaskHandle_t BlinkGorevHandle = NULL;

// ISR için oluşturulan volatile değişkenler
volatile bool gorevSuspended = false;
volatile uint32_t sonInterruptZamani = 0;
const uint32_t debounceSuresi = 100; 

void IRAM_ATTR buttonISR() 
{
  // Debounce kontrolü
  uint32_t simdikiZaman = millis();
  if (simdikiZaman - sonInterruptZamani < debounceSuresi) 
  {
    return;
  }
  sonInterruptZamani = simdikiZaman;

  // Toggle gorev durumu
  gorevSuspended = !gorevSuspended;
  if (gorevSuspended) 
  {
    vTaskSuspend(BlinkGorevHandle);
    Serial.println("Blink Gorev Askiya alindi");
  } 
  else 
  {
    vTaskResume(BlinkGorevHandle);
    Serial.println("BlinkTask devam ediyor");
  }
}


void BlinkGorev(void *parameter) 
{
  for (;;) 
  { 
    digitalWrite(LED_Pin, HIGH);
    Serial.println("BlinkGorev: LED ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1000ms
    digitalWrite(LED_Pin, LOW);
    Serial.println("BlinkGorev: LED OFF");
    Serial.print("BlinkGorev cekirdek uzerinde calisiyor: ");
    Serial.println(xPortGetCoreID());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void setup() 
{
  Serial.begin(115200);
  pinMode(LED_Pin, OUTPUT);
  pinMode(Buton_Pin, INPUT_PULLUP); // Internal pull-up resistor

  // Kesme butonu eklendi
  attachInterrupt(digitalPinToInterrupt(Buton_Pin), buttonISR, FALLING);

  // Gorev oluşturma
  xTaskCreatePinnedToCore(
    BlinkGorev,         // Gorev fonksiyonu
    "BlinkGorev",       // Gorev adı
    10000,             // Yığın Boyutu (bytes)
    NULL,              // parametreler
    1,                 // Öncelik
    &BlinkGorevHandle,  // Gorev handle
    1                  // Çekirdek 1
  );
}

void loop() 
{
  
}
