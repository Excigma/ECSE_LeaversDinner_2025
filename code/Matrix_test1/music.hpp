/* ========================================
 *
 * Bad Apple Music Player for Core 1
 * Adapted from Norman the Nomad's music system
 *
 * ========================================
*/

#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <stdint.h>

extern uint8_t numSongs;
extern int8_t currentSong;

/**
 * Launch music on the second core.
 */
void core1MusicMain(void);

uint8_t isPlaying(void);
void toggleMusic();
void stopMusic();
int8_t playSong(int8_t songID);
void initMusic();

#endif /* MUSIC_HPP */
