# MAME360
 is a multi-arcade emulator port for XBox360

MAME0.72 is a Native port to XBox 360 by Lantus and other developers

MAME (Multiple Arcade Machine Emulator), it is a PC emulator that faithfully 
recreates how arcade systems work.

- Arcadez
- wolf3s
- Gamezfan
- iq_132
- BritneysPAIRS
- Traace


Features
========
- Easy to use interface.
- Previews.
- Perfectly emulated graphics and sounds.
- 4 player support.
- Cheat menu support.
- Hiscore.dat support.
 

Usage
=====
You need to find roms that work with MAME 0.72
By default roms go to GAME:\roms
In the mame.ini file under [Directories] you can edit it and put up to 4 paths. Supported devices are:

Usb0:
Usb1:
Usb2:
Hdd1:
Memunit0:
Memunit1:
DVD:

The Cheat menu can be enabled under [General] (in the mame.ini).

To enable it, set CheatsEnabled = 1

To enable VSync. you need to edit the mame.ini as follows:

[Video]
VSYNC = 1

Controls
========
Controls can be configured easily via the MAME menu. The default settings are:

- Back + Start to return to the menu.
- Click with the right stick to enter Configuration Mode.
- Click with the left stick for display options.
