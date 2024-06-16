# Plasma Monitoring

LabView Interface + Arduino code to control the frequency of the optical signal of a plasma. 

Signal from a photodiode is read by the arduino, that sends the data via USB to the PC. LabView interprets the data and does several operations on it - display, controlling LEDs based on the frequency of the optical signal, saving data to files, etc. LabView has action buttons, ment to ajust the frequency of the signal in a real process, and has an automatic mode, where the Arduino automatically controls the action buttons. LabView also sends data to the Arduino, so that if an operator changes a parameter in the interface, the Arduino in automatic mode can function accordingly.

![image](https://github.com/Rui6filipe/Plasma-Monitoring/assets/162214885/d25fe4fc-4ed4-482f-8563-36186d2ce5ea)
