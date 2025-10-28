#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "hardware/adc.h"
#include "matrix_display.hpp"
#include "pindefs.hpp"
#include "pico_flash.hpp"
#include "clw_dbgutils.h"
#define STR_BUFFER_LEN 128
#define BASELINE_SAMPLES 200
#define AVERAGE_WINDOW 10
#define DEBUG_TEMP_PRINT 1

void init_gpio(void){
    gpio_init_mask(MASK_ALL_COLS|MASK_ALL_ROWS);
    gpio_set_dir_masked(MASK_ALL_COLS|MASK_ALL_ROWS, MASK_ALL_COLS|MASK_ALL_ROWS);
    gpio_put_masked(MASK_ALL_ROWS,MASK_ALL_ROWS);
    gpio_set_drive_strength(LED_C1, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C2, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C3, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C4, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(LED_C5, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_dir(PB1,0);
    gpio_set_dir(PB2,0);
    gpio_pull_up(PB1);
    gpio_pull_up(PB2);
    
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}

uint counter = 0;
uint scroll_count = 0;
const uint8_t * current_char;

float max_brightness = 1.0f;
float current_brightness = 0.05f;
float baseline_adc_temp = 0;

// Function to read temperature and update brightness
void update_brightness_from_temp(void) {
    static uint32_t last_update = 0;
    static uint32_t last_debug_print = 0;
    static uint32_t last_baseline_update = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    // Update only every 50ms
    if (now - last_update < 50) {
        return;
    }
    last_update = now;
    
    uint16_t raw_adc_temp = adc_read();

    static float adc_history[AVERAGE_WINDOW] = {0};
    static uint8_t adc_index = 0;
    static bool history_filled = false;

    // Add to averaging window of AVERAGE_WINDOW samples
    adc_history[adc_index] = raw_adc_temp;
    adc_index = (adc_index + 1) % AVERAGE_WINDOW;
    if (adc_index == 0) history_filled = true;
    
    // Calculate average temperature
    float adc_temp = 0;
    uint8_t samples = history_filled ? AVERAGE_WINDOW : (adc_index == 0 ? AVERAGE_WINDOW : adc_index);
    for (uint8_t i = 0; i < samples; i++) {
        adc_temp += adc_history[i];
    }
    adc_temp /= samples;
    
    static uint8_t baseline_count = 0;
    static float baseline_sum = 0;
    
    // Measure baseline temperature over first 20 readings
    if (baseline_count < BASELINE_SAMPLES) {
        baseline_sum += adc_temp;
        baseline_count++;
        baseline_adc_temp = baseline_sum / baseline_count;
        if (baseline_count == BASELINE_SAMPLES) {
            printf("Baseline ADC temp (averaged): %.1f\n", baseline_adc_temp);
            last_baseline_update = now;
        }
    }

    float temp_diff = adc_temp - baseline_adc_temp;

    // Absolute, so heating and cooling have same effect
    float abs_temp_diff = temp_diff;
    if (abs_temp_diff < 0) abs_temp_diff = -abs_temp_diff;

    float brightness_change = abs_temp_diff * 0.25f;
    float target_brightness = (max_brightness * 0.15f) + brightness_change;
    
    // Clamp brightness to 5% to 100% of max brightness
    if (target_brightness > max_brightness) {
        target_brightness = max_brightness;
    }
    if (target_brightness < max_brightness * 0.15f) {
        target_brightness = max_brightness * 0.15f;
    }
    
    // Smoothly update current brightness - faster decay when decreasing
    if (target_brightness < current_brightness) {
        // Faster response when dimming
        current_brightness = current_brightness * 0.6f + target_brightness * 0.4f;
    } else {
        // Slower response when brightening
        current_brightness = current_brightness * 0.98f + target_brightness * 0.02f;
    }
    
#if DEBUG_TEMP_PRINT
    if (now - last_debug_print > 1000) {
        printf("ADC: %.1f, Baseline: %.1f, RawDiff: %.1f, AbsDiff: %.1f, Brightness: %.1f%%\n", 
               adc_temp, baseline_adc_temp, temp_diff, abs_temp_diff, current_brightness * 100.0f);
        last_debug_print = now;
    }
#endif
}

//const char * testString = ;
enum disp_mode{
    USER = 0,
    ECSE = 1,
    EASTER = 2
};

disp_mode display_mode = USER;


char userStringBuffer[STR_BUFFER_LEN] = " Use PuTTY to Program (115200b)";
char presetStringBuffer[STR_BUFFER_LEN] = " ECSE LEAVERS 2025";
char easterEggStr[STR_BUFFER_LEN] = " COMPSYS ON TOP";
char tempBuffer[STR_BUFFER_LEN] = {0};

char * dispBuff = userStringBuffer;

char * strings[] = {
    userStringBuffer,
    presetStringBuffer,
    easterEggStr
};
uint8_t tempBufferIdx = 0;
void scroll_screen(void){
    scroll_chars();
    scroll_count++;
    if(scroll_count==7){
        counter = (counter+1) % strlen(strings[display_mode]);
        const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
        add_char_to_scroll(disp_char);
        scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
    }
}

void screen_start(void){
    const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
    add_char_to_scroll(disp_char);
    scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
}

repeating_timer_t scroll_timer = {0};
bool scroll_timer_cb(repeating_timer_t * timer){
    scroll_screen();
    return true;
}

void print_info(void){
    printf("------------------------------------------------\n");
    printf(BR_BLUE "ECSE LEAVERS DINNER INVITATIONS 2025\n" COLOUR_NONE);
    printf("------------------------------------------------\n");
    printf("Hello! Congratulations on Finishing your undergraduate degree.\n");
    printf("To update the displayed string, simply send a new one over serial.\n");
    printf("All ASCII printable characters should be implemented.\n");
    printf("Try the buttons to see if you can find any easter eggs,\n");
    printf("or check the git repo for more info.\n");
    printf("github.com/campbelllwright/ECSE_LeaversDinner_2025\n");
    printf("--------------------------------------------------\n");
    printf(" - PCB by : Campbell Wright, James West\n");
    printf(" - Code by: Campbell Wright\n");
    printf("---------------------------------------\n");
}



int main()
{
    tempBuffer[0] = ' ';
    tempBufferIdx = 1;
    stdio_init_all();
    init_gpio();
    read_name_from_flash(userStringBuffer, STR_BUFFER_LEN);
    screen_start();
    printf("hello, world!");
    add_repeating_timer_ms(-100,scroll_timer_cb,0,&scroll_timer);
    
    while (true) {
        static bool pb1_last = 1, pb2_last = 1;
        bool pb1_val = gpio_get(PB1);
        bool pb2_val = gpio_get(PB2);
        if((pb1_last != pb1_val)||(pb2_last!=pb2_val)){
            if((pb1_val==0) &&(pb2_val ==0)){
                display_mode = EASTER;
                counter = 0;
                const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
                add_char_to_scroll_start(disp_char);
                add_char_to_scroll(disp_char);
                scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
            }else if((pb1_val == 0)){
                display_mode = ECSE;
                counter = 0;
                const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
                add_char_to_scroll_start(disp_char);
                add_char_to_scroll(disp_char);
                scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
                print_info();
            }else if(pb2_val == 0){
                display_mode = USER;
                counter = 0;
                const uint8_t * disp_char = char_to_matrix(strings[display_mode][counter]);
                add_char_to_scroll_start(disp_char);
                add_char_to_scroll(disp_char);
                scroll_count=5-((disp_char[0]&0xE0)>>5); //3MSB of first col of char = length (0-7)
            }
            
            //add_char_to_scroll(char_to_matrix(stringBuffer[counter]));
        }
        pb1_last = pb1_val;

        if(gpio_get(PB2)){
            current_char=scroll_buff;//char_to_matrix(stringBuffer[counter]);
        }
        else{
            //scroll_screen();
            //current_char = char_to_matrix(128);
            //print_print_buff();
        }
        for(int i = 0; i < 100; i++){
            update_brightness_from_temp();
            disp_char(current_char, current_brightness); 
            //This is janky - ISRs were being weird so we just do 100 display cycles for every button poll
            //which means our polling rate is worst case 100us*25*100 = 250ms.
            //if ISRs still funky maybe throw this on core 1? would be cool and leave core 0 available for user code/polling.
        }
        //scroll_screen();
        char inChar = getchar_timeout_us(10);
        if(inChar != 0xFE){
            if(inChar != '\n'){
                tempBuffer[tempBufferIdx] = inChar;
                tempBufferIdx++;
            }
            else{
                tempBuffer[tempBufferIdx] = 0;
                printf("Displaying String \"%s\"\n", tempBuffer);
                memccpy(userStringBuffer, tempBuffer, 0, 64);
                uint rc = write_name_to_flash(userStringBuffer);
                if(!rc){
                    printf("wrote string \"%s\" (%d bytes) to flash\n", userStringBuffer, strlen(userStringBuffer)+1);
                }
                counter = 0;
                tempBuffer[0] = ' ';
                tempBufferIdx = 1;
            }
        }
        
    }
}
