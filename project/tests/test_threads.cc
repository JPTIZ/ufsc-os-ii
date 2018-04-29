// EPOS cout Test Program

#include <alarm.h>
#include <thread.h>
#include <utility/ostream.h>


using namespace EPOS;

OStream cout;

int printer()
{
	while(1) {
		cout << "IDLE Thread" << endl;
	    Alarm::delay(10000000/2);
	}
	return 0;
}

int main()
{
	Thread * thread = new Thread(&printer);
	thread->priority(Thread::IDLE);
    while(1) {
        cout << "Hello, World!" << endl;
    	Alarm::delay(10000000);
    }

    return 0;
}
