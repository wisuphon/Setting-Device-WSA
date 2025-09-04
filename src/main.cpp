#include <Arduino.h>
#include <ModbusMaster.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define display_RESET - 1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define MODBUS Serial1
#define MAX485_DE 14
#define modbus_rx 16
#define modbus_tx 17

uint8_t addressID[] = {200, 20, 21, 22};
uint8_t addrID = 200;
uint8_t state = 0;
uint8_t countFine = 0;
String state_mode[] = {"OFF", "ON"};
String mode;
bool fineAddress = true;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, display_RESET);
ModbusMaster node;

void setAddr_PM();
void runFan();
void readFan();
void setAddr_Air();
void test_Air();

void preTransmission()
{

  digitalWrite(MAX485_DE, 0);
}

void postTransmission()
{

  digitalWrite(MAX485_DE, 1);
}

void functionset_PM()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Set Address Fan PM2.5");
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.println("   :WSA   ");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Fine");
  display.println("Address");
  display.display();
  fineAddress = true;
  while (fineAddress)
  {
    int8_t result;
    for (int i = 0; i < 4; i++)
    {
      node.begin(addressID[i], MODBUS);
      result = node.readHoldingRegisters(0x00, 1);
      if (result == node.ku8MBSuccess)
      {
        addrID = addressID[i];
        fineAddress = false;
        break;
      }
      delay(100);
    }
    countFine++;
    if (countFine == 10)
    {
      display.clearDisplay();
      display.display();
      while (true)
      {

        display.setTextSize(1.5);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Not found Address");
        display.println();
        display.println("Presses 3 Restart");
        display.display();
        if (!digitalRead(5))
        {
          ESP.restart();
        }
      }
    }
  }
  delay(1000);
  display.clearDisplay();
  display.display();

  while (1)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Fan PM2.5 ID: ");
    // display.print("AddressID: ");
    display.println(addrID);
    display.println("> 1.Set Address");
    display.println("> 2.Test Fan");
    display.println("> 3.Back");
    display.display();

    if (digitalRead(2) == 1)
    {
      setAddr_PM();
    }
    if (digitalRead(12) == 1)
    {
      runFan();
    }
    if (digitalRead(5) == 0)
    {
      display.clearDisplay();
      display.display();
      break;
    }
    // Serial.print(digitalRead(2));
    // Serial.println(digitalRead(12));
    delay(100);
  }
}

void functionset_AIR()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(" Set Address Airintake");
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.println("   :WSA   ");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Fine");
  display.println("Address");
  display.display();
  fineAddress = true;
  while (fineAddress)
  {
    int8_t result;
    node.begin(0, MODBUS);
    result = node.readHoldingRegisters(0x00, 1);
    if (result == node.ku8MBSuccess)
    {
      addrID = node.getResponseBuffer(0);
      fineAddress = false;
      break;
    }
    delay(100);
    countFine++;
    if (countFine == 10)
    {
      display.clearDisplay();
      display.display();
      while (true)
      {

        display.setTextSize(1.5);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Not found Address");
        display.println();
        display.println("Presses 3 Restart");
        display.display();
        if (!digitalRead(5))
        {
          ESP.restart();
        }
      }
    }
  }
  delay(1000);
  display.clearDisplay();
  display.display();

  while (1)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("AirintakeID: ");
    // display.print("AddrID: ");
    display.println(addrID);
    display.println("> 1.Set Airintake");
    display.println("> 2.Test Airintake");
    display.println("> 3.Back");
    display.display();

    if (digitalRead(2) == 1)
    {
      setAddr_Air();
    }
    if (digitalRead(12) == 1)
    {
      test_Air();
    }
    if (digitalRead(5) == 0)
    {
      display.clearDisplay();
      display.display();
      break;
    }
    delay(100);
  }
}
// SETUP:
void setup()
{
  pinMode(2, INPUT);
  pinMode(5, INPUT_PULLUP);
  pinMode(12, INPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  Serial.begin(115200);
  MODBUS.begin(9600, SERIAL_8N1, modbus_rx, modbus_tx);
  // node.begin(address, MODBUS);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();
}

void loop()
{
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 0);
  display.println(":WSA");
  display.setTextSize(1);
  display.println("> 1.Set PM 2.5 Fan");
  display.println("> 2.Set Airintake");
  display.display();

  if (digitalRead(2) == 1)
  {
    functionset_PM();
  }
  if (digitalRead(12) == 1)
  {
    functionset_AIR();
  }
}

