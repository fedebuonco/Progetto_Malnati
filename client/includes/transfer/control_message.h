#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <document.h>     // rapidjson's DOM-style API
#include <prettywriter.h> // for stringify JSON

/// Encapsulates a control message. A control message is either an auth or a treet message.
/// It has various attributes and a "to_json" method that will generate a result that will be sent to the server
class ControlMessage {

    /// Type of the message, in the server we switch actions depending on this type.
    rapidjson::Document document_;

public:
    /// Type, identifies the message, get parsed first in order to correctly parse the rest of the message.
    int type_;

    explicit ControlMessage(int tp);
    explicit ControlMessage(const std::string& json_code);

    std::string ToJSON();

    void AddElement(std::string element, std::string value);
    std::string GetElement(const std::string& element);
};

