#pragma once

#include <optional>
#include <string>

#include "boost/json.hpp"
#include "boost/noncopyable.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/ssl.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
namespace json = boost::json;

using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

namespace Strava
{
	using HeaderItem = std::pair<http::field, std::string>;
	using HeaderVector = std::vector<HeaderItem>;

	class ClientNetworkWrapper : public boost::noncopyable
	{
	public:
		ClientNetworkWrapper(const std::string& host);
		~ClientNetworkWrapper();

		bool Initialize();

		bool SendRequest(http::verb reqType, const std::string& path, const HeaderVector& header, const json::object& body, unsigned int& status, json::value& response);

	private:
		bool ConnectIfNeeded();
		bool DisconnectIfNeeded();

		void BootstrapRequest(http::request<http::string_body>& request);

		static boost::beast::string_view GetContentType(const http::response<http::string_body>& response);

	private:
		bool m_initialized;

		net::io_context m_ioc;
		ssl::context m_sslContext;

		const std::string m_host;

		std::optional<tcp::resolver::results_type> m_resolvedEndpoints;

		beast::flat_buffer m_readingBuffer;
		std::optional<beast::ssl_stream<beast::tcp_stream>> m_sslStream;
	};
}