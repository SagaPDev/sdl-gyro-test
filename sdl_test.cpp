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
using namespace std;
int main () {
  SDL_Event event;  
  SDL_GameController *controller =nullptr;
  bool isRunning=true;
  bool gyroEnabled=false;
  bool accelEnabled=false; 
  vector<float> gyro(3);
  vector<float> accel(3);
	static constexpr float toDegPerSec = float(180. / M_PI);
	static constexpr float toGs = 1.f / 9.8f;

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
      cout.precision(5);
      SDL_GameControllerGetSensorData(controller,SDL_SENSOR_GYRO, &gyro[0], 3);
      cout<<"gyro X= "<<setw(11)<<gyro[0]*toDegPerSec<<" gyro y= "<<setw(11)<<gyro[1]*toDegPerSec<<" gyro z= "<<setw(11)<<gyro[2]*toDegPerSec<<" -- ";
    }

    /*IMU accelerometer*/
    if (accelEnabled==true){
      cout<<fixed;
      cout.precision(5);
      SDL_GameControllerGetSensorData(controller,SDL_SENSOR_GYRO, &accel[0], 3);
      cout<<"accel X= "<<setw(11)<<accel[0]*toGs<<" accel y= "<<setw(11)<<accel[1]*toGs<<" accel z= "<<setw(11)<<accel[2]*toGs<<"\r"<<flush;
    }

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
