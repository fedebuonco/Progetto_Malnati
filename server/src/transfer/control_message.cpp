//
// Created by fede on 10/7/20.
//

#include <iostream>
#include "control_message.h"

/// Constructor for the control message. Type is mandatory, then we can add other info by using AddElement()
/// \param tp Type of the message (1 for auth, 2 for tree, etc)
ControlMessage::ControlMessage(int tp) {
    json_mess_.reset(new boost::property_tree::ptree);
    this->type_ = tp;
    this->json_mess_->put("Type" , tp);
}

/// Constructor for the control message , used for message arrivving from the client
/// Build from the json string recived and also stores the type,
/// username and hashed password  in members of the class
/// for faster access.
/// \param  Contains the parsed json
ControlMessage::ControlMessage(std::string json_code){

    json_mess_.reset(new boost::property_tree::ptree);
    //We have the request in a json formatted string, let's parse it in a request_ptree
    std::stringstream ss;
    ss << json_code;
    boost::property_tree::read_json(ss, *this->json_mess_);
    // And also store some infos, for faster access
    int t =  json_mess_->get<int>("Type");
    this->type_ = t;
    // TODO check if not found then we should notify the user.
    this->username_ = json_mess_->get<std::string>("Username");
    this->hashkey_ = json_mess_->get<std::string>("HashPassword");

};

/// Adds the parameter and the value to the ptree of the ControlMessage
/// \param element Name of the propriety
/// \param value  Value of the propriety
void ControlMessage::AddElement(std::string element, std::string value){
    this->json_mess_->put(element , value);
}

/// Get an element from the message
/// \param element Name of the propriety
/// \return value of the propriety
std::string ControlMessage::GetElement(std::string element) {
    //TODO se elemento non c'è crasha tutto
    std::string result = json_mess_->get<std::string>(element);
    return result;
}

/// This will convert the object into a suitable JSON string (One that follows our protocol).
std::string ControlMessage::ToJSON() {
    //Create the variable to host the result
    std::string is;
    std::stringstream ss;
    boost::property_tree::write_json(ss, *json_mess_);
    is = ss.str();
    return is;
}
