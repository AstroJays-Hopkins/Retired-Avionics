#include <SPI.h>
#include <RH_RF95.h>
// Singleton instance of the radio driver
RH_RF95 rf95;
int led = 9;
void setup()
{
  //flash the led
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  Serial.begin(9600);
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  rf95.setFrequency(915.0);
  //rf95.setModemConfig(RH_RF95::Bw31_25Cr48Sf512);  //set for pre-configured long range
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);  //set for pre-configured long range
/*
 * Bw125Cr45Sf128  
Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range.
Bw500Cr45Sf128  
Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
Bw31_25Cr48Sf512  
Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
Bw125Cr48Sf4096   
Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.
 * 
 */
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
void loop()
{
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  uint8_t data[] = "PING!";
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
      delay(100);
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      digitalWrite(led, LOW);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(750);
}

