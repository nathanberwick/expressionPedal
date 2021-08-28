//#include <unwind-cxx.h>
//#include <StandardCplusplus.h>
//#include <system_configuration.h>
//#include <utility.h>

#include "ArduinoSTL.h"
#include "vector"

class outputPin
{
  public:
  outputPin(){};
  outputPin(int pinNumber)
  {
    if (!setPinNumber(pinNumber))
      return; //MSG::could not set outputPin. defaulted at zero
  }
  
  bool getState() {return m_state;};
  bool setState(bool state) {m_state = state;};

  bool getSwitchState() {return m_switchState;};
  bool setSwitchState(bool state) {m_switchState = state;};

  int getPinNumber() {return m_pinNumber;};
  bool setPinNumber(int newPin) {
    if ( newPin < m_minPinNum || newPin > m_maxPinNum )
      return false;
    m_pinNumber = newPin;
    return true;
  }

  float getCurrentValue() {return m_currentValue;};
  bool setCurrentValue(float input)
  {
    if (input < 0.0f || input > 5.0f)
      return false;
    m_currentValue = input;
    return true;
  }
  
private:
  float m_currentValue = 0;
  bool m_state = false;
  int m_maxPinNum = 10;
  int m_minPinNum = 0;
  int m_pinNumber = 0;
  bool m_switchState = false;
};

int m_expressionReadPin = 0;
int m_smoothingReadPin = 1;
std::vector<outputPin> m_outputPins(8);

float m_currentInputValue = 0;
float m_scaledSmoothingValue = 0;
float m_directSmoothingValue = 0;
bool m_multiControl = true; //not happy with this name.

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
  m_outputPins[0].setPinNumber(0);
  m_outputPins[1].setPinNumber(1);
  m_outputPins[2].setPinNumber(2);
  m_outputPins[3].setPinNumber(3);
  m_outputPins[4].setPinNumber(4);
  m_outputPins[5].setPinNumber(5);
  m_outputPins[6].setPinNumber(6);
  m_outputPins[7].setPinNumber(7);

  m_outputPins[0].setState(true);
}

void loop() {
  //update buttons pressed
  for (auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter)
  {
    bool readIn = digitalRead(iter->getPinNumber());

    if (readIn == iter->getSwitchState())
      continue;
    else if( readIn && !iter->getSwitchState())
    {
      if(!m_multiControl)
      {
        bool temp = iter->getState();
        //std::for_each(m_outputPins.begin(), m_outputPins.end(), this->setState(false)); //TODO: implement rather than a second loop....
        for (auto iter2 = m_outputPins.begin(); iter2 != m_outputPins.end(); ++iter2)
        {
          iter2->setState(false);
        }
        iter->setState(!temp);
      }
      iter->setSwitchState(true);
      iter->setState(!iter->getState()); //invert the current setting
      
      //TODO:
      //update LED output
    }
    else if (!readIn && iter->getSwitchState())
    {
      iter->setSwitchState(false);
    }
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
