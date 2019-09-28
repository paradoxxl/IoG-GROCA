#include "Arduino.h"
#include "lightStateTree.h"

lightStateTree::lightStateTree(){}
lightStateTree::lightStateTree(timeEvent *events[], int length){
    for (int i = 0; i<length;i++){
        insert(events[i]->hour,events[i]->minute,events[i]->state);
    }
}


bool lightStateTree::getState(uint8_t hour, uint8_t minute){
    if (rootNode == NULL){
        return false;
    }
    lightStateNode *n = searchLTNode(rootNode,timeMerge(hour,minute));
    if(n != NULL){
        return n->state;
    }
    return getLatestElement(rootNode);
}

lightStateNode* lightStateTree::getLatestElement(lightStateNode* node){
    if(node->right == NULL){
        return node;
    }

    return getLatestElement(node->right);
}
lightStateNode* lightStateTree::searchLTNode(lightStateNode* node,uint16_t time){
    if(node->time == time){
        return node;
    }
    if(time < node->time){
        if(node->left == NULL){
            return node;
        }
        lightStateNode* n = searchLTNode(node->left,time);
        return(abs(n->time-time) > abs(node->time - time))?node:n;
    }
    return NULL;
}


uint16_t lightStateTree::timeMerge(uint8_t hour, uint8_t minute){
    return hour >> 8 | minute;
}
void lightStateTree::insert(uint8_t hour, uint8_t minute, bool state){
    lightStateNode* newNode = {};
    newNode->time = timeMerge(hour,minute);
    newNode->state = state;

    if(rootNode == NULL){
        rootNode = newNode;
        return;
    }
    if (rootNode->time == newNode->time){
        rootNode->time = newNode->time;
        return;
    }
    insertNode(rootNode,newNode);
}
void lightStateTree::remove(uint8_t hour, uint8_t minute){

}



lightStateNode* lightStateTree::nodeFromEvent(timeEvent event){
    return NULL;
}


void lightStateTree::insertNode(lightStateNode* node,lightStateNode* newNode){
    if(newNode->time < node->time){
        if(node->left == NULL){
            node->left = newNode;
        }else{
            insertNode(node->left,newNode);
        }
    }else{
        if(node->right == NULL){
            node->right = newNode;
        }else{
            insertNode(node->right,newNode);
        }
    }
}
