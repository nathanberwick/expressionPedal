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
  void setPinNumber(int newPin) {m_pinNumber = newPin;};

  float getCurrentValue() {return m_currentValue;};
  bool setCurrentValue(float input)
  {
    if (input < 0.0f || input > 5.0f)
      return false;
    m_currentValue = input;
    return true;
  }

  int getLedPinNumber() {return m_ledPinNum;};
  void setLedPinNumber(int pinNum) { m_ledPinNum = pinNum;};
  
private:
  float m_currentValue = 0;
  bool m_state = false;
  int m_pinNumber = 0;
  int m_ledPinNum = 0;
  bool m_switchState = false;
};