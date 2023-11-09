# toolset

List of libraries:
* [base](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/base) : Toolset base library (geometry,networking,inputs...)
* [opengl-utility](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/opengl-utility) : OpenGL utility library (Rendering)
* [3d-engine](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/3d-engine) : 3D engine library using OpenGL, SFML and ImGui
* [qt-utility](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/qt-utility) : utility library to be used in Qt environments (Logging, convertions, widgets...)
* [nodes](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/nodes) : nodes visualization library for Qt (forked from [nodeeditor](https://github.com/paceholder/nodeeditor))

List of applications:
* [demos](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/demos) : demos app using 3d-engine library
* Volumetric video streaming/recording pipeline from depth cameras:
  * Kinect2:
     * [scaner-component](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/scaner-component) : base components used in grabber and manager
     * [scaner-grabber](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/scaner-grabber) : client program for fetching Kinect2 data to be sent to UDP
     * [scaner-manager](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/scaner-manager) : server program for getting scaner-grabber data from UDP (settings and calibration)
     * [guardian](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/guardian)  : app for managing scaner-grabber execution 
  * Azure Kinect:
     * [k4-scaner-grabber](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/k4-scaner-grabber)
     * [k4-scaner-manager](https://github.com/FlorianLance/toolset/tree/main/cpp-projects/k4-scaner-manager)
