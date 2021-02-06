#include <iostream>
#include <config.h>
#include <control_message.h>


/**
 * ControlMessage constructor (overload 1).
 * Create a control message object with a given type tp (argument)
 * @param tp : Type of the message (1 for auth, 2 for tree, 3 for deleted)
 */
ControlMessage::ControlMessage(int tp) {
    boost::property_tree::ptree  root;

    try{
        root.put("Type", tp);

        this->type_ = tp;
        this->ptree_mess = root;
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

   boost::property_tree::ptree  root;
   std::stringstream ss;

   try{
       //Create a strings stream with the json_message
       ss << json_message;

       //Fill the property tree (root) with json_message.
       //Now we have parsed the property tree
       boost::property_tree::read_json(ss, root);

       //Take the 'Type' of the message
       int t = root.get<int>("Type");
       this->type_ = t;
       this->ptree_mess=root;
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
void ControlMessage::AddElement(const std::string& element,const std::string& value){

    try{
        this->ptree_mess.put(element , value);
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
        auto result = ptree_mess.get<std::string>(element);
        return result;
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

        //Fill the stream ss with the ptree message
        //Now we inside the stream the message
        boost::property_tree::write_json(ss, this->ptree_mess);

        return  ss.str();
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
