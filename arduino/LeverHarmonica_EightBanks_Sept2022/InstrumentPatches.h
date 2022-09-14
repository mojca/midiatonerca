#pragma once
/*
 * Defines properties of patches used for accordions
 * Treble note offset and bass note offset: some example fonts are an octave higher or lower. Often intentional, sometimes not, and fixed by specifying the offset (+/- 12)
 * Special instrument: if true then bass chords are recorded as a single note with special rules. This applies to the first patch only.
 *
 * Patches:
 * 0. Accordion recorded in studio by Mojca&Vincent on August 1, 2022.
 * 1. Accordion from Samplerbox website
 * 2. MKII Flute
 * 3. Lately Bass
 * 4. Saw
 * 5. Square
 * 6. Piano
 * 7. Airy Keys
 * 8. Chrous Keys
 * 9. Melanchosia
 * 10. Music box
 * 11. Octave bass
 * 12. Organ
 * 13. Rezo bass
 * 14. Simple bass
 * 15. Accordion recorded home by Mojca&Vincent on July 27, 2022
 *
 */

const int PATCH_COUNT = 16;
int isSpecialInstrument[PATCH_COUNT] = {true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true}; // 0-based
int trebleNoteOffsetPerPatch[PATCH_COUNT] = {0,-12,0,12,12,12,0,0,12,0,0,0,0,0,0,0}; // 0-based
int bassNoteOffsetPerPatch[PATCH_COUNT] =   {0,-12,0,12,12,12,0,0,12,0,0,0,0,0,0,0}; // 0-based



