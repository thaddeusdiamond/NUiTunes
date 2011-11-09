NUiTunes Kinect Hack
====================
The software contained herein is a Kinect hack that allows the user to plug in their
Kinect to the computer and control iTunes using natural motion and natural language.
This is not for commercial use, and was implemented in a short period of time as a
simple proof of concept.


License
=======
This software is licensed under the MIT License (see LICENSE)


Credit
======
Credit is due to Kartik Venkatramann, Christopher Musco, and Cameron Musco for help
designing the algorithms that were implemented in the system.  Thanks to KinecTunes
for inspiration in the voice processing.  Also, thanks and credit to MaximizedSoftware
(http://www.maximized.com/download) for their VB scripts to control iTunes. 


Prerequisites
=============
  Hardware
  --------
    - Kinect for XBOX with power supply and USB cable
    - Computer with specs required by Windows 7 or later

  Software
  --------
    - Microsoft Kinect Research SDK
    - Microsoft Kinect English Language Pack
    - Speech Platform Runtime (v10.2) x86
    - Speech Platform SDK (v10.2)


Installation/Running
====================
  - Ensure that the prerequisites have been downloaded and installed
  - Plug in the Kinect for XBOX into the USB port in your computer
  - Run the batch file "NUiTunes.bat"
  - Minimize both CMD prompts and open iTunes
  - Begin using NUiTunes!


Supported Gestures
==================
  - Next song       => Swipe from the right side of your body to the left with either arm
  - Previous song   => Swipe from the left side of your body to the right with either arm
  - Volume up       => Hold your arm above your head and away from your body for >1 sec
  - Volume down     => Hold your arm below your waist and away from your body for >1 sec
  - Mute/Unmute     => Put a finger to your lips for 1-2 seconds


Supported Voice Commands
========================
  - "iTunes play"
  - "iTunes pause"
  - "iTunes stop"
  - "iTunes next"
  - "iTunes previous"
  - "iTunes volume up"
  - "iTunes volume down"
  - "iTunes mute"
  - "iTunes play ..." (attempts to find a matching playlist, song, artist or album)
  - "iTunes play playlist ..."
  - "iTunes play album ..."
  - "iTunes play artist ..."
  - "iTunes play song ..."