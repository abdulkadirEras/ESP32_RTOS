#include <Arduino.h>

#define LED1_Pin 2
#define LED2_Pin 4

static uint32_t sonKontol = 0;

//gorev handle'lari oluşturuldu
TaskHandle_t Gorev1Handle = NULL;
TaskHandle_t Gorev2Handle = NULL;


//fonksiyon prototipleri
void Gorev1(void *parameter);
void Gorev2(void *parameter);
void sistemi_baslat(void);

void setup() 
{
  Serial.begin(115200);

  sistemi_baslat();
}

void loop() 
{
   
  if (millis() - sonKontol > 5000) 
  {
    Serial.println("********************** LOOP **********************");
    Serial.printf("Mevcut bos bellek: %u bytes\n", xPortGetFreeHeapSize());
    sonKontol = millis();
  }
}


void sistemi_baslat(void)
{
  xTaskCreatePinnedToCore(
    Gorev1,             // Gorev Fonksiyonu
    "Task1",           // Görev adı
    10000,             // Yığın Boyutu (bytes)
    NULL,              // Parametreler 
    1,                 // Öncelik
    &Gorev1Handle,      // Gorev handle
    1                  // Çekirdek 1
  );

  xTaskCreatePinnedToCore(
    Gorev2,            // Gorev Fonksiyonu
    "Task2",          // Gore v adı
    10000,            // Yığın Boyutu (bytes)
    NULL,             // Parametreler     
    1,                // Öncelik
    &Gorev2Handle,     // görev handle
    0                 // Çekirdek 0
  );
}

/**************************************Gorev fonksiyonlarının tanımlanması********************************** */

void Gorev1(void *parameter) 
{
  pinMode(LED1_Pin, OUTPUT);
  for (;;) 
  {
    Serial.println("********************** GOREV 1 **********************");
    digitalWrite(LED1_Pin, HIGH);
    Serial.println("Gorev1: LED1 ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED1_Pin, LOW);
    Serial.println("Gorev1: LED1 OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.print("Gorev 1 calistigi cekirdek: ");
    Serial.println(xPortGetCoreID());
    Serial.printf("Gorev1 Stack Bos Alan: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  }
}

void Gorev2(void *parameter) 
{
  pinMode(LED2_Pin, OUTPUT);
  for (;;) 
  {
    Serial.println("********************** GOREV 2 **********************");
    digitalWrite(LED2_Pin, HIGH);
    Serial.println("Gorev2: LED2 ON");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    digitalWrite(LED2_Pin, LOW);
    Serial.println("Gorev2: LED2 OFF");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    
    Serial.print("Gorev 2 calistigi cekirdek: ");
    Serial.println(xPortGetCoreID());

    Serial.printf("Gorev2 Stack Bos Alan: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  }
}
