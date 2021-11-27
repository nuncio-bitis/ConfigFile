/*******************************************************************************
 * ConfigFile.h
 *
 *  Created on: Dec 4, 2008
 *      Author: J. Parziale
 *******************************************************************************
 * HISTORY:
 * 00 12/04/2008
 *    Original version
 * 01 12/07/2010
 *    Copied to new project to use Xerces instead of TinyXML
 * 02 10/21/2020
 *    Porting over to TinyXML2
 ******************************************************************************/

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

//******************************************************************************

#include <string>
#include <tinyxml2.h>

//******************************************************************************

/**
 * This class is used to load configuration variables from an XML configuration file.<br>
 * To get an option, you would specify a "path" into the XML tree, which consists
 * of XML tree element names separated by "." and the last element name is the option
 * you're looking for.<br>
 * This class also provides a method for setting an option in the XML data.
 * The XML data is not written back to the file until the ConfigFile object is destroyed,
 * and then only if any data has changed.
 */
class ConfigFile {
    //-------------------------------------------------------------------------
public:

    ConfigFile();
    ~ConfigFile();

    int LoadFile(const std::string filename);

    /**
     * Return true if the specified element path exists.
     * @param elementPath
     *   Full "path" of the element in the XML tree to get info for.
     * @return
     *   True if element exists, False otherwise.
     */
    bool exists(std::string elementPath);

    /**
     * Find the given option node XML tree and return its value.
     * A node's value is specified in its text node.<br>
     * @param option
     *   The path of the option whose value to return.
     * @return
     *   The value of the option specified. "N/A" if not found.
     */
    std::string getOption(std::string option);

    /**
     * Find the given option node XML tree, and get the value of a specified attribute.
     * @param option
     *   The path of the option that owns the attribute.
     * @param attribute
     *   The name of the attribute whose value to return.
     * @return
     *   The value of the option's attribute. "N/A" if not found.
     */
    std::string getAttribute(std::string option, std::string attribute);

    /**
     * Find the given option node in the XML tree and set its value.
     * @param option
     *   The path of the option whose value to set.
     * @param value
     *   The value to set the option to.
     * @return
     *   True on success, False on failure.
     */
    bool setOption(std::string option, std::string value);

    /**
     * Find the given option node in the XML tree and set the attribute's value.
     * @param option
     *   The path of the option that owns the attribute.
     * @param value
     *   The value to set the attribute to.
     * @return
     *   True on success, False on failure.
     */
    bool setAttribute(std::string option, std::string attribute, std::string value);

    /**
     * Parse and print the configuration file in a human-readable format.
     */
    void printConfigFile(void);

    static const std::string cUNAVAILABLE;

    //-------------------------------------------------------------------------
private:

    // Private data

    /**
     * Name of the input XML file.
     * Used for saving the (possibly modified) contents of the XML Document.
     */
    std::string xmlFilename;

    /**
     * XML document.
     */
    tinyxml2::XMLDocument xmlDoc;

    /**
     * Flag indicating an option in the XML doc has been modified.
     * This affects the class destructor, which would write the XML doc
     * to a file if the data has changed.
     */
    bool xmlDocChanged;

    /**
     * Error code returned from TinyXML2 calls.
     */
    int xmlErrorId;

    // Document root element
    tinyxml2::XMLElement *m_DocumentRoot;

    //-------------------------------------------------------------------------
    // Private methods

    /**
     * Recursively parse the tree under the given node to find the specified option.
     * @param node
     *   The tree node to parse.
     * @param optionName
     *   Option to search for under the node.
     * @param parentNs
     *   Parent's namespace (in the form one.two.three)
     * @return
     *   Returns the XMLNode of the option specified by optionName, or null if not found.
     */
    tinyxml2::XMLNode *findNode(tinyxml2::XMLNode *node, std::string optionName = "", std::string parentNs = "");


    /**
     * Recursively print the DOM tree info under the given node.
     * @param node
     *   The tree node to parse.
     * @param indent
     *   Level of indentation used for debug output.
     */
    void printNodeTree(tinyxml2::XMLNode *node, std::string indent = "  ");


    /**
     * Write back to the XML file.
     * @param myDocument
     *   The document object
     * @param filePath
     *   File name to save XML data to.
     * @return True on success, False on failure.
     */
    bool outputXml(tinyxml2::XMLDocument &myDocument, std::string filePath);

    //-------------------------------------------------------------------------
};
// end ConfigFile

//******************************************************************************
#endif /* CONFIGFILE_H_ */
