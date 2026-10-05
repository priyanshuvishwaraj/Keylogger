# Keylogger
A keylogger script that captures input events on the linux, stores them as readable text in a .log file with timestamps.

It also shows a WPM meter and a Backstroke%. And when exited it displays a heatmap of the top 5 most frequent keystrokes.

## Compilation
First you need to replace "event3" with your keyboard's event at line 53. (you can find this using "ls -l /dev/input/by-id/", this will display a list of active input devices, see the one with "event-kbd" in it and replace.)

Then compile the Keylogger.c file into an executable using gcc.
Run command: "gcc Keylogger.c -o Keylogger"

## Usage
Run command: "sudo ./Keylogger" while in the same directory.

Now the keylogger is working press Ctrl+Esc to exit the program safely.
