#ifndef JSONTOOLKIT_CONTRIB_RESOLVER_H_
#define JSONTOOLKIT_CONTRIB_RESOLVER_H_

#include <future>   // std::future
#include <optional> // std::optional
#include <string>   // std::string

#include <jsontoolkit/json.h>

namespace sourcemeta::jsontoolkit::contrib {

class Resolver {
public:
  auto operator()(const std::string &identifier)
      -> std::future<std::optional<sourcemeta::jsontoolkit::JSON>>;
};

} // namespace sourcemeta::jsontoolkit::contrib

#endif