//
// Created by Hackman.Lo on 4/6/2023.
//

#ifndef OBELISK_MODEL_H
#define OBELISK_MODEL_H

#include "DB.h"
#include <string>
#include "model_base.h"
#include <unordered_map>
#include "model_auto_fill.h"
#include "model_timestamps.h"
#include "model_soft_delete.h"
#include "model_soft_delete.h"
#include <boost/variant2.hpp>
#include "common/details/request_param_container.h"
#include "exception/database_exception.h"
#include "exception/validation_exception.h"
#include "orm/visitors/stmt_bind_visitor.h"
#include "model_to_json.h"
#include "model_find.h"

namespace obelisk::orm {
  template<class UserModel, bool TimeStamps, bool SoftDelete>
  class model : public model_base ,
      public std::enable_if_t<TimeStamps, model_timestamps<UserModel>>,
      public std::enable_if_t<SoftDelete, model_soft_delete<UserModel>>,
      public model_auto_fill<UserModel>,
      public model_to_json<UserModel> {
  public:

    static UserModel instance(obelisk::request_param_container &data) {
      UserModel model;
      auto properties = rttr::type::get<UserModel>().get_properties();
      model.set_field_(data);
      return model;
    }

    UserModel& update(obelisk::request_param_container &data) {
      this->set_field_(data);
      if constexpr(TimeStamps) this->auto_set_updated_at_();
      std::string fields, values;
      auto dirty_datas = get_dirty_data_();
      prepare_sql_(dirty_datas, fields, values);
      std::string sql = "insert into " + std::string(UserModel::table()) + " (" + fields + ") values (" + values + ")";
      auto connection = DB::get_connection<rosetta::database_connection_base>("default");
      if(!connection) throw sahara::exception::database_exception(500, "No Connection Available");
      auto stmt = connection->prepared_statement(sql);
      if(!stmt) throw sahara::exception::database_exception(500, "No Statement Available");
      std::size_t index = 0;
      for(auto &dirty_field: dirty_datas){
        std::visit(obelisk::orm::visitor::stmt_bind_visitor(index, stmt), dirty_field.second);
        ++index;
      }
      stmt->execute();
      return *(UserModel*)this;
    }

    UserModel& create() {
      if constexpr(TimeStamps) this->auto_set_created_at_();
      if constexpr(TimeStamps) this->auto_set_updated_at_();
      std::string fields, values;
      auto dirty_datas = get_dirty_data_();
      bool first = true;
      for(auto& dirty_data: dirty_datas){
        if(first) first = false;
        else fields += ",", values += ",";
        fields += dirty_data.first, values += "?";
      }
      std::string sql = (boost::format("insert into %1%(%2%) values (%3%)") % UserModel::table() % fields % values).str();
      execute_sql_(sql, dirty_datas);
      return *(UserModel*)this;
    }

    UserModel& update() {
      std::string fields;
      auto dirty_datas = get_dirty_data_();
      bool first = true;
      for(auto& dirty_data: dirty_datas){
        if(first) first = false;
        else fields += ",";
        fields += dirty_data.first + "=?";
      }
      std::string sql = (boost::format("update %1% set %2% where %3%=?") % UserModel::table() % fields % UserModel::primary_key()).str();
      execute_sql_(sql, dirty_datas);
      return *(UserModel*)this;
    }

  protected:

    void execute_sql_(std::string& sql, std::unordered_map<std::string, rosetta::sql_varint_value> dirty_datas, bool bind_primary_key = false){
      auto properties = rttr::type::get(instance).get_properties();
      auto connection = DB::get_connection<rosetta::database_connection_base>("default");
      if(!connection) throw sahara::exception::database_exception(500, "No Connection Available");
      auto stmt = connection->prepared_statement(sql);
      if(!stmt) throw sahara::exception::database_exception(500, "No Statement Available");
      std::size_t index = 0;
      for(auto &dirty_field: dirty_datas){
        std::visit(obelisk::orm::visitor::stmt_bind_visitor(index, stmt), dirty_field.second);
        ++index;
      }
      stmt->execute();
    }
    void prepare_sql_(const std::unordered_map<std::string, rosetta::sql_varint_value>& dirty_datas, std::string& fields, std::string& values){

    }



    std::unordered_map<std::string, rosetta::sql_varint_value> get_dirty_data_() {
      rttr::instance model((UserModel*) this);
      std::unordered_map<std::string, rosetta::sql_varint_value> dirty_data;
      auto properties = rttr::type::get<UserModel>().get_properties();
      for (const rttr::property &property: properties) {
        auto value_var = property.get_value(model);
        std::string property_name = property.get_name().to_string();
        if(type_match<rosetta::string>(property)){
          auto &value = value_var.get_value<rosetta::string&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value);
        } else if(type_match<rosetta::integer>(property)){
          auto &value = value_var.get_value<rosetta::integer&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value_var.get_value<rosetta::integer&>());
        } else if(type_match<rosetta::unsigned_integer>(property)){
          auto &value = value_var.get_value<rosetta::unsigned_integer&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value_var.get_value<rosetta::unsigned_integer&>());
        } else if(type_match<rosetta::long_double>(property)){
          auto &value = value_var.get_value<rosetta::long_double&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value_var.get_value<rosetta::long_double &>());
        } else if(type_match<rosetta::boolean>(property)){
          auto &value = value_var.get_value<rosetta::boolean&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value_var.get_value<rosetta::boolean &>());
        } else if(type_match<rosetta::timestamp>(property)){
          auto &value = value_var.get_value<rosetta::timestamp&>();
          if(value.dirty())
            dirty_data.emplace(property_name, value_var.get_value<rosetta::timestamp&>());
        }
      }
      return dirty_data;
    }
  };
}
#endif //OBELISK_MODEL_H
