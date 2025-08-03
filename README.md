# 📦 ESP32 FreeRTOS Task Communication Example

## 📝 Description

This project demonstrates multitasking, inter-task communication, and synchronization on the ESP32 using FreeRTOS. It simulates a simplified control system with three concurrent tasks, a mailbox queue, a regular queue, and a binary semaphore for efficient task scheduling.

---

## ✅ Features

- Three FreeRTOS tasks with different priorities
- One-element mailbox queue using `xQueuePeek` and `xQueueOverwrite`
- Regular queue for integer values with size limit
- Binary semaphore for efficient notification between tasks
- Structured logging to the serial monitor for debugging

---

## 📁 File Structure

```plaintext
integrativeTask_XLAB_2/
│
├── src/
│ └── main.cpp # Main source file with tasks logic
├── platformio.ini # PlatformIO settings file
├── README.md # Project description and usage guide
```

---

## 📦 Requirements

- **Hardware**:
  - ESP32 development board (e.g., ESP32 DevKit v1)
  - USB cable for flashing and serial communication
- **Software**:
  - [Visual Studio Code](https://code.visualstudio.com/)
  - [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
  - Serial Monitor for output logging

---

## ⚙️ Compilation & Execution

1. **Install VSCode**  
2. **Install the PlatformIO extension** from the VSCode Extensions Marketplace.
3. Clone or download this project folder.
4. Open the project directory in VSCode.
5. Make sure the `platformio.ini` file is configured for your ESP32 board.
6. Connect your ESP32 board via USB.
7. In the PlatformIO toolbar (left side of VSCode), click the "Upload" button (arrow icon) to compile and flash the firmware.
8. After upload, click "Monitor" (plug icon) to open the serial monitor and view real-time output.

---

## ▶️ Example Usage

Once the ESP32 starts:

- Task 2 publishes a new `Data` struct every 5 seconds.
- Task 1 detects any change in the Mailbox Queue and prints the updated values. It also sends `(count % 5)` copies of `random` to a second queue.
- Task 3 reads and prints each value from the second queue as they arrive.

Example Serial Output:
```plaintext
T2>> Data sent to mailboxQueue
T1>> Change received from mailboxQueue
T1>> count: 1
T1>> random: 84
T1>> division: 84.00
T3>> Received from intsQueue: 84
T2>> Data sent to mailboxQueue
T1>> Change received from mailboxQueue
T1>> count: 2
T1>> random: 29
T1>> division: 14.50
T3>> Received from intsQueue: 29
T3>> Received from intsQueue: 29
T2>> Data sent to mailboxQueue
T1>> Change received from mailboxQueue
T1>> count: 3
T1>> random: 64
T1>> division: 21.33
T3>> Received from intsQueue: 64
T3>> Received from intsQueue: 64
T3>> Received from intsQueue: 64
T2>> Data sent to mailboxQueue
T1>> Change received from mailboxQueue
T1>> count: 4
T1>> random: 74
T1>> division: 18.50
T3>> Received from intsQueue: 74
T3>> Received from intsQueue: 74
T3>> Received from intsQueue: 74
T3>> Received from intsQueue: 74
T2>> Data sent to mailboxQueue
T1>> Change received from mailboxQueue
T1>> count: 5
T1>> random: 66
T1>> division: 13.20
```

---

## 👨‍💻 Author

Created by Oscar Ortiz Torres – [oscar_ot09@outlook.es]

---