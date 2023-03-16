//
// Created by Hackman.Lo on 3/16/2023.
//

#include "route/route_item.h"

namespace obelisk {
  bool route_item::method_allowed(boost::beast::http::verb verb) {
    return available_method_.contains(verb);
  }
} // obelisk