void setAddr_PM()
{
  delay(500);
  display.clearDisplay();
  uint8_t setID;
  uint8_t result;
  while (true)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Fan PM2.5 ID: ");
    // display.print("AddressID: ");
    display.println(addrID);
    display.println("> 1.Set to 20");
    display.println("> 2.Set to 21");
    display.println("> 3.Set to 22");
    display.display();

    if (digitalRead(2) == 1)
    {
      setID = 20;
      break;
    }
    if (digitalRead(12) == 1)
    {
      setID = 21;
      break;
    }
    if (digitalRead(5) == 0)
    {
      setID = 22;
      break;
    }
  }
  delay(100);
  node.writeSingleRegister(0x04, 0x55);
  delay(100);
  node.writeSingleRegister(0x00, setID);
  delay(500);
  node.begin(setID, MODBUS);
  result = node.readHoldingRegisters(0x00, 1);
  if (result == node.ku8MBSuccess)
  {
    addrID = node.getResponseBuffer(0);
    Serial.println(addrID);
  }
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Finish");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void runFan()
{
  delay(500);
  // readFan();
  display.clearDisplay();
  display.display();
  mode = "";
  while (true)
  {
    if (digitalRead(2) == 1)
    {
      node.writeSingleRegister(0x05, 0);
      readFan();
      display.clearDisplay();
      display.display();
    }
    if (digitalRead(12) == 1)
    {
      node.writeSingleRegister(0x05, 1);
      readFan();
      display.clearDisplay();
      display.display();
    }
    if (digitalRead(5) == 0)
    {
      display.clearDisplay();
      display.display();
      break;
    }
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Fan PM2.5 ID: ");
    // display.print("AddressID: ");
    display.println(addrID);
    display.print("Fan Mode: ");
    display.println(mode);
    display.print("> 1. OFF  ");
    display.println("> 2. ON");
    display.println("> 3. Back");
    display.display();
  }
  delay(300);
}

void readFan()
{
  uint8_t result;
  result = node.readHoldingRegisters(0x05, 1);
  if (result == node.ku8MBSuccess)
  {
    if (node.getResponseBuffer(0) == 0)
    {
      mode = state_mode[0];
    }
    else
    {
      mode = state_mode[1];
    }
  }
}

void setAddr_Air()
{
  delay(500);
  display.clearDisplay();
  uint8_t setID;
  uint8_t result;
  while (true)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("AirintakeID: ");
    // display.print("AddrID: ");
    display.println(addrID);
    display.println("> 1.Set to 10");
    display.println("> 2.Set to 11");
    display.println("> 3.Set to 12");
    display.display();

    if (digitalRead(2) == 1)
    {
      setID = 10;
      break;
    }
    if (digitalRead(12) == 1)
    {
      setID = 11;
      break;
    }
    if (digitalRead(5) == 0)
    {
      setID = 12;
      break;
    }
  }
  delay(100);

  node.begin(0, MODBUS);
  node.setTransmitBuffer(0, lowWord(setID));
  node.setTransmitBuffer(1, highWord(setID));
  result = node.writeMultipleRegisters(0x00, 1);
  delay(500);
  result = node.readHoldingRegisters(0x00, 1);
  if (result == node.ku8MBSuccess)
  {
    addrID = node.getResponseBuffer(0);
    Serial.println(addrID);
  }
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Finish");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void test_Air()
{
  delay(500);
  display.clearDisplay();
  display.display();
  mode = "";
  node.begin(addrID, MODBUS);
  node.writeSingleCoil(0x0, 0);
  node.writeSingleCoil(0x1, 0);
  unsigned long st_relay = 0;
  bool ready_st = false;
  while (true)
  {
    if (ready_st && millis() - st_relay > 5000)
    {
       node.writeSingleCoil(0x0, 0);
      node.writeSingleCoil(0x1, 0);
      ready_st = false;
      mode = "STOP";
      display.clearDisplay();
      display.display();
    }
    if (digitalRead(2) == 1)
    {
      node.writeSingleCoil(0x0, 0);
      node.writeSingleCoil(0x1, 0);
      node.writeSingleCoil(0x1, 1);
      mode = state_mode[0];
      display.clearDisplay();
      display.display();
      st_relay = millis();
      ready_st = true;
    }
    if (digitalRead(12) == 1)
    {
      node.writeSingleCoil(0x0, 0);
      node.writeSingleCoil(0x1, 0);
      node.writeSingleCoil(0x0, 1);
      mode = state_mode[1];
      display.clearDisplay();
      display.display();
      st_relay = millis();
      ready_st = true;
    }
    if (digitalRead(5) == 0)
    {
      node.writeSingleCoil(0x0, 0);
      node.writeSingleCoil(0x1, 0);
      display.clearDisplay();
      display.display();
      break;
    }
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("AirintakeID: ");
    // display.print("AddrID: ");
    display.println(addrID);
    display.print("Airintake Mode: ");
    display.println(mode);
    display.print("> 1. OFF  ");
    display.println("> 2. ON");
    display.println("> 3. Back");
    display.display();
  }
  delay(300);
}

