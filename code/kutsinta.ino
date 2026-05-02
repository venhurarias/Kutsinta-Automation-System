#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Streaming.h>
#include <Chrono.h>
#include <Keypad.h>
#include "HX711.h"
#include <OneWire.h>
#include <DallasTemperature.h>


#define START_PIN 3  //SPST
#define STOP_PIN 4   //SPST

//CONVEYOR
#define CONVEYOR_PIN A1  //SSR

//CONTAINER DISPENSER
#define CONTAINER_DISPENSE_IR_PIN 13  //DIGITAL SENSOR

//MIXING
#define HOT_WATER_TEMP_PIN 5              //DS18B20
#define WATER_LEVEL A14                    //ANALOG SENSOR
#define HOT_WATER_VALVE_REFILL_PIN 10      //RELAY - Selonoid VALVE - OK
#define HOT_WATER_VALVE_PIN 26             //RELAY - AC - BALL VALVE - OK
#define HOT_WATER_HEATER_PIN A2            //SSR  - 220 1500w
#define CONTAINER_MIXER_IR_PIN 14         //DIGITAL SENSOR
#define CONTAINER_MIXER_STOPPER_A_PIN 2  //RELAY - OK
#define CONTAINER_MIXER_STOPPER_B_PIN 3  //RELAY - OK
#define COLD_WATER_VALVE_PIN 28           //RELAY - BALL VALVE - OK
#define COLD_FLOW_SENSOR 21                //interupt
#define SOLID_MIX_CONVEYOR_1_PIN 11       //RELAY - 12v - OK
#define SOLID_MIX_LINEAR_1_A_PIN 4       //RELAY - OK
#define SOLID_MIX_LINEAR_1_B_PIN 5       //RELAY - OK
#define SOLID_MIX_CONVEYOR_2_PIN 12       //RELAY - 12v - OK
#define SOLID_MIX_LINEAR_2_A_PIN 6       //RELAY - OK
#define SOLID_MIX_LINEAR_2_B_PIN 7       //RELAY - OK
#define SOLID_MIX_CONVEYOR_3_PIN 13       //RELAY - 12v -OK
#define SOLID_MIX_LINEAR_3_A_PIN 8       //RELAY - OK
#define SOLID_MIX_LINEAR_3_B_PIN 9       //RELAY - OK
#define LOADCELL_DOUT_PIN 24
#define LOADCELL_SCK_PIN 25
#define ORANGE_VALVE 33     //RELAY - 12v - OK
#define VIOLET_VALVE 32     //RELAY - 12v - OK
#define LY_WATER_VALVE 31   //RELAY - 12v - OK
#define MIXER_PIN A0        //SSR
#define MIXER_VALVE_PIN 27  //RELAY - AC - BALL VALVE - OK

//STEAMING
#define STEAMER_TEMP_PIN 6       //DS18B20
#define STEAMER_HEATER_1_PIN A3   //SSR
#define STEAMER_HEATER_2_PIN A4  //SSR
#define STEAMER_LID_1_A_PIN 41    //RELAY - OK
#define STEAMER_LID_1_B_PIN 40    //RELAY - OK
#define STEAMER_LID_2_A_PIN 39   //RELAY - OK
#define STEAMER_LID_2_B_PIN 38    //RELAY - OK

//CUTTING
#define CONTAINER_CUTTING_IR_PIN 14         //DIGITAL SENSOR
#define CONTAINER_CUTTING_STOPPER_A_PIN 37  //RELAY - OK
#define CONTAINER_CUTTING_STOPPER_B_PIN 36  //RELAY - OK
#define COOLING_FAN_PIN 30                   //RELAY - AC
#define CUTTING_A_PIN 35                     //RELAY - 12v - OK
#define CUTTING_B_PIN 34                     //RELAY - 12v - OK

