// Define Pins
const int ReadPin = A0;             // Analog pin to read sensor value
const int ReduceFreqPin = 2;        // Digital pin to control Reduce Frequency LED
const int IncreaseFreq1Pin = 4;     // Digital pin to control Increase Frequency 1 LED
const int IncreaseFreq2Pin = 7;     // Digital pin to control Increase Frequency 2 LED
const int IgnitionPin = 12;         // Digital pin to control Ignition LED

// Define global variables and 
// some default values
int sensorValue = 0;                // Variable to store sensor value
volatile int interval = 20;         // Interval between acquisition (in milliseconds)
int acquisition = 1;                // Flag to indicate whether acquisition should be running
unsigned long letter_timer;         // Timer to elapse after 2 seconds of no letters (a, b, c, d) being received
unsigned long interval_timer;       // Timer to elapse after interval has passed since last acquisition
int ready = 1;                      // Flag to indicate whether the interval has elapsed so that the signal can be acquired again

unsigned long previous_R;           // Previous R wave time
unsigned long current_R;            // Current R wave time
unsigned long RR_time;              // Time between two R waves (R-R interval)
float freq = 0;                     // Last frequency calculated
int R = 0;                          // Flag to indicate whether R wave is being detected
int high = 0;                       // High threshold for R wave detection
int low = 0;                        // Low threshold for R wave detection
int read_high = 0;                  // Flag to indicate when to receive high threshold

int dead = 0;                       // Flag to indicate whether no waves were detected for 10 seconds
int ignition_led = 0;               // Flag to indicate when ignition LED is ON
unsigned long dead_timer;           // Timer to elapse after 10 seconds of no waves being detected
unsigned long ignition_timer;       // Timer to elapse after ignition LED was ON for 5 seconds

int autom = 0;                      // Flag to indicate whether automatic mode is ON



void setup() 
{
  Serial.begin(9600);                    // Initialize serial communication at 9600 baud rate
  pinMode(ReduceFreqPin, OUTPUT);        // Set LED pins to output mode
  pinMode(IncreaseFreq1Pin, OUTPUT); 
  pinMode(IncreaseFreq2Pin, OUTPUT); 
  pinMode(IgnitionPin, OUTPUT);

  initialize();      
}


void initialize() 
{
  // Initialize critical values every time acquisition is started
  dead_timer = millis();
  previous_R = millis();
  float freq = 0;
}


void freq_read()                                        
{
  // Acquisition and signal processing

  sensorValue = analogRead(ReadPin);                    // Read the sensor value
  int signal_byte = map(sensorValue, 0, 1023, 0, 255);  // Map sensor value to a byte range
  Serial.print(char(signal_byte));                      // Send the byte to Serial as a char

  if (signal_byte < low) {
    R = 0;                                              // Reset R flag if signal is below low threshold
  }
  if ((signal_byte > high) && (R == 0)) {
    R = 1;                                              // Set R flag if signal is above high threshold
    current_R = millis();                               
    RR_time = current_R - previous_R;                   // Calculate R-R interval
    freq = 1000.0 / RR_time;                            // Calculate frequency with R-R interval   
    previous_R = current_R; 
    dead_timer = previous_R;                            // Reset dead timer
    dead = 0;                                           // Reset dead flag
  }

  if ((millis() - dead_timer > 10000)) {                // Check if dead_timer has elapsed (10 seconds have passed since last R wave detection)
    dead = 1;                                           // Set dead flag
    freq = 0;                                           // Change frequency to 0 Hz
  }

  if (autom == 1 && ignition_led == 0) {                // Check if automatic mode is ON and if ignition LED is off (not supposed to light other LEDs)
    action_control();                                   // Calls function to control LEDs based on the last frequency calculated
  }              
}


void action_control() 
{
  // Control action LEDs based on frequency ranges
  // Only one LED can be ON at a time

  if (freq > 3) {                           
    digitalWrite(ReduceFreqPin, HIGH);
    digitalWrite(IncreaseFreq1Pin, LOW);
    digitalWrite(IncreaseFreq2Pin, LOW);
  }
  else if (freq > 0.3 && freq < 0.6) {
    digitalWrite(ReduceFreqPin, LOW);
    digitalWrite(IncreaseFreq1Pin, HIGH);
    digitalWrite(IncreaseFreq2Pin, LOW);
  }
  else if (freq < 0.3) {
    digitalWrite(ReduceFreqPin, LOW);
    digitalWrite(IncreaseFreq1Pin, LOW);
    digitalWrite(IncreaseFreq2Pin, HIGH);
  }
  else {
    digitalWrite(ReduceFreqPin, LOW);
    digitalWrite(IncreaseFreq1Pin, LOW);
    digitalWrite(IncreaseFreq2Pin, LOW);
  }
}


