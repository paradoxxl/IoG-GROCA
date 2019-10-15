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
      value = s.equalsIgnoreCase("ON")?ON:OFF;
  }
  constexpr lightState(Value aState) : value(aState) { }

  bool operator==(lightState a) { return value == a.value; }
  bool operator!=(lightState a) { return value != a.value; }
  bool operator<<(lightState a) { return value == ON? "ON":"OFF"; } 
  operator bool() const {return value==ON;}


boolean State(){
    return value;
}
 // constexpr bool IsYellow() const { return value == Banana; }

private:
  Value value;
};

#endif