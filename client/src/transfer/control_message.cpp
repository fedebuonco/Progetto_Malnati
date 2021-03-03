#include <iostream>
#include <config.h>
#include <control_message.h>
/**
 * ControlMessage constructor (overload 1).
 * Create a control message object with a given type tp (argument)
 * @param tp : Type of the message (1 for auth, 2 for tree, 3 for deleted)
 */
ControlMessage::ControlMessage(int tp) : type_(tp) {

    // Define the document as an object rather than an array
    // The structure of the JSON will be as KEY - VALUE and not KEY AND MANY VALUES
    document_.SetObject();

    try{
        std::string s_type = std::to_string(tp);
        AddElement("Type",s_type);
    }
    catch (std::exception &e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "Error while adding the " << tp << " in ControlMessage " << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * ControlMessage constructor (overload 2).
 * Parse the string message (JSON formatted) passed by the server into a property tree
 * @param json_message : Message form server formatted in JSON
 */
ControlMessage::ControlMessage(const std::string& json_message){
   try{
       rapidjson::ParseResult ok = this->document_.Parse(json_message.c_str());
       if (!ok) {
        std::cerr << "error parse" << std::endl;
        }
       //Take the 'Type' of the message, we do this for faster check. The other data will be extracted only when needed.
       rapidjson::Value& val = document_["Type"];
       std::string t = val.GetString();
       this->type_ = std::stoi(t);



   }
   catch (std::exception &e){
       if(DEBUG) std::cerr << e.what() << std::endl;
       std::cerr << "Error while adding the " << json_message << " element"<< std::endl;
       std::exit(EXIT_FAILURE);
   }
}

/// Adds the parameter and the value to the ptree of the ControlMessage
/// \param element Name of the propriety
/// \param value  Value of the propriety
void ControlMessage::AddElement( std::string element, std::string value){       //// NOLINT Do not change in const&; ignore the clang warning
    try{

        rapidjson::Document::AllocatorType& alloc = document_.GetAllocator();
        rapidjson::Value key(element.c_str(), alloc);
        rapidjson::Value val(value.c_str(), alloc);
        document_.AddMember(key, val, alloc);



    }
    catch (std::exception &e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "Error while adding the " << element << " element with value " << value << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// Get an element from the message
/// \param element Name of the propriety
/// \return value of the propriety
std::string ControlMessage::GetElement(const std::string& element) {

    try{
        rapidjson::Value& val = document_[element.c_str()];
        return val.GetString();
    }
    catch (std::exception &e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "Error while retrieving the " << element << " element" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// This will convert the ControlMessage into a suitable JSON string (One that follows our protocol).
/// \return Json string of our control message.
std::string ControlMessage::ToJSON() {
    std::string string_to_json;
    try{

        // We create a buffer and assign it to a writer. Then we call the accept that will take the writer and fill the buffer with the JSON
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        this->document_.Accept(writer);

        //then we get the string and return it.
        string_to_json = buffer.GetString();
        return  string_to_json;
    }
    catch (std::exception &e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "Error while converting to JSON" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
