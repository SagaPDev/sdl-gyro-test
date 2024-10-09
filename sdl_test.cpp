#include <SDL2/SDL.h>
#include <ios>
#include <iostream>
#include <ostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include "GamepadMotion.hpp"

using namespace std;

SDL_Event event;  
SDL_GameController *controller =nullptr;
bool isRunning=true;
bool gyroEnabled=false;
bool accelEnabled=false;
float deltaTime=0.0;
vector<float> gravity(3);
vector<float> gyro(3);
vector<float> accel(3);
vector<float> orientation(4);
int count=0;

static constexpr float toDegPerSec = float(180. / M_PI);
static constexpr float toGs = 1.f / 9.8f;

GamepadMotion gyroSensor;

void controller_init(){
  SDL_GameController *test_controller =nullptr;
  bool test_gyroEnabled;
  bool test_accelEnabled;
  /*controller initialization*/
  for (int i=0;i<SDL_NumJoysticks();i++){
    if (SDL_IsGameController(i)){
      cout<<SDL_GameControllerNameForIndex(i)<<" detected"<<"\n";
      test_controller = SDL_GameControllerOpen(i);
      if(SDL_IsGameController(i)){
        /*test gyro*/
        if (SDL_GameControllerHasSensor(test_controller,SDL_SENSOR_GYRO)){
          cout << "Gyro Detected\n";
          SDL_GameControllerSetSensorEnabled(test_controller,SDL_SENSOR_GYRO,SDL_TRUE);
          test_gyroEnabled=true;
        }
        else {
          cout<<"gyro not detected\n";
          test_gyroEnabled=false;
        }

        /*test accelerometer*/
        if (SDL_GameControllerHasSensor(test_controller,SDL_SENSOR_ACCEL)){
          cout << "accelerometer Detected\n";
          SDL_GameControllerSetSensorEnabled(test_controller,SDL_SENSOR_ACCEL,SDL_TRUE);
        }
        else{
          cout << "accelerometer not Detected\n";
          test_accelEnabled=false;
        }
      }
      if (test_accelEnabled && test_gyroEnabled){
        cout<<"\n";
        controller = test_controller;
        cout<<SDL_GameControllerNameForIndex(i)<<"\n";
        gyroEnabled=true;
        accelEnabled=true;
      }
    }
  }


}

int main () {
  
  chrono::steady_clock::time_point oldTime;
  chrono::steady_clock::time_point newTime;

  newTime=chrono::steady_clock::now();
  oldTime=newTime;
  cout.precision(5);
  cout<<fixed;
  SDL_GameControllerGetSensorData(controller,SDL_SENSOR_GYRO, &gyro[0], 3);

  /*SDL initializATION*/
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
  if((SDL_Init(SDL_INIT_GAMECONTROLLER))<0){
    cout<<"could not initialize SDL \n"<<SDL_GetError();
    return -1;
  }
  else{
    cout<<"SDL initialized \n";
  }

  controller_init();
  /*GAME LOOP*/
  while(isRunning){
    /*IMU gyro*/
    if (gyroEnabled==true){
      SDL_GameControllerGetSensorData(controller,SDL_SENSOR_GYRO, &gyro[0], 3);
      cout<<"\33[K"<<"gyro  X= "<<setw(8)<<gyro[0]*toDegPerSec<<" Y= "<<setw(8)<<gyro[1]*toDegPerSec<<" Z= "<<setw(8)<<gyro[2]*toDegPerSec<<"\n";
    }

    /*IMU accelerometer*/
    if (accelEnabled==true){
      SDL_GameControllerGetSensorData(controller,SDL_SENSOR_ACCEL, &accel[0], 3);
      cout<<"\33[K"<<"accel X= "<<setw(8)<<accel[0]*toGs<<" Y= "<<setw(8)<<accel[1]*toGs<<" Z= "<<setw(8)<<accel[2]*toGs<<"\n";
    }


    /*Sensor Fussion*/
    if (gyroEnabled && accelEnabled){
      if (oldTime!=newTime)
        newTime=chrono::steady_clock::now();

      deltaTime=((float)chrono::duration_cast<chrono::microseconds>(newTime-oldTime).count()) / 1000000.0f;

      gyroSensor.ProcessMotion(gyro[0]*toDegPerSec, gyro[1]*toDegPerSec, gyro[2]*toDegPerSec, accel[0]*toGs, accel[1]*toGs, accel[2]*toGs,deltaTime);
      oldTime=chrono::steady_clock::now();

      gyroSensor.GetOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
      cout<<"\33[K"<<"orien X= "<<setw(8)<<orientation[1]<<" Y= "<<setw(8)<<orientation[2]<<" Z= "<<setw(8)<<orientation[3]<<" W= "<<setw(8)<<orientation[0]<<"\n";
      cout<<"\33[K"<<deltaTime<<"\n";
    }

    cout<<"\33[4F";

    /*event loop*/
    while(SDL_PollEvent(&event)){
      switch (event.type) {
        case SDL_CONTROLLERBUTTONDOWN:
          cout<<"\33[1F";
          cout<<"\n";
          cout<<"\33[K";
          gyroSensor.GetGravity(gravity[0],gravity[1],gravity[2]);
          cout<<SDL_GameControllerGetStringForButton(SDL_GameControllerButton(event.cbutton.button))<<" "<<gravity[0]<<"/"<<gravity[1]<<"/"<<gravity[2]<<"\n"; 
          break;
        case SDL_CONTROLLERAXISMOTION:
          /*cout<<SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(event.caxis.axis))<<" "<<event.caxis.value<<"\n";*/
          break;
        /*hot pluging*/
        case SDL_CONTROLLERDEVICEADDED:
          if (!controller){
            cout<<"controller conected\n";
            controller_init();
          }
          break;
        case SDL_CONTROLLERDEVICEREMOVED:
          SDL_GameControllerClose(controller);
          controller=nullptr;
          cout<<"controller removed\n";
          gyroEnabled=false;
          accelEnabled=false;
          break;
      /*-------------------*/
        case SDL_QUIT:
          cout<<"\33[4E";
          cout<<"Quiting SDL.\n";
          isRunning=false;
        default:
          break;
      }
    }
  }


  /*SDL  quit*/
  SDL_Quit();
  return 0;
}
