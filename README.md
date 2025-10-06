# Smart-parking-system-cupcarbon
This project demonstrates a Multi-node Smart Parking System designed using CupCarbon IoT Simulator, ESP32, and ESP8266 boards.   Each parking spot acts as an IoT node that detects car presence, communicates status through MQTT, and displays it via LEDs and dashboards.

Urban areas often face parking congestion due to poor space visibility.  
The **Smart Parking System** solves this by continuously monitoring each parking slot and sharing live updates with a central dashboard.

In this implementation:
- 10 parking nodes are created:
  - **Parking Spots 1–3** → Based on **ESP32** with **NEO Pixels**
  - **Parking Spots 4–5** → Based on **ESP8266** with **NEO Pixels**
  - **Parking Spots 6–10** → Based on **ESP32** with **RGB LED**
  
- Each node detects the vehicle using ultrasonic  and sends MQTT messages to indicate the status simulated using **CupCarbon** and  Central computer **Terminal**

---
Real-time parking space monitoring  
###  ESP32 with NeoPixel Array
(Used in Parking Spots 1–3)

- Uses **Adafruit NeoPixel** RGB LED strip for colorful, dynamic visual feedback.  
- Each LED pixel can represent a **parking slot state** with smooth transitions.  
- Detects vehicle presence using ultrasonic sensors.  
- Communicates over **Wi-Fi** with MQTT broker for centralized updates.  
- CupCarbon simulation reflects the data transmission from ESP32 to MQTT.  
- Offers **multi-color animations** for status feedback:
  - Green → Slot Free  
  - Red → Occupied / Reserved Occupied  
  - Blue → Device Not Connected  
  - Magenta → Reserved Free 
  - White → Connection status / system ready  


### 🔵 ESP32 with RGB LED
(Used in Parking Spots 6–10 for design simulation)

- Uses a **standard RGB LED** with three digital pins for R, G, and B control.  
- Simpler setup representing low-cost deployment.  
- Each slot indicates state through color mixing (no pixel animation).  
- Also simulated in **CupCarbon** for virtual monitoring.  
- Ideal for scaling to more parking nodes with less power consumption.  

---

### ⚪ ESP8266 Nodes (Spots 4–5)
Built using ESP8266 (NodeMCU) boards with Open Smart NeoPixel (WS2812B) LED arrays.

Function as fully active parking nodes.

Detect vehicle presence via ultrasonic sensors and display status with vivid NeoPixel colors.

Communicate wirelessly with the MQTT broker over Wi-Fi for live status updates.

Provide the same four color states as ESP32 nodes for a consistent visual experience.

Integrated into the CupCarbon simulation for synchronized virtual-physical testing.

---

## 🌍 CupCarbon Simulation

All 10 parking nodes are represented in **CupCarbon**, forming a smart parking network:
- Virtual IoT nodes correspond to physical devices.
- MQTT packets flow between simulated nodes and broker.

CupCarbon provides a visual layout of the city parking area with connected sensors and wireless communication lines.

---

## 💡 Status Indication System

| Status | NeoPixel Color | RGB LED Color | Description |
|---------|----------------|----------------|--------------|
| Free | 🟢 Green | Green | Slot available |
| Occupied | 🔴 Red | Red | Vehicle detected |
| Reserved Free | 🔵 Blue | Blue | Device not connected in Neo pixel/ Slot reserved but empty in RGB LED |
| Reserved Occupied | 🟣 Magenta| Yellow | Reserved free |
| System Ready | ⚪ White | — | Initialization / connected |

## ⚙️ Workflow

1. Sensors detect the presence of a car in each slot.  
2. ESP32 processes the data and updates LED color accordingly.  
3. Status is published to the MQTT topic (e.g., `parking/spot/1`).  
4. CupCarbon simulates message transfer between all virtual nodes.  
5. Dashboard or broker receives and displays the combined parking map.

---

## 🔌 Hardware Connections
ESP32 with NeoPixel Array (Parking Spots 1–3)
| Component                | ESP32 Pin | Description                |
| ------------------------ | --------- | -------------------------- |
| Ultrasonic Sensor – VCC  | 5V        | Power supply               |
| Ultrasonic Sensor – GND  | GND       | Ground                     |
| Ultrasonic Sensor – TRIG | D12       | Trigger signal             |
| Ultrasonic Sensor – ECHO | D14       | Echo input                 |
| NeoPixel DIN             | D15       | Data input for LED strip   |
| NeoPixel Power           | 5V        | Power supply for LED array |
| NeoPixel Ground          | GND       | Common ground              |

ESP8266 with NeoPixel Array (Parking Spots 4–5)

