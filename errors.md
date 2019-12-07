# hello #

## cabinet/groca1/command/light/plan ## nullpointer exception
**body**
```json
[
  {
    "Time": 1111,
    "State": true
  },
  {
    "Time": 1,
    "State": false
  },
  {
    "Time": 2222,
    "State": false
  },
    {
    "Time": 6,
    "State": true
  }
]
```

**console ouput**
```
callback ok
lightTimer cmdLightPlan
lightTimer cmdLightPlan - len plan: 4
lightTimer cmdLightPlan - time: 1111 state:0
lightTimer cmdLightPlan - time: 1 state:0
lightTimer cmdLightPlan - time: 2222 state:0
lightTimer cmdLightPlan - time: 6 state:0
stateCircularList<T>::insert2x
stateCircularList<T>::insert2x-statenode ptr created
stateCircularList<T>::insert2x-timemerged
stateCircularList<T>::insert2x-state assigned
stateCircularList<T>::insert2x
stateCircularList<T>::insert2x-statenode ptr created
stateCircularList<T>::insert2x-timemerged
stateCircularList<T>::insert2x-state assigned
stateCircularList<T>::insertNode - entry
stateCircularList<T>::newNode->time < node->time
stateCircularList<T>::insert2x
stateCircularList<T>::insert2x-statenode ptr created
stateCircularList<T>::insert2x-timemerged
stateCircularList<T>::insert2x-state assigned
stateCircularList<T>::insertNode - entry
stateCircularList<T>::newNode->time => node->time
stateCircularList<T>::insert2x
stateCircularList<T>::insert2x-statenode ptr created
stateCircularList<T>::insert2x-timemerged
stateCircularList<T>::insert2x-state assigned
stateCircularList<T>::insertNode - entry
stateCircularList<T>::newNode->time < node->time
stateCircularList<T>::insertNode - entry
stateCircularList<T>::newNode->time => node->time
callback executed
CORRUPT HEAP: Bad head at 0x3ffcc97e. Expected 0xabba1234 got 0x796c7065
assertion "head != NULL" failed: file "/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/multi_heap_poisoning.c", line 214, function: multi_heap_free
abort() was called at PC 0x4012fccb on core 1

```


## startup sometimes ##
**error**
```
{"timestamp":1574457642,"ledState":true,"ledIntensity":153}
Sent status successfully
lightTimer loop start
lightTimer evaluate state
lightTimer getState
stateCircularList<T>::getState
stateCircularList<T>::getState searchLTNode
stateNode<T>::searchLTNode nullpur?0 NULL?0
stateNode<T>::searchLTNode time < node->time
stateNode<T>::searchLTNode nullpur?0 NULL?0
Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
Core 1 register dump:
PC      : 0x400d5517  PS      : 0x00060c30  A0      : 0x800d554b  A1      : 0x3ffb1f10
A2      : 0x3ffd78c0  A3      : 0x00000004  A4      : 0x00000014  A5      : 0x3ffc16a0
A6      : 0x00000000  A7      : 0x3ffb1ecc  A8      : 0x800d550f  A9      : 0x3ffb1e70
A10     : 0x00000004  A11     : 0x78aaa7b3  A12     : 0x78aaa7b3  A13     : 0x3ffc1a90
A14     : 0x3ffb1eac  A15     : 0x3ffb1eac  SAR     : 0x0000000a  EXCCAUSE: 0x0000001c
EXCVADDR: 0x00000004  LBEG    : 0x400014fd  LEND    : 0x4000150d  LCOUNT  : 0xffffffff

Backtrace: 0x400d5517:0x3ffb1f10 0x400d5548:0x3ffb1f30 0x400d5600:0x3ffb1f50 
```


## arduinojson lightplan - deserialize json bool ##
the following input is deserialized as time:correct, state:always false
```
[
  {
    "Time": 1111,
    "State": true
  },
  {
    "Time": 1,
    "State": false
  },
  {
    "Time": 2222,
    "State": false
  },
    {
    "Time": 6,
    "State": true
  }
]
```