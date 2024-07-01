# Jan Reinberg Final Project


 Max MSP external object 'tosc~' made with max-sdk
 
 by Jan Reinberg
 
 This is a table wave oscillator external object made with the max-sdk from Cyclin' 74.  
 It features an msp object with two inlets and a single outlet.  
 
 Inlet one is for frequency.
 Inlet two is for the amount of harmonics.
 Both can be float or int values (the script will adjust accordingly).
 
 Frequency and harmonics can be inputed manually for set parameters, or adjusted dynamically via external data objects into the inlets.
 
 What you will need for this build:
 1. The tosc~ folder with C code and cmakelist provided in this repository:
 2. The max-sdk from the Cyclin' 74 website.  You will need version 8.2.
    Here is repository link: https://github.com/Cycling74/max-sdk.git
    
 3. Xcode IDE
 4. Max 8 to test and user the object.
 5. cmake to create this build.

    
 Instructions for this build are very similar to those of the general max-sdk for MacOS.
 You will need to use the following terminal commands for the initial portion of the process:
 
 1. First, you will need to clone the repository to your system:
    git clone https://github.com/Cycling74/max-sdk.git
 2. Then:
    cd max-sdk
 3. Then:
    git module update --init --recursive
 4. Go into the finder window of the folder or cd to the directory to get to the audio folder:
    max-sdk -> source -> audio
 5. Copy and paste and move the tosc~ folder with the .c code into this folder alongside the other msp external object folders
 6. Return to the terminal and create a build folder:
    mkdir build
 7. Make the build folder your directory:
    cd build
 8. Next you will create an Xcode project within this build folder:
    cmake -G Xcode ..
 9. Then go back into the finder window of the build folder, you should see a max-sdk.xcodeproj.
 10. Open the project, and then in the lower right of Xcode, using the filter, search for tosc~.  The project should come up if you wish to make any adjustments to the code.
 11. Otherwise, lastly you click build settings transport window in at the top, search for tosc_tilde and make sure select "My Mac" for build platform.
 12. Press CMD + B, or press the "play" button to begin the build.  
 13. After the build is successful, you will go into the max-sdk folder and search for the externals folder that appeared.
 14. There should be a tosc~.mxo file, this is the file you will need to put in your external folder of your Max application source folder.
 15. The path to the proper folder in Max should: 
    Max (right-click application) -> Contents -> Resources -> C74 -> externals -> msp
 16. Move or copy and paste the tosc~.mxo file into this 'msp' folder
 17. Now when you go into Max, and search for the object, it should appear.
 
 **Warning**
 
 This oscillator on its own can have harsh frequency content depending on use.  I have reduced the initial amplitude for safety, but you can adjust this with a gain~ object, if needed.
 
 HAVE FUN WITH YOUR NEW MAX OBJECT.
    
 
 
 
 
 
