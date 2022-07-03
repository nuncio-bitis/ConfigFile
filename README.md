# ConfigFile
A class used to parse XML configuration files.  
This relies on tinyxml2.

General configuration file format:

```xml
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<root>
  <Application1>
    <option key="option_a">10</option>
    <option key="option_b">24</option>
    <option key="option_c">42</option>
  </Application1>
  <Application2>
    <option key="option_x">One</option>
    <option key="option_y">Two</option>
    <option key="option_z">Three</option>
  </Application2>
</root>
```
---------------------------------------------------------------------

## HISTORY

**December 07, 2010**

Original version, written using Xerces-C.

---------------------------------------------------------------------

**December 16, 2010**

Completed ConfigFile class and sample code.

---------------------------------------------------------------------

**October 9, 2020**

Converted project to use TinyXML2.

---------------------------------------------------------------------

**November 27, 2021**

Converted project to a CMake project, posted to GitHub.

---------------------------------------------------------------------
