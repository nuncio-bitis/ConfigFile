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


---

# I'm Using GitHub Under Protest

This project is currently hosted on GitHub.  This is not ideal; GitHub is a proprietary,  
trade-secret system that is not Free and Open Souce Software (FOSS).  
I am deeply concerned about using a proprietary system like GitHub to develop my FOSS project.  
I urge you to read about the
[Give up GitHub](https://GiveUpGitHub.org) campaign from
[the Software Freedom Conservancy](https://sfconservancy.org)  
to understand some of the reasons why GitHub is not a good place to host FOSS projects.

Any use of this project's code by GitHub Copilot, past or present, is done without my permission.  
I do not consent to GitHub's use of this project's code in Copilot.

I plan to move this project to another hosting site (TBD) and will leave a link to it here in this README file.

---

![Logo of the GiveUpGitHub campaign](https://sfconservancy.org/img/GiveUpGitHub.png)
