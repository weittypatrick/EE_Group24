#include <SPI.h> // SPI communication library
#include <MFRC522.h> // RFID library
#include <Servo.h> // Servo library

#define SS_PIN 10 // RFID SS pin
#define RST_PIN 9 // RFID reset
#define LED_PIN1 8 // Access granted LED(Green)
#define LED_PIN2 7 // Access denied LED(Red)
#define SERVO_PIN 2 // Servo pin
#define BUZZER_PIN 6 // Buzzer pin

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID object
Servo gateServo; // Servo object

void setup(){
  Serial.begin(9600); // Start serial monitor
  SPI.begin(); // Start SPI
  mfrc522.PCD_Init(); // Initialize RFID reader

  pinMode(LED_PIN1, OUTPUT); // LED1(Green) output
  pinMode(LED_PIN2, OUTPUT); // LED2(Red) output
  pinMode(BUZZER_PIN, OUTPUT); // Buzzer output
  gateServo.attach(SERVO_PIN); // Attach servo
  gateServo.write(0); // Gate closed

  Serial.println("Scan your RFID card..."); // User prompt
}

void loop(){
  if(!mfrc522.PICC_IsNewCardPresent()) return; // Check card
  if(!mfrc522.PICC_ReadCardSerial()) return; // Read card UID

Serial.print("UID tag: "); // Print UID label
String content = ""; // Store UID

for(byte i = 0; i < mfrc522.uid.size; i++){ // Loop through UID bytes
  if(mfrc522.uid.uidByte[i] < 0x10){
    content.concat("0");   // Leading zero
  }
  content.concat(String(mfrc522.uid.uidByte[i], HEX)); // Convert to HEX
}

Serial.println(content); // Show UID

// List of valid card UIDs
String validCards[]= {"11223344", "01020304", "aabbccdd"}; // Authorized cards
int numberOfCards = 3; // Number of cards

bool accessGranted = false; // Access flag

for(int i = 0; i < numberOfCards; i++){ // Check cards
  if(content == validCards[i]){ // Compare UID
    accessGranted = true; // Valid card
    break; // Exit loop
  }
}

if(accessGranted){ // If card is valid
  Serial.println("Access Granted"); // Message
  digitalWrite(LED_PIN1, HIGH); // Turn on LED1(Green)
  gateServo.write(90);   //Unlock by rotating 90 degrees
  delay(3000); // Wait 3s
  gateServo.write(0);   //lock again
  digitalWrite(LED_PIN1, LOW); // Turn LED(Green) off
}
else{ // If card not valid
  Serial.println("Access Denied"); // Message
  digitalWrite(LED_PIN2, HIGH); // Turn on LED2(Red)
  digitalWrite(BUZZER_PIN, HIGH); // Sound buzzer
  delay(3000); // Wait 3s
  digitalWrite(LED_PIN2, LOW); // Turn LED off
  digitalWrite(BUZZER_PIN, LOW); // Stop buzzer
}
}