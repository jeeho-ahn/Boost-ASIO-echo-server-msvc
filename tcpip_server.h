#ifndef TCPIP_SERVER_H
#define TCPIP_SERVER_H
//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;


class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    //extract data
                    auto rcvd_msg = std::string(data_).substr(0,length);
                    std::cout << "Received: " << rcvd_msg << std::endl;
                    do_write(length);
                    //send back to TCPIP
                    //do_write(data_);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        std::string out_msg = "hi bmo";
        //echo
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        //send back a custom message
        //boost::asio::async_write(socket_, boost::asio::buffer(out_msg.c_str(), out_msg.size()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                do_read();
            }
        });
    }

    void do_write(std::string out_msg)
    {
        auto self(shared_from_this());

        //boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        boost::asio::async_write(socket_, boost::asio::buffer(out_msg.c_str(), out_msg.size()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::cout << "Client Contacted" << std::endl;
                    std::make_shared<session>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

#endif // TCPIP_SERVER_H
