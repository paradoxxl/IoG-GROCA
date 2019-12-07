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
    T *state;
    stateNode<T> *previous;
    stateNode<T> *next;
};

template <class T>
class stateCircularList
{
private:
    stateNode<T> *_currentNode;
    T *_defaultState;
    stateNode<T> *nodeFromEvent(timeEvent<T> event);

    void insertNode(stateNode<T> *newNode);
    stateNode<T> *searchLTNode(stateNode<T> *head, uint16_t time);
    uint16_t timeMerge(uint8_t hour, uint8_t minute);

public:
    stateCircularList();
    stateCircularList(timeEvent<T> *events[], int length, T *defaultState);
    T *getState(uint8_t hour, uint8_t minute);
    void insertEvents(timeEvent<T> *events[], int length);
    void insert(uint8_t hour, uint8_t minute, T *state);
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
        insert(events[i]->hour, events[i]->minute, events[i]->state);
    }
}

template <class T>
stateCircularList<T>::stateCircularList(timeEvent<T> *events[], int length, T *defaultState)
{
    _defaultState = defaultState;
    for (int i = 0; i < length; i++)
    {
        insert(events[i]->hour, events[i]->minute, events[i]->state);
    }
}

template <class T>
T *stateCircularList<T>::getState(uint8_t hour, uint8_t minute)
{
    Serial.println("stateCircularList<T>::getState");
    if (_currentNode == nullptr || _currentNode == NULL)
    {
        Serial.println("stateCircularList<T>::getState rootnode null");
        Serial.printf("stateCircularList<T>::getState defaultstate null?%d \r\n", (_defaultState == nullptr));

        return _defaultState;
    }

    uint16_t time = timeMerge(hour, minute);

    stateNode<T> *tmpNode = _currentNode;

    // traverse next
    while (tmpNode->time < time && tmpNode->next != NULL && tmpNode->next != nullptr && tmpNode->next->time <= time && tmpNode->time < tmpNode->next->time)
    {
        tmpNode = tmpNode->next;
    }

    // traverse previous
    while (tmpNode->time > time && tmpNode->previous != NULL && tmpNode->previous != nullptr && tmpNode->previous->time >= time && tmpNode->time > tmpNode->previous->time)
    {
        tmpNode = tmpNode->previous;
    }

    if (tmpNode != nullptr && tmpNode != NULL)
    {
        Serial.printf("stateCircularList<T>::getState - set currentNode. before: %d-%d  now: %d-%d\r\n", _currentNode->time, bool(_currentNode->state), tmpNode->time, bool(tmpNode->state));
        _currentNode = tmpNode;
        return _currentNode->state;
    }

    return _defaultState;
}

template <class T>
void stateCircularList<T>::insert(uint8_t hour, uint8_t minute, T *state)
{
    Serial.println("stateCircularList<T>::insert");
    stateNode<T> *newNode = {};
    Serial.println("stateCircularList<T>::insert-statenode ptr created");
    newNode->time = timeMerge(hour, minute);
    Serial.println("stateCircularList<T>::insert-timemerged");
    newNode->state = state;
    Serial.println("stateCircularList<T>::insert-state assigned");

    // if (_currentNode == NULL || _currentNode == nullptr)
    // {
    //     _currentNode = newNode;
    //     return;
    // }
    // if (_currentNode->time == newNode->time)
    // {
    //     _currentNode->state = newNode->state;
    //     return;
    // }
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
    newNode->state = &state;
    Serial.println("stateCircularList<T>::insert2x-state assigned");

    // if (_currentNode == NULL || _currentNode == nullptr)
    // {
    //     Serial.println("stateCircularList<T>::insert2x-state assigned");
    //     _currentNode = newNode;
    //     return;
    // }
    // if (_currentNode->time == newNode->time)
    // {
    //     _currentNode->state = newNode->state;
    //     return;
    // }
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

    if (head->next == head)
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

    if (_currentNode == NULL || _currentNode == nullptr)
    {
        _currentNode = newNode;
        _currentNode->previous = _currentNode;
        _currentNode->next = _currentNode;
        return;
    }
    if (newNode == NULL || newNode == nullptr)
    {
        Serial.println("stateCircularList<T>:insertNode - node or newnode == null / nullptr");
        return;
    }

    stateNode<T> *tmpNode = _currentNode;
    uint16_t time = newNode->time;

    // traverse next
    Serial.println("stateCircularList<T>:insertNode - traverse next entry");
    while (tmpNode->time < time && tmpNode->next != NULL && tmpNode->next != nullptr && tmpNode->next->time <= time && tmpNode->time < tmpNode->next->time)
    {
        Serial.println("stateCircularList<T>:insertNode - traverse next");
        tmpNode = tmpNode->next;
    }

    // traverse previous
    Serial.println("stateCircularList<T>:insertNode - traverse previous entry");
    while (tmpNode->time > time && tmpNode->previous != NULL && tmpNode->previous != nullptr && tmpNode->previous->time >= time && tmpNode->time > tmpNode->previous->time)
    {
        Serial.printf("stateCircularList<T>:insertNode - traverse previous- insert %d-%d. previous: %d-%d \r\n", newNode->time, bool(newNode->state), tmpNode->time, bool(tmpNode->state));
        tmpNode = tmpNode->previous;
    }

    if (tmpNode->time == time)
    {
        Serial.println("stateCircularList<T>:insertNode - overwrite existing");
        tmpNode->state = newNode->state;
        return;
    }

    if (tmpNode != nullptr && tmpNode != NULL)
    {
        Serial.println("stateCircularList<T>:insertNode - insert inbetween");
        newNode->next = tmpNode->next;
        newNode->previous = tmpNode;
        Serial.println("stateCircularList<T>:insertNode - insert inbetween - newNode neighbours ok");
        tmpNode->next->previous = newNode;
        Serial.println("stateCircularList<T>:insertNode - insert inbetween - next node neighbour ok");
        tmpNode->next = newNode;
        Serial.println("stateCircularList<T>:insertNode - insert inbetween - previous node neighbour ok");
        return;
    }
}
template <class T>
uint16_t stateCircularList<T>::timeMerge(uint8_t hour, uint8_t minute)
{
    return hour >> 8 | minute;
}

#endif