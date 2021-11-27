//-----------------------------------------------------------------------------

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "parser.hpp"

using namespace xercesc;
using namespace std;

//-----------------------------------------------------------------------------

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

GetConfig::GetConfig()
{
  try {
    XMLPlatformUtils::Initialize(); // Initialize Xerces infrastructure
  } catch (XMLException& e) {
    char* message = XMLString::transcode(e.getMessage());
    cerr << "XML toolkit initialization error: " << message << endl;
    XMLString::release(&message);
    // throw exception here to return ERROR_XERCES_INIT
  }

  m_ConfigFileParser = new XercesDOMParser;
}

//-----------------------------------------------------------------------------

/**
 *  Class destructor frees memory used to hold the XML tag and 
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

GetConfig::~GetConfig()
{
  // Free memory

  delete m_ConfigFileParser;

  // Terminate Xerces
  try {
    XMLPlatformUtils::Terminate(); // Terminate after release of memory
  } catch (xercesc::XMLException& e) {
    char* message = xercesc::XMLString::transcode(e.getMessage());

    cerr << "XML ttolkit teardown error: " << message << endl;
    XMLString::release(&message);
  }
}

//-----------------------------------------------------------------------------

/**
 *  This function:<br>
 *  - Tests the access and availability of the XML configuration file.<br>
 *  - Configures the xerces-c DOM parser.<br>
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param configFile The text string name of the HLA configuration file.
 */

void GetConfig::readConfigFile(std::string& configFile) throw (std::runtime_error)
{
  // Test to see if the file is ok.
  struct stat fileStatus;

  int iretStat = stat(configFile.c_str(), &fileStatus);
  if (errno == ENOENT)
    throw(std::runtime_error("Path '" + configFile + "' does not exist, or path is an empty string."));
  else if (iretStat == ENOTDIR)
    throw(std::runtime_error("A component of the path is not a directory."));
  else if (iretStat == ELOOP)
    throw(std::runtime_error("Too many symbolic links encountered while traversing the path."));
  else if (iretStat == EACCES)
    throw(std::runtime_error("Permission denied."));
  else if (iretStat == ENAMETOOLONG)
    throw(std::runtime_error("File can not be read."));

  //---------------------------------------------------------------------------
  // Configure DOM parser.

  m_ConfigFileParser->setValidationScheme(XercesDOMParser::Val_Never);
  m_ConfigFileParser->setDoNamespaces(false);
  m_ConfigFileParser->setDoSchema(false);
  m_ConfigFileParser->setLoadExternalDTD(false);

  try {
    //-------------------------------------------------------------------------
    m_ConfigFileParser->parse(configFile.c_str());

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

    //-------------------------------------------------------------------------
    // Get the top-level element: Name is "root". No attributes for "root"
    DOMElement* elementRoot = xmlDoc->getDocumentElement();
    if (!elementRoot)
      throw(std::runtime_error("Empty XML document"));

    //-------------------------------------------------------------------------
    // Parse through entire document tree.
    parseNode(elementRoot);

    //-------------------------------------------------------------------------
  } catch (const OutOfMemoryException&) {

    XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } catch (const xercesc::XMLException& e) {

    char* message = xercesc::XMLString::transcode(e.getMessage());
    ostringstream errBuf;
    errBuf << "Error parsing file: " << message << flush;
    XMLString::release(&message);

    //-------------------------------------------------------------------------
  } catch (const DOMException& e) {

    const unsigned int maxChars = 2047;
    XMLCh errText[maxChars + 1];

    XERCES_STD_QUALIFIER cerr << XERCES_STD_QUALIFIER endl
        << "DOM Error during parsing: '" << configFile << "'" << XERCES_STD_QUALIFIER endl
        << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

    if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
      XERCES_STD_QUALIFIER cerr << "Message is: " << errText << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } catch (...) {

    XERCES_STD_QUALIFIER cerr << XERCES_STD_QUALIFIER endl
        << "An error occurred during parsing."
        << XERCES_STD_QUALIFIER endl;

    // Make the caller aware of the exception.
    throw;
  }

} // end readConfigFile

//-----------------------------------------------------------------------------

