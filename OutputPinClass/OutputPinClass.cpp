//
// Created by Berwick, Nathan on 29/08/2021.
//

#include "OutputPinClass.h"

bool OutputPinClass::getState()
{
  return m_state;
}

bool OutputPinClass::setState(bool state)
{
  m_state = state;
}

bool OutputPinClass::getSwitchState()
{
  return m_switchState;
}

bool OutputPinClass::setSwitchState(bool state)
{
  m_switchState = state;
}

int OutputPinClass::getPinNumber()
{
  return m_pinNumber;
}

void OutputPinClass::setPinNumber(int newPin)
{
  m_pinNumber = newPin;
}

float OutputPinClass::getCurrentValue()
{
  return m_currentValue;
}
bool OutputPinClass::setCurrentValue(float input)
{
  if (input < 0.0f || input > 5.0f)
    return false;
  m_currentValue = input;
  return true;
}

int OutputPinClass::getLedPinNumber()
{
  return m_ledPinNum;
}

void OutputPinClass::setLedPinNumber(int pinNum)
{
  m_ledPinNum = pinNum;
}

int OutputPinClass::getSwitchNumber()
{
  return m_switchNumber;
}

void OutputPinClass::setSwitchNumber(int switchNum)
{
  m_switchNumber = switchNum;
}