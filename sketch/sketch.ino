#include <MatrixMini.h>

#define SPEED1 10 * 3
#define SPEED2 8 * 3

#define SPEED_TURBO1 10 * 10
#define SPEED_TURBO2 8 * 10

#define MOTOR1 1
#define MOTOR2 4
#define MOTOR_ELEVATOR 3
#define MOTOR_LIFT 2
#define SERVO_TOWER 4
#define SERVO_CAPTURE 2

#define MAX_FLAG_DISTANCE 200
#define MAX_ASSFLAG_DISTANCE 350

#define AUTO_MODE_DELAY 300

bool ctrlMode = false;
bool autoMode = true;

void setup() {
  Serial.begin(115200);
  Mini.begin();

  // Чекаем подключение джостика
  if(Mini.PS2.begin()){ // 4 Wires on D1, 2 Wires on D3
        Serial.println("PS2 controller detect!");
  } else {
        Serial.println("could not find PS2 controller");
        Serial.println("make sure your controller power is on and press the start button");
        while(1);  
  }

  // Изменяем направление вращения мотора
  Mini.I2C1.MXctrl.motorInvert(MOTOR1, true);
  // Проверяем батарею робота
  Serial.print("Battery charge: ");
  Serial.println(Mini.I2C1.MXctrl.getBattery());
}

void setSpeed(int motor, int speed) {  // Задает мотор и его скорость
  Mini.I2C1.MXctrl.motorSet(motor, speed);
}

void drive(int speed1, int speed2, int time) {  // Функция езды по времени
  setSpeed(MOTOR1, SPEED1 * speed1);
  setSpeed(MOTOR2, SPEED2 * speed2);
  delay(time);

  setSpeed(MOTOR1, 0);
  setSpeed(MOTOR2, 0);
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
  if(turbo){  // Проверка ускорения
    setSpeed(MOTOR1, SPEED_TURBO1 * sp * x1);
    setSpeed(MOTOR2, SPEED_TURBO2 * sp * x2); 
  } else {
    setSpeed(MOTOR1, SPEED1 * sp * x1);
    setSpeed(MOTOR2, SPEED2 * sp * x2); 
  }

}

int readDistance(bool asslaser) {  // Проверка растояния датчиком
  if(asslaser){
    return Mini.I2C4.MXlaser.getDistance();
  } else {
    return Mini.I2C2.MXlaser.getDistance();
  }
}

bool flagDetected(bool assdis) {  // Проверка нахождения флага
  if(assdis){
    return readDistance(true) <= MAX_ASSFLAG_DISTANCE;
  } else {
    return readDistance(false) <= MAX_FLAG_DISTANCE;
  } 
}

void automode(bool simple, bool ass) {  // Запуск автономного периода
  if(ass) { //задний датчик ( синяя часть поля )
    if(simple) {
      drive(-1, 1, 800);
      drive(2, 2, 800);
    } else {
      delay(800);
      drive(1, 1, 900);
      delay(AUTO_MODE_DELAY);
      drive(-1, 1, 800);
      delay(AUTO_MODE_DELAY);
      drive(1, 1, 400);
      delay(AUTO_MODE_DELAY);
      for(int i = 0; i < 3; i++) {
        if(flagDetected(true)) {      
          Mini.RGB1.setRGB(0, 255, 0);
          if(i==0) {  //point 3
            drive(1, -1, 800);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1300);
            delay(AUTO_MODE_DELAY);
            drive(-1, 1, 700);
            setSpeed(MOTOR_ELEVATOR, 40);
            delay(200);
            setSpeed(MOTOR_ELEVATOR, 0);
            delay(10);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 2300);
            break;
          } else if(i==1) {  //point2
            drive(1, -1, 800);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1600);
            delay(AUTO_MODE_DELAY);
            drive(1, -1, 800);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1500);
            break;
          }

        } else {
            Mini.RGB1.setRGB(255, 0, 0);
        }

        if(i < 1) {
          drive(1, 1, 700);
          delay(300);
        } else {  //point 1
          drive(1, -1, 900);
          delay(AUTO_MODE_DELAY);
          drive(1, 1, 2600);
          delay(AUTO_MODE_DELAY);
          drive(-1, 1, 500);
          delay(AUTO_MODE_DELAY);
          drive(1, 1, 1600);
          break;
        }
      }
    }
    delay(300);
  }else {  //передний датчик ( красная часть поля )
    if(simple) {
      delay(800);
      drive(1, -1, 1000);
      drive(2, 2, 1600);
    } else {
      delay(800);
      drive(1, 1, 1100);
      delay(AUTO_MODE_DELAY);
      drive(1, -1, 800);
      delay(AUTO_MODE_DELAY);
      for(int i = 0; i < 3; i++) {
        if(flagDetected(false)) {      
          Mini.RGB1.setRGB(0, 255, 0);
          if(i==0) {  //point 3
            drive(-1, 1, 800);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1300);
            delay(AUTO_MODE_DELAY);
            drive(1, -1, 1000);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 2300);
            break;
          } else if(i==1) {  //point2
            drive(-1, 1, 800);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1600);
            delay(AUTO_MODE_DELAY);
            drive(-1, 1, 500);
            delay(AUTO_MODE_DELAY);
            drive(1, 1, 1200);
            break;
          }

        } else {
            Mini.RGB1.setRGB(255, 0, 0);
        }

        if(i < 1) {
          drive(1, 1, 800);
          delay(300);
        } else {  //point 1
          drive(-1, 1, 1000);
          delay(AUTO_MODE_DELAY);
          drive(1, 1, 2600);
          delay(AUTO_MODE_DELAY);
          drive(1, -1, 500);
          delay(AUTO_MODE_DELAY);
          drive(1, 1, 1600);
          break;
        }
      }
    }
    delay(300);
  }
}

void elCtrl(bool reverse=false) {  // Функция движения элеватором
  int turn = 1;
  if(reverse) {  // Изменения направления движения мотора
    turn = -1;
  }
  setSpeed(MOTOR_ELEVATOR, turn * 40);
  delay(200);
  setSpeed(MOTOR_ELEVATOR, 0);
  delay(10);
}

//___________

void capture(int servo) {
  int servo = 0
  if(Mini.PS2.DOWN) {
    servo++;
  } else if(Mini.PS2.UP) {
    servo--;
  }
    //Mini.M2.set(8 * servo);
    Mini.I2C1.MXctrl.servoSet(SERVO_TOWER, 1 * servo);
}

void loop() {
  if(autoMode && Mini.PS2.START){  // Запуск автономного периода
    automode(false, true);
    autoMode = false;
  }

  if(Mini.PS2.CIRCLE){  // Запуск управляемого периода
    ctrlMode = true;
  }

  delay(50);
  Mini.PS2.polling();

  // Поворот робота
  if(ctrlMode) {
    int sp = 0;
    if(Mini.PS2.R1) {
      sp = 1;
    } else if(Mini.PS2.L2) {
      sp = -1;
    }

    int hhj = 0;
    if(Mini.PS2.DOWN) {
      hhj = 1;
    } else if(Mini.PS2.UP) {
      hhj = -1;
    }



    capture(servo);

    // Движение захвата
    if(Mini.PS2.CROSS) {
      elCtrl();
    } else if(Mini.PS2.TRIANGLE) {
      elCtrl(true);
    }

    // Движение робота в автономном периоде
    int x = Mini.PS2.RX;
    move(x, sp, Mini.PS2.R2);
  }

}