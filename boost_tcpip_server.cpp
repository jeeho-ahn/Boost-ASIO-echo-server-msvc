#include "tcpip_server.h"
#include <memory>

int main(int argc, char** argv)
{
    std::cout << "Starting TCP/IP Server" << std::endl;
    try
    {
        boost::asio::io_context io_context;
        server s(io_context, 1129);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


    return 0;
}
