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
int main () {
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
  chrono::steady_clock::time_point oldTime;
  chrono::steady_clock::time_point newTime;
  GamepadMotion gyroSensor;

  cout.precision(5);
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

  /*GAME LOOP*/
  while(isRunning){

    /*IMU gyro*/
    if (gyroEnabled==true){
      cout<<fixed;
      /*cout<<"gyro X= "<<setw(11)<<gyro[0]*toDegPerSec<<" gyro y= "<<setw(11)<<gyro[1]*toDegPerSec<<" gyro z= "<<setw(11)<<gyro[2]*toDegPerSec<<" -- ";*/
    }

    /*IMU accelerometer*/
    if (accelEnabled==true){
      cout<<fixed;
      cout.precision(5);
      SDL_GameControllerGetSensorData(controller,SDL_SENSOR_GYRO, &accel[0], 3);

      newTime=chrono::steady_clock::now(); 
      deltaTime=((float)chrono::duration_cast<chrono::microseconds>(oldTime-newTime).count()) / 1000000.0f;
      gyroSensor.ProcessMotion(gyro[0]*toDegPerSec, gyro[1]*toDegPerSec, gyro[2]*toDegPerSec, accel[0]*toGs, accel[1]*toGs, accel[2]*toGs,deltaTime);
    auto oldTime=chrono::steady_clock::now();
      gyroSensor.GetOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
      cout << "w= "<<setw(11)<<orientation[0]<<" x= "<<setw(11)<<orientation[1]<<" y= "<<setw(11)<<orientation[2]<<" z= "<<setw(11)<<orientation[3]<<"\r"<<flush;
      /*cout<<"accel X= "<<setw(11)<<accel[0]*toGs<<" accel y= "<<setw(11)<<accel[1]*toGs<<" accel z= "<<setw(11)<<accel[2]*toGs<<"\r"<<flush*/;
    }

    newTime=chrono::steady_clock::now(); 
    deltaTime=((float)chrono::duration_cast<chrono::microseconds>(oldTime-newTime).count()) / 1000000.0f;
    gyroSensor.ProcessMotion(gyro[0]*toDegPerSec, gyro[1]*toDegPerSec, gyro[2]*toDegPerSec, accel[0]*toGs, accel[1]*toGs, accel[2]*toGs,deltaTime); /* TODO: find a way to get deltaTime*/
    auto oldTime=chrono::steady_clock::now();
    /*event loop*/
    while(SDL_PollEvent(&event)){
     switch (event.type) {
        case SDL_CONTROLLERBUTTONDOWN:
          cout<<"\33[2K"<<SDL_GameControllerGetStringForButton(SDL_GameControllerButton(event.cbutton.button))<<"\n"; 
          break;
        case SDL_CONTROLLERAXISMOTION:
          cout<<"\33[2K"<<SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(event.caxis.axis))<<" "<<event.caxis.value<<"\n";
          break;
      /*hot pluging*/
        case SDL_CONTROLLERDEVICEADDED:
          cout<<"\33[2K"<<"controller conected\n";
          
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
          break;
        case SDL_CONTROLLERDEVICEREMOVED:
          cout<<"\33[2K"<<"controller removed\n";
          break;
      /*-------------------*/
        case SDL_QUIT:
          isRunning=false;
        default:
          break;
     }
    
    }
  }


  /*SDL  quit*/
  cout<<"\33[2K"<<"Quiting SDL.\n";
  SDL_Quit();
  return 0;
}
