/* ========================================
 *
 * Bad Apple Music Player for Core 1
 * Adapted from Norman the Nomad's music system
 *
 * ========================================
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "music.hpp"
#include "imitation/ToneGen.hpp"

#define CyDelayUs(x) sleep_us(x)

static uint32_t timer_period_ms = 100;
static uint8_t timer_enable = 0;

// Clock divider compare vals for 100khz freq counter
uint16_t notes[13][9] = {
    {8032, 4016, 2008, 1004, 502, 251, 126, 63, 31}, // C          0
    {7581, 3791, 1895, 948, 474, 237, 118, 59, 30}, // C#         1
    {7156, 3578, 1789, 894, 447, 224, 112, 56, 28}, // D          2
    {6754, 3377, 1689, 844, 422, 211, 106, 53, 26}, // Eb         3
    {6375, 3188, 1594, 797, 398, 199, 100, 50, 25},  // E          4
    {6017, 3009, 1504, 752, 376, 188, 94, 47, 24},  // F          5
    {5680, 2840, 1420, 710, 355, 177, 89, 44, 22},  // F#         6
    {5361, 2680, 1340, 670, 335, 168, 84, 42, 21},  // G          7
    {5060, 2530, 1265, 632, 316, 158, 79, 40, 20},   // Ab         8
    {4776, 2388, 1194, 597, 299, 149, 75, 37, 19},   // A          9
    {4508, 2254, 1127, 563, 282, 141, 70, 35, 18},   // Bb         A
    {4255, 2127, 1064, 532, 266, 133, 66, 33, 17},   // B          B
    {0,     0,    0,    0,    0,    0,   0,   0,   0}};   // CONTROL    C

// Control notes: 0x0c = no note. 0x1c = end of song, stop. 0x2c = end of song, loop. 0x3c = change speed (following note is read as new speed in ms period)
//               0x4c = slide (between 2 notes)
#define END_OF_SONG 0x1c
#define LOOP 0x2c
#define SPEED 0x3c
#define SLIDE 0x4c

#define DEFAULT_SPEED 125

#define FALSE_TRIPLET(note, note2, note3) note, note, 0x0c, note2, note2, 0x0c, note3, note3
#define FALSE_TRIPLET_SMOOTH(note, note2, note3) note, note, note, note2, note2, note2, note3, note3

uint16_t noteCounter = 0;
static uint8_t slideCounter = 0;
static uint16_t slideReturnSpeed = 0; // set to a number if in slide

#define SLIDE_LENGTH 12
static uint16_t slide[SLIDE_LENGTH];

// Bad Apple Song Data
#define BAD_APPLE_DRUM_SPEED 112
#define BAD_APPLE_BASS_SPEED 108
#define BAD_APPLE_MELODY_SPEED 107

#define INTRO_1 SPEED, BAD_APPLE_DRUM_SPEED * 2, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c, SPEED, BAD_APPLE_DRUM_SPEED / 2, 0x21, 0x0c, 0x21, 0x0c, 0x21, 0x0c, 0x21, 0x0c
#define INTRO_2 SPEED, BAD_APPLE_DRUM_SPEED * 2, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c, SPEED, BAD_APPLE_DRUM_SPEED, 0x1A, 0x0c, 0x1A, 0x0c
#define BASS_1 0x13, 0x13, 0x21, 0x23, 0x0c, 0x23, 0x21, 0x23
#define BASS_2 0x13, 0x13, 0x23, 0x26, 0x28, 0x28, 0x26, 0x28
#define BASS_3 0x28, 0x28, 0x26, 0x28, 0x26, 0x26, 0x21, 0x23
#define MELODY_1 0x43, 0x45, 0x46, 0x48, 0x4A, 0x4A, 0x53, 0x51, 0x4A, 0x4A, 0x43, 0x43, 0x4A, 0x48, 0x46, 0x45, 0x43, 0x45, 0x46, 0x48, 0x4A, 0x4A, 0x48, 0x46
#define MELODY_1A 0x43, 0x45, 0x46, 0x48, 0x4A, 0x4A, 0x53, 0x51, 0x4A, 0x4A, 0x43, 0x43, 0x48, 0x48, 0x46, 0x45, 0x43, 0x45, 0x46, 0x48, 0x4A, 0x4A, 0x48, 0x46
#define MELODY_2 0x45, 0x43, 0x45, 0x46, 0x45, 0x43, 0x42, 0x45
#define MELODY_3 0x45, 0x0c, 0x46, 0x0c, 0x48, 0x48, 0x4A, 0x4A
#define MELODY_3A 0x45, 0x0c, 0x46, 0x0c, 0x48, 0x0c, 0x4A, 0x0c
#define MELODY_4 0x51, 0x53, 0x4A, 0x48, 0x4A, 0x4A, 0x48, 0x4A, 0x51, 0x53, 0x4A, 0x48, 0x4A, 0x4A
#define MELODY_5 0x48, 0x4A, 0x48, 0x46, 0x45, 0x41, 0x43, 0x43, 0x41, 0x43, 0x45, 0x46, 0x48, 0x4A, 0x43, 0x43, 0x48, 0x4A
#define MELODY_6 0x53, 0x55, 0x56, 0x55, 0x53, 0x51, 0x4A, 0x4A, 0x48, 0x4A, 0x48, 0x46, 0x45, 0x41, 0x43, 0x43
#define MELODY_4A 0x52, 0x54, 0x4B, 0x49, 0x4B, 0x4B, 0x49, 0x4B, 0x52, 0x54, 0x4B, 0x49, 0x4B, 0x4B
#define MELODY_5A 0x49, 0x4B, 0x49, 0x47, 0x46, 0x42, 0x44, 0x44, 0x42, 0x44, 0x46, 0x47, 0x49, 0x4B, 0x44, 0x44, 0x49, 0x4B
#define MELODY_6A 0x54, 0x56, 0x57, 0x56, 0x54, 0x52, 0x4B, 0x4B, 0x49, 0x4B, 0x49, 0x47, 0x46, 0x42, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x0c, 0x0c
#define PAUSE 0x0c, 0x0c

uint8_t badApple[] = {
    // Drum Beat
    SPEED, BAD_APPLE_DRUM_SPEED * 2, PAUSE, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c, SPEED, BAD_APPLE_DRUM_SPEED / 2, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c,
    INTRO_2,
    INTRO_1, INTRO_2,
    INTRO_1, INTRO_2,
    INTRO_1, SPEED, BAD_APPLE_DRUM_SPEED * 2, 0x1A, 0x0c, 0x1A, 0x0c, 0x1A, 0x0c, 0x0c, 0x0c,
    // Bass Line Enters
    SPEED, BAD_APPLE_BASS_SPEED,
    BASS_1, BASS_1, BASS_1, BASS_2,
    BASS_1, BASS_1, BASS_1, BASS_3,
    BASS_1, BASS_1, BASS_1, BASS_2,
    BASS_1, BASS_1, BASS_1, SPEED, BAD_APPLE_BASS_SPEED / 2, FALSE_TRIPLET(0x28, 0x26, 0x28), FALSE_TRIPLET(0x26, 0x25, 0x26),
    // Melody Enters
    SPEED, BAD_APPLE_MELODY_SPEED * 2,
    MELODY_1, MELODY_2, MELODY_1A, MELODY_3,
    MELODY_1, MELODY_2, MELODY_1A, MELODY_3A,
    MELODY_4, MELODY_5, MELODY_4, MELODY_5,
    MELODY_4, MELODY_5, MELODY_4, MELODY_6, 0x48, 0x4A,
    MELODY_4, MELODY_5, MELODY_4, MELODY_5,
    MELODY_4, MELODY_5, MELODY_4, MELODY_6, 0x43, 0x0c,
    // Bass line bacc
    SPEED, BAD_APPLE_BASS_SPEED,
    BASS_1, BASS_1, BASS_1, BASS_2,
    BASS_1, BASS_1, BASS_1, BASS_3,
    BASS_1, BASS_1, BASS_1, BASS_2,
    BASS_1, BASS_1, BASS_1, SPEED, BAD_APPLE_BASS_SPEED / 2, FALSE_TRIPLET(0x28, 0x26, 0x28), FALSE_TRIPLET(0x26, 0x25, 0x26),
    // Melody bacc
    SPEED, BAD_APPLE_MELODY_SPEED * 2,
    MELODY_1, MELODY_2, MELODY_1A, MELODY_3,
    MELODY_1, MELODY_2, MELODY_1A, MELODY_3A,
    MELODY_4, MELODY_5, MELODY_4, MELODY_5,
    MELODY_4, MELODY_5, MELODY_4, MELODY_6, /*goes up one semitone*/ 0x49, 0x4B,
    MELODY_4A, MELODY_5A, MELODY_4A, MELODY_5A,
    MELODY_4A, MELODY_5A, MELODY_4A, MELODY_6A,
    END_OF_SONG};