#define IR_READING_DELAY 1500
#define DESPENSER_CLOSE_DELAY 1500
#define coldWaterCount 1000
#define solidLinearDelay 2000
#define SolidWeight1 1000
#define SolidWeight2 1000
#define SolidWeight3 1000
#define hotWaterDelay 3000
#define lyeDelay 3000
#define colorDelay 3000
#define mixDelay 3000
#define mixValveDelay 3000
#define SteamDelay 1800
#define coolDelay 1000
#define cuttingExtendDelay 3000
#define steamLidDelay 3000

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 29, 27, 25, 23 };
byte colPins[COLS] = { 22, 24, 26, 28 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


LiquidCrystal_I2C lcd(0x27, 20, 4);

HX711 scale;

OneWire oneWire1(HOT_WATER_TEMP_PIN);
DallasTemperature TempSensor1(&oneWire1);

OneWire oneWire2(STEAMER_TEMP_PIN);
DallasTemperature TempSensor2(&oneWire2);


Chrono myChrono, dispensingContainerChrono, containerInMixChrono, containerInSteamChrono, steamChrono, coolingChrono, dispenseChrono, mixChrono;

int mode = 0;
int dispensingMode = 0;
int mixingMode = 0;
int steamMode = 0;
int coolMode = 0;
int stopperMixMode = 0;
int stopperSteamMode = 0;
int stopperCutMode = 0;

volatile byte pulseCount;

int steamCounter = 0;


int doneProcessCounter = 0;

int orangeNum = 0;
int voiletNum = 0;

String orangeInput;
String violetInput;

void setup() {
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0, 0), lcd << F("====================");
  lcd.setCursor(0, 1), lcd << F("      KUTSINTA      ");
  lcd.setCursor(0, 2), lcd << F("        MAKER       ");
  lcd.setCursor(0, 3), lcd << F("====================");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);

  pinMode(CONVEYOR_PIN, OUTPUT);

  pinMode(CONTAINER_DISPENSE_IR_PIN, INPUT);


  pinMode(WATER_LEVEL, INPUT);
  pinMode(HOT_WATER_VALVE_PIN, OUTPUT);
  pinMode(HOT_WATER_HEATER_PIN, OUTPUT);
  pinMode(CONTAINER_MIXER_IR_PIN, INPUT);
  pinMode(CONTAINER_MIXER_STOPPER_A_PIN, OUTPUT);
  pinMode(CONTAINER_MIXER_STOPPER_B_PIN, OUTPUT);
  pinMode(COLD_WATER_VALVE_PIN, OUTPUT);
  pinMode(SOLID_MIX_CONVEYOR_1_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_1_A_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_1_B_PIN, OUTPUT);
  pinMode(SOLID_MIX_CONVEYOR_2_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_2_A_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_2_B_PIN, OUTPUT);
  pinMode(SOLID_MIX_CONVEYOR_3_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_3_A_PIN, OUTPUT);
  pinMode(SOLID_MIX_LINEAR_3_B_PIN, OUTPUT);
  pinMode(ORANGE_VALVE, OUTPUT);
  pinMode(VIOLET_VALVE, OUTPUT);
  pinMode(LY_WATER_VALVE, OUTPUT);
  pinMode(MIXER_PIN, OUTPUT);
  pinMode(MIXER_VALVE_PIN, OUTPUT);


  pinMode(CONTAINER_CUTTING_STOPPER_A_PIN, OUTPUT);
  pinMode(CONTAINER_CUTTING_STOPPER_B_PIN, OUTPUT);

  pinMode(STEAMER_HEATER_1_PIN, OUTPUT);
  pinMode(STEAMER_HEATER_2_PIN, OUTPUT);
  pinMode(STEAMER_LID_1_A_PIN, OUTPUT);
  pinMode(STEAMER_LID_1_B_PIN, OUTPUT);
  pinMode(STEAMER_LID_2_A_PIN, OUTPUT);
  pinMode(STEAMER_LID_2_B_PIN, OUTPUT);

  pinMode(CONTAINER_CUTTING_IR_PIN, INPUT);
  pinMode(CONTAINER_CUTTING_STOPPER_A_PIN, OUTPUT);
  pinMode(CONTAINER_CUTTING_STOPPER_B_PIN, OUTPUT);
  pinMode(COOLING_FAN_PIN, OUTPUT);
  pinMode(CUTTING_A_PIN, OUTPUT);
  pinMode(CUTTING_B_PIN, OUTPUT);

  pinMode(COLD_FLOW_SENSOR, INPUT);
  attachInterrupt(0, pulseCounter, FALLING);
}

