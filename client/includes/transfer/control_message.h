#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

/// Encapsulates a control message. A control message is either an auth or a treet message.
/// It has various attributes and a "to_json" method that will generate a result that will be sent to the server
class ControlMessage {

<<<<<<< Updated upstream
    /// Type of the message, in the server we switch actions depending on this type.
    boost::property_tree::ptree ptree_mess;

public:
    int type_;  //TODO: Make it private and create a method to access it @marco

    explicit ControlMessage(int tp);
    explicit ControlMessage(const std::string& json_code);

=======

    boost::property_tree::ptree ptree_mess;

public:
    /// Type of the message, in the server we switch actions depending on this type.
    int type_;
    ControlMessage(int tp);
    ControlMessage(const std::string& json_code);
>>>>>>> Stashed changes
    std::string ToJSON();

    void AddElement(const std::string& element,const std::string& value);
    std::string GetElement(const std::string& element);
};

