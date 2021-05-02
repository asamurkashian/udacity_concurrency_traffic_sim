#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(_mtx);
    _cond.wait(uLock, [this]{return !_que.empty();});
    T message = std::move(_que.back());
    _que.clear();
    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex>  uLock(_mtx);
    _que.push_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    
    while(true){
        if(_msgque.receive()==TrafficLightPhase::green){return;}
        //else if(_msgque.receive()==TrafficLightPhase::red){std::cout << "Light is red! \n";}
        //else{std::cout << "Received something unexpected! \n";}
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    
    std::chrono::system_clock::time_point stopwatch_start_time  = std::chrono::system_clock::now(); 
    //long int time_duration_milliseconds = ((rand() % 2) + 4)*1000;
    
    long int  time_duration_milliseconds = 4000;
    //std::cout << "cycle time " << time_duration_seconds << "\n";
    while(true){

        std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
        long int  elapsed_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(current_time-stopwatch_start_time).count();    
        if (elapsed_time_milliseconds >= time_duration_milliseconds){
            
            if(_currentPhase == red){
                _currentPhase = green;
            }else{
                _currentPhase = red;
            }
        
            auto message = _currentPhase;
            _msgque.send(std::move(message));
            
            //time_duration_milliseconds = ((rand() % 2) + 4)*1000;
            std::chrono::system_clock::time_point stopwatch_start_time = std::chrono::system_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