// #include <Arduino.h>
// #include <ModbusMaster.h>

// #define MODBUS Serial1
// #define MAX485_DE 14
// #define modbus_rx 16
// #define modbus_tx 17
// // instantiate ModbusMaster object
// ModbusMaster node;

// void preTransmission()
// {

//   digitalWrite(MAX485_DE, 0);
// }

// void postTransmission()
// {

//   digitalWrite(MAX485_DE, 1);
// }

// void setup()
// {
//   // use Serial (port 0); initialize Modbus communication baud rate
//   Serial.begin(115200);
//   pinMode(MAX485_DE, OUTPUT);
//   digitalWrite(MAX485_DE, 0);
//   MODBUS.begin(9600, SERIAL_8N1, modbus_rx, modbus_tx);
//   node.begin(1, MODBUS);
//   node.preTransmission(preTransmission);
//   node.postTransmission(postTransmission);
//   // communicate with Modbus slave ID 2 over Serial (port 0)
  
// }

// void loop() {
//   uint8_t j, result;
//   uint16_t data[2];

//   // // Toggle the coil at address 0x0002 (Manual Load Control)
//   // result = node.writeSingleRegister(0x0002, state);
//   // state = !state;

//   // Read 16 registers starting at 0x3100)
//   result = node.readInputRegisters(0x01, 2);
//   if (result == node.ku8MBSuccess) {
//     for (j = 0; j < 2; j++) {
//       data[j] = node.getResponseBuffer(j);
//       Serial.println(data[j]);
//     }
//     // Serial.println(node.getResponseBuffer(0));
//   }
//   Serial.println("<>");

//   delay(1000);
// }



// void loop()
// {
//   static uint32_t i;
//   uint8_t j, result;
//   uint16_t data[6];
  
//   i = 10;
//   node.begin(0, MODBUS);
//   Serial.println(i);
  
//   // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
//   node.setTransmitBuffer(0, lowWord(i));
  
//   // // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
//   node.setTransmitBuffer(1, highWord(i));
  
//   // // slave: write TX buffer to (2) 16-bit registers starting at register 0
//   result = node.writeMultipleRegisters(0x00, 1);
//   Serial.println(result,HEX);
//   // slave: read (6) 16-bit registers starting at register 2 to RX buffer
//   result = node.readHoldingRegisters(0x00, 1);
//   Serial.println(result,HEX);
  
//   // do something with data if read is successful
//   if (result == node.ku8MBSuccess)
//   {
//     int addrID = node.getResponseBuffer(0);
//     Serial.println(addrID);
//   }
//     // Serial.println(); 
//   delay(1000);
// }


// void setup()
// {
//   Serial.begin(115200);
//   pinMode(2, INPUT);
//   pinMode(5, INPUT_PULLUP);
//   pinMode(12, INPUT);
//   //   pinMode(2, OUTPUT);
//   // pinMode(5, OUTPUT);
//   // pinMode(12, OUTPUT);
// }
// void loop()
// {
//   Serial.print(digitalRead(2));
//   Serial.print("<>");
//   Serial.print(digitalRead(5));
//   Serial.print("<>");
//   Serial.println(digitalRead(12));
//   // digitalWrite(2,1);
//   // digitalWrite(5,1);
//   // digitalWrite(12,1);
//   // delay(500);
//   //   digitalWrite(2,0);
//   // digitalWrite(5,0);
//   // digitalWrite(12,0);
//   delay(100);
// }