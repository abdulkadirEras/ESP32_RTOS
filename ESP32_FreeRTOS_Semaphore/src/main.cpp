#include <Arduino.h>


// Demo amaçlı sadece çekirdek 1 üzerinde çalışır
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Ayarlamalar ve sabitler
enum {BUF_BOYUT = 5};                  // Paylaşılan buffer boyutu
static const int num_prod_gorevler = 5;  // Producer görevlerinin sayısı
static const int num_cons_gorevler = 2;  // Consumer görevlerinin sayısı
static const int num_writes = 3;      // Her yapımcı buf'a yazıyor

// Global değişkenler
static int buf[BUF_BOYUT];             // Paylaşılan buffer
static int head = 0;                  // Buffer'ın indeksine yazma
static int tail = 0;                  // Buffer'ın indeksinden okuma
static SemaphoreHandle_t bin_sem;     // Okunabilecek parametreleri okumak için ikili semafor
static SemaphoreHandle_t mutex;       // buffer ve serial erişimi için mutex
static SemaphoreHandle_t sem_empty;   // Boş slot sayısını sayar
static SemaphoreHandle_t sem_filled;  // Dolu slot sayısını sayar

//*****************************************************************************
// Tasks

// Producer: Paylaşılan arabelleğe belirli bir kez yazın
void producer(void *parameters) 
{

  // Parametreleri yerel bir değişkene kopyalayın
  int num = *(int *)parameters;

  // İkili semaforu serbest bırakın
  xSemaphoreGive(bin_sem);

  // Paylaşılan arabelleği görev numarasıyla doldurun
  for (int i = 0; i < num_writes; i++) 
  {

    // Bufferdaki  boş yuvanın mevcut olmasını bekleyin
    xSemaphoreTake(sem_empty, portMAX_DELAY);

    // Kritik bölümü muteks ile kilitle
    xSemaphoreTake(mutex, portMAX_DELAY);
    buf[head] = num;
    head = (head + 1) % BUF_BOYUT;
    xSemaphoreGive(mutex);

    // Signal to consumer tasks that a slot in the buffer has been filled
    xSemaphoreGive(sem_filled);
  }

  // Kendi kendini sil
  vTaskDelete(NULL);
}

// Consumer: continuously read from shared buffer
void consumer(void *parameters) 
{

  int val;

  // Bufferdan sürekli okuma
  while (1) 
  {

    // Wait for at least one slot in buffer to be filled
    xSemaphoreTake(sem_filled, portMAX_DELAY);

    // Lock critical section with a mutex
    xSemaphoreTake(mutex, portMAX_DELAY);
    val = buf[tail];
    tail = (tail + 1) % BUF_BOYUT;
    Serial.println(val);
    xSemaphoreGive(mutex);

    // Signal to producer thread that a slot in the buffer is free
    xSemaphoreGive(sem_empty);
  }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() 
{

  char task_name[12];
  
  Serial.begin(115200);

  // başlatmadan önce kısa bir gecikme
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Semaphore ---");

  //Taskları oluşturmadan önce semaforları ve mutex'i oluştur
  bin_sem = xSemaphoreCreateBinary();
  mutex = xSemaphoreCreateMutex();
  sem_empty = xSemaphoreCreateCounting(BUF_BOYUT, BUF_BOYUT);
  sem_filled = xSemaphoreCreateCounting(BUF_BOYUT, 0);

  // Producer görevini başlat (her birinin argüman okumasını beklenir)
  for (int i = 0; i < num_prod_gorevler; i++) 
  {
    sprintf(task_name, "Producer %i", i);
    xTaskCreatePinnedToCore(producer,
                            task_name,
                            1024,
                            (void *)&i,
                            1,
                            NULL,
                            app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }

  // consumer görevini başlat
  for (int i = 0; i < num_cons_gorevler; i++) 
  {
    sprintf(task_name, "Consumer %i", i);
    xTaskCreatePinnedToCore(consumer,
                            task_name,
                            1024,
                            NULL,
                            1,
                            NULL,
                            app_cpu);
  }

  // Tüm görevlerin oluşturulduğunu bildirin (Mutex ile Serial Kilitle)
  xSemaphoreTake(mutex, portMAX_DELAY);
  Serial.println("tum gorevler olusturuldu.");
  xSemaphoreGive(mutex);// semaforu serbest bırak
}

void loop() 
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}