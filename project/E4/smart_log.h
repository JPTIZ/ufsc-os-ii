#include <smart_data.h>
#include <alarm.h>
#include <utility/array.h>
#include <thread.h>
#include <semaphore.h>

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
    Logger(T * t, Smart_Log* log, const byte_t* key, const int num_logs=3):
        num_logs(num_logs),
        key{key},
        thread_semaphore(0),
        _data(t),
        log(log),
        logs_size(0),
        logs(),
        thread{new Thread(&send_log, this)},
        sending{false}
    {
        _data->attach(this);
        thread->priority(Thread::IDLE);
    }

    ~Logger() { _data->detach(this); }

    void update(Smart_Data_Common::Observed * obs) {
        if (sending) {
            return;
        }

        if (logs_size == 0) {
            initial_timestamp = _data->time();
        }

        logs[logs_size] = *_data;
        ++logs_size;

        if (logs_size == num_logs) {
            final_timestamp = _data->time();
            // if (thread)
            //     delete thread;
            sending = true;
            thread_semaphore.v();
        }
    }

private:
    static int send_log(Logger* _logger) {
        using crypto::Sha256;
        using crypto::SHA256_BLOCK_SIZE;

        Logger& logger = *_logger;

        while (true) {
            logger.thread_semaphore.p();
            
            *logger.log = logger.initial_timestamp;
            *logger.log = logger.final_timestamp;

            Sha256 hasher = Sha256();
            for (int i = 0; i < logger.logs_size; i++) {
                hasher.update(reinterpret_cast<unsigned char*>(&logger.logs[i]), 64 / 8);
            }

            byte_t log_hash[SHA256_BLOCK_SIZE];
            hasher.digest(log_hash);

            byte_t cryptographed_hash[32];
            logger.encrypt_hash(log_hash, cryptographed_hash);
            
            const int bus_size = 64; // I32 = 32, I64 = 64
            const int sends = 8 * SHA256_BLOCK_SIZE / bus_size;

            for (int i = 0; i < sends; i++) {
                long long value = 0;

                const int parts_size = bus_size / 8;
                for (int j = 0; j < parts_size; ++j) {
                    long long part = cryptographed_hash[i * parts_size + j];
                    value |= part << (j*8);
                }

                *logger.log = value;
            }

            // TODO: separa em métodos

            logger.logs_size = 0;
            logger.sending = false;
        }
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

    const int num_logs;
    const byte_t* key;
    Semaphore thread_semaphore;
    T * _data;
    Smart_Log * log;
    int logs_size;
    Thread * thread;
    bool sending;
    long initial_timestamp;
    long final_timestamp;
    Smart_Log::Value logs[10];
};

__END_SYS