#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <linux/input.h>

#define MAX_KEYCODE 256

typedef struct {int keycode;int count;} KeyFreq;

int compare_freq(const void *a, const void *b) {
    return ((KeyFreq*)b)->count - ((KeyFreq*)a)->count;
}

const char* translate_key(int code) {
    switch(code) {

        case KEY_SPACE: return "[Space]";
        case KEY_ENTER: return "[Enter]";
        case KEY_LEFTSHIFT: return "[Shift]";
        case KEY_RIGHTSHIFT: return "[Shift]";
        case KEY_LEFTCTRL: return "[Ctrl]";
        case KEY_RIGHTCTRL: return "[Ctrl]";
        case KEY_LEFTALT:  return "[Shift]";
        case KEY_RIGHTALT: return "[Alt]";
        case KEY_TAB: return "[Tab]";
        case KEY_BACKSPACE: return "[Backspace]";
        case KEY_ESC: return "[ESC]";

        case KEY_A: return "a"; case KEY_B: return "b"; case KEY_C: return "c";
        case KEY_D: return "d"; case KEY_E: return "e"; case KEY_F: return "f";
        case KEY_G: return "g"; case KEY_H: return "h"; case KEY_I: return "i";
        case KEY_J: return "j"; case KEY_K: return "k"; case KEY_L: return "l";
        case KEY_M: return "m"; case KEY_N: return "n"; case KEY_O: return "o";
        case KEY_P: return "p"; case KEY_Q: return "q"; case KEY_R: return "r";
        case KEY_S: return "s"; case KEY_T: return "t"; case KEY_U: return "u";
        case KEY_V: return "v"; case KEY_W: return "w"; case KEY_X: return "x";
        case KEY_Y: return "y"; case KEY_Z: return "z";

        case KEY_1: return "1"; case KEY_2: return "2"; case KEY_3: return "3";
        case KEY_4: return "4"; case KEY_5: return "5"; case KEY_6: return "6";
        case KEY_7: return "7"; case KEY_8: return "8"; case KEY_9: return "9";
        case KEY_0: return "0";
        default: return "[Unknown]";
    }
}

int main() {
    int total_keystrokes = 0;
    int backspace_count = 0;
    int ctrl_pressed = 0;
    
    unsigned long freq_table[MAX_KEYCODE] = {0};
    
    time_t start_time = time(NULL);
    
    int inputFile = open("/dev/input/event4", O_RDONLY);
    if (inputFile == -1) {
        printf("Access denied. Use sudo");
        return 0;
    }
    
    FILE* logFile = fopen("Keylogger.log", "a");
    if (!logFile) {
        printf("Log File error.");
        close(inputFile);
        return 0;
    }
    
    struct input_event ev;
    while (read(inputFile, &ev, sizeof(struct input_event)) > 0) {
        if (ev.type == EV_KEY) {
            if (ev.code == KEY_LEFTCTRL || ev.code == KEY_RIGHTCTRL) {
                ctrl_pressed = (ev.value != 0);
            }
            
            if (ev.code == KEY_ESC && ev.value == 1 && ctrl_pressed) {
                break;
            }
            
            if (ev.value == 1) {
                total_keystrokes++;
                
                if (ev.code < MAX_KEYCODE) {
                    freq_table[ev.code]++;
                }
                
                if (ev.code == KEY_BACKSPACE) {
                    backspace_count++;
                }
                
                time_t now = time(NULL);

                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
                
                const char* key_pressed = translate_key(ev.code);
                fprintf(logFile, "[%s] %s\n", timestamp, key_pressed);
                fflush(logFile);
                
                double timeSinceStart = difftime(now,start_time);

                double minutes = timeSinceStart / 60.0;
                if (minutes < 0.01) minutes = 0.01; 
                
                double wpm = (total_keystrokes / 5.0) / minutes;
                double error_rate = ((double)backspace_count / total_keystrokes) * 100.0;

                printf("\rWPM: %.1f | Backspace Error Rate: %.1f%%", wpm, error_rate);
                fflush(stdout);
            }
        }
    }
    
    time_t end_time = time(NULL);
    double total_runtime = difftime(end_time, start_time);
    
    fclose(logFile);
    close(inputFile);
    
    printf("\n\n=== SESSION SUMMARY ===\n");
    printf("Total Runtime: %.0f seconds\n", total_runtime);
    printf("Total Keystrokes: %d\n", total_keystrokes);
    
    KeyFreq sorted_freq[MAX_KEYCODE];
    int unique_keys = 0;
    for (int i = 0; i < MAX_KEYCODE; i++) {
        if (freq_table[i] > 0) {
            sorted_freq[unique_keys].keycode = i;
            sorted_freq[unique_keys].count = freq_table[i];
            unique_keys++;
        }
    }
    
    qsort(sorted_freq, unique_keys, sizeof(KeyFreq), compare_freq);
    
    printf("\nTOP 5 MOST FREQUENT KEYS:\n");
    printf("-------------------------\n");
    int limit = unique_keys < 5 ? unique_keys : 5;
    for (int i = 0; i < limit; i++) {
        printf("%d. %-12s : %d times\n", i + 1, translate_key(sorted_freq[i].keycode), sorted_freq[i].count);
    }
    
    return EXIT_SUCCESS;
}
