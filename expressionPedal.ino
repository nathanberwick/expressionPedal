#include "ArduinoSTL.h"
#include "vector"
#include "OutputPinClass.h"

int m_expressionReadPin = A0;
int m_currentInputValue = 0; //10 bit

int m_smoothingReadPin = A1;
float m_scaledSmoothingValue = 0;
float m_directSmoothingValue = 0; //10 bit
float m_smoothingMsMin = 2.0f;
float m_smoothingMsMax = 1000.0f;

bool m_multiControl = false; //not happy with this name.
int m_multiControlPin = 2; //digital 2

std::vector<OutputPinClass> m_outputPins(4);

bool ledBlink = true;
int m_rate = 10; //in ms. how many times the main loop is called per ms. //TODO:: calculate this with oscilloscope

float interpolate(float oldVal, float newVal, float smoothingVal)
{
  if (oldVal != newVal)
  {
    float increment = (newVal-oldVal) / (smoothingVal * m_rate);
    oldVal = oldVal + increment;
  }

  if (oldVal > 5.0f)
    oldVal = 5.0f;
  else if (oldVal< 0.0f)
    oldVal = 0.0f;

  return oldVal;
}

void setup() {
  m_outputPins[0].setPinNumber(3);
  m_outputPins[1].setPinNumber(5);
  m_outputPins[2].setPinNumber(6);
  m_outputPins[3].setPinNumber(9);

  m_outputPins[0].setLedPinNumber(4);
  m_outputPins[1].setLedPinNumber(7);
  m_outputPins[2].setLedPinNumber(8);
  m_outputPins[3].setLedPinNumber(11);

  m_outputPins[0].setSwitchNumber(A2);
  m_outputPins[1].setSwitchNumber(A3);
  m_outputPins[2].setSwitchNumber(A4);
  m_outputPins[3].setSwitchNumber(A5);

  m_outputPins[0].setState(true);
  digitalWrite(m_outputPins[0].getLedPinNumber(), m_outputPins[0].getState());
}

void loop() {
  ledBlink = !ledBlink;
  digitalWrite(13, ledBlink);
  
  //update switch mode
  m_multiControl = digitalRead(m_multiControlPin);
  
  //update buttons pressed
  for (auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter)
  {
    bool readIn = digitalRead(iter->getSwitchNumber());

    if (readIn == iter->getSwitchState())
      continue;
    else if(readIn && !iter->getSwitchState())
    {
      iter->setSwitchState(true);
      bool temp = iter->getState();
      if(!m_multiControl)
      {
        //std::for_each(m_outputPins.begin(), m_outputPins.end(), this->setState(false)); //TODO: implement rather than a second loop....
        for (auto iter2 = m_outputPins.begin(); iter2 != m_outputPins.end(); ++iter2)
        {
          iter2->setState(false);
        }
      }
      iter->setState(!temp);
    }
    else if (!readIn && iter->getSwitchState())
    {
      iter->setSwitchState(false);
    }
  }

  //update LEDs
  for (auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter)
  {
    digitalWrite(iter->getLedPinNumber(), iter->getState());
  }
  
  //read in analogue value
  m_currentInputValue = analogRead(m_expressionReadPin);
  int temp = analogRead(m_smoothingReadPin);

  //only recalculate if necessary
  if (m_directSmoothingValue != temp)
  {
    m_directSmoothingValue = temp;
    m_scaledSmoothingValue = ((m_directSmoothingValue / 1023.0f) * (m_smoothingMsMax - m_smoothingMsMin)) + m_smoothingMsMin;
  }

  //update values
  for(auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter)
  {
    if(!iter->getState())
      continue;

    iter->setCurrentValue(interpolate(iter->getCurrentValue(), m_currentInputValue, m_scaledSmoothingValue));
    analogWrite(iter->getPinNumber(), (unsigned int)iter->getCurrentValue()>>2);
    
    if(!m_multiControl)
      break; //exit for loop because only one should be changed :)
  }
}
