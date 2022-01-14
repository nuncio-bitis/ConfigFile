/*
 * This file is part of the DataGatheringSystem distribution
 *   (https://github.com/nuncio-bitis/source
 * Copyright (c) 2022 James P. Parziale.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*******************************************************************************
 * ConfigFile.cpp
 *
 * This class implements the control of data to and from a configuration file.
 * The configuration file is an XML file.
 * The variable-value pairs are implemented using a map.
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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

#include "common.h"
#include "ConfigFile.h"
#include "ConfigFileInfo.h"

const std::string ConfigFile::cUNAVAILABLE("N/A");

//******************************************************************************

ConfigFile::ConfigFile()
    : xmlDocChanged(false), xmlErrorId(0), m_DocumentRoot(NULL)
{
    std::cout << "ConfigFile::ConfigFile()" << " Version " << ConfigFile_VERSION_MAJOR << "."
              << ConfigFile_VERSION_MINOR << std::endl << std::endl;

}

//******************************************************************************

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It also terminates use of the xerces-C
 *  framework.
 */
ConfigFile::~ConfigFile()
{
  // If data has changed, save the XML document to a file (based on the input filename).
  if (xmlDocChanged) {
    char *baseFilename = basename((char *)xmlFilename.c_str());
    xmlFilename = baseFilename;

    std::cout << "Writing XML Doc _" << xmlFilename << " ..." << std::endl;
    std::string tempName("_" + xmlFilename);
    if (outputXml(xmlDoc, tempName)) {
        std::cout << "XML Doc written successfully." << std::endl;
    } else {
        std::cerr << "ERROR: Could not write XML file." << std::endl;
    }
  }
  std::cerr << std::endl;
}

//******************************************************************************

int ConfigFile::LoadFile(const std::string filename)
{
    xmlFilename = filename;

    // Check if file is good. tinyxml2 crashes if the file doesn't exist.
    std::ifstream f(xmlFilename.c_str(), std::ios::in);
    if (!f.good()) {
        xmlErrorId = tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED;
        std::cerr << "ERROR: Could not open file '" << xmlFilename << "'" << std::endl;
        return EXIT_FAILURE;
    }

    // Load document information
    xmlDoc.LoadFile(xmlFilename.c_str());
    xmlErrorId = xmlDoc.ErrorID();
    if (xmlErrorId != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "ERROR: Could not load XML file '" << xmlFilename << "'" << std::endl;
        return EXIT_FAILURE;
    }

    m_DocumentRoot = xmlDoc.RootElement();
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: XML file has no document root node." << std::endl;
        return EXIT_FAILURE;
    }

    std::string rootNodeName = m_DocumentRoot->Name();
    std::cout << "The configuration file '" << xmlFilename << "' has been loaded." << std::endl;
    std::cout << "  Root node: '" << rootNodeName << "'" << std::endl;
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

//******************************************************************************

/**
 * Parse and print the configuration file in a human-readable format.
 */
void ConfigFile::printConfigFile(void)
{
    std::cout << std::endl;
    //xmlDoc.Print(); // Print "formatted" raw XML
    //std::cout << std::endl;

    if (m_DocumentRoot) {
        printNodeTree(m_DocumentRoot, "");
    } else {
        std::cerr << "ERROR: Empty XML document." << std::endl;
    }
}


//******************************************************************************

/**
 * Return true if the specified element path exists.
 * @param elementPath
 *   Full "path" of the element in the XML tree to get info for.
 * @return
 *   True if element exists, False otherwise.
 */
bool ConfigFile::exists(std::string elementPath)
{
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: Empty XML document." << std::endl;
        return false;
    }

    // Find the node for the given option.
    tinyxml2::XMLNode *node = findNode(m_DocumentRoot, elementPath);

    return (node != NULL);
}

//******************************************************************************

/**
 * Find the given option node XML tree and return its value.
 * A node's value is specified in its text node.<br>
 * @param elementPath
 *   The path of the option whose value to return.
 * @return
 *   The value of the option specified. "N/A" if not found.
 */
std::string ConfigFile::getOption(std::string elementPath)
{
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: Empty XML document." << std::endl;
        return cUNAVAILABLE;
    }

    tinyxml2::XMLNode *node = findNode(m_DocumentRoot, elementPath);
    if (node) {
        // Option found as a node. Its value is its text
        if (node->ToElement()) {
            if (node->ToElement()->GetText()) {
                return node->ToElement()->GetText();
            } else {
                // Node exists, but has no text. Return an empty string.
                return "";
            }
        }
    }
    return cUNAVAILABLE;
}

//******************************************************************************

