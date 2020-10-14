//
// Created by fede on 10/14/20.
//

#include "control_message.h"
#include <iostream>

/// Constructor for the control message. Type + username and password or Type + tree are mandatory, we cant send a mess
/// without type.
/// \param tp Type of the message (1 for auth, 2 for tree, etc)
/// \param user Username of the user
/// \param pwd Password for the specified user
/// \param tr Tree of the current selected directory
ControlMessage::ControlMessage(int tp, std::string user, std::string pwd, std::string tr) {
    this->type_ = tp;
    this->username_ = user;
    this->password_ = pwd;
    this->tree_ = tr;

}

/// Constructor for the control message. Build from the pt passed
//TODO pass this in a better way (look at the various diff and advantages of passing in one  way
/// \param pt Contains the parsed json
ControlMessage::ControlMessage(boost::property_tree::ptree pt){

    this->type_ = pt.get<int>("Type");
    this->username_ = pt.get<std::string>("Username");
    this->password_ = pt.get<std::string>("Password");
};



/// This will convert the object into a suitable JSON string (One that follows our protocol).
std::string ControlMessage::ToJSON() {
    //Destroy the current managed object of the smart pointer
    //useful if we plan to generate the controlmessage json multiple time
    json_mess_.reset(new boost::property_tree::ptree);
    // let's load all the values
    json_mess_->put("Type", type_);
    json_mess_->put("Username", username_);
    json_mess_->put("Password", password_);
    json_mess_->put("Tree", tree_);

    //Create the variable to host the result
    std::string is;
    std::stringstream ss;
    boost::property_tree::write_json(ss, *json_mess_);
    is = ss.str();
    return is;
}