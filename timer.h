/**
 * Timer class to time the amount of time it takes for a process
 *
 * @sourceFile      timer.h
 *
 * @program      
 *
 * @classes         Timer
 *
 * @functions       n/a
 *
 * @date            2014-09-29
 *
 * @revisions
 *
 * @designer        Jonathan Chu
 *
 * @programmer      Jonathan Chu
 *
 * @notes
 *
 *                  declares a Timer class.
 */

#include <ctime>

/*
    //Testing purposes

#include <iostream>
#include <windows.h>
*/

using namespace std;

/**
 * Provides a time counting class to keep track of the amount of time it takes
 *
 * @class           Timer
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
class Timer 
{
    public:
        Timer();
        void fnClockStart();
        void fnClockStop();
        int fnTimeElapsed();

    private:
        clock_t tStart;
        clock_t tStop;
        bool bStarted;
};

/** number of clock ticks per millisecond */
const float TICKS_PER_MILLISECOND = CLOCKS_PER_SEC * 1000;
