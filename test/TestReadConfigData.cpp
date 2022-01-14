/*
 * This file is part of the DataGatheringSystem distribution
 *   (https://github.com/nuncio-bitis/test
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
/*
 * TestReadConfigData.c
 *
 *  Created on: Dec 8, 2008
 *      Author: parziaj
 */

#include <iostream>
#include <string>

#include "ConfigFile.h"

using namespace std;

//------------------------------------------------------------------------------

/**
 * Name of the configuration file to read.
 */
#define CONFIG_FILE_NAME "../data/ArmDevConfigs.xml"
#define xmlFileTopNode "../data/TestReadConfigData"

//------------------------------------------------------------------------------

// Application application_variables to be read from config file

// The hardware configuration to choose
#define SELECTED_TARGET_HW "LPC2148_HW"
// The application to choose for the chosen target hardware
#define SELECTED_APPLICATION "FreeRTOS-LPC2148_APP"

// Programming options
#define PROGRAMMING_PROG (SELECTED_TARGET_HW ".programming.program")
#define PROGRAMMING_OPTS (SELECTED_TARGET_HW ".programming.options")
#define PROGRAMMING_FEXT (SELECTED_TARGET_HW ".programming.flash_file_extension")
#define PROGRAMMING_PORT (SELECTED_TARGET_HW ".programming.port")
#define PROGRAMMING_BAUD (SELECTED_TARGET_HW ".programming.baud")
#define PROGRAMMING_XTAL (SELECTED_TARGET_HW ".programming.xtal")

// These definitions are the names of the application parameters to read from the config file.
#define APPLICATION_PARAM_PORT       (SELECTED_TARGET_HW "." SELECTED_APPLICATION ".port")
#define APPLICATION_PARAM_BAUD       (SELECTED_TARGET_HW "." SELECTED_APPLICATION ".baud")
#define APPLICATION_PARAM_SYMTABLE   (SELECTED_TARGET_HW "." SELECTED_APPLICATION ".symtable")
#define APPLICATION_PARAM_DEBUGLEVEL (SELECTED_TARGET_HW "." SELECTED_APPLICATION ".debug_level")

// These are the application_variables that hold the application parameters
string port;
string baud;
string symtable;
string debug_level;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  //----------------------------------------------------------------------------

  string config_file_name = CONFIG_FILE_NAME;

  // Read configuration data
