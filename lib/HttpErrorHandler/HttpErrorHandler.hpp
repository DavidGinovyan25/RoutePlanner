#include <cpr/cpr.h>

#include <string>

class HttpErrorHandler {
public:
    enum class StatusCode {
      OK = 200,
      BAD_REQUEST = 400,
      UNAUTHORIZED = 401,
      FORBIDDEN = 403,
      NOT_FOUND = 404,
      TOO_MANY_REQUEST = 429,
      INTERNAL_SERVER_ERROR = 500,
      SERVICE_UNAVAILABLE = 503
    };
    std::string HandleNetworkErrors(cpr::Response& r);
    std::string HandleStatusCodeErrors(cpr::Response& r);
};
