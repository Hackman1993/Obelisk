//
// Created by Hackman.Lo on 3/15/2023.
//

#ifndef OBELISK_ROUTE_PARAM_H
#define OBELISK_ROUTE_PARAM_H

#include <string_view>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace obelisk {
    using namespace boost::spirit::x3;

    struct router_param {
        std::string name_;
        bool static_ = false;
    };

    using boost::fusion::operator<<;
} // obelisk

BOOST_FUSION_ADAPT_STRUCT(obelisk::router_param, name_, static_);

#endif //OBELISK_ROUTE_PARAM_H
