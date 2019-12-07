#ifndef stateCircularList_h
#define stateCircularList_h

#include "Arduino.h"
#include "timeEvent.h"

const uint16_t MAXTIME = 2400;
const uint16_t MAXTIMEENTRY = MAXTIME - 1;

template <class T>
struct stateNode
{
    uint16_t time;
    T state;
    stateNode<T> *previous;
    stateNode<T> *next;
};

template <class T>
class stateCircularList
{
private:
    stateNode<T> *_currentNode;
    T _defaultState;
    stateNode<T> *nodeFromEvent(timeEvent<T> event);

    void insertNode(stateNode<T> *newNode);
    stateNode<T> *searchLTNode(stateNode<T> *head, uint16_t time);
    uint16_t timeMerge(uint8_t hour, uint8_t minute);

public:
    stateCircularList();
    stateCircularList(timeEvent<T> *events[], int length, T defaultState);
    T getState(uint8_t hour, uint8_t minute);
    void insertEvents(timeEvent<T> *events[], int length);
    void insert(uint8_t hour, uint8_t minute, T state);
    void insert(uint16_t time, T state);
    void remove(uint8_t hour, uint8_t minute);
    void remove(uint16_t time);

    void clear();
};

template <class T>
void stateCircularList<T>::insertEvents(timeEvent<T> *events[], int length)
{
    for (int i = 0; i < length; i++)
    {
        insert(events[i]->hour, events[i]->minute, *(events[i]->state));
    }
}

template <class T>
stateCircularList<T>::stateCircularList(timeEvent<T> *events[], int length, T defaultState)
{
    _defaultState = defaultState;
    for (int i = 0; i < length; i++)
    {
        insert(events[i]->hour, events[i]->minute, *(events[i]->state));
    }
}

template <class T>
T stateCircularList<T>::getState(uint8_t hour, uint8_t minute)
{
    Serial.println("stateCircularList<T>::getState");
    if (_currentNode == nullptr || _currentNode == NULL)
    {
        Serial.println("stateCircularList<T>::getState rootnode null");

        return _defaultState;
    }

    uint16_t time = timeMerge(hour, minute);
    stateNode<T> *tmpNode = searchLTNode(_currentNode, time);

    if (tmpNode == nullptr)
    {
        return _defaultState;
    }

    Serial.printf("stateCircularList<T>::getState ltNode: %d \r\n", tmpNode->time);

    return tmpNode->state;
}

template <class T>
void stateCircularList<T>::insert(uint8_t hour, uint8_t minute, T state)
{
    Serial.println("stateCircularList<T>::insert");
    stateNode<T> *newNode = {};
    Serial.println("stateCircularList<T>::insert-statenode ptr created");
    newNode->time = timeMerge(hour, minute);
    Serial.println("stateCircularList<T>::insert-timemerged");
    newNode->state = state;
    Serial.println("stateCircularList<T>::insert-state assigned");

    insertNode(newNode);
}

template <class T>
void stateCircularList<T>::insert(uint16_t time, T state)
{
    Serial.println("stateCircularList<T>::insert2x");
    stateNode<T> *newNode = new stateNode<T>();
    Serial.println("stateCircularList<T>::insert2x-statenode ptr created");
    newNode->time = time;
    Serial.println("stateCircularList<T>::insert2x-timemerged");
    newNode->state = state;
    Serial.println("stateCircularList<T>::insert2x-state assigned");

    insertNode(newNode);
}

//TODO: implement me
template <class T>
void stateCircularList<T>::remove(uint8_t hour, uint8_t minute)
{
}

//TODO: implement me
template <class T>
void stateCircularList<T>::remove(uint16_t time)
{
}

//TODO: Will the garbage collector kick in? Any pointers on nodes previous?
template <class T>
void stateCircularList<T>::clear()
{
    _currentNode = nullptr;
}

template <class T>
stateNode<T> *stateCircularList<T>::searchLTNode(stateNode<T> *head, uint16_t time)
{

    if (head == nullptr || head->next == head)
    {
        return head;
    }

    stateNode<T> *tmp = head;

    do
    {
        if (time == tmp->time)
        {
            return tmp;
        }

        // inbetween next (asc case)
        if (time > tmp->time && time < tmp->next->time)
        {
            return tmp;
        }

        // inbetween next (daybreak case)
        if (tmp->next->time < tmp->time &&
            (time > tmp->time || time < tmp->next->time))
        {
            return tmp;
        }

        // inbetween prev (dsc case)
        if (time < tmp->time && time >= tmp->previous->time)
        {
            return tmp->previous;
        }

        // inbetween prev (daybreak case)
        if (tmp->previous->time > tmp->time &&
            (time >= tmp->previous->time || time < tmp->time))
        {
            return tmp->previous;
        }

        if (time > tmp->time)
        {
            tmp = tmp->next;
        }
        else
        {
            tmp = tmp->previous;
        }

    } while (tmp != head);

    return tmp;
}

template <class T>
void stateCircularList<T>::insertNode(stateNode<T> *newNode)
{
    Serial.println("stateCircularList<T>::insertNode - entry");

    stateNode<T> *ltNode = searchLTNode(_currentNode, newNode->time);
    if (ltNode == NULL || ltNode == nullptr)
    {
        Serial.println("stateCircularList<T>::insertNode - first node in the list");
        _currentNode = newNode;
        _currentNode->previous = _currentNode;
        _currentNode->next = _currentNode;
        return;
    }

    newNode->previous = ltNode;
    newNode->next = ltNode->next;

    ltNode->next->previous = newNode;
    ltNode->next = newNode;
    Serial.printf("stateCircularList<T>::insertNode - inserted %d-%d between %d and %d\r\n", newNode->time, bool(newNode->state), newNode->previous->time, newNode->next->time);

    Serial.print("stateCircularList<T>::insertNode - list:");
    stateNode<T> *printNode = _currentNode;
    do
    {
        Serial.printf(" %d", printNode->time);
        printNode = printNode->next;
    } while (printNode != _currentNode);
    Serial.println();
}
template <class T>
uint16_t stateCircularList<T>::timeMerge(uint8_t hour, uint8_t minute)
{
    return hour >> 8 | minute;
}

#endif