| Component              | ESP8266 Pin | Description                               |
| ---------------------- | ----------- | ----------------------------------------- |
| NeoPixel (DIN)         | D2          | Data input pin for the NeoPixel LED array |
| NeoPixel (VCC)         | 5V (Vin)    | Power for the LED strip (5V recommended)  |
| NeoPixel (GND)         | GND         | Common ground between ESP8266 and LEDs    |
| Ultrasonic Sensor VCC  | 5V (Vin)    | Power supply for sensor                   |
| Ultrasonic Sensor GND  | GND         | Ground connection                         |
| Ultrasonic Sensor TRIG | D5          | Trigger pin for distance measurement      |
| Ultrasonic Sensor ECHO | D6          | Echo input pin                            |
| Reserved (optional)    | D7 / D8     | For adding more sensors or control logic  |



ESP32 with RGB LED (Parking Spots 6–10)

| Component                  | ESP32 Pin     | Description              |
| -------------------------- | ------------- | ------------------------ |
| **Ultrasonic Sensor VCC**  | 5 V           | Power supply             |
| **Ultrasonic Sensor GND**  | GND           | Common ground            |
| **Ultrasonic Sensor TRIG** | D12           | Trigger output pin       |
| **Ultrasonic Sensor ECHO** | D14           | Echo input pin           |
| **RGB LED – R (Red)**      | D15           | Controls red intensity   |
| **RGB LED – G (Green)**    | D2            | Controls green intensity |
| **RGB LED – B (Blue)**     | D4            | Controls blue intensity  |
| **VCC (LED)**              | 3.3 V or 5 V* | Power to RGB LED         |
| **GND (LED)**              | GND           | Common ground            |

**Install Libraries**
- `Adafruit_NeoPixel`
- `PubSubClient`
- `WiFi.h` or `ESP8266WiFi.h`

**Arduino**

Use the codes from the github and assign wifi details and password and ip address of the computer

**Note Connect same wifi to the central computer and the Esp32 devices**

if 1-3 programs are in blue led and not Changing to Green.Turn off the wifi of the Central computer and also Power off the Devices and Connect the Wifi first in the computer and then power the esp32 

----

**CupCarbon Simulation Guide**


This project is also implemented and visualized in the CupCarbon IoT Simulator, where all parking spots (1–10) are represented as IoT nodes connected over a virtual Wi-Fi/MQTT network.
This allows testing IoT communication and sensor logic even without physical hardware.

Step-by-Step Setup

1.Install CupCarbon

2.Run the CupCarbon

        ./run.sh
   

3.Create a New Project

4.Zoom to a suitable location and switch to the IoT Layer.

5.Add IoT Nodes (Parking Spots 💡)

6.CLick on the program and Add the Program from the Scripts folder in the github

7.Assign each node to the program 
-Open Device Parameters and left click on the Iot node(bulb symbol on the map) and in script file in the device paremeter select sensor1.py and then press run button next to the script

<img width="265" height="339" alt="image" src="https://github.com/user-attachments/assets/41d20705-47f2-487a-9950-5127e5c62119" />



8.Run the Simulation

<img width="1850" height="1053" alt="image" src="https://github.com/user-attachments/assets/eba5e757-6fbe-41a4-89eb-4ce5b6b8b989" />



<img width="1850" height="1053" alt="image" src="https://github.com/user-attachments/assets/3b634aed-437b-420f-93aa-515f958ca48a" />

----


**Central Computer**


1.Download the mqtt.py from the github files and make it executable using (Linux)

    chmod +x mqtt.py
    
2. Run the Program (Linux)
   
          python3 mqtt.py
   
  
<img width="882" height="396" alt="Screenshot from 2025-10-06 12-59-00" src="https://github.com/user-attachments/assets/b9a6d6c7-f4b0-4b80-a169-c2a275d94536" />

3. Assign Parking status--in terminal run the program and press q to change it to reserved or unreserved mode.

4. Press L for log of the status of the Smart Parking System.

____
<img width="1276" height="1280" alt="image" src="https://github.com/user-attachments/assets/898fe373-f051-426a-ba71-d122fc19d2f3" />
<img width="1280" height="1147" alt="image" src="https://github.com/user-attachments/assets/6349074a-67ce-400d-9623-b912dd1964f3" />
<img width="1280" height="1265" alt="image" src="https://github.com/user-attachments/assets/f3442fa4-24ac-4fa4-adc2-133a92b5fd11" />
<img width="1150" height="1280" alt="image" src="https://github.com/user-attachments/assets/d95614ed-5827-447d-8e34-7fafb809f9c0" />
<img width="687" height="1280" alt="image" src="https://github.com/user-attachments/assets/b6ff0295-6113-41b8-84b7-b8b6f7bfc895" />


## 🌟 Conclusion & Acknowledgment

The **Smart Parking System using CupCarbon** combines the power of real IoT hardware and virtual simulation to demonstrate scalable smart parking solutions.  
This work reflects my interest in **Robotics, Embedded Systems, and Smart City IoT research**.  
I would like to express my sincere gratitude to Dr. Ahcene Bounceur,
lead developer of the CupCarbon IoT Simulator (Klines 7.1 version),
for his supervision, guidance, and continuous support during this project.
His contributions to IoT simulation research and tools made this entire work possible.

🤝 Contributions, improvements, and suggestions are always welcome!  
📩 For queries : saitejavarma801@gmail.com


