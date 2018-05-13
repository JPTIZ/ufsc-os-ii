#include <smart_data.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

template <typename T>
class Logged_Transducer
{
public:
    // === Technicalities ===
    // We won't use these, but every Transducer must declare them
    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

    static void attach(Observer * obs) {}
    static void detach(Observer * obs) {}

public:
    // === Sensor characterization ===
    static const unsigned int UNIT = TSTP::Unit::Force;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    // === SmartData interaction ===
    Logged_Transducer() {}

    static void sense(unsigned int dev, Smart_Data<Logged_Transducer<T>> * data) {
        cout << "sense(dev=" << dev << ")" << endl;
    }

    static void actuate(unsigned int dev, Smart_Data<Logged_Transducer<T>> * data, const Smart_Data<Logged_Transducer<T>>::Value & command) {
        cout << "actuate(dev=" << dev << ", command=" << command << ")" << endl;
        data->_value = command;
    }
};

// A SmartData that personifies the transducer defined above
typedef Smart_Data<Logged_Transducer<Vibration>> My_Temperature;

int main()
{
    Alarm::delay(5000000);
    cout << "Hello!" << endl;
    My_Temperature t(0, 15000000, My_Temperature::ADVERTISED);
    t = 10;

    while(true) {
        Alarm::delay(5000000);
        //cout << "Temperature = " << t << " at " << t.location() << ", " << t.time() << endl;
        //cout << "Vibration = " << v << " at " << v.location() << ", " << v.time() << endl;
    }

    return 0;
}
