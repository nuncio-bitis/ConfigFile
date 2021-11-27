/*
 * tConfigFile.cpp
 *
 *  Created on: Oct 27, 2020
 *      Author: jparziale
 */
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

#include "ConfigFile.h"

int main(int argc, char* argv[])
{
    std::string configFile;
    if (argc < 2) {
        configFile = "sample.xml";
    } else {
        configFile = argv[1];
    }

    std::string buf;

    try {
        // Create a config file parser object, giving it the name of the config file.
        ConfigFile appConfig;
        appConfig.LoadFile(configFile);

        // Print out the config file info in human-readable format
        std::cout << "---- Configuration file data ----" << std::endl;
        appConfig.printConfigFile();

        // Print out selected options.
        // Only some will be available (non-blank) depending on the intput file.

        if (appConfig.exists("ApplicationSettings")) {
            std::cout << std::endl;
            std::cout << "---- Application Settings (sample.xml) ----" << std::endl;
            std::cout << "option A = '"
                    << appConfig.getOption("ApplicationSettings.option_a") << "'" << std::endl;
            std::cout << "option B = '"
                    << appConfig.getOption("ApplicationSettings.option_b") << "'" << std::endl;
            std::cout << "option C = '"
                    << appConfig.getOption("ApplicationSettings.option_c") << "'" << std::endl;
            if (!appConfig.exists("ApplicationSettings.option_c")) {
                std::cout << "*** option_c DOES NOT EXIST" << std::endl;
            }
            std::cout << "option D = '"
                    << appConfig.getOption("ApplicationSettings.option_d") << "'" << std::endl;
            std::cout << "option Y = '"
                    << appConfig.getOption("option_y") << "'" << std::endl;

            std::cout << std::endl;
            std::cout << "Get OtherStuff.option_x..." << std::endl;
            buf = appConfig.getOption("OtherStuff.option_x");
            std::cout << "option X = '" << buf << "'" << std::endl;

            // Change option x
            if (buf.length() > 0) {
                // Option has been set. Increment its value.
                std::stringstream ss(buf);
                int x;
                ss >> x;
                ss.clear();
                x++;
                ss << x;
                appConfig.setOption("OtherStuff.option_x", ss.str());
                std::cout << "option X changed to '"
                        << appConfig.getOption(std::string("OtherStuff.option_x")) << "'" << std::endl;
            } else {
                // Option not set - set it if it exists.
                std::cout << "Option X not set" << std::endl;
                if (appConfig.exists("OtherStuff.option_x")) {
                    std::cout << "Option X EXISTS" << std::endl;
                    std::cout << "Setting option X..." << std::endl;
                    appConfig.setOption("OtherStuff.option_x", "123");
                    std::cout << "option X set to '"
                            << appConfig.getOption("OtherStuff.option_x") << "'" << std::endl;
                } else {
                    std::cout << "Option X doesn't exist.  Not modifying it." << std::endl;
                }
            }
        }

        if (appConfig.exists("LPC2106_HW")) {
            std::cout << std::endl;
            std::cout << "---- LPC2106 Application Settings (ArmDevConfigs.xml) ----" << std::endl;
            std::cout << "program = '" << appConfig.getOption("LPC2106_HW.programming.program") << "'" << std::endl;
            std::cout << "flash_file_extension = '"
                    << appConfig.getOption("LPC2106_HW.programming.flash_file_extension")
                    << "'" << std::endl;
            std::cout << "port = '"
                    << appConfig.getOption("LPC2106_HW.FreeRTOS-LPC2106_APP.port")
                    << "'" << std::endl;
            std::cout << "symtable = '"
                    << appConfig.getOption("LPC2106_HW.FreeRTOS-LPC2106_APP.symtable")
                    << "'" << std::endl;
        }

        if (appConfig.exists("LPC2148_HW")) {
            if (appConfig.exists("LPC2148_HW.LPC2148_APP.debug_level")) {
                std::cout << std::endl;
                std::cout << "*** LPC2148_HW.LPC2148_APP.debug_level EXISTS" << std::endl;
            }

            if (!appConfig.exists("LPC2148_HW.LPC2148_APP.debug")) {
                std::cout << "*** LPC2148_HW.LPC2148_APP.debug DOES NOT EXIST" << std::endl;
            }

            std::cout << std::endl;
            std::cout << "---- LPC2148 Application Settings (ArmDevConfigs.xml) ----" << std::endl;
            std::cout << "program = '"
                    << appConfig.getOption("LPC2148_HW.programming.program") << "'" << std::endl;
            std::cout << "flash_file_extension = '"
                    << appConfig.getOption("LPC2148_HW.programming.flash_file_extension") << "'" << std::endl;
            std::cout << "port = '"
                    << appConfig.getOption("LPC2148_HW.FreeRTOS-LPC2148_APP.port") << "'" << std::endl;
            std::cout << "symtable = '"
                    << appConfig.getOption("LPC2148_HW.FreeRTOS-LPC2148_APP.symtable") << "'" << std::endl;
        }

    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
