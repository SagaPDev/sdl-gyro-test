#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_sensor.h>
#include <SDL2/SDL_stdinc.h>
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
vector<float> gyro(3);
vector<float> accel(3);
vector<float> orientation(4);
static constexpr float toDegPerSec = float(180. / M_PI);
static constexpr float toGs = 1.f / 9.8f;

GamepadMotion gyroSensor;

void controller_init(){
  /*controller initialization*/
  for (int i=0;i<SDL_NumJoysticks();i++){
    if(SDL_IsGameController(i)){
      cout<<SDL_IsGameController(i)<<"\n";
      controller = SDL_GameControllerOpen(i);
      cout<<SDL_GameControllerNameForIndex(i)<<"\n";
      }
    }
  /*test gyro*/
  if (SDL_GameControllerHasSensor(controller,SDL_SENSOR_GYRO)){
    cout << "Gyro Detected\n";
    SDL_GameControllerSetSensorEnabled(controller,SDL_SENSOR_GYRO,SDL_TRUE);
  }
  else cout << "Gyro not Detected\n";

  if(SDL_GameControllerIsSensorEnabled(controller, SDL_SENSOR_GYRO)){
    cout<<"gyro enabled\n";
    gyroEnabled=true;
  }
  else cout<<"gyro disabled\n";

  /*test accelerometer*/
  if (SDL_GameControllerHasSensor(controller,SDL_SENSOR_ACCEL)){
    cout << "accelerometer Detected\n";
    SDL_GameControllerSetSensorEnabled(controller,SDL_SENSOR_ACCEL,SDL_TRUE);
  }
  else cout << "accelerometer not Detected\n";

      if(SDL_GameControllerIsSensorEnabled(controller, SDL_SENSOR_ACCEL)){
        cout<<"accelerometer enabled\n";
        accelEnabled=true;
      }
      else cout<<"accelerometer disabled\n";
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
      auto oldTime=chrono::steady_clock::now();

      gyroSensor.GetOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
      cout<<"\33[K"<<"orien X= "<<setw(8)<<orientation[1]<<" Y= "<<setw(8)<<orientation[2]<<" Z= "<<setw(8)<<orientation[3]<<" W= "<<setw(8)<<orientation[0]<<"\n";
      cout<<"\33[K"<<deltaTime<<"\n";
    }

    cout<<"\33[4F";

    if (newTime!=oldTime) newTime=chrono::steady_clock::now(); 

    deltaTime=((float)chrono::duration_cast<chrono::microseconds>(oldTime-newTime).count()) / 1000000.0f;

    gyroSensor.ProcessMotion(gyro[0]*toDegPerSec, gyro[1]*toDegPerSec, gyro[2]*toDegPerSec, accel[0]*toGs, accel[1]*toGs, accel[2]*toGs,deltaTime); 
    oldTime=chrono::steady_clock::now();

    /*event loop*/
    while(SDL_PollEvent(&event)){
      switch (event.type) {
        case SDL_CONTROLLERBUTTONDOWN:
          cout<<"\33[1F";
          cout<<"\n";
          cout<<"\33[K";
          cout<<SDL_GameControllerGetStringForButton(SDL_GameControllerButton(event.cbutton.button))<<"\n"; 
          break;
        case SDL_CONTROLLERAXISMOTION:
          /*cout<<SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(event.caxis.axis))<<" "<<event.caxis.value<<"\n";*/
          break;
        /*hot pluging*/
        case SDL_CONTROLLERDEVICEADDED:
          cout<<"controller conected\n";
          controller_init();
          break;
        case SDL_CONTROLLERDEVICEREMOVED:
          cout<<"controller removed\n";
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
