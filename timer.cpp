/**
 * method definitions of the Timer declarations in timer.h
 *
 * @sourceFile      Timer.cpp
 *
 * @program      
 *
 * @classes         Timer
 *
 * @methods      
 *                  void Timer::clockStart();
 *                  void Timer::clockStop();
 *                  float Time::timeElapsed();
 *
 * @parameter       
 *                  clock_t  start
 *                  clock_t  stop
 *                  bool     started 
 *
 * @date            November 29, 2014
 *
 * @revisions
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes        
 *
 */
#include "timer.h"

/**
 * Instantiates a Timer object
 *
 * @date            November 29, 2014
 *
 * @class           Timer
 *
 * @signature       Timer::Timer() 
 *
 * @param           
 *
 * @return          
 *
 * @revisions    
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 *
 */
Timer::Timer()
{
    bStarted = false;
}

/**
 * Start the timer
 *
 * @date            November 29, 2014
 *
 * @class           Timer
 *
 * @signature       void Timer::clockStart() 
 *
 * @param           
 *
 * @return          
 *
 * @revisions    
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 *
 */
void Timer::fnClockStart() 
{
   tStart = std::clock();
   bStarted = true;
}

/**
 * Stops the timer
 *
 * @date            November 29, 2014
 *
 * @class           Timer
 *
 * @signature       Timer::clockStop() 
 *
 * @param           
 *
 * @return          
 *
 * @revisions    
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 *
 */
void Timer::fnClockStop()
{
   tStop = std::clock();
   bStarted = false;
}

/**
 * Instantiates a comm port object
 *
 * @date            November 29, 2014
 *
 * @class           Timer
 *
 * @signature       Timer::timeElapsed() 
 *
 * @param           
 *
 * @return          returns the time in milliseconds that has passed from the
 *                   start of the timer
 *                   until the stop of the timer.
 *
 * @revisions    
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 *
 */
int Timer::fnTimeElapsed()
{
    if (!bStarted)
        return ( tStop - tStart ) / TICKS_PER_MILLISECOND;
    else
        return ( std::clock() - tStart ) / TICKS_PER_MILLISECOND;
}

/*
      //Testing purposes

#include <windows.h>
#include <iostream>
#include <sstream>
int main( void ) 
{
    Timer hey;

    hey.fnClockStart();
    Sleep(5000);
    hey.fnClockStop();

	std::stringstream sstm;
	sstm << hey.fnTimeElapsed() << "\r\n";
	OutputDebugString(sstm.str().c_str());
}
*/
