#ifndef SRC_SERVER_ISTASERVERHANDLER_HPP_
#define SRC_SERVER_ISTASERVERHANDLER_HPP_

#include "channel/Messages.hpp"
#include "common/IMessageExecutor.hpp"

#include <string>

namespace stamask {



/**
 * Executes method on STA engine's side.
 */
class IStaServerHandler : public IMessageExecutor {
public:

	virtual ~IStaServerHandler() {}

};

}




#endif /* SRC_SERVER_ISTASERVERHANDLER_HPP_ */
