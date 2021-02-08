#include <iostream>
#include <config.h>
#include <control_message.h>
/**
 * ControlMessage constructor (overload 1).
 * Create a control message object with a given type tp (argument)
 * @param tp : Type of the message (1 for auth, 2 for tree, 3 for deleted)
 */
ControlMessage::ControlMessage(int tp) {
    //boost::property_tree::ptree  root;
    // define the document as an object rather than an array
    document_.SetObject();
    try{

        std::string s_type = std::to_string(tp);
        AddElement("Type",s_type);

    }
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << "Type" << "\" was not found inside config.json." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nUnable to parse the message." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * ControlMessage constructor (overload 2).
 * Parse the string message (JSON formatted) passed by the server into a property tree
 * @param json_message : Message form server formatted in JSON
 */
ControlMessage::ControlMessage(const std::string& json_message){

   //boost::property_tree::ptree  root;
   //std::stringstream ss;


   try{
       // We parse the json
       this->document_.Parse(json_message.c_str());
       std::cerr << this->ToJSON() << std::endl;
       //Take the 'Type' of the message, we do this for faster check. The other data will be extracted only when needed.
       rapidjson::Value& val = document_["Type"];
       std::string t = val.GetString();
       this->type_ = std::stoi(t);
   }
   catch (const boost::property_tree::ptree_bad_path& e){
       if(DEBUG) std::cerr << e.what() << std::endl;
       std::cerr << "\nThe \"" << "Type" << "\" was not found inside config.json." << std::endl;
       std::exit(EXIT_FAILURE);
   }
   catch (const boost::property_tree::json_parser::json_parser_error& e) {
       if(DEBUG) std::cerr << e.what() << std::endl;
       std::cerr <<"\nMessage arrived from server is bad formatted. Unable to parse the message." << std::endl;
       std::exit(EXIT_FAILURE);
   }
}

/// Adds the parameter and the value to the ptree of the ControlMessage
/// \param element Name of the propriety
/// \param value  Value of the propriety
void ControlMessage::AddElement( std::string element, std::string value){

    try{

        rapidjson::Value j_key;
        j_key.SetString(element.c_str(), element.length(), document_.GetAllocator());
        rapidjson::Value j_val;
        j_val.SetString(value.c_str(), value.length(), document_.GetAllocator());
        document_.AddMember(j_key, j_val, document_.GetAllocator());


    }
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << "Type" << "\" was not found inside config.json." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nUnable to parse the message." << std::endl;
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
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << "Type" << "\" was not found inside config.json." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nThe configuration file was not found or is bad formatted." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// This will convert the ControlMessage into a suitable JSON string (One that follows our protocol).
/// \return Json string of our control message.
std::string ControlMessage::ToJSON() {
    std::stringstream ss;
    std::string string_to_json;
    try{

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        this->document_.Accept(writer);
        string_to_json = buffer.GetString();
        return  string_to_json;
    }
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << "Type" << "\" was not found inside config.json." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nUnable to parser the message." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
