#ifndef OUTPUTPINCLASS_H
#define OUTPUTPINCLASS_H

class OutputPinClass
{
  public:
  bool getState();
  bool setState(bool state);

  bool getSwitchState();
  bool setSwitchState(bool state);

  int getPinNumber();
  void setPinNumber(int newPin);

  float getCurrentValue();
  bool setCurrentValue(float input);

  int getLedPinNumber();
  void setLedPinNumber(int pinNum);

  int getSwitchNumber();
  void setSwitchNumber(int switchNum);
  
private:
  float m_currentValue = 0;
  bool m_state = false;
  int m_pinNumber = 0;
  int m_ledPinNum = 0;
  bool m_switchState = false;
  int m_switchNumber = 0;
};

#endif