uint8_t *noteList;

uint16_t note = 0;
uint16_t compare = 0;

uint8_t musicOn = 0;
int8_t currentSong = -1;

static int64_t nextNote(alarm_id_t id, __unused void *user_data);

uint8_t *songList[] = {badApple};
enum Song {
    SONG_BAD_APPLE = 0
};

uint8_t numSongs = sizeof(songList) / sizeof(uint8_t *);

// prepare note
static void prepareNote(void);

/**
 * Main Loop for music code.
 * Runs on core 1.
 */
void core1MusicMain(void)
{
    initMusic();
    // Wait for signal to start playing
    timer_enable = 0;
    
    printf("Core 1 music loop starting\n");
    
    while (1) {
        if (timer_enable && musicOn) {
            nextNote(0, NULL);  // Pass 0 instead of NULL for alarm_id_t
            sleep_ms(timer_period_ms);
        } else {
            sleep_ms(1); // Small sleep when not playing
        }
    }
}

////////// MUSIC LOGIC ///////////

void initMusic()
{
    printf("Initializing music system on core 1\n");
    ToneGen_Init();
    stopMusic();
}

void stopMusic()
{
    printf("Stopping music\n");
    ToneGen_Sleep();
    timer_enable = 0;
    currentSong = -1;
    musicOn = 0;
}