void loop() {
}

void normalProccess() {
  char customKey;
  if (mode > 35) {
    steamerTo100();
    lcd.setCursor(0, 0), lcd << F("====================");
    lcd.setCursor(0, 1), lcd << F("     ON PROCESS     ");
    lcd.setCursor(0, 2), lcd << F("COUNTER: ") << max(doneProcessCounter - 3, 0) << " ";
    lcd.setCursor(0, 3), lcd << F("====================");
  }

  switch (mode) {
    case 0:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("     PRESS START    ");
      lcd.setCursor(0, 2), lcd << F("   TO BEGIN PROCESS ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (isStartPressed()) {
        mode = 1;
      }
      break;

    case 1:
      lcd.setCursor(0, 0), lcd << F("=======INPUT========");
      lcd.setCursor(0, 1), lcd << F("  NUMBER OF ORANGE  ");
      lcd.setCursor(0, 2), lcd << orangeInput << F("   ");
      lcd.setCursor(0, 3), lcd << F("(A) OK     (D)Delete");
      customKey = customKeypad.getKey();
      if (customKey) {
        if (customKey == 'A') {
          if (orangeInput != "") {
            orangeNum = orangeInput.toInt();
            if (orangeNum <= 6) {
              mode = 2;
            } else {
              myChrono.restart();
              mode = 3;
            }
          }
        } else if (customKey == 'D') {
          orangeInput = "";
          lcd.clear();
        } else if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#') {
          orangeInput = orangeInput + customKey;
        }
      }
      break;

    case 2:
      lcd.setCursor(0, 0), lcd << F("=======INPUT========");
      lcd.setCursor(0, 1), lcd << F("  NUMBER OF VIOLET  ");
      lcd.setCursor(0, 2), lcd << orangeInput << F("   ");
      lcd.setCursor(0, 3), lcd << F("(A) OK     (D)Delete");
      customKey = customKeypad.getKey();
      if (customKey) {
        if (customKey == 'A') {
          if (violetInput != "") {
            voiletNum = violetInput.toInt();
            if (voiletNum <= 6 - orangeNum) {
              mode = 10;
            } else {
              myChrono.restart();
              mode = 3;
            }
          }
        } else if (customKey == 'D') {
          violetInput = "";
          lcd.clear();
        } else if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#') {
          violetInput = violetInput + customKey;
        }
      }
      break;

    case 3:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F(" MAXIMUM OUTPUT IS  ");
      lcd.setCursor(0, 2), lcd << F("         6          ");
      lcd.setCursor(0, 3), lcd << F("====================");
      if (myChrono.hasPassed(2000)) {
        mode = 1;
      }
      break;


    case 10:
      if (fullHotWater()) {
        mode = 20;
      }
      break;
    case 20:
      if (hotWaterTo100()) {
        mode = 30;
      }
      break;

    case 30:
      if (steamerTo100()) {
        dispensingContainerChrono.restart();
        mode = 35;
        dispensingMode = 0;
        mixingMode = 0;
        steamMode = 0;
        coolMode = 0;
      }
      break;

    case 35:
      //check if done
      if (doneProcessCounter >= orangeNum + voiletNum + 3) {
        mode = 70;
      } else {
        mode = 40;
      }
      break;

    case 40:
      bool isDoneDespenseProcess = doDispensing();
      bool isDoneMixingProcess = doneProcessCounter > 0 || doMixing();
      bool isDoneSteamProcess = doneProcessCounter > 1 || doSteam();
      bool isDoneCuttingProccess = doneProcessCounter > 2 || doCoolingAndCutting();
      if (isDoneDespenseProcess && isDoneMixingProcess && isDoneSteamProcess && isDoneCuttingProccess) {
        stopperMixMode = 0;
        stopperSteamMode = 0;
        stopperCutMode = 0;
        mode = 50;
        myChrono.restart();
      }
      break;

    case 50:
      if (doneProcessCounter > 0) {
        steamLid1Open();
      }

      if (myChrono.hasPassed(steamLidDelay)) {
        mode = 60;
      }
      break;

    case 60:  //conveyor
      bool mixDone = isStopperMixDone();
      bool steamDone = isStopperSteamDone();
      bool cutDone = isStopperCutDone();
      if (mixDone && steamDone && cutDone) {
        mode = 35;
        doneProcessCounter++;
      }
      break;

    case 70:
      lcd.setCursor(0, 0), lcd << F("====================");
      lcd.setCursor(0, 1), lcd << F("     PROCESS DONE   ");
      lcd.setCursor(0, 2), lcd << F("PRESS START TO EXIT ");
      lcd.setCursor(0, 3), lcd << F("====================");
      allOFF();
      if (isStartPressed()) {
        mode = 0;
      }

      break;

    default:
      // statements
      break;
  }
}
void allOFF() {
  ConveyorOff();
  hotWaterValveClose();
  hotWaterHeaterOff();
  mixerStopperExtend();
  coldWaterValveClose();
  solidMix1DespenserOff();
  solidMix2DespenserOff();
  solidMix3DespenserOff();
  solidMix1LinearOff();
  solidMix2LinearOff();
  solidMix3LinearOff();
  orageValveClose();
  violetValveClose();
  lyeValveClose();
  mixerOff();
  mixerValveClose();
  steamHeater1Off();
  steamHeater2Off();
  steamLid1Close();
  steamLid2Close();
  cuttingStopperExtend();
  fanOff();
  cuttingOff();
}

