# ECSE Leavers Dinner Invitations 2025

This Repo contains all resources for the Invitations for the 2025 University of Auckland Department of Electrical, Computer, and Software Engineering's Part IV Leavers' dinner.
The Invitations are rp2040 based development boards in a 'credit card' form factor, with USB-C Power/data, a 5x5 LED matrix, two user buttons, and 20 exposed GPIO (16 Digital pins + 4 Digital/ADC). 

## What's running on it?
Whatever you want! When you recieved the board, it will scroll your name across the LED matrix (try pressing the buttons to find other behaviour). The source code for this can be found under [/code/ECSE_LD_Invite_2025](/code/ECSE_LD_Invite_2025). However, I encourage you to hack away to your heart's content - The rp2040 is a very capable microcontroller, and you can write code for it in C, or micropython if that's more your thing. Just fork this repository if you do, so we can see everyone's creations in one place.

### How do I change my name?
Great question! - to modify the name text stored on the board, simply plug it in to any computer, identify the Port in use (usually /dev/ttyACM0 or /dev/ttyUSB0 on linux or COMxxx on windows), and connect (use 115200 baud, 8N1 serial, with 'LF' line endings if possible). Then send your new string and it should appear on the display shortly.

### What about changing the code?
If you want to write code for this, Raspberry Pi has a great guide on getting started with the pi pico [here](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf), which should be applicable. A custom board file is in development, but for now just select Pi Pico.
Once you're set up, either create a new project or open  [/code/ECSE_LD_Invite_2025](/code/ECSE_LD_Invite_2025) to pick up where we've left off.

## Can I make more? 
Absolutely! These were ordered from JLCPCB, and the altium project files, exported gerber files, and BOM/Centroid files for PCBA are under  [/LeaversDinner_invite](/LeaversDinner_invite). 


## Who Made this?
The PCB was designed by [Campbell Wright (Me)](https://github.com/campbelllwright) and [James West](https://github.com/Jwaemsets), with some graphics courtesy of [@Excigma](https://github.com/Excigma).
The code was developed by [Campbell Wright (Me)](https://github.com/campbelllwright).
