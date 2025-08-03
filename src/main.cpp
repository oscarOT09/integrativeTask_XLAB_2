/********************************************************
 * Author: Oscar Ortiz Torres
 * Created on: 03/08/2025
 * Last modified: 13/08/2025
 * Description:
 *  This program demonstrates the use of FreeRTOS tasks,
 *  queues, and semaphores on an ESP32.
 *  It creates three tasks with different priorities:
 *  - Task 1 waits for updates in a Mailbox Queue (containing
 *    a Data struct), detects changes, prints the new values,
 *    and sends data to a secondary queue.
 *  - Task 2 periodically generates random data, updates the
 *    Mailbox Queue, and signals Task 1 using a semaphore.
 *  - Task 3 waits for integers from the secondary queue
 *    and prints them when received.
 * 
 *  The example showcases inter-task communication and
 *  synchronization in a real-time embedded environment.
 ********************************************************/

#include <Arduino.h>

/**
 * @struct Data
 * @brief Struct to hold shared data.
 * 
 */
struct Data {
    int count;
    int random;
    float division;
};

// ----------------------------
// Global objects and variables
// ----------------------------

// Mailbox Queue (stores exactly one Data element)
QueueHandle_t mailboxQueue;

// Queue to store int values sent by Task 1, read by Task 3
QueueHandle_t intsQueue;

// Binary semaphore used for synchronization between Task 2 and Task 1
SemaphoreHandle_t xSemaphore;

// Structs for Task 1 to compare new and previous data
Data prevData;
Data actData;

// Global struct and variables used by Task 2
Data globalData;
int globalCount; // Counter used to track how many times data has been sent
int globalRandom; // Random number generated for each update

// Variable to hold values received by Task 3
int receivedInt;

/**
 * @brief Utility: Print task label to serial.
 * @param numTask Task number.
 * 
 */
void taskLog(int numTask) {
  Serial.print("T");
  Serial.print(numTask);
  Serial.print(">> ");
}

// ----------------------------
// Task 1 Functionality
// Priority: 1
// Waits on semaphore from Task 2
// Peeks MailboxQueue, compares values with previous
// If changed, prints and sends (count % 5) values to intsQueue
// ----------------------------
void task1Job() {
  // Wait for semaphore from Task 2
  xSemaphoreTake(xSemaphore, (TickType_t)portMAX_DELAY);

  // Peek the single element in the mailbox queue
  if(xQueuePeek(mailboxQueue, &actData, (TickType_t)portMAX_DELAY)) {
    // Check if the data has changed
    if((actData.count != prevData.count) || (actData.division != prevData.division) || (actData.random != prevData.random)) {
      // Log updated values to serial monitor
      taskLog(1);
      Serial.println("Change received from mailboxQueue");
      taskLog(1);
      Serial.print("count: "); Serial.println(actData.count);
      taskLog(1);
      Serial.print("random: "); Serial.println(actData.random);
      taskLog(1);
      Serial.print("division: "); Serial.println(actData.division);

      // Send the random number to intsQueue (count % 5) times
      for(int i = 0; i < (actData.count%5); i++) {
        xQueueSend(intsQueue, &actData.random, NULL);
      }

      // Update previous data for next comparison
      prevData = actData;
    }
  }
}

// ----------------------------
// Task 2 Functionality
// Priority: 2
// Waits 5s before first run, then updates MailboxQueue every 5s
// Also gives semaphore to Task 1
// ----------------------------
void task2Job() {
  // Populate the Data struct
  globalData.count = globalCount;
  globalRandom = rand() % 101; // Random number between 0 and 100
  globalData.random = globalRandom;
  // Avoid division by zero // (optional safety)
  globalData.division = static_cast<float>(globalData.random) / static_cast<float>(globalData.count);

  // Overwrite the mailbox queue with the new data
  if(xQueueOverwrite(mailboxQueue, &globalData)) {
    taskLog(2); Serial.println("Data sent to mailboxQueue");
    // Increment counter for next update
    globalCount+=1;
    // Notify Task 1 via semaphore
    xSemaphoreGive(xSemaphore);
  };
}

// ----------------------------
// Task 3 Functionality
// Priority: 3
// Waits for data in intsQueue, prints on reception
// ----------------------------
void task3Job() {
  // Wait indefinitely for a new integer in the queue
  if(xQueueReceive(intsQueue, &receivedInt, (TickType_t)portMAX_DELAY)) {
    taskLog(3);
    Serial.print("Received from intsQueue: "); Serial.println(receivedInt); 
  }
}

// ----------------------------
// Task 1 Callback Loop
// Loops task1Job, small delay to allow system breathability
// ----------------------------
void task1Callback(void *pvParameters) {
  while(true) {
    task1Job();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ----------------------------
// Task 2 Callback Loop
// Delays 5s before each execution to simulate periodic updates
// ----------------------------
void task2Callback(void *pvParameters) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(5000)); // 5 second delay between updates
    task2Job();
  }
}

// ----------------------------
// Task 3 Callback Loop
// Waits for new data constantly
// ----------------------------
void task3Callback(void *pvParameters) {
  while(true) {
    task3Job();
  }
};

// ----------------------------
// Setup function
// Initializes serial monitor, creates queues and semaphore
// Launches the 3 tasks with their respective priorities
// ----------------------------
void setup() {
  Serial.begin(115200); // Begin serial communication
  globalCount = 1; // Initialize counter to avoid division by 0

  // Create binary semaphore for task synchronization
  xSemaphore = xSemaphoreCreateBinary();

  // Create queues
  mailboxQueue = xQueueCreate(1, sizeof(Data)); // Mailbox: only 1 element
  intsQueue = xQueueCreate(4, sizeof(uint32_t)); // Queue: max 4 ints

  // Create tasks
  xTaskCreateUniversal(task1Callback,
                       "Task 1",
                       CONFIG_ARDUINO_LOOP_STACK_SIZE,
                       NULL,
                       1,
                       NULL,
                       CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreateUniversal(task2Callback,
                       "Task 2",
                       CONFIG_ARDUINO_LOOP_STACK_SIZE,
                       NULL,
                       2,
                       NULL,
                       CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreateUniversal(task3Callback,
                       "Task 3",
                       CONFIG_ARDUINO_LOOP_STACK_SIZE,
                       NULL,
                       3,
                       NULL,
                       CONFIG_ARDUINO_RUNNING_CORE);
}

// ----------------------------
// Loop function
// Not used in FreeRTOS multitasking model
// ----------------------------
void loop() {}