bool doCoolingAndCutting() {
  if (coolMode == 0) {
    coolingChrono.restart();
    coolMode = 10;
  } else if (coolMode == 10) {
    fanOn();
    if (coolingChrono.hasPassed(coolDelay)) {
      coolingChrono.restart();
      coolMode = 20;
    }
  } else if (coolMode == 20) {
    cuttingExtend();
    if (coolingChrono.hasPassed(cuttingExtendDelay)) {
      coolMode = 30;
    }
  } else if (coolMode == 30) {
    cuttingRetract();
    if (coolingChrono.hasPassed(cuttingExtendDelay)) {
      coolMode = 40;
    }
  } else if (coolMode == 40) {
    return true;
  }
  return false;
}

bool doSteam() {
  if (steamMode == 0) {
    steamChrono.restart();
    steamCounter = 0;
    steamMode = 10;
  } else if (steamMode == 10) {
    if (steamChrono.hasPassed(1000)) {
      steamCounter++;
      if (steamCounter > SteamDelay) {
        return true;
      } else if (steamCounter > (SteamDelay / 2)) {
      }
    }
  }
  return false;
}
bool doMixing() {
  if (mixingMode == 0) {
    pulseCount = 0;
    mixingMode = 10;
  } else if (mixingMode == 10) {
    if (pulseCount >= coldWaterCount) {
      coldWaterValveClose();
      resetScale();
      mixChrono.restart();
      mixingMode = 20;
    } else {
      coldWaterValveOpen();
    }
  } else if (mixingMode == 20) {
    solidMix1LinearRetract();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 30;
      mixerOn();
    }
  } else if (mixingMode == 30) {
    if (getWeight() >= SolidWeight1) {
      solidMix1DespenserOff();
      mixChrono.restart();
      mixingMode = 40;
    } else {
      solidMix1DespenserOn();
    }
  } else if (mixingMode == 40) {
    solidMix1LinearExtend();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 50;
      resetScale();
      mixChrono.restart();
    }
  } else if (mixingMode == 50) {
    solidMix2LinearRetract();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 60;
    }

  } else if (mixingMode == 60) {
    if (getWeight() >= SolidWeight2) {
      solidMix2DespenserOff();
      mixingMode = 70;
      mixChrono.restart();
    } else {
      solidMix2DespenserOn();
    }
  } else if (mixingMode == 70) {
    solidMix2LinearExtend();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 80;
      resetScale();
      mixChrono.restart();
    }
  } else if (mixingMode == 80) {
    solidMix3LinearRetract();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 90;
    }

  } else if (mixingMode == 90) {
    if (getWeight() >= SolidWeight2) {
      solidMix3DespenserOff();
      mixingMode = 100;
      mixChrono.restart();
    } else {
      solidMix3DespenserOn();
    }
  } else if (mixingMode == 100) {
    solidMix3LinearExtend();
    if (mixChrono.hasPassed(solidLinearDelay)) {
      mixingMode = 110;
      mixChrono.restart();
    }
  } else if (mixingMode == 110) {
    hotWaterValveOpen();
    if (mixChrono.hasPassed(hotWaterDelay)) {
      mixingMode = 120;
      hotWaterValveClose();
      mixChrono.restart();
    }
  } else if (mixingMode == 120) {
    lyeValveOpen();
    if (mixChrono.hasPassed(lyeDelay)) {
      mixingMode = 130;
      mixChrono.restart();
      lyeValveClose();
    }
  } else if (mixingMode == 130) {
    if (isOrange()) {
      orageValveOpen();
    } else {
      violetValveOpen();
    }

    if (mixChrono.hasPassed(colorDelay)) {
      mixingMode = 140;
      orageValveClose();
      violetValveClose();
      mixChrono.restart();
    }
  } else if (mixingMode == 140) {
    if (mixChrono.hasPassed(mixDelay)) {
      mixerOff();
      mixingMode = 150;
      mixChrono.restart();
    }
  } else if (mixingMode == 150) {
    mixerValveOpen();
    if (mixChrono.hasPassed(mixValveDelay)) {
      mixerValveClose();
      mixingMode = 160;
    }
  } else if (mixingMode == 160) {
    mixerValveOpen();
    if (mixChrono.hasPassed(mixValveDelay)) {
      mixerValveClose();
      return true;
    }
  }
  return false;
}

