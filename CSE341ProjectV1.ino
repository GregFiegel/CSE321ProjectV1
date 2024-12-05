
//PS5 USB library - developed by Kristian Sloth Lauszus
//Github repository: github.com/felis/USB_Host_Shield_2.0
#include <PS5USB.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

void consoleRequestDetected();
void buildBitSeq();
void pollControllerInput();

USB Usb;
PS5USB PS5(&Usb);

#define DATA_PIN 12

#define N64_HIGH DDRB &= ~0x01
#define N64_LOW DDRB |= 0x01

uint16_t lastMessageCounter = -1;
uint32_t responseOld = 0x00000000;
uint32_t response = 0x00000000;
uint8_t analogX = 0x00;
uint8_t analogY = 0x00;
bool bitSeq[128] = {false};
volatile bool requestRecieved = false;
int i=0;

void setup(){
//serial connection pulled from "PS5USB.ino"
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS5 USB Library Started"));
//serial connection pulled from "PS5USB.ino"

  pinMode(DATA_PIN, INPUT);
  PS5.setLed(Red);
  Serial.print(F("\r\nBefore attatching"));
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), consoleRequestDetected, FALLING);
  Serial.print(F("\r\nAfter attatching"));
}



void loop(){
  Usb.Task();
  //Serial.print(("\r\nLoop number:"));
  //Serial.print(i);

  
  if (requestRecieved){
      Serial.print("\r\nRequest Flag set");
      sendResponse(response);
      Serial.print("\r\nResponse Sent");
      requestRecieved = false;
      Serial.print("\r\nrequestRecieved = false");
    }
  

  sendResponse(response);
  
  //int pinState = digitalRead(DATA_PIN);
  //Serial.print("\r\nrequestRecieved: ");
  //Serial.print(requestRecieved);

  if (PS5.connected() && lastMessageCounter != PS5.getMessageCounter()){

    responseOld = response;

    pollControllerInput();
    
    buildBitSeq();
    
    //Output response if it has changed
    if (response != responseOld){
      Serial.print("\r\n");
      Serial.print(response, BIN);
    }
  }
}

void pollControllerInput(){
   //Cross (A)
    if (PS5.getButtonPress(CROSS)){
      response = response | 0x80000000;
      //Serial.print(F("\r\nCross"));
    } else{
      response = response & 0x7FFFFFFF;
    }

    //Square (B)
    if (PS5.getButtonPress(SQUARE)){
      response = response | 0x40000000;
      //Serial.print(F("\r\nSquare"));
    } else{
      response = response & 0xBFFFFFFF;
    }

    //L1 (Z)
    if (PS5.getButtonPress(L1)){
      response = response | 0x20000000;
      //Serial.print(F("\r\nL1"));
    } else{
      response = response & 0xDFFFFFFF;
    }

    //Options/PS (Start)
    if (PS5.getButtonPress(OPTIONS) || PS5.getButtonPress(PS)){
      response = response | 0x10000000;
      //Serial.print(F("\r\nOption/PS"));
    } else{
      response = response & 0xEFFFFFFF;
    }

    //Up (dU)
    if (PS5.getButtonPress(UP)){
      response = response | 0x08000000;
      //Serial.print(F("\r\nUp"));
    } else{
      response = response & 0xF7FFFFFF;
    }

    //Down (dD)
    if (PS5.getButtonPress(DOWN)){
      response = response | 0x04000000;
      //Serial.print(F("\r\nDown"));
    } else{
      response = response & 0xFBFFFFFF;
    }

    //Left (dL)
    if (PS5.getButtonPress(LEFT)){
      response = response | 0x02000000;
      //Serial.print(F("\r\nLeft"));
    } else{
      response = response & 0xFDFFFFFF;
    }

    //Right (dR)
    if (PS5.getButtonPress(RIGHT)){
      response = response | 0x01000000;
      //Serial.print(F("\r\nRight"));
    } else{
      response = response & 0xFEFFFFFF;
    }

    //L2 (L)
    if (PS5.getAnalogButton(L2) > 200){
      response = response | 0x00200000;
      //Serial.print(F("\r\nL2 (asDigital)"));
    } else{
      response = response & 0xFFDFFFFF;
    }

    //R1 (R)
    if (PS5.getButtonPress(R1)){
      response = response | 0x00100000;
      //Serial.print(F("\r\nR1"));
    } else{
      response = response & 0xFFEFFFFF;
    }

    //RHatY- (cU)
    if (PS5.getAnalogHat(RightHatY) <= 50){
      response = response | 0x00080000;
      //Serial.print(F("\r\nRHatY-"));
    } else{
      response = response & 0xFFF7FFFF;
    }

    //RHatY+ (cD)
    if (PS5.getAnalogHat(RightHatY) >= 205){
      response = response | 0x00040000;
      //Serial.print(F("\r\nRHatY+"));
    } else{
      response = response & 0xFFFBFFFF;
    }

    //RHatX- (cL)
    if (PS5.getAnalogHat(RightHatX) <= 50){
      response = response | 0x00020000;
      //Serial.print(F("\r\nRHatX-"));
    } else{
      response = response & 0xFFFDFFFF;
    }

    //RHatX+ (cR)
    if (PS5.getAnalogHat(RightHatX) >= 205){
      response = response | 0x00010000;
      //Serial.print(F("\r\nRHatX+"));
    } else{
      response = response & 0xFFFEFFFF;
    }

    //Zero out (might cause a race condition but we'll see)
    response = response & 0xFFFF0000;
    //get analog y and flip the first bit (unsigned->signed, then -128)
    analogY = PS5.getAnalogHat(LeftHatY);
    //Y Deadzone
    if ((analogY > 123) && (analogY < 133)){
      analogY = 128;
    }
    analogY = analogY ^ 0x80;
    //get analog x and flip the first bit (unsigned->signed, then -128)
    analogX = PS5.getAnalogHat(LeftHatX);
    //X Deadzone
    if ((analogX > 123) && (analogX < 133)){
      analogX = 128;
    }
    analogX = analogX ^ 0x80;
    
    //LHatX (x-axis)
    response = response | (analogX << 8);
    //LHatY (y-axis)
    response = response | (analogY);
}

void consoleRequestDetected(){
  requestRecieved = true;
}

void sendResponse(uint32_t response){
  pinMode(DATA_PIN, OUTPUT);
  //send each bit
  for (bool bit : bitSeq){
    sendBit(bit);
  }
  pinMode(DATA_PIN, INPUT);
}

void buildBitSeq(){
  int j=0;
  for (int i=31; i>=0; i--){
    bool bit = (response >> i) & 1;
    if(bit){
      bitSeq[j] = 0;
      bitSeq[j+1] = 1;
      bitSeq[j+2] = 1;
      bitSeq[j+3] = 1;
    } else{
      bitSeq[j] = 0;
      bitSeq[j+1] = 0;
      bitSeq[j+2] = 0;
      bitSeq[j+3] = 1;
    }
    j += 4;
  }
}

void sendBit(bool bit){
  if(bit){
    digitalWrite(DATA_PIN, HIGH);
  }else{
    digitalWrite(DATA_PIN, LOW);
  }
delayMicroseconds(1);
}
