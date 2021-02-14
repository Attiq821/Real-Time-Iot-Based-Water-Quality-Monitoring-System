# Real-Time-Iot-Based-Water-Quality-Monitoring-System

The traditional system to determine water quality is not reliable. It rely on collecting water samples, testing and analyses in water laboratories is costly and time consuming.
Human errors can occur and may not provide accurate result.It lack capability for real-time data capture, analyses and fast dissemination of information to relevant stakeholders for making timely and appropriate decisions.  

In this project, we present a design and development of a system for real time monitoring of the water quality using IoT (internet of things).The system consist of several sensors
are used to measuring physical and chemical parameters of the water. The parameters such as temperature, PH, turbidity, conductivity of the water can be measured.

The pH sensor (SKU: SEN0161/0169) measures the concentration of hydrogen ions. It shows the water is acidic or alkaline. Pure water has 7pH value, less than 7pH has acidic, more 
than 7pH has alkaline. The range of pH is 0-14 ph. For drinking purpose, it should be 6.5-8.5pH. 

Turbidity sensor (SKU: SEN0189) is a measure of the degree to which the water loses its transparency due to the presence of suspended particulates. The more total suspended solids
in the water, the murkier it seems and the higher the turbidity. 

Conductivity Sensor (SKU: SEN 0244) is a measure of water’s capability to pass electrical flow. This ability is directly related to the concentration of ions in the water. The 
more ions that are present, the higher the conductivity of water. 

Temperature sensor (DS18B20) is used to measure the temperature of the water.

The microcontroller will obtain and process the data from the sensors and sent the data to the IoT cloud server (Thingspeak) through NodeMCU/Wemos D1 R1 mini.

Applications:
==>Maintain a continuously healthy water supply
==>Industrial Waste Cleaning purpose
==>Fish pond water quality testing
==>Helpful in saving Aquatic life