void GetConfig::parseNode(xercesc::DOMNode* node, std::string indent)
{
  bool parseChildren = true;

  if (node->getNodeType() == DOMNode::ELEMENT_NODE)
  {
    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*> (node);

    // Print DOMElement info...
    XERCES_STD_QUALIFIER cout << indent << "Element: " << XMLString::transcode(currentElement->getTagName ()) << XERCES_STD_QUALIFIER endl;

    // TODO: *** THIS IS A MESS ***
    // Get option attributes under the specified element.
//    if (XMLString::equals(currentElement->getTagName(), TAG_ApplicationSettings)) {
//      // Node is type Element and of name "ApplicationSettings".
//
//      // Read attributes of element "ApplicationSettings".
//      // Option will be set to empty string if attribute not found.
//      const XMLCh* xmlch_OptionA = currentElement->getAttribute(ATTR_OptionA);
//      m_OptionA = XMLString::transcode(xmlch_OptionA);
//
//      const XMLCh* xmlch_OptionB = currentElement->getAttribute(ATTR_OptionB);
//      m_OptionB = XMLString::transcode(xmlch_OptionB);
//
//      const XMLCh* xmlch_OptionC = currentElement->getAttribute(ATTR_OptionC);
//      m_OptionC = XMLString::transcode(xmlch_OptionC);
//    }

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
    DOMAttr* attr = dynamic_cast<xercesc::DOMAttr*> (node);

    XERCES_STD_QUALIFIER cout << indent << "Attribute: "
      << XMLString::transcode(attr->getName()) << " = "
      << XMLString::transcode(attr->getValue()) << XERCES_STD_QUALIFIER endl;

    // Attribute has child that is text, which is its value.
    // Since the value was just processed, don't parse attribute's children.
    parseChildren = false;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::TEXT_NODE) {

    char * text = XMLString::transcode(node->getTextContent());
    XMLString::trim(text);
    if (strlen(text) > 0)
      XERCES_STD_QUALIFIER cout << indent << "Text: " << text << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::CDATA_SECTION_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "CDATA Section Node: (" << node->getNodeType() << ")" << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "Entity Reference Node: (" << node->getNodeType() << ") "
        << XMLString::transcode(node->getNodeName()) << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::ENTITY_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "Entity Node: (" << node->getNodeType() << ") "
        << XMLString::transcode(node->getNodeName()) << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::PROCESSING_INSTRUCTION_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "Processing Instruction Node: (" << node->getNodeType() << ") "
        << XMLString::transcode(node->getNodeName()) << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::COMMENT_NODE) {
    char * text = XMLString::transcode(node->getTextContent());
    XMLString::trim(text);
    XERCES_STD_QUALIFIER cout << indent << "<!-- "
        << text << " -->" << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::DOCUMENT_NODE) {

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "DocType Node: (" << node->getNodeType() << ") "
        << XMLString::transcode(node->getNodeName()) << XERCES_STD_QUALIFIER endl;

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::DOCUMENT_FRAGMENT_NODE) {

    //-------------------------------------------------------------------------
  } else if (node->getNodeType() == DOMNode::NOTATION_NODE) {
    XERCES_STD_QUALIFIER cout << indent << "Notation Node: (" << node->getNodeType() << ") "
        << XMLString::transcode(node->getNodeName()) << XERCES_STD_QUALIFIER endl;
  }


  //---------------------------------------------------------------------------
  // Process node's attributes

  if (node->hasAttributes()) {
    DOMNamedNodeMap *attrs = node->getAttributes();
    const XMLSize_t attrCount = attrs->getLength();

    for (XMLSize_t attrIndex = 0; attrIndex < attrCount; ++attrIndex) {
      DOMNode* currentNode = attrs->item(attrIndex);
      parseNode(currentNode, std::string(indent + "  "));
    }
  }


  //---------------------------------------------------------------------------
  // Get and parse list of child nodes

  if (parseChildren) {
    DOMNodeList* children = node->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for (XMLSize_t childIndex = 0; childIndex < nodeCount; ++childIndex) {
      DOMNode* currentNode = children->item(childIndex);
      parseNode(currentNode, std::string(indent + "  "));
    } // end for (child nodes)
  }

} // end parseNode

//-----------------------------------------------------------------------------

/**
 * Find the given option node or attribute in the DOM tree and return its value.
 * Note that the option name can specify either a node or an attribute.<br>
 * A node's value is specified in its text node.<br>
 * An attribute is already paired with its value.<br>
 * This lends flexibility to the format of the XML configuration file.
 * @param option
 *   The name of the option whose value to return.
 * @return
 *   The value of the option specified.
 */
std::string GetConfig::getOption(std::string option)
{
  std::string ret = "";

  // TODO: Find option node or attribute in tree and return its value.

  return ret;
}  // end getOption

//-----------------------------------------------------------------------------

#ifdef MAIN_TEST
/* This main is provided for unit test of the class. */

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: parser <config_file.xml>\n";
    return 1;
  }

  string configFile(argv[1]); //="sample.xml"; // stat file. Get ambigious segfault otherwise.

  GetConfig appConfig;

  try {

    cout << endl;
    appConfig.readConfigFile(configFile);
    cout << endl;

  } catch (std::exception &e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
#endif // MAIN_TEST

//-----------------------------------------------------------------------------