bool doDispensing() {
  if (dispensingMode == 0) {
    dispensingContainerChrono.restart();
    dispensingMode = 10;
  } else if (dispensingMode == 10) {
    if (withContainerDispense()) {
      dispensingMode = 20;
      dispenseChrono.restart();
    } else {
    }
  } else if (dispensingMode == 20) {
    if (dispenseChrono.hasPassed(DESPENSER_CLOSE_DELAY)) {
      dispensingMode = 30;
    }
  } else {
    return true;
  }
  return false;
}


bool isOrange() {
  return false;
}
bool isStopperMixDone() {
  if (stopperMixMode == 0) {
    mixerStopperRetract();
    if (!isMixerHasContainer()) {
      stopperMixMode = 1;
    }
    return false;
  } else if (stopperMixMode == 1) {
    mixerStopperExtend();
    if (isMixerHasContainer()) {
      stopperMixMode = 2;
    }
    return false;

  } else if (stopperMixMode == 2) {
    return true;
  }
}

bool isStopperSteamDone() {
  if (stopperSteamMode == 0) {
    if (!withContainerSteam()) {
      stopperSteamMode = 1;
    }
    return false;
  } else if (stopperSteamMode == 1) {
    if (withContainerSteam()) {
      stopperSteamMode = 2;
    }
    return false;

  } else if (stopperSteamMode == 2) {
    steamLid1Close();
    return true;
  }
}

bool isStopperCutDone() {
  if (stopperCutMode == 0) {
    cuttingStopperRetract();
    if (!withContainerCutting()) {
      stopperCutMode = 1;
    }
    return false;
  } else if (stopperCutMode == 1) {
    cuttingStopperExtend();
    if (withContainerCutting()) {
      stopperCutMode = 2;
    }
    return false;

  } else if (stopperCutMode == 2) {
    return true;
  }
}


bool steamerTo100() {
  steamHeater1On();
  if (getSteamTemp() >= 100) {
    steamHeater2Off();
    return true;
  } else if (getSteamTemp() >= 80) {
    steamHeater2On();
    return true;
  } else {
    steamHeater2On();
    return false;
  }
}
bool hotWaterTo100() {
  if (getHotWaterTemp() >= 100) {
    hotWaterHeaterOff();
    return true;
  } else {
    hotWaterHeaterOn();
    return false;
  }
}
bool fullHotWater() {
  if (isHotWaterFull()) {
    hotWaterValveClose();
    return true;
  } else {
    hotWaterValveOpen();
    return false;
  }
}

bool isStartPressed() {
  return !digitalRead(START_PIN);
}

bool isStopPressed() {
  return !digitalRead(STOP_PIN);
}

long getWeightRaw() {
  return scale.read();
}

