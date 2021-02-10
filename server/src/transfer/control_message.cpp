//
// Created by fede on 10/7/20.
//

#include <iostream>
#include "control_message.h"

/**
 * ControlMessage constructor (overload 1).
 * Create a control message object with a given type tp (argument)
 * @param tp : Type of the message (1 for auth, 2 for tree, 3 for deleted)
 */
ControlMessage::ControlMessage(int tp) {
    //boost::property_tree::ptree  root;
    // define the document as an object rather than an array
    document_.SetObject();
    std::string s_type = std::to_string(tp);
    AddElement("Type",s_type);
}

/// Constructor for the control message , used for message arrivving from the client
/// Build from the json string recived and also stores the type,
/// username and hashed password  in members of the class
/// for faster access.
/// \param  Contains the parsed json
ControlMessage::ControlMessage(std::string json_message) {
    // We parse the json
    this->document_.Parse(json_message.c_str());
    //std::cerr << this->ToJSON() << std::endl;
    //Take the 'Type' of the message, we do this for faster check. The other data will be extracted only when needed.
    rapidjson::Value &val = document_["Type"];
    std::string temp = val.GetString();
    this->type_ = std::stoi(temp);
    // We are in the server we also take the username and passw
    rapidjson::Value &val1 = document_["Username"];
    temp = val1.GetString();
    this->username_ = temp;
    //-
    rapidjson::Value &val2 = document_["HashPassword"];
    temp = val2.GetString();
    this->hashkey_ = temp;
}

/// Adds the parameter and the value to the ptree of the ControlMessage
/// \param element Name of the propriety
/// \param value  Value of the propriety
void ControlMessage::AddElement( std::string element, std::string value){



        rapidjson::Value j_key;
        j_key.SetString(element.c_str(), element.length(), document_.GetAllocator());
        rapidjson::Value j_val;
        j_val.SetString(value.c_str(), value.length(), document_.GetAllocator());
        document_.AddMember(j_key, j_val, document_.GetAllocator());



}

/// Get an element from the message
/// \param element Name of the propriety
/// \return value of the propriety
std::string ControlMessage::GetElement(std::string element) {
        rapidjson::Value& val = document_[element.c_str()];
        return val.GetString();
}

/// This will convert the ControlMessage into a suitable JSON string (One that follows our protocol).
/// \return Json string of our control message.
std::string ControlMessage::ToJSON() {
    std::string string_to_json;
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    this->document_.Accept(writer);
    string_to_json = buffer.GetString();
    return string_to_json;

}