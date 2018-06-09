#include <smart_data.h>
#include <alarm.h>
#include <utility/array.h>
#include <thread.h>

#include "hash.h"

__BEGIN_SYS

OStream cout;

typedef unsigned char byte_t;

class Log_Transducer
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
    static const unsigned int NUM = TSTP::Unit::I64;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = false;

public:
    // === SmartData interaction ===
    static void sense(
            unsigned int dev,
            Smart_Data<Log_Transducer> * data)
    {
    }

    static void actuate(
            unsigned int dev,
            Smart_Data<Log_Transducer> * data,
            const Smart_Data<Log_Transducer>::Value & command)
    {
        data->_value = command;
    }
};

typedef Smart_Data<Log_Transducer> Smart_Log;

template<typename T>
class Logger: public Smart_Data_Common::Observer
{
public:
    Logger(T * t, Smart_Log* log, const byte_t* key):
        _data(t),
        log(log),
        logs_size(0),
        logs(),
        thread{0},
        sending{false},
        key{key}
    {
        _data->attach(this);
        //print(_data->db_series());
    }
    ~Logger() { _data->detach(this); }

    void update(Smart_Data_Common::Observed * obs) {
        if (sending) {
            return;
        }
        if (logs_size == 0)
            initial_timestamp = _data->time();

        logs[logs_size] = *_data;
        ++logs_size;
        if (logs_size == N_LOGS) {
            final_timestamp = _data->time();
            if (thread)
                delete thread;
            sending = true;
            thread = new Thread(&send_log, this);
            thread->priority(Thread::IDLE);
        }
    }

private:

    static void print_as_hex(const char* text, const unsigned char* what, const int len=crypto::SHA256_BLOCK_SIZE) {
        const char HEX_CHARS[] = {"0123456789abcdef"};
        cout << text << ": ";
        for (int i = 0; i < len; ++i) {
            char c = what[i];
            cout << HEX_CHARS[(c >> 4) & 0b1111] << HEX_CHARS[c & 0b1111];
        }
        cout << '\n';
    }

    static int send_log(Logger* _logger) {
        using crypto::Sha256;
        using crypto::SHA256_BLOCK_SIZE;

        Logger& logger = *_logger;
        *logger.log = logger.initial_timestamp;
        *logger.log = logger.final_timestamp;

        cout << "hashing...\n";
        Sha256 hasher = Sha256();
        for (int i = 0; i < logger.logs_size; i++) {
            cout << logger.logs[i] << ", ";

            print_as_hex("nha", reinterpret_cast<unsigned char*>(&logger.logs[i]), 64 / 8);
            hasher.update(reinterpret_cast<unsigned char*>(&logger.logs[i]), 64 / 8);
        }
        cout << "\nDone\n";

        byte_t log_hash[SHA256_BLOCK_SIZE];
        hasher.digest(log_hash);
        print_as_hex("Hash", log_hash);

        byte_t cryptographed_hash[32];
        logger.encrypt_hash(log_hash, cryptographed_hash);
        print_as_hex("cry", reinterpret_cast<unsigned char*>(cryptographed_hash), 32);
        
        const int bus_size = 64; // I32 = 32, I64 = 64
        const int sends = 8 * SHA256_BLOCK_SIZE / bus_size;

        for (int i = 0; i < sends; i++) {
            long long value = 0;

            const int parts_size = bus_size / 8;
            for (int j = 0; j < parts_size; ++j) {
                long long part = cryptographed_hash[i * parts_size + j];
                value |= part << (j*8);
            }
            print_as_hex("val", reinterpret_cast<unsigned char*>(&value), 64 / 8);

            *logger.log = value;
        }

        // TODO: taca o AES na log_hash
        // TODO: separa em métodos
        // TODO: envia

        logger.logs_size = 0;
        logger.sending = false;
        logger.thread->exit();

        return 0;
    }

    void encrypt_hash(const byte_t *hash, byte_t *output)
    {
        AES_Common::AES<16> enigma = AES_Common::AES<16>(AES_Common::ECB);
        byte_t input_aes1[16];
        byte_t input_aes2[16];
        for (int i = 0; i < 16; i++) {
            input_aes1[i] = hash[i];
            input_aes2[i] = hash[i+16];
        }

        byte_t output_aes1[16];
        byte_t output_aes2[16];
        enigma.encrypt(input_aes1, key, output_aes1);
        enigma.encrypt(input_aes2, key, output_aes2);

        for (int i = 0; i < 16; i++) {
            output[i] = output_aes1[i];
            output[i+16] = output_aes2[i];
        }
    }

    static const int N_LOGS = 3;
    T * _data;
    Smart_Log * log;
    int logs_size;
    Thread * thread;
    Smart_Log::Value logs[10];
    bool sending;
    long initial_timestamp;
    long final_timestamp;
    const byte_t* key;
};

__END_SYS