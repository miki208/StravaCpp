#include "impl/ServerNetworkWrapper.h"

namespace Strava
{
	namespace Async
	{
		HttpsSession::HttpsSession(tcp::socket&& socket, ssl::context& context, const OnSessionStart& onSessionStartCb, const OnSessionEnd& onSessionEndCb, const OnRequest& onRequestCb) :
			m_sslStream(std::move(socket), context), m_onSessionStartCb(onSessionStartCb), m_onSessionEndCb(onSessionEndCb), m_onRequestCb(onRequestCb)
		{
		}

		void HttpsSession::start()
		{
			if (m_onSessionStartCb && !m_onSessionStartCb(m_sslStream))
				return;

			m_sslStream.async_handshake(ssl::stream_base::server, [self = shared_from_this()](const error_code& ec)
				{
					if (ec)
					{
						if(self->m_onSessionEndCb)
							self->m_onSessionEndCb(self->m_sslStream, ec);

						return;
					}

					self->doRead();
				}
			);
		}

		void HttpsSession::doRead()
		{
			m_request = {};

			http::async_read(m_sslStream, m_buffer, m_request, [self = shared_from_this()](const error_code& ec, std::size_t)
				{
					if (ec)
						return;

					self->processRequest();
				}
			);
		}

		void HttpsSession::processRequest()
		{
			bool shouldKeepAlive = false;
			
			m_response = {};

			if (m_onRequestCb)
				shouldKeepAlive = m_onRequestCb(m_request, m_response);
			else
				m_response.result(http::status::internal_server_error);

			http::async_write(m_sslStream, m_response, [self = shared_from_this(), shouldKeepAlive](const error_code& ec, std::size_t)
				{
					if (ec)
					{
						if (self->m_onSessionEndCb)
							self->m_onSessionEndCb(self->m_sslStream, ec);

						return;
					}

					if (!shouldKeepAlive)
					{
						if (self->m_onSessionEndCb)
							self->m_onSessionEndCb(self->m_sslStream, boost::system::errc::make_error_code(boost::system::errc::success));

						return;
					}

					self->doRead();
				}
			);
		}

		ServerNetworkWrapper::ServerNetworkWrapper() :
			m_sslContext(ssl::context::tlsv12_server), m_initialized(false), m_started(false)
		{

		}

		ServerNetworkWrapper::~ServerNetworkWrapper()
		{

		}

		error_code ServerNetworkWrapper::Initialize(const std::string& certificateFile, const std::string& pkFile, const std::string& pkPassphrase, const OnStoppedListening& onStoppedListeningCb, const HttpsSession::OnSessionStart& onSessionStartCb, const HttpsSession::OnSessionEnd& onSessionEndCb, const HttpsSession::OnRequest& onRequestCb)
		{
			error_code ec;

			if (m_initialized)
				return boost::system::errc::make_error_code(boost::system::errc::success);

			if(!pkPassphrase.empty())
				m_sslContext.set_password_callback([pkPassphrase](size_t, ssl::context_base::password_purpose) {
					return pkPassphrase;
				}, ec);

			if (ec)
				return ec;

			if (m_sslContext.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2, ec))
				return ec;

			if (m_sslContext.use_certificate_chain_file(certificateFile, ec))
				return ec;

			if (m_sslContext.use_private_key_file(pkFile, ssl::context::file_format::pem, ec))
				return ec;

			m_onStoppedListeningCb = onStoppedListeningCb;
			m_onSessionStartCb = onSessionStartCb;
			m_onSessionEndCb = onSessionEndCb;
			m_onRequestCb = onRequestCb;

			m_initialized = true;

			return  boost::system::errc::make_error_code(boost::system::errc::success);
		}

		std::pair<error_code, std::string> ServerNetworkWrapper::Start(const std::string& interface, uint16_t port)
		{
			if (!m_initialized)
				return { boost::system::errc::make_error_code(boost::system::errc::protocol_error), "Server network wrapper is not initialized." };

			if (m_started)
				return { boost::system::errc::make_error_code(boost::system::errc::protocol_error), "Server network wrapper is already started." };

			if (m_tcpAcceptor)
				m_tcpAcceptor.reset();

			error_code ec;

			tcp::endpoint endpoint(net::ip::make_address(interface), port);

			m_tcpAcceptor = tcp::acceptor(m_ioc);

			if (m_tcpAcceptor->open(endpoint.protocol(), ec))
				return { ec, "" };

			if (m_tcpAcceptor->set_option(net::socket_base::reuse_address(true), ec))
				return { ec, "" };

			if (m_tcpAcceptor->bind(endpoint, ec))
				return { ec, "" };

			if (m_tcpAcceptor->listen(net::socket_base::max_listen_connections, ec))
				return { ec, "" };

			m_ioThread = std::thread([this] {
				m_started = true;

				m_tcpAcceptor->async_accept(m_ioc, beast::bind_front_handler(&ServerNetworkWrapper::OnAccept, this));

				m_ioc.run();
			});

			return { boost::system::errc::make_error_code(boost::system::errc::success), "" };
		}

		bool ServerNetworkWrapper::Stop()
		{
			if (!m_initialized)
				return false;

			bool prevStatus = m_started.exchange(false);
			
			if (!prevStatus)
				return false;

			m_ioc.stop();

			if (m_ioThread && m_ioThread->joinable())
			{
				m_ioThread->join();

				m_ioThread.reset();
			}

			if(m_onStoppedListeningCb)
				m_onStoppedListeningCb(boost::system::errc::make_error_code(boost::system::errc::success), "");

			return true;
		}

		void ServerNetworkWrapper::OnAccept(error_code ec, tcp::socket socket)
		{
			if (ec)
			{
				if (m_onStoppedListeningCb)
					m_onStoppedListeningCb(ec, ec.message());

				return;
			}

			std::make_shared<HttpsSession>(std::move(socket), m_sslContext, m_onSessionStartCb, m_onSessionEndCb, m_onRequestCb)->start();

			m_tcpAcceptor->async_accept(m_ioc, beast::bind_front_handler(&ServerNetworkWrapper::OnAccept, this));
		}
}
}