float getWeight() {
  return scale.get_units();
}

void resetScale() {
  scale.tare();
}


void ConveyorOn() {
  digitalWrite(CONVEYOR_PIN, HIGH);
}

void ConveyorOff() {
  digitalWrite(CONVEYOR_PIN, LOW);
}



bool withContainerDispense() {
  if (digitalRead(CONTAINER_DISPENSE_IR_PIN)) {
    if (dispensingContainerChrono.hasPassed(1500)) {
      return true;
    } else {
      return false;
    }
  } else {
    dispensingContainerChrono.restart();
    return false;
  }
}

float getHotWaterTemp() {
  return TempSensor1.getTempCByIndex(0);
}

bool isHotWaterFull() {
  return analogRead(WATER_LEVEL) > 100;
}

void hotWaterValveOpen() {
  digitalWrite(HOT_WATER_VALVE_PIN, LOW);
}

void hotWaterValveClose() {
  digitalWrite(HOT_WATER_VALVE_PIN, HIGH);
}


void hotWaterHeaterOn() {
  digitalWrite(HOT_WATER_HEATER_PIN, HIGH);
}

void hotWaterHeaterOff() {
  digitalWrite(HOT_WATER_HEATER_PIN, LOW);
}

bool isMixerHasContainer() {
  if (digitalRead(CONTAINER_MIXER_IR_PIN)) {
    if (containerInMixChrono.hasPassed(500)) {
      return true;
    } else {
      return false;
    }
  } else {
    containerInMixChrono.restart();
    return false;
  }
}

void mixerStopperOff() {
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, HIGH);
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, HIGH);
}

void mixerStopperExtend() {
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, HIGH);
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, LOW);
}

void mixerStopperRetract() {
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, LOW);
  digitalWrite(CONTAINER_MIXER_STOPPER_A_PIN, HIGH);
}

void coldWaterValveOpen() {
  digitalWrite(COLD_WATER_VALVE_PIN, LOW);
}

void coldWaterValveClose() {
  digitalWrite(COLD_WATER_VALVE_PIN, HIGH);
}


void solidMix1DespenserOn() {
  digitalWrite(SOLID_MIX_CONVEYOR_1_PIN, LOW);
}

void solidMix1DespenserOff() {
  digitalWrite(SOLID_MIX_CONVEYOR_1_PIN, HIGH);
}

void solidMix2DespenserOn() {
  digitalWrite(SOLID_MIX_CONVEYOR_2_PIN, LOW);
}

void solidMix2DespenserOff() {
  digitalWrite(SOLID_MIX_CONVEYOR_2_PIN, HIGH);
}

void solidMix3DespenserOn() {
  digitalWrite(SOLID_MIX_CONVEYOR_3_PIN, LOW);
}

void solidMix3DespenserOff() {
  digitalWrite(SOLID_MIX_CONVEYOR_3_PIN, HIGH);
}

void solidMix1LinearOff() {
  digitalWrite(SOLID_MIX_LINEAR_1_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_1_B_PIN, HIGH);
}
void solidMix1LinearExtend() {
  digitalWrite(SOLID_MIX_LINEAR_1_A_PIN, LOW);
  digitalWrite(SOLID_MIX_LINEAR_1_B_PIN, HIGH);
}

void solidMix1LinearRetract() {
  digitalWrite(SOLID_MIX_LINEAR_1_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_1_B_PIN, LOW);
}
void solidMix2LinearOff() {
  digitalWrite(SOLID_MIX_LINEAR_2_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_2_B_PIN, HIGH);
}
void solidMix2LinearExtend() {
  digitalWrite(SOLID_MIX_LINEAR_2_A_PIN, LOW);
  digitalWrite(SOLID_MIX_LINEAR_2_B_PIN, HIGH);
}

void solidMix2LinearRetract() {
  digitalWrite(SOLID_MIX_LINEAR_2_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_2_B_PIN, LOW);
}
void solidMix3LinearOff() {
  digitalWrite(SOLID_MIX_LINEAR_3_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_3_B_PIN, HIGH);
}

void solidMix3LinearExtend() {
  digitalWrite(SOLID_MIX_LINEAR_3_A_PIN, LOW);
  digitalWrite(SOLID_MIX_LINEAR_3_B_PIN, HIGH);
}