void toggleMusic()
{
    if(musicOn == 0)
    {
        musicOn = 1;
        noteCounter = 0;
    }
    else
    {
        stopMusic();
    }
}

int8_t playSong(int8_t songID) {
    if((songID >= numSongs) || (songID < 0)){
        printf("ERROR: Invalid songID %d (numSongs=%d)\n", songID, numSongs);
        return -1;
    }
    
    printf("Starting song %d - Bad Apple\n", songID);
    noteCounter = 0;
    noteList = songList[songID];
    timer_period_ms = DEFAULT_SPEED;
    slideReturnSpeed = 0; // no slide
    prepareNote();
    
    musicOn = 1;
    timer_enable = 1;
    currentSong = songID;
    
    printf("Music enabled: musicOn=%d, timer_enable=%d, period=%dms\n", musicOn, timer_enable, timer_period_ms);
    return songID;
}

uint8_t isPlaying(void)
{
    return musicOn;
}

static inline uint16_t lerp(uint8_t progress, uint16_t start, uint16_t end) {
    switch (progress){
    case 0:
        return start;
    case 1:
        return (start*3 + end)/4;
    case 2:
        return (start*2 + end*2)/4;
    case 3:default:
        return (start + end*3)/4;
    }
}

static int64_t nextNote(alarm_id_t id, __unused void *user_data)
{
    if (!timer_enable || !musicOn)
        return timer_period_ms * (int64_t)1000;
    
    static uint32_t debug_counter = 0;
    // if (debug_counter++ % 10 == 0) {
    if (true) {
        printf("Note %d: counter=%d, period=%dms, note=0x%02x\n", 
               debug_counter, noteCounter, timer_period_ms, noteList[noteCounter]);
    }
    
    // enter slide (setup in previous tick)
    if (noteList[noteCounter] == SLIDE && slideCounter == 0) {
        uint16_t startNote = notes[(noteList[noteCounter+1]&0x0F)][((noteList[noteCounter+1] & 0xF0)>>4)];
        uint16_t endNote = notes[(noteList[noteCounter+2]&0x0F)][((noteList[noteCounter+2] & 0xF0)>>4)];
        for (uint8_t i = 0; i < 4; i++) {
            slide[i] = lerp(i, startNote, endNote);
        }
        for (uint8_t i = 4; i < SLIDE_LENGTH; i++) {
            slide[i] = endNote;
        }
        
        noteCounter += 3;
    }
    
    // slide handled differently from other stuff
    if (slideReturnSpeed) {
        note = slide[slideCounter];
        slideCounter++;
    } else {
        note = notes[(noteList[noteCounter]&0x0F)][((noteList[noteCounter] & 0xF0)>>4)];
        
        if(noteList[noteCounter] == LOOP){
            noteCounter = 0;
        } else if (noteList[noteCounter] == END_OF_SONG) {
            stopMusic();
            return timer_period_ms * (int64_t)1000;
        } else {
            // Increment
            noteCounter++;
        }
    }
    
    compare = note/2;
    if (note == 0) {
        // Rest - disable PWM
        ToneGen_Sleep();
    } else {
        // Play note
        ToneGen_WritePeriod(note);
        ToneGen_WriteCompare(compare);
        ToneGen_Enable();
    }

    // All period changes have to be done before the next note, it seems
    
    // return from slide
    if (slideReturnSpeed && slideCounter == SLIDE_LENGTH) {
        timer_period_ms = slideReturnSpeed;
        slideReturnSpeed = 0;
    }

    if (!slideReturnSpeed && musicOn /*not stopped*/) {
        prepareNote();
    }

    return timer_period_ms * (int64_t)1000;
}

static void prepareNote(void)
{
    // speed command
    // after return as speed may be the next instruction
    if (noteList[noteCounter] == SPEED) {
        uint8_t ms = noteList[noteCounter + 1];
        timer_period_ms = ((uint16_t)ms);

        noteCounter += 2;
    }

    // but, we could have a slide instruction after a speed change
    // so we check it last
    // annoying
    if (noteList[noteCounter] == SLIDE) {
        slideReturnSpeed = (uint16_t)timer_period_ms;
        timer_period_ms = (slideReturnSpeed/SLIDE_LENGTH);
        slideCounter = 0;

        /*incrementing note counter happens next tick at the beginning, where the actual mechanics of the slide are handled*/
    }
}