/**
 * Find the given option node XML tree, and get the value of a specified attribute.
 * @param elementPath
 *   The path of the option that owns the attribute.
 * @param attribute
 *   The name of the option whose value to return.
 * @return
 *   The value of the option's attribute. "N/A" if not found.
 */
std::string ConfigFile::getAttribute(std::string elementPath, std::string attribute)
{
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: Empty XML document." << std::endl;
        return cUNAVAILABLE;
    }

    // Get node to option, then the attribute
    tinyxml2::XMLNode *node = findNode(m_DocumentRoot, elementPath);
    if (node) {
        // Option found as a node. Search its attributes.
        if (node->ToElement()) {
            const tinyxml2::XMLAttribute *pAttrib = node->ToElement()->FindAttribute(attribute.c_str());
            if (pAttrib) {
                return pAttrib->Value();
            }
        }
    }
    return cUNAVAILABLE;
}

//******************************************************************************

/**
 * Find the given option node or attribute in the DOM tree and set its value.
 * @param elementPath
 *   The name of the option whose value to set.
 * @param value
 *   The value to set the option to.
 * @return
 *   True on success, False on failure.
 */
bool ConfigFile::setOption(std::string elementPath, std::string value)
{
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: Empty XML document." << std::endl;
        return false;
    }

    bool ret = false;

    // Find the node for the given option.
    tinyxml2::XMLNode *node = findNode(m_DocumentRoot, elementPath);
    if (node) {
        // Option found as a node. Its value is its text
        if (node->ToElement()) {
            node->ToElement()->SetText(value.c_str());
            // Update the XML doc changed flag.
            xmlDocChanged = true;
            ret = true;
        }
    }

    return ret;
}

//******************************************************************************

/**
 * Find the given option node in the XML tree and set the attribute's value.
 * @param elementPath
 *   The path of the option that owns the attribute.
 * @param value
 *   The value to set the attribute to.
 * @return
 *   True on success, False on failure.
 */
bool ConfigFile::setAttribute(std::string elementPath, std::string attribute, std::string value)
{
    if (!m_DocumentRoot) {
        std::cerr << "ERROR: Empty XML document." << std::endl;
        return false;
    }

    bool ret = false;

    // Find the node for the given option.
    tinyxml2::XMLNode *node = findNode(m_DocumentRoot, elementPath);
    if (node) {
        // Option found as a node. Search its attributes.
        if (node->ToElement()) {
            tinyxml2::XMLAttribute *pAttrib = (tinyxml2::XMLAttribute *)node->ToElement()->FindAttribute(attribute.c_str());
            if (pAttrib) {
                pAttrib->SetAttribute(value.c_str());
                xmlDocChanged = true;
                ret = true;
            }
        }
    }

    return ret;
}

//******************************************************************************
// PRIVATE METHODS
//******************************************************************************


/**
 * Recursively print the DOM tree info under the given node.
 * @param node
 *   The tree node to parse.
 * @param indent
 *   Level of indentation used for debug output.
 */
void ConfigFile::printNodeTree(tinyxml2::XMLNode *node, std::string indent)
{
    tinyxml2::XMLElement *nodeElement = node->ToElement();

    // Print node info (name, text)
    std::cout << indent << nodeElement->Name() << ":";

    if (nodeElement->GetText()) {
        std::string text = nodeElement->GetText();
        text = trim(text);
        std::cout << " \"" << text << "\"";
    }
    // Value is the same as Name
    //if (nodeElement->Value()) {
    //    std::cout << ", Value: " << nodeElement->Value();
    //}
    std::cout << std::endl;

    // Print node attributes (name, value)
    const tinyxml2::XMLAttribute * attrib = nodeElement->FirstAttribute();
    while (attrib != NULL) {
        std::cout << indent << "  " << attrib->Name();
        if (attrib->Value()) {
            std::cout << " = " << attrib->Value();
        }
        std::cout << std::endl;
        attrib = attrib->Next();
    }

    // Print info for children
    tinyxml2::XMLNode *child = node->FirstChild();
    while (child != NULL) {
        if (child->ToComment()) {
            // Comment
            std::string cmt = child->ToComment()->Value();
            cmt = ReplaceAll(cmt, "\n", "\n" + indent + "#");
            std::cout << indent << "# " << cmt << std::endl;
        } else if (child->ToElement()) {
            // Element
            printNodeTree(child, indent + "    ");
        }
        // Text was handled with the node name above.
        //else if (child->ToText()) {
        //    // Element Text
        //    std::string text = child->ToText()->Value();
        //    text = trim(text);
        //    std::cout << indent << " \"" << text << "\"" << std::endl;
        //}

        child = child->NextSibling();
    }
}

