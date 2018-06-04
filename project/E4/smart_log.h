#include <smart_data.h>
#include <alarm.h>
#include <utility/array.h>
#include <thread.h>

__BEGIN_SYS

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

    static const bool INTERRUPT = true;
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
    Logger(T * t, Smart_Log* log) :
        _data(t),
        log(log),
        logs_size(0),
        logs(),
        thread{0},
        sending{false}
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
    static int send_log(Logger* _logger) {

        Logger& logger = *_logger;
        *logger.log = logger.initial_timestamp;
        *logger.log = logger.final_timestamp;
        for (int i = 0; i < logger.logs_size; i++) {
            *logger.log = logger.logs[i];
        }
        logger.logs_size = 0;
        logger.sending = false;
        logger.thread->exit();
        return 0;
    }

    static const int N_LOGS = 3;
    T * _data;
    Smart_Log * log;
    int logs_size;
    Thread * thread;
    Smart_Log::Value logs[10];
    bool sending;
    long int initial_timestamp;
    long int final_timestamp;
};

__END_SYS
