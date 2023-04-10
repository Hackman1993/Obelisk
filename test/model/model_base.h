//
// Created by Hackman.Lo on 4/10/2023.
//

#ifndef OBELISK_MODEL_BASE_H
#define OBELISK_MODEL_BASE_H
#include <string_view>
#include "nullable_types.h"
namespace obelisk::orm {

  class model_base {
  protected:
    virtual std::string_view table() = 0;
    virtual std::string_view primary_key() = 0;
    virtual rosetta::string generate_id() {
      return rosetta::string(nullptr);
    };
  };

} // model

#endif //OBELISK_MODEL_BASE_H
