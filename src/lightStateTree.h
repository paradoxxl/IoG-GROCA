#ifndef lightStateTree_h
#define lightStateTree_h

#include "Arduino.h"
#include "timeEvent.h"

const uint16_t MAXTIME = 2400;
const uint16_t MAXTIMEENTRY =  MAXTIME-1;



struct lightStateNode{
    uint16_t time;
    bool state;
    lightStateNode *parent;
    lightStateNode *left;
    lightStateNode *right;
};

class lightStateTree
{
private:
    lightStateNode* rootNode;
    lightStateNode* nodeFromEvent(timeEvent event);

    void insertNode(lightStateNode* node,lightStateNode* newNode);
    uint16_t timeMerge(uint8_t hour, uint8_t minute);

    lightStateNode* searchLTNode(lightStateNode* node,uint16_t time);
    lightStateNode* getLatestElement(lightStateNode* node);


public:
    lightStateTree();
    lightStateTree(timeEvent *events[], int length);
    bool getState(uint8_t hour, uint8_t minute);
    void insert(uint8_t hour, uint8_t minute, bool state);
    void remove(uint8_t hour, uint8_t minute);
};

#endif