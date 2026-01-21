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
dimensions array. Need to add the name and clear just a couple of symbols after the array.
