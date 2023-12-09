#include "APIFactory.h"

#include "API.h"
#include "impl/ServerNetworkParametersBundle.h"

namespace Strava
{
	std::shared_ptr<IAPIPublicInterface> APIFactory::CreateAPIInstance(int32_t clientId, const std::string& clientSecret, std::shared_ptr<Async::IServerNetworkParametersBundleSetter> pSrvNetParamBundle)
	{
		return std::shared_ptr<API>(new API(clientId, clientSecret, pSrvNetParamBundle));
	}

	std::shared_ptr<Async::IServerNetworkParametersBundleSetter> APIFactory::CreateServerNetworkParametersBundle(const std::string& serverInterface, uint16_t serverPort, const std::string& certFile, const std::string& pkFile, const std::string& pkPassphrase)
	{
		return std::shared_ptr<Async::ServerNetworkParametersBundle>(new Async::ServerNetworkParametersBundle(serverInterface, serverPort, certFile, pkFile, pkPassphrase));
	}
}