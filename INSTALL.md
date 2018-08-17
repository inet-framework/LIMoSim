## Using with OMNeT++ and INET framework:

##### Step 1: Install OMNeT++ with INET framework

##### Step 2: Clone the LIMoSim repo
- cd <your_omnetpp_workspace>
- git clone https://github.com/BenSliwa/LIMoSim

##### Step 3: Open LIMoSim project in OMNeT++ IDE
- click on menu: 'File' -> 'Open Projects from File System...'
- click 'Directory...' button and select the root of LIMoSim repo
- click to 'Finish'

#### Running Example
  example found in LIMoSim/omnet/example folder



## UI Part

#### Qt 5.7
- Download and install Qt from https://www.qt.io/download/ (at least Qt 5.7 is required)
- Open the Qt-project file LIMoSim/ui/LIMoSim.pro
- Make sure that the build folder is outside the inet workspace. Qt generates .cpp files during the build process, which would interfer with the .cc files of INET

#### Running an Example
- Currently, the path the map directory needs to be specified in the main.cpp file
- Change the **dir** variable to the absolute path of your LIMoSim/resources/ location
- The current version does not include a UI-based tool for generating vehicle configuration files yet but the cars will print their current configuration as XML-data to the command line if they are clicked in the UI