void loop() 
{
  if (Serial.available() >= 1) {            // Check if data is available on serial port
    if (read_high == 1) {                   // Check if next incoming byte is to set the high threshold
      int threshold_byte = Serial.read();   // Read incoming byte as an int
      high = threshold_byte + 80;           // Set high threshold
      low = high - 10;                      // Set low threshold
      read_high = 0;                        
    }
    else if (read_high == 0) {
      char inByte = Serial.read();          // Read the incoming byte as a char to interpret commands
      switch (inByte) {                     // a-e: sampling rate and acquisition; f-m: manual mode actions; n-o: automatic mode ON/OFF; z: high threshold
        case 'a':                           
          if (acquisition == 0) {           // Check if acquisition is being started
            initialize();                   // Call function to initialize critical values
          }
          letter_timer = millis();          // Record when last letter was received
          interval = 40;                    // Set interval, which defines sampling rate
          acquisition = 1;                  // Acquisition has started or is continuing
          break;
        case 'b':
          if (acquisition == 0) {
            initialize();
          }
          letter_timer = millis();
          interval = 20;
          acquisition = 1;
          break;
        case 'c':
          if (acquisition == 0) {
            initialize();  
          }
          letter_timer = millis();
          interval = 10;
          acquisition = 1;
          break;
        case 'd':
          if (acquisition == 0) {
            initialize();
          }
          letter_timer = millis();
          interval = 5;
          acquisition = 1;
          break;
        case 'e':                         
          acquisition = 0;                          // Stop acquiring signal
          break;
        case 'f':                         
          if (autom==0){                            // Check if automatic mode is OFF
              digitalWrite(ReduceFreqPin, HIGH);    // Acts on action LEDs directly
          }
          break;
        case 'g':
          if (autom==0){
              digitalWrite(ReduceFreqPin, LOW);
          }
          break;
        case 'h':
          if (autom==0){
              digitalWrite(IncreaseFreq1Pin, HIGH);
          }
          break;
        case 'i':
          if (autom==0){
              digitalWrite(IncreaseFreq1Pin, LOW);
          }
          break;
        case 'j':
          if (autom==0){
              digitalWrite(IncreaseFreq2Pin, HIGH);
          }
          break;
        case 'k':
          if (autom==0){
              digitalWrite(IncreaseFreq2Pin, LOW);
          }
          break;
        case 'l':
          if (autom==0){
              digitalWrite(IgnitionPin, HIGH);
          }
          break;
        case 'm':
          if (autom==0){
              digitalWrite(IgnitionPin, LOW);
          }
          break;
        case 'n':                      
          autom = 1;                                // Automatic mode ON
          break;
        case 'o':
          autom = 0;                                // Automatic mode OFF
          digitalWrite(ReduceFreqPin, LOW);         // Turn OFF all action LEDs
          digitalWrite(IncreaseFreq1Pin, LOW);
          digitalWrite(IncreaseFreq2Pin, LOW);
          digitalWrite(IgnitionPin, LOW);
          break;
        case 'z':                                   
          read_high = 1;                            // Indicate that next incoming byte is to set high threshold
        default:
          break;
      }
    }
  }


  if (acquisition) {                                 // Check if acquisition is set
    if (ready) {                                     // Check if ready is set (interval has elapsed)
      freq_read();                                   // Call function to acquire signal and calculate frequency
      ready = 0;                                     // Reset ready flag
      interval_timer = millis();                     // Record when last acquisition was made
    }
    if ((millis() - interval_timer) >= interval) {   // Check if interval_timer has elapsed (interval has passed since last acquisition)
      ready = 1;                                     // Set ready flag
    }
  }


  if (autom == 1) {                                  // Automatic mode logic for ignition LED control:
    if (dead == 1 && ignition_led == 0) {            // Check if dead flag is set and the ignition LED is not already ON
      digitalWrite(IgnitionPin, HIGH);               // Turn ON ignition LED
      digitalWrite(ReduceFreqPin, LOW);              // Turn OFF all other action LEDs
      digitalWrite(IncreaseFreq1Pin, LOW);
      digitalWrite(IncreaseFreq2Pin, LOW);
      ignition_timer = millis();                     // Start timer for ignition LED 
      ignition_led = 1;                              // Set ignition LED flag
    }
    if (ignition_led == 1 && (millis() - ignition_timer) > 5000) {      // Check if ignition_timer has elapsed (ignition LED was ON for 5 seconds)
      digitalWrite(IgnitionPin, LOW);                                   // Turn OFF ignition LED
      ignition_led = 0;                                                 // Reset dead led acquisition
      dead = 0;
      dead_timer = millis();
    }
  }


  if ((millis() - letter_timer) > 2100) {           // Check if letter_timer has elapsed (more than 2 seconds have passed since last letter received)
    acquisition = 0;                                // Set acquisition to 0, so that signal acquisition is stopped
    digitalWrite(ReduceFreqPin, LOW);               // Turn OFF all action LEDs
    digitalWrite(IncreaseFreq1Pin, LOW);
    digitalWrite(IncreaseFreq2Pin, LOW);
    digitalWrite(IgnitionPin, LOW);
  }
}
