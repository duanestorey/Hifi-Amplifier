#include "timer.h"
#include "debug.h"

#include <list>

Timer::Timer() : mCurrentEventID( 1 ) {
}

uint32_t
Timer::setTimer( uint32_t timeout, Queue &queue, bool recurring ) {
    TimerInfo timerInfo;
    uint32_t futureTime = getCurrentTimeInMS() + timeout;
    
    ScopedLock lock( mMutex );

    timerInfo.mEventID = mCurrentEventID;
    timerInfo.mQueue = &queue;
    timerInfo.mEventTime = futureTime;
    timerInfo.mTimeout = timeout;
    timerInfo.mRecurring = recurring;
  
    mTimers[ timerInfo.mEventID ] = timerInfo;

    mCurrentEventID++;

    AMP_DEBUG_I( "Setting new time with timeout %lu and ID %lu, futureTime %lu, currentTime %lu", timeout, timerInfo.mEventID , futureTime, getCurrentTimeInMS() );

    return timerInfo.mEventID;
}

 void 
 Timer::cancelTimer( uint32_t timerID ) {
    AMP_DEBUG_I( "Timer Cancelled" );
    ScopedLock lock( mMutex );

    TimerMap::iterator i = mTimers.find( timerID );
    if ( i != mTimers.end() ) {
        mTimers.erase( i );
    }
 }

uint32_t 
Timer::getCurrentTimeInMS() const {
    return ((uint32_t)( xTaskGetTickCount() * portTICK_PERIOD_MS ));
}

void
Timer::processTick() {
    std::list<TimerInfo> toDispatch;

    // Hold the lock while we assemble a list of all the dispatches
    {   
        ScopedLock lock( mMutex );

      //  AMP_DEBUG_SI( "Processing timer tick " << mTimers.size() << " " << toDispatch.size() );

        uint32_t currentTime = getCurrentTimeInMS();
        for ( TimerMap::iterator i = mTimers.begin(); i != mTimers.end(); ) {
            TimerInfo &timerInfo = i->second;

            if ( currentTime >= timerInfo.mEventTime ) {
                toDispatch.push_back( timerInfo );

                if ( !timerInfo.mRecurring ) {
                    TimerMap::iterator toErase = i;
                    i++;
                    mTimers.erase( toErase );
                } else {
                    timerInfo.mEventTime += timerInfo.mTimeout;
                    ++i;
                }
            } else {
                ++i;
            }
        }
    }

    // Lock is released, so can call setTimer from other task
    for ( std::list<TimerInfo>::iterator i = toDispatch.begin(); i != toDispatch.end(); i++ ) {
        TimerInfo &timerInfo = *i;

     //   AMP_DEBUG_SI( "Dispatching timer event " << timerInfo.mEventID );

        timerInfo.mQueue->add( Message::MSG_TIMER, timerInfo.mEventID );
    }
}