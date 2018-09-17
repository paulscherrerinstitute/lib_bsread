#ifndef BSREAD_ZEROCOPYSENDER_H
#define BSREAD_ZEROCOPYSENDER_H

#include "Sender.h"

namespace bsread {
    class ZeroCopySender : public Sender {

    public:
        ZeroCopySender(std::string address, int sndhwm=10, int sock_type=ZMQ_PUSH, int linger=1000,
                compression_type data_header_compression=compression_none, int n_io_threads=1,
                size_t max_header_len=MAX_HEADER_LEN, size_t max_data_header_len=MAX_DATA_HEADER_LEN);

        virtual send_status send_message(uint64_t pulse_id, bsread::timestamp global_timestamp);

    private:
        std::atomic<bool> m_header_buffer_free;
        std::unique_ptr<char[]> m_header_buffer;
        size_t m_header_buffer_len;

        std::atomic<bool> m_data_header_buffer_free;
        std::unique_ptr<char[]> m_data_header_buffer;
        size_t m_data_header_buffer_len;

        zmq_msg_t zmq_message;
    };
}

#endif //BSREAD_ZEROCOPYSENDER_H
