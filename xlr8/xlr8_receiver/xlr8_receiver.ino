#include <esp_now.h>
#include <WiFi.h>

//Right motor
int enableRightMotor=33; 
int rightMotorPin1=5;
int rightMotorPin2=26;
//Left motor
int enableLeftMotor=32;
int leftMotorPin1=27;
int leftMotorPin2=14;

#define MAX_MOTOR_SPEED 255

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;

//new
struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData receiverData;

//This function is used to map 0-4095 joystick value to 0-254. hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick values range from 0-4095. But its center value is not always 2047. It is little different.
//So we need to add some deadband to center value. in our case 1800-2200. Any value in this deadband range is mapped to center 127.
int mapAndAdjustJoystickDeadBandValues(int value)
{
  if (value >= 2200)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800)
  {
    value = map(value, 1800, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }
  return value;
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  Serial.print("right : ");
  Serial.println(rightMotorSpeed);
  Serial.print("left : ");
  Serial.println(leftMotorSpeed);
  if (rightMotorSpeed < 0)
  {
    analogWrite(enableRightMotor, abs(rightMotorSpeed));
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    analogWrite(enableRightMotor, rightMotorSpeed);
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,LOW);      
  }
  
  if (leftMotorSpeed < 0)
  {
    analogWrite(enableLeftMotor, abs(leftMotorSpeed));
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    analogWrite(enableLeftMotor, leftMotorSpeed);
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,LOW);      
  } 
}

int convertToSpeed(int joystick_value){
  return map(joystick_value, 0, 255, 0, MAX_MOTOR_SPEED);
}

void simpleMovements()
{
  if(receiverData.xAxisValue >= 130 && receiverData.yAxisValue<=105){
    rotateMotor(80, map(receiverData.yAxisValue, 105, 0, 0, MAX_MOTOR_SPEED*0.95));
  }
  else if(receiverData.xAxisValue <= 105 && receiverData.yAxisValue<=105){
    rotateMotor(map(receiverData.yAxisValue, 105, 0, 0, MAX_MOTOR_SPEED*0.95), 80);

  }
  else if(receiverData.xAxisValue >= 130 && receiverData.yAxisValue>=130){
    rotateMotor(-80,- map(receiverData.yAxisValue, 130, 255, 0, MAX_MOTOR_SPEED*0.95));
  }
  else if(receiverData.xAxisValue <= 105 && receiverData.yAxisValue>=130){
    rotateMotor(-map(receiverData.yAxisValue, 130, 255, 0, MAX_MOTOR_SPEED*0.95),-80);
  }
  else if (receiverData.xAxisValue >= 130)  //Move car Right
  {
    rotateMotor(
      -map(receiverData.xAxisValue, 130, 255, 0, MAX_MOTOR_SPEED*0.75),
      map(receiverData.xAxisValue, 130, 255, 0, MAX_MOTOR_SPEED*0.75)
    );
  }
  else if (receiverData.xAxisValue <= 105)   //Move car Left
  {
    rotateMotor(map(receiverData.xAxisValue, 105, 0, 0, MAX_MOTOR_SPEED*0.75), 
      -map(receiverData.xAxisValue, 105, 0, 0, MAX_MOTOR_SPEED*0.75)
    );
  }
  else if (receiverData.yAxisValue <= 105)       //Move car Forward
  {
    rotateMotor(  map(receiverData.yAxisValue, 105, 0, 0, MAX_MOTOR_SPEED)
      , map(receiverData.yAxisValue, 105, 0, 0, MAX_MOTOR_SPEED));
  }
  else if (receiverData.yAxisValue >= 130)   //Move car Backward
  {
    rotateMotor( -map(receiverData.yAxisValue, 130, 255, 0, MAX_MOTOR_SPEED)
      , -map(receiverData.yAxisValue, 130, 255, 0, MAX_MOTOR_SPEED));
  }
  else                                      //Stop the car
  {
    rotateMotor(0, 0);
  }   
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  // receiverData.xAxisValue = map(receiverData.xAxisValue, 0, 255, 0, 4095);
  // receiverData.yAxisValue = map(receiverData.yAxisValue, 0, 255, 0, 4095);
  String inputData ="";
  inputData = inputData + "values " + 
  
  receiverData.xAxisValue + "  " + 
  receiverData.yAxisValue
   + "  " + receiverData.switchPressed;
  Serial.println(inputData);
  Serial.println("HEYYYY");
  // if (receiverData.switchPressed == true)
  // {
  //   if (throttleAndSteeringMode == false)
  //   {
  //     throttleAndSteeringMode = true;
  //   }
  //   else
  //   {
  //     throttleAndSteeringMode = false;
  //   }
  // }

  // if (throttleAndSteeringMode)
  // {
  //   throttleAndSteeringMovements();
  // }
  // else
  // {
    simpleMovements();
  // }
  
  lastRecvTime = millis();   
}

void setUpPinModes()
{
  pinMode(enableRightMotor,OUTPUT);
  pinMode(rightMotorPin1,OUTPUT);
  pinMode(rightMotorPin2,OUTPUT);
  digitalWrite(enableRightMotor, HIGH);
  
  pinMode(enableLeftMotor,OUTPUT);
  pinMode(leftMotorPin1,OUTPUT);
  pinMode(leftMotorPin2,OUTPUT);
  digitalWrite(enableLeftMotor, HIGH);


  rotateMotor(0,0);
}

void setup() {
  // put your setup code here, to run once:
  setUpPinModes();
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  // put your main code here, to run repeatedly:
    //Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    rotateMotor(0, 0);
  }

}
