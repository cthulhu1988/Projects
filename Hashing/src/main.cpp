#include <Arduino.h>
#include <Hash.h>

char data[] = "the quick brown fox jumps over the lazy dog.";
void setup() {
    Serial.begin(115200);   // Initialize serial communications with the PC
}

void loop() {
    Serial.print(sha1(data));
  // put your main code here, to run repeatedly:
  delay(2000);
}
