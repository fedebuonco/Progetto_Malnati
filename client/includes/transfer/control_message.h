//
// Created by fede on 10/7/20.
//

#ifndef CLIENT_CONTROL_MESSAGE_H
#define CLIENT_CONTROL_MESSAGE_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

///Encapsulates a control message. A control message is either an auth or a tree message. It has various attributes and
/// a "to_json" method that will generate a result that will be sent to the server
class ControlMessage {
    int type_;
    //TODO for now we send password and username in clear, we could use a encripted key later on
    std::string username_;
    std::string password_;
    std::string tree_;
    std::unique_ptr<boost::property_tree::ptree> json_mess_;
public:
    ControlMessage(int tp, std::string user, std::string pwd, std::string tr);
    std::string ToJSON();
};


#endif //CLIENT_CONTROL_MESSAGE_H
