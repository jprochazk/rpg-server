
#pragma once
#ifndef SERVER_NETWORK_NET_H
#define SERVER_NETWORK_NET_H

#include "pch.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = net::ip::tcp;                       // from <boost/asio/ip/tcp.hpp>
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = beast::http;                   // from <boost/beast/http.hpp>

#endif // SERVER_NETWORK_NET_H
