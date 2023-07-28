#include "common/route/route_item.h"
#include "common/response/empty_response.h"

namespace obelisk {
    bool route_item::method_allowed(const sahara::string &method) {
        return available_method_.contains(method);
    }

    route_item::route_item(const sahara::string &path, std::function<std::shared_ptr<http_response>(http_request &)> handler) : handler_(std::move(handler)) {
        std::vector<sahara::string> splited = path.split("/");
        auto router_param_parser = rule<class route_parser, router_param>{"RouterParam"} = (("{" > (+~char_("{}/") >> attr(false)) > "}") | (+~char_("/") >> attr(true)));
        for (auto &item: splited) {
            if (item.empty()) continue;
            router_param param;
            std::string actual_data = item.to_std();
            if (!boost::spirit::x3::parse(actual_data.begin(), actual_data.end(), router_param_parser, param)) {
                throw exception::system_exception("Router Parse Failed; Route: " + path);
            }
            params_.emplace_back(param);
        }
    }

    bool route_item::match(std::vector<sahara::string> &split) {
        if (any_match_) return true;
        if (split.size() != params_.size()) return false;

        for (int i = 0; i < params_.size(); i++)
            if (params_[i].static_ && params_[i].name_ != split[i]) return false;
        return true;
    }

    route_item &route_item::add_method(const sahara::string &method) {
        available_method_.emplace(method, true);
        return *this;
    }

    std::unordered_map<sahara::string, sahara::string> route_item::parse(std::vector<sahara::string> &split) {
        if (any_match_) return {};

        std::unordered_map<sahara::string, sahara::string> result;
        for (int i = 0; i < params_.size(); i++)
            if (!params_[i].static_) result.emplace(params_[i].name_, split[i]);
        return std::move(result);
    }

    std::shared_ptr<http_response> route_item::handle(http_request &request) {
        std::vector<sahara::string> split_path = request.path().split("/");
        auto method = request.method();
        if (!this->match(split_path)) return nullptr;
        if (method.iequals("OPTIONS") || method.iequals("HEAD")) {
            return std::make_unique<empty_response>(204);
        }
        if (!this->method_allowed(method))
            return std::make_unique<empty_response>(405);
        for (auto &middleware: this->get_middlewares()) {
            auto resp = middleware.handle(request);
            if (resp) return resp;
        }
        if (handler_) {
            request.route_params_ = parse(split_path);
            return std::move(handler_(request));
        }
        return nullptr;
    }

    route_item &route_item::middleware(const sahara::string &params, middleware_callback trigger) {
        middlewares_.emplace_back(params, std::move(trigger));
        return *this;
    }

    std::vector<middleware_trigger> &route_item::get_middlewares() {
        return middlewares_;
    }

    route_item &route_item::allow_cors(const std::string &origin) {
        allow_cros_.emplace_back(origin);
        return *this;
    }

    std::vector<std::string> &route_item::allow_cors() {
        return allow_cros_;
    }
}