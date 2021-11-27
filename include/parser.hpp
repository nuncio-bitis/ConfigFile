//-----------------------------------------------------------------------------

#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP
//-----------------------------------------------------------------------------

#include <string>
#include <stdexcept>

//-----------------------------------------------------------------------------

/**
 * This class is used to parse an XML file.<br>
 * It is mainly an example of XML file parsing.
 */
class GetConfig {
public:
  GetConfig();
  ~GetConfig();

  void readConfigFile(std::string&) throw (std::runtime_error);
  std::string getOption(std::string option);

private:
  xercesc::XercesDOMParser *m_ConfigFileParser;
  void parseNode(xercesc::DOMNode* node, std::string indent = "");
};

//-----------------------------------------------------------------------------
#endif // XML_PARSER_HPP