/*
  cout << endl
  << "Reading application and programming settings for target hardware '" << SELECTED_TARGET_HW << "'" << endl
       << " and target application '" << SELECTED_APPLICATION << "'" << endl
       << " from file: '" << config_file_name << "'" << endl;
*/

  try {
    // Config file handler object.
    ConfigFile config_file;
    config_file.LoadFile(config_file_name);

    // Read configuration data
    cout << endl
    << "Reading application and programming settings from file: '" << config_file_name << "'" << endl;

    // Print out the config file info in human-readable format
    cout << endl;
    cout << "---- Configuration file data ----" << endl;
    config_file.printConfigFile();
    cout << "---------------------------------" << endl;

    //--------------------------------------------------------------------------

    cout << endl;
    cout << "---- Existence checks ----" << endl;

    cout << "Target hardware configuration LPC2138_HW:" << endl << "    ";
    if (! config_file.exists("LPC2138_HW")) {
      cout << "NOT FOUND => OK" << endl;
    } else {
      cout << "FOUND     => ERROR" << endl;
    }

    cout << "Setting LPC2148_HW.LPC2148_APP.debug_level:" << endl << "    ";
    if (config_file.exists("LPC2148_HW.LPC2148_APP.debug_level")) {
      cout << "FOUND     => OK" << endl;
    } else {
      cout << "NOT FOUND => ERROR" << endl;
    }

    cout << "Setting LPC2148_HW.LPC2148_APP.debug:" << endl << "    ";
    if (! config_file.exists("LPC2148_HW.LPC2148_APP.debug")) {
      cout << "NOT FOUND => OK" << endl;
    } else {
      cout << "FOUND     => ERROR" << endl;
    }

    //--------------------------------------------------------------------------

    cout << endl;
    if (config_file.exists("LPC2106_HW")) {
      cout << "---- LPC2106 Application Settings ----" << endl;
      cout << "program = '" << config_file.getOption(std::string("LPC2106_HW.programming.program")) << "'" << endl;
      cout << "flash_file_extension = '" << config_file.getOption(std::string("LPC2106_HW.programming.flash_file_extension")) << "'" << endl;
      cout << "port = '" << config_file.getOption(std::string("LPC2106_HW.FreeRTOS-LPC2106_APP.port")) << "'" << endl;
      cout << "symtable = '" << config_file.getOption(std::string("LPC2106_HW.FreeRTOS-LPC2106_APP.symtable")) << "'" << endl;
    } else {
      cout << "ERROR: Target hardware settings for LPC2106_HW NOT FOUND" << endl;
    }

    //--------------------------------------------------------------------------

    cout << endl;
    if (config_file.exists("LPC2148_HW")) {

      cout << "---- LPC2148 Application Settings ----" << endl;
      cout << "program = '" << config_file.getOption(std::string("LPC2148_HW.programming.program")) << "'" << endl;
      cout << "flash_file_extension = '" << config_file.getOption(std::string("LPC2148_HW.programming.flash_file_extension")) << "'" << endl;
      cout << "port = '" << config_file.getOption(std::string("LPC2148_HW.FreeRTOS-LPC2148_APP.port")) << "'" << endl;
      cout << "symtable = '" << config_file.getOption(std::string("LPC2148_HW.FreeRTOS-LPC2148_APP.symtable")) << "'" << endl;
    } else {
      cout << "ERROR: Target hardware settings for LPC2148_HW NOT FOUND" << endl;
    }

    //--------------------------------------------------------------------------

    cout << endl << endl;
    if (config_file.exists(SELECTED_TARGET_HW)) {

      cout << "---- " << SELECTED_TARGET_HW << " Programming Settings ----" << endl;
      cout << "program = '" << config_file.getOption(std::string(PROGRAMMING_PROG)) << "'" << endl;
      cout << "program options = '" << config_file.getOption(std::string(PROGRAMMING_OPTS)) << "'" << endl;
      cout << "flash_file_extension = '" << config_file.getOption(std::string(PROGRAMMING_FEXT)) << "'" << endl;
      cout << "programming port = '" << config_file.getOption(std::string(PROGRAMMING_PORT)) << "'" << endl;
      cout << "programming baud = '" << config_file.getOption(std::string(PROGRAMMING_BAUD)) << "'" << endl;
      cout << "programming xtal freq = '" << config_file.getOption(std::string(PROGRAMMING_XTAL)) << "'" << endl;

      cout << endl;
      cout << "---- " << SELECTED_APPLICATION << " Application Settings ----" << endl;
      cout << "port = '" << config_file.getOption(std::string(APPLICATION_PARAM_PORT)) << "'" << endl;
      cout << "baud = '" << config_file.getOption(std::string(APPLICATION_PARAM_BAUD)) << "'" << endl;
      cout << "symtable = '" << config_file.getOption(std::string(APPLICATION_PARAM_SYMTABLE)) << "'" << endl;
      cout << "debug_level = '" << config_file.getOption(std::string(APPLICATION_PARAM_DEBUGLEVEL)) << "'" << endl;
    } else {
      cout << "ERROR: Target hardware settings for LPC2148_HW NOT FOUND" << endl;
    }

    //--------------------------------------------------------------------------
  } catch (std::exception &e) {
    cerr << "EXCEPTION: " << e.what() << endl;
    return -1;
  }

  cout << endl;

  //----------------------------------------------------------------------------

  return 0;
}

//------------------------------------------------------------------------------
