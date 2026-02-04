This is a program to control an Inverter and Battery charger
Is unfinished.
Microcontroller: ESP 32
LANGUAGE= C, IDF IDE, FREERTOS ESP32

In the directory exist a file called array_modifier.c and is to make
the array for the background as follows.

Use the program Image converter to dump a one line of all the screen in uint16_t

https://sourceforge.net/projects/lcd-image-converter/files/

Must find the buttom preview and see a scan screen. Modify to RGB 565 and uint16_t
in the menu of the screen. And then copy and paste to an empty file called
background.c 
(a description to use the image converter program can be found here:

https://www.instructables.com/Display-Your-Photo-on-OLED-Display/  )

compile the array_modifier.c 
just gcc array_modifier.c -o array_modifier

in the same folder put both array_modifier and background.c and run the array_modifier.

in linux sudo ./array_modifier

To prepare the image and scalling to the screen I used the GNU image manipulation program

https://www.gimp.org/

And to dump a bipmap for each character used in screen update i used the lvgl/lv_font_converter

https://github.com/lvgl/lv_font_conv?tab=readme-ov-file

Each bitmap needs Its own array.

Since are small and only I need numbers I just did fonts manually.

when you open the background.c again have plitted the data amd bracket it to do a 2 
dimensions array. Need to add the name and clear just a couple of symbols after the array

Since all ge gpio are completed in this design I adddd an IC2 port expander MCP2017 and I used for the column of keypad.

Espressif have a wifi patented and available in esp32 that is Long Range WiFi and can transmit and receive data to other Long Range WiFi for up to one Kilometer. So is useful to handle at 2Ghz information away if in the base you have other esp32 to receive the data. I will not use this now. But I will create an Android application to can see all the data of my microcontroller program, to update date and time, and to have buttons to can open a gate, turn on a pump or light, check which device is charging, and other things. I will use the 11 pins free of the MCP23017 to can setup time scheduler. All time will be saved into Non Volatile Storage so if get disconnected the Esp32 don't loose the time and configuration. I will delay some days reading to setup both WiFi. I am install the android Studio that also is free. I will use my pixel 2 to test it android 11. I also have a Sharp Aquos S6 android 13 to try after it. Then I will upload the android program to work in it into GitHub.

And the 11 mcp23017 free pins to an RF encoder and all the things that have to be turned on or of are connected with an RF decoder without any cable. Really can be connected more than this eleven and quantity can be huge (1024 codes available but have to be tested the ESP32 for how many action can be done). Each device to be controlled to stop start can have one or more code for example to open and close in two speeds will have 4 codes. Circuits and decoder are shown Im the files not exactly but a reference downloaded from internet. 

Also using the same circuit a normal key fob can be used. So you can use the phone or key fob for each action one keyfob for each 4 codes.
