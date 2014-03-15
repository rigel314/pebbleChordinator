#ifndef chord_h
#define chord_h

// create and push chord display window to stack, using the given
// string as a lookup for the chords file
void display_chord(const char *chord);
int whiteKeys(int index);
int blackKeyIndex(int index);
bool isBlackKey(int index);

#endif
