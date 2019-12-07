#ifndef lightState_h
#define lightState_h

#include "Arduino.h"

class lightState
{
public:
  enum Value : boolean
  {
    OFF = false,
    ON = true,
  };

  lightState() = default;
  lightState(String s){
      value = s.equalsIgnoreCase("ON")||s.equalsIgnoreCase("TRUE")?ON:OFF;
  }
  constexpr lightState(Value aState) : value(aState) { }

  bool operator==(lightState a) { return value == a.value; }
  bool operator!=(lightState a) { return value != a.value; }
  bool operator<<(lightState a) { return value == ON? "ON":"OFF"; } 
  operator bool() const {return value==ON;}


boolean State(){
    return value;
}

private:
  Value value;
};

#endif