//******************************************************************************

/**
 * Recursively parse the DOM tree under the given node to find the specified option.
 * @param node
 *   The tree node to parse.
 * @param optionName
 *   Option to search for under the node.
 * @param parentNs
 *   Parent's namespace (in the form one.two.three)
 * @return
 *   Returns the DOMNode of the option specified by optionName, or null if not found.
 */
tinyxml2::XMLNode *ConfigFile::findNode(tinyxml2::XMLNode *node, std::string optionName, std::string parentNs)
{
    std::string nodeName;
    std::string remaining;

    std::size_t firstDot = optionName.find_first_of(".");
    if (firstDot == std::string::npos) {
        nodeName = optionName;
    } else {
        nodeName = optionName.substr(0, firstDot);
        remaining = optionName.substr(firstDot+1);
    }

//    std::cout << optionName << "; Looking for '" << nodeName << "' under " << node->Value() << ", remaining = " << remaining << std::endl; // XXX

    // Find in this node's direct children
    tinyxml2::XMLNode *child = node->FirstChild();
    while (child != NULL) {
        if (child->ToElement()) {
//            std::cout << "(child) " << child->Value() << "???" << std::endl; // XXX
            if (child->Value() == nodeName) {
//                std::cout << "FOUND " << nodeName << std::endl; // XXX
//                std::cout << "  remaining = '" << remaining << "', " << remaining.size() << std::endl; // XXX
                if (remaining.size() == 0) {
                    return child;
                } else {
                    return findNode(child, remaining);
                }
            }
        }
        child = child->NextSibling();
    }

//    // Recursively parse this node's direct children
//    child = node->FirstChild();
//    while (child != NULL) {
//        if (child->ToElement()) {
//            std::cout << "(child) " << child->ToElement()->Name() << "???" << std::endl; // XXX
//            // Element
//            if (child->ToElement()->Name() == nodeName) {
//                std::cout << "FOUND " << nodeName << std::endl; // XXX
//                return child->ToElement();
//            }
//            findNode(child, remaining);
//        }
//        child = child->NextSibling();
//    }

    return NULL;

#if 0 // TODO
    std::string ns = parentNs;

    if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
        // Found node which is an Element. Re-cast node as element
        tinyxml2::XMLElement * currentElement = node->ToElement();

        // Update namespace string
        // Don't use document root in namespace string.
        if (node != m_DocumentRoot) {
            if (ns.length() == 0) {
                ns = XMLString::transcode(currentElement->getTagName());
            } else {
                std::string name = XMLString::transcode(
                        currentElement->getTagName());
                if (name.length() > 0) {
                    ns = parentNs + "." + name.c_str();
                }
            }
        }

        // Check if this element is an option being searched for.
        if (ns.compare(optionName) == 0) {
            return node;
        }

        //-------------------------------------------------------------------------
    } else if (node->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
        DOMAttr* attr = dynamic_cast<xercesc::DOMAttr*>(node);

        // Update namespace string
        {
            std::string name = XMLString::transcode(attr->getName());
            if (name.length() > 0) {
                ns = parentNs + "." + name.c_str();
            }
        }

        // Check if this attribute is an option being searched for.
        if (ns.compare(optionName) == 0) {
            return node;
        }

        //-------------------------------------------------------------------------
    } // end if Node Type

    //---------------------------------------------------------------------------
    // Process node's attributes
    if (node->hasAttributes()) {
        DOMNamedNodeMap *attrs = node->getAttributes();
        const XMLSize_t attrCount = attrs->getLength();

        for (XMLSize_t attrIndex = 0; attrIndex < attrCount; ++attrIndex) {
            DOMNode* foundNode = findNode(attrs->item(attrIndex), optionName,
                    ns);
            if (foundNode != NULL) {
                return foundNode;
            }
        }
    }

    //---------------------------------------------------------------------------
    // Get and parse list of child nodes
    DOMNodeList* children = node->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for (XMLSize_t childIndex = 0; childIndex < nodeCount; ++childIndex) {
        DOMNode* foundNode = findNode(children->item(childIndex), optionName,
                ns);
        if (foundNode != NULL) {
            return foundNode;
        }
    } // end for (child nodes)
#endif
} // end findNode


//******************************************************************************

bool ConfigFile::outputXml(tinyxml2::XMLDocument &myDocument, std::string filePath)
{
    if (myDocument.SaveFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "ERROR: Couldn't write XML document" << std::endl;
        return false;
    }
    return true;
}

//******************************************************************************
