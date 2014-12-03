#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include <cstdlib>

class Generator
{
public:
    /** sets the seed of the random generator. */
    static void set_seed( unsigned int seed );

    /** gets the next number of the random generator */
    static int next( void );

    /** returns true if the roll was successful; false otherwise. */
    static bool roll( int numSides );

    /** returns an integer value within the passed bounds ( inclusive ). */
    static int range( int min, int max );

private:
    /** hide the constructor, so nobody can instantiate this. */
    Generator( void );

    /** last number generated. */
    static int lastNum;
};

#endif