void solidMix3LinearRetract() {
  digitalWrite(SOLID_MIX_LINEAR_3_A_PIN, HIGH);
  digitalWrite(SOLID_MIX_LINEAR_3_B_PIN, LOW);
}

void orageValveOpen() {
  digitalWrite(ORANGE_VALVE, LOW);
}

void orageValveClose() {
  digitalWrite(ORANGE_VALVE, HIGH);
}

void violetValveOpen() {
  digitalWrite(VIOLET_VALVE, LOW);
}

void violetValveClose() {
  digitalWrite(VIOLET_VALVE, HIGH);
}

void lyeValveOpen() {
  digitalWrite(LY_WATER_VALVE, LOW);
}

void lyeValveClose() {
  digitalWrite(LY_WATER_VALVE, HIGH);
}

void mixerOn() {
  digitalWrite(MIXER_PIN, HIGH);
}

void mixerOff() {
  digitalWrite(MIXER_PIN, LOW);
}

void mixerValveOpen() {
  digitalWrite(MIXER_VALVE_PIN, LOW);
}

void mixerValveClose() {
  digitalWrite(MIXER_VALVE_PIN, HIGH);
}


float getSteamTemp() {
  return TempSensor2.getTempCByIndex(0);
}


bool withContainerSteam() {
  // if (digitalRead(CONTAINER_STEAM_IR_PIN)) {
  //   if (containerInSteamChrono.hasPassed(500)) {
  //     return true;
  //   } else {
  //     return false;
  //   }
  // } else {
  //   containerInSteamChrono.restart();
  //   return false;
  // }
  return true;
}




void steamHeater1On() {
  digitalWrite(STEAMER_HEATER_1_PIN, HIGH);
}


void steamHeater1Off() {
  digitalWrite(STEAMER_HEATER_1_PIN, LOW);
}

void steamHeater2On() {
  digitalWrite(STEAMER_HEATER_2_PIN, HIGH);
}

void steamHeater2Off() {
  digitalWrite(STEAMER_HEATER_2_PIN, LOW);
}

void steamLid1Open() {
  digitalWrite(STEAMER_LID_1_A_PIN, LOW);
  digitalWrite(STEAMER_LID_1_B_PIN, HIGH);
}

void steamLid1Close() {
  digitalWrite(STEAMER_LID_1_A_PIN, HIGH);
  digitalWrite(STEAMER_LID_1_B_PIN, LOW);
}

void steamLid2Open() {
  digitalWrite(STEAMER_LID_2_A_PIN, LOW);
  digitalWrite(STEAMER_LID_2_B_PIN, HIGH);
}

void steamLid2Close() {
  digitalWrite(STEAMER_LID_2_A_PIN, HIGH);
  digitalWrite(STEAMER_LID_2_B_PIN, LOW);
}


bool withContainerCutting() {
  return digitalRead(CONTAINER_CUTTING_IR_PIN);
}

void cuttingStopperExtend() {
  digitalWrite(CONTAINER_CUTTING_STOPPER_A_PIN, HIGH);
  digitalWrite(CONTAINER_CUTTING_STOPPER_B_PIN, LOW);
}

void cuttingStopperRetract() {
  digitalWrite(CONTAINER_CUTTING_STOPPER_A_PIN, LOW);
  digitalWrite(CONTAINER_CUTTING_STOPPER_B_PIN, HIGH);
}

void fanOn() {
  digitalWrite(COOLING_FAN_PIN, LOW);
}

void fanOff() {
  digitalWrite(COOLING_FAN_PIN, HIGH);
}
void cuttingOff() {
  digitalWrite(CUTTING_A_PIN, HIGH);
  digitalWrite(CUTTING_B_PIN, HIGH);
}
void cuttingExtend() {
  digitalWrite(CUTTING_A_PIN, HIGH);
  digitalWrite(CUTTING_B_PIN, LOW);
}

void cuttingRetract() {
  digitalWrite(CUTTING_A_PIN, LOW);
  digitalWrite(CUTTING_B_PIN, HIGH);
}

void pulseCounter() {
  pulseCount++;
}