#include "generator.h"

int Generator::lastNum = 0;

void Generator::set_seed( unsigned int seed )
{
    lastNum = seed;
}

int Generator::next( void )
{
    set_seed(lastNum);
    lastNum = rand();

    return rand();
}

bool Generator::roll( int numSides )
{
    return numSides == next() % numSides + 1;
}

int Generator::range( int min, int max )
{
    return next() % ( max - min + 1 ) + min;
}

Generator::Generator( void )
{}

/*
#include <iostream>
int main( void )
{
using std::cout;
using std::endl;

Generator::set_seed( 10 );
cout << Generator::next() << endl;
cout << Generator::roll( 2 ) << endl;
cout << Generator::roll( 2 ) << endl;
cout << Generator::roll( 4 ) << endl;
cout << Generator::roll( 5 ) << endl;
cout << Generator::range( 1, 2 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;
cout << Generator::range( 10, 15 ) << endl;

return 0;
}
*/
