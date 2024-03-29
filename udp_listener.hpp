#include <boost/asio.hpp>
#include <boost/signals2.hpp>

namespace tcp_udp_listener {

    class udp_socket;

    class udp_listener {
    public:
        boost::signals2::signal<void(udp_socket*, size_t, const boost::system::error_code& error)> receive_event_signal;
        boost::signals2::signal<void(udp_socket*, const boost::system::error_code& error)> confirm_event_signal;

    public:
        bool listen_on(unsigned short port);
        void stop_listening_on(unsigned short port);
        void unsubscribe_logger();
        void unsubscribe_data_reader();

    public:
        template<typename T>
        void subscribe_logger(void(T::* logger)(std::stringstream&), T& obj) {
            logger_ = [=, &obj](std::stringstream& stream) {
                (obj.*logger)(stream);
            };
        }

        template<typename T>
        void subscribe_data_reader(void(T::* reader)(std::string), T& obj) {
            data_reader_ = [=, &obj](std::string data) {
                (obj.*reader)(data);
            };
        }

    public:
        udp_listener(boost::asio::io_context* context);
        ~udp_listener();

    private:
        boost::asio::io_context* context_;
        std::vector<udp_socket*> sockets_;
        std::function<void(std::stringstream&)> logger_;
        std::function<void(std::string)> data_reader_;

    private:
        void handle_receive_event(udp_socket* udp_socket, size_t bytes_transferred, const boost::system::error_code& error);
        void handle_confirm_event(udp_socket* udp_socket, const boost::system::error_code& error);
        void log_receive_event(udp_socket* udp_socket, size_t bytes_transferred, const boost::system::error_code& error);
        void log_confirm_event(udp_socket* udp_socket, const boost::system::error_code& error);
        void send_confirmation_message(udp_socket* udp_socket, size_t bytes_transferred);
        void close_socket(__gnu_cxx::__normal_iterator<udp_socket**, std::vector<udp_socket*>>& iterator);
        void close_socket_and_reset(__gnu_cxx::__normal_iterator<udp_socket**, std::vector<udp_socket*>>& iterator);
    };
}