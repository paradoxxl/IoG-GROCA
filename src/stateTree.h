#ifndef stateTree_h
#define stateTree_h

#include "Arduino.h"
#include "timeEvent.h"

const uint16_t MAXTIME = 2400;
const uint16_t MAXTIMEENTRY = MAXTIME - 1;

template <class T>
struct stateNode
{
    uint16_t time;
    T *state;
    stateNode<T> *parent;
    stateNode<T> *left;
    stateNode<T> *right;
};

template <class T>
class stateTree
{
private:
    stateNode<T> *rootNode;
    T *_defaultState;
    stateNode<T> *nodeFromEvent(timeEvent<T> event);

    void insertNode(stateNode<T> *node, stateNode<T> *newNode);
    uint16_t timeMerge(uint8_t hour, uint8_t minute);

    stateNode<T> *searchLTNode(stateNode<T> *node, uint16_t time);
    stateNode<T> *getLatestElement(stateNode<T> *node);

public:
    stateTree();
    stateTree(timeEvent<T> *events[], int length, T *defaultState);
    T *getState(uint8_t hour, uint8_t minute);
    void insertEvents(timeEvent<T> *events[], int length);
    void insert(uint8_t hour, uint8_t minute, T *state);
    void insert(uint16_t time, T state);
    void remove(uint8_t hour, uint8_t minute);
    void remove(uint16_t time);

    void clear();
};

template <class T>
void stateTree<T>::insertEvents(timeEvent<T> *events[], int length)
{
    for (int i = 0; i < length; i++)
    {
        insert(events[i]->hour, events[i]->minute, events[i]->state);
    }
}

template <class T>
stateTree<T>::stateTree(timeEvent<T> *events[], int length, T *defaultState)
{
    _defaultState = defaultState;
    for (int i = 0; i < length; i++)
    {
        insert(events[i]->hour, events[i]->minute, events[i]->state);
    }
}

template <class T>
T *stateTree<T>::getState(uint8_t hour, uint8_t minute)
{
    Serial.println("stateTree<T>::getState");
    if (rootNode == nullptr)
    {
        Serial.println("stateTree<T>::getState rootnode null");
        Serial.printf("stateTree<T>::getState defaultstate null?%d \r\n", (_defaultState == nullptr));

        return _defaultState;
    }
    Serial.println("stateTree<T>::getState searchLTNode");

    stateNode<T> *n = searchLTNode(rootNode, timeMerge(hour, minute));
    Serial.println("stateTree<T>::getState searchLTNode ok");

    if (n != NULL)
    {
        return n->state;
    }
    return getLatestElement(rootNode)->state;
}

template <class T>
void stateTree<T>::insert(uint8_t hour, uint8_t minute, T *state)
{
    Serial.println("stateTree<T>::insert");
    stateNode<T> *newNode = {};
    newNode->time = timeMerge(hour, minute);
    newNode->state = state;

    if (rootNode == NULL || rootNode == nullptr)
    {
        rootNode = newNode;
        return;
    }
    if (rootNode->time == newNode->time)
    {
        rootNode->time = newNode->time;
        return;
    }
    insertNode(rootNode, newNode);
}

template <class T>
void stateTree<T>::insert(uint16_t time, T state)
{
    Serial.println("stateTree<T>::insert");
    stateNode<T> *newNode = {};
    newNode->time = time;
    newNode->state = &state;

    if (rootNode == NULL || rootNode == nullptr)
    {
        rootNode = newNode;
        return;
    }
    if (rootNode->time == newNode->time)
    {
        rootNode->time = newNode->time;
        return;
    }
    insertNode(rootNode, newNode);
}

//TODO: implement me
template <class T>
void stateTree<T>::remove(uint8_t hour, uint8_t minute)
{

}

//TODO: implement me
template <class T>
void stateTree<T>::remove(uint16_t time)
{

}

//TODO: Will the garbage collector kick in? Any pointers on nodes left?
template <class T>
void stateTree<T>::clear()
{
    rootNode = nullptr;
}

template <class T>
void stateTree<T>::insertNode(stateNode<T> *node, stateNode<T> *newNode)
{
    if (newNode->time < node->time)
    {
        if (node->left == NULL)
        {
            node->left = newNode;
        }
        else
        {
            insertNode(node->left, newNode);
        }
    }
    else
    {
        if (node->right == NULL)
        {
            node->right = newNode;
        }
        else
        {
            insertNode(node->right, newNode);
        }
    }
}
template <class T>
uint16_t stateTree<T>::timeMerge(uint8_t hour, uint8_t minute)
{
    return hour >> 8 | minute;
}

template <class T>
stateNode<T> *stateTree<T>::searchLTNode(stateNode<T> *node, uint16_t time)
{
    Serial.printf("stateNode<T>::searchLTNode nullpur?%d NULL?%d\r\n", (node == nullptr), (node == NULL));
    if (node == NULL || node == nullptr){
        return node;
    }

    if (node->time == time)
    {
        Serial.println("stateNode<T>::searchLTNode node->time == time ");
        return node;
    }
    if (time < node->time)
    {
        Serial.println("stateNode<T>::searchLTNode time < node->time");

        if (node->left == nullptr)
        {
            return node;
        }
        stateNode<T> *n = searchLTNode(node->left, time);
        return (abs(n->time - time) > abs(node->time - time)) ? node : n;
    }
    return nullptr;
}
template <class T>
stateNode<T> *stateTree<T>::getLatestElement(stateNode<T> *node)
{
    if (node->right == NULL)
    {
        return node;
    }

    return getLatestElement(node->right);
}

#endif