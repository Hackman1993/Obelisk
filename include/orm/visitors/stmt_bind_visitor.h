//
// Created by Hackman.Lo on 4/11/2023.
//

#ifndef OBELISK_STMT_BIND_VISITOR_H
#define OBELISK_STMT_BIND_VISITOR_H

#include <common/sql_types.h>
#include <common/sql_statement_base.h>
namespace obelisk::orm::visitor {
  struct stmt_bind_visitor {
    stmt_bind_visitor(std::size_t index, std::shared_ptr<rosetta::sql_statement_base> stmt) : stmt_(stmt), index_(index) {}

    void operator()(const rosetta::string &value) const {
      stmt_->bind_param(index_, value);
    }

    void operator()(const rosetta::integer &value) const {
      stmt_->bind_param(index_, value);
    }

    void operator()(const rosetta::unsigned_integer &value) const {
      stmt_->bind_param(index_, value);
    }

    void operator()(const rosetta::boolean &value) const {
      stmt_->bind_param(index_, value);
    }

    void operator()(const rosetta::long_double &value) const {
      stmt_->bind_param(index_, value);
    }

    void operator()(const rosetta::timestamp &value) const {
      stmt_->bind_param(index_, value);
    }
    std::size_t index_;
    std::shared_ptr<rosetta::sql_statement_base> stmt_;
  };
} // obelisk::orm::visitor

#endif //OBELISK_STMT_BIND_VISITOR_H
