#include <MatrixMini.h>
#include <MatrixLaserSensor.h>
#include <Arduino>

#define SPEED1 21
#define SPEED2 30

#define SPEED_TURBO1 14 * 5
#define SPEED_TURBO2 20 * 5

#define MOTOR1 1
#define MOTOR2 4

bool ctrlMode = false;
bool autoMode = true;

void setup() {
  Serial.begin(115200);
  Mini.begin();

  if(Mini.PS2.begin()){ // 4 Wires on D1, 2 Wires on D3
        Serial.println("PS2 controller detect!");
  } else {
        Serial.println("could not find PS2 controller");
        Serial.println("make sure your controller power is on and press the start button");
        while(1);  
  }

}

inline void setSpeed(int motor, int speed) {
  if(motor == MOTOR1) {
    Mini.I2C1.MXctrl.motorSet(motor, -speed);
  } else {
    Mini.I2C1.MXctrl.motorSet(motor, speed);
  }
}

void drive(int speed1,int speed2,int time) {
  Mini.I2C1.MXctrl.motorSet(MOTOR1, SPEED1 * -speed1);
  Mini.I2C1.MXctrl.motorSet(MOTOR2, SPEED2 * speed2);
  delay(time);

  Mini.I2C1.MXctrl.motorSet(MOTOR1, 0);
  Mini.I2C1.MXctrl.motorSet(MOTOR2, 0);
  delay(100);
}

// движение робота и повороты
void move(int x, int sp, bool turbo) {

  float x1 = 1.0;
  float x2 = 1.0;

  if(Mini.PS2.LEFT) {
    x1 = -1;
    x2 = 1;
  } else if(Mini.PS2.RIGHT) {
    x1 = 1;
    x2 = -1;
  }
  if(turbo){
    setSpeed(MOTOR1, SPEED_TURBO1 * sp * x1);
    setSpeed(MOTOR2, SPEED_TURBO2 * sp * x2); 
  } else {
    setSpeed(MOTOR1, SPEED1 * sp * x1);
    setSpeed(MOTOR2, SPEED2 * sp * x2); 
  }

}
//___________

void loop() {

  if(autoMode && Mini.PS2.START){
    drive(1, -1, 800);
    drive(2, 2, 800);
    delay(300);
    autoMode = false;
  }

  if(Mini.PS2.CIRCLE){
    ctrlMode = true;
  }

  delay(50);
  Mini.PS2.polling();

  if(ctrlMode){

    int sp = 0;
    if(Mini.PS2.R1) {
      sp = 1;
    } else if(Mini.PS2.L2) {
      sp = -1;
    }

// вверх вниз
    if(Mini.PS2.TRIANGLE) {
      Mini.I2C1.MXctrl.motorSet(2, 40);
      delay(200);
      Mini.I2C1.MXctrl.motorSet(2, 0);
      delay(10);
    } else if(Mini.PS2.CROSS) {
      Mini.I2C1.MXctrl.motorSet(2, -40);
      delay(200);
      Mini.I2C1.MXctrl.motorSet(2, 0);
      delay(10);
    }
//___________
    

   // Serial.println(sp);
    int x = Mini.PS2.RX;

    //int q = Mini.PS2.RX;
    Serial.println(sp);

    move(x, sp, Mini.PS2.R2);

  }

}
