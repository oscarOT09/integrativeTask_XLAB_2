#include <Arduino.h>
#include <bits/stdc++.h>

struct Data {
    int count;
    int random;
    float division;
};

// Queues
QueueHandle_t mailboxQueue;
QueueHandle_t intsQueue;

// Task 1 structs
Data prevData;
Data actData;

// Task 2 struct and variables
Data globalData;
int globalCount;
int globalRandom;

// Task 3 variable
int receivedInt;

// Function to identify task in terminal
void taskLog(int numTask) {
  Serial.print("T");
  Serial.print(numTask);
  Serial.print(">> ");
}

// ---------- Tasks routines ----------
void task1Job() {
  if(xQueuePeek(mailboxQueue, &actData, (TickType_t)portMAX_DELAY)) {
    // Checks for change
    if((actData.count != prevData.count) || (actData.division != prevData.division) || (actData.random != prevData.random)) {
      // Prints new values
      taskLog(1);
      Serial.println("Change received from mailboxQueue");
      taskLog(1);
      Serial.print("count: "); Serial.println(actData.count);
      taskLog(1);
      Serial.print("random: "); Serial.println(actData.random);
      taskLog(1);
      Serial.print("division: "); Serial.println(actData.division);

      // Sends values to intsQueue
      for(int i = 0; i < (actData.count%5); i++) {
        if(xQueueSend(intsQueue, &actData.random, NULL)) {
          taskLog(1);
          Serial.println("random value sent to intsQueue");
        }
      }

      // Stores new values
      prevData = actData;
    }
  }
}

void task2Job() {
  // Data store in struct
  globalData.count = globalCount;
  globalRandom = rand();
  globalData.random = globalRandom;
  globalData.division = static_cast<float>(globalRandom) / globalCount;

  // Sends struct to Queue
  if(xQueueOverwrite(mailboxQueue, &globalData)) {
    taskLog(2);
    Serial.println("Data sent to mailboxQueue");
    globalCount+=1;
  };
}

void task3Job() {
  // Waits for new element in Queue
  if(xQueueReceive(intsQueue, &receivedInt, (TickType_t)portMAX_DELAY)) {
    taskLog(3);
    Serial.print("Received from intsQueue: "); Serial.println(receivedInt); 
  }
}

// ---------- Tasks callbacks ----------
void task1Callback(void *pvParameters) {
  while(true) {
    task1Job();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void task2Callback(void *pvParameters) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    task2Job();
  }
}

void task3Callback(void *pvParameters) {
  while(true) {
    task3Job();
  }
};

void setup() {
  // Serial monitor initialization
  Serial.begin(115200);
  
  // Counter variable 
  globalCount = 1;

  // Queues creation
  mailboxQueue = xQueueCreate(1, sizeof(Data));
  intsQueue = xQueueCreate(4, sizeof(uint32_t));

  // Tasks creation
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

void loop() {}