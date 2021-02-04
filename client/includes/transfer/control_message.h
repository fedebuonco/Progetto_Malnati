//
// Created by fede on 10/7/20.
//

#ifndef CLIENT_CONTROL_MESSAGE_H
#define CLIENT_CONTROL_MESSAGE_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

/// Encapsulates a control message. A control message is either an auth or a treet message.
/// It has various attributes and
/// a "to_json" method that will generate a result
/// that will be sent to the server
class ControlMessage {
private:
    //std::unique_ptr<boost::property_tree::ptree> json_mess_;

public:
    /// Type of the message, in the server we switch actions depending on this type.
    int type_;
    boost::property_tree::ptree ptree_mess;

    ControlMessage(int tp);
    ControlMessage(const std::string& json_code);
    std::string ToJSON();
    void AddElement(const std::string& element,const std::string& value);
    std::string GetElement(const std::string& element);
};


#endif //CLIENT_CONTROL_MESSAGE_H
