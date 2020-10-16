//
// Created by fede on 10/14/20.
//

#ifndef SERVER_CONTROL_MESSAGE_H
#define SERVER_CONTROL_MESSAGE_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

///Encapsulates a control message. A control message is either an auth or a tree message. It has various attributes and
/// a "to_json" method that will generate a result that will be sent to the server
class ControlMessage {
private:
    std::unique_ptr<boost::property_tree::ptree> json_mess_;

public:
    int type_;

    ControlMessage(int tp);
    ControlMessage(std::string json_code);
    std::string ToJSON();
    void AddElement(std::string element, std::string value);
    std::string GetElement(std::string element);
};


#endif //SERVER_CONTROL_MESSAGE_H
