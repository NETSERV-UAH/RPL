# RPL

Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);

                     (1) GIST, University of Alcala, Spain.
                     
                     (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
                     
                     (3) UPC, Castelldefels, Spain.

                     
Developed in OMNet++5.2.1, based on INET framework.

LAST UPDATE OF THE INET FRAMEWORK: inet3.6.3 @ December 22, 2017

In the simulation, the MAC sub layer, and all of the physical layer are based on the implementation of IEEE802.15.4 over the INET framework.


Markup :  #### Compile and run ####

To compile and run the project, you can follow the next steps.

Markup : 1. After extracting and before installing OMNeT++, exchange "PREFER_CLANG=yes" to "PREFER_CLANG=no" in the "configure.user" file in your OMNeT++ installation folder.

2. Install OMNeT++.

3. To check if your OMNeT++ correctly work, run an example of OMNeT++ such as dyna, aloha, tictoc, or etc.

4. Install and build INET.

5. To check if your INET correctly work, run an example of INET such as inet/examples/adhoc/ieee80211, or etc.

6. Since our files only include implementation code, the project may not be probably imported by IDE, and you must manually create a new OMNeT++ project by the same name. So, do the next substeps.

  1. Click on the menu of "File".

  2. Select "New".

  3. Select "OMNeT++ Project".

  4. Type the name of project.

  5. Click on the "Next".

  6. Select "Empty Project".

  7. click on the "Finish".

7. Copy all the files/folders in this folder to the project folder you created.

8. You must introduce INET to your project as a reference/library, so you must do the next substeps.

  1. Right-click on the project in "Project Explorer" window.

  2. Select "Properties".

  3. Select "Project Reference" in the left list.

  4. Select "INET" in the right list.

  5. Click on the "Apply and close".

9. Build and run the project.


