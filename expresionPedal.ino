//#include <unwind-cxx.h>
//#include <StandardCplusplus.h>
//#include <system_configuration.h>
//#include <utility.h>

#include "ArduinoSTL.h"
#include "vector"
#include "OutputPinClass.h"

int m_expressionReadPin = A0;
int m_smoothingReadPin = A1;
std::vector<OutputPinClass> m_outputPins(5);
float m_currentInputValue = 0;
float m_scaledSmoothingValue = 0;
float m_directSmoothingValue = 0;
bool m_multiControl = false; //not happy with this name.
int m_multiControlPin = 2; //digital 2
float m_smoothingMsMin = 2.0f;
float m_smoothingMsMax = 100.0f;

float interpolate(float oldVal, float newVal, float smoothingVal)
{
  //TODO: factor in smoothing value
  
  if (oldVal < newVal)
    oldVal = oldVal + 0.01;
  else if (oldVal > newVal)
    oldVal = oldVal - 0.01;

  if (oldVal > 5.0f)
    oldVal = 5.0f;
  else if (oldVal< 0.0f)
    oldVal = 0.0f;

  return oldVal;
}

void setup() {
  //TODO: check state of unused pins on default could just set to zero here.
  //TODO: possible to save last used state before off?
  m_outputPins[0].setPinNumber(3);
  m_outputPins[1].setPinNumber(5);
  m_outputPins[2].setPinNumber(6);
  m_outputPins[3].setPinNumber(9);
  m_outputPins[4].setPinNumber(10);

  m_outputPins[0].setLedPinNumber(4);
  m_outputPins[1].setLedPinNumber(7);
  m_outputPins[2].setLedPinNumber(8);
  m_outputPins[3].setLedPinNumber(11);
  m_outputPins[4].setLedPinNumber(12);

  m_outputPins[0].setSwitchNumber(A2);
  m_outputPins[1].setSwitchNumber(A3);
  m_outputPins[2].setSwitchNumber(A4);
  m_outputPins[3].setSwitchNumber(A5);
  m_outputPins[4].setSwitchNumber(A6);

  m_outputPins[0].setState(true);
  digitalWrite(m_outputPins[0].getLedPinNumber(), m_outputPins[0].getState());
}

void loop() {
  
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
  float temp = analogRead(m_smoothingReadPin);

  //only recalculate if necessary
  if (m_directSmoothingValue != temp)
  {
    m_directSmoothingValue = temp;
    m_scaledSmoothingValue = ((m_directSmoothingValue / 5.0f) * (m_smoothingMsMax - m_smoothingMsMin)) + m_smoothingMsMin;
  }

  //update values
  for(auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter)
  {
    if(!iter->getState())
      continue;

    iter->setCurrentValue(interpolate(iter->getCurrentValue(), m_currentInputValue, m_scaledSmoothingValue));
    analogWrite(iter->getPinNumber(), iter->getCurrentValue()); //TODO: see note at bottom
    
    if(!m_multiControl)
      break; //exit for loop because only one should be changed :)
  }
}

/* OUTPUT VALUE NOTES:
 * output value needs to be scaled to between 0 and 255. then the hardware should have an LPF to smooth this to a consistent intermediary voltage.
 * Board  PWM Pins  PWM Frequency

Uno, Nano, Mini
  

3, 5, 6, 9, 10, 11
  

490 Hz (pins 5 and 6: 980 Hz)
 * 
 * 
 * 
 */
