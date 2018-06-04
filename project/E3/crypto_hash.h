#ifndef CRYPTO_HASH_H
#define CRYPTO_HASH_H

#include <smart_data.h>

typedef unsigned char BYTE;


__BEGIN_SYS

class Encrypted_Hash_Transducer
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
    static const unsigned int UNIT = 0 << 31 | 5 << 16 | 8 << 0;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

public:
    // === SmartData interaction ===
    Encrypted_Hash_Transducer() {}

    static void sense(unsigned int dev, Smart_Data<Encrypted_Hash_Transducer> * data) {
    }

    static void actuate(unsigned int dev, Smart_Data<Encrypted_Hash_Transducer> * data,
                        const Smart_Data<Encrypted_Hash_Transducer>::Value & command) {
        // cout << "actuate(dev=" << dev << ", command=" << command << ")" << endl;
        data->_value = command;
    }
};

// A SmartData that personifies the transducer defined above
typedef Smart_Data<Encrypted_Hash_Transducer> Encrypted_Hash;

__END_SYS

#endif