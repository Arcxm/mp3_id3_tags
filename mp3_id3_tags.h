/* mp3_id3_tags: v2.2 by Marc Specht

    no warranty implied; use at your own risk

    Do this:
        #define MP3_ID3_TAGS_IMPLEMENTATION
    before you include this file in *one* C or C++ file to create the implementation.

    // i.e. it should look like this:
    #include ...
    #include ...
    #include ...
    #define MP3_ID3_TAGS_IMPLEMENTATION
    #include "mp3_id3_tags.h" 

    You can #define MP3_ID3_TAGS_USE_GENRES before the #include to be able to read the genre.

LICENSE

    See end of file for license information.

REVISION HISTORY:

    v2.2 (2023-08-17) Unhardcode magic numbers and use strncpy for genre
    v2.1 (2022-12-17) Typedef tag type and fix error when reading genre
    v2.0 (2022-09-07) Refactor library
    v1.2 (2022-01-11) Const correctness for mp3_id3_genres
    v1.1 (2020-07-27) Added mp3_id3_read_tag and mp3_id3_file_read_tag, updated documentation
    v1.0 (2020-07-26) First release

*/

#ifndef _MP3_ID3_TAGS_H
#define _MP3_ID3_TAGS_H

// DOCUMENTATION
//
// Supported tags
//
// - Title
// - Artist
// - Album
// - Year
// - Comment
// - Genre (when MP3_ID3_TAGS_USE_GENRES is defined)
//
// ============================================================================================
//
// Functions
//
// Each function comes in two variants:
//  1. takes a filename and opens a file by itself
//  2. takes a file pointer and uses the already opened file ('file' in function name)
//
// mp3_id3_has_tags and mp3_id3_file_has_tags
//  checks if an mp3 file has tag information
//  returns 1 on success, 0 on failure
//
// mp3_id3_read_tag and mp3_id3_file_read_tag
//  read a specific tag from an mp3 file
//  returns a null terminated string which contains the requested tag's data or NULL on failure
//  NOTE: this string has to be freed after usage
//
// mp3_id3_read_tags and mp3_id3_file_read_tags
//  read all tags from an mp3 file
//  returns 1 on success, 0 on failure
//  the supplied mp3_id3_tags structure will contain the tag information
//
// ============================================================================================
//
// Errors
//
// If a function fails, you can use mp3_id3_failure_reason to get an error message.
//
// ============================================================================================
//
// Example
//
// if (mp3_id3_has_tags(filename)) {
//     mp3_id3_tags tags;
//     if (mp3_id3_read_tags(filename, &tags)) {
//         fprintf(stdout, "%s by %s\n", tags.title, tags.artist);
//     } else {
//         fprintf(stderr, "error: %s\n", mp3_id3_failure_reason());
//     }
// } else {
//     fprintf(stderr, "%s has no tags\n", filename");
// }
//

#include <stdio.h> // for fclose, fopen, fread, fseek
#include <stdlib.h> // for malloc
#include <string.h> // for strncmp, strcpy, strncpy

#define MP3_ID3_BLOCK_SZ 128
#define MP3_ID3_SEEK_OFFSET (-MP3_ID3_BLOCK_SZ)
#define MP3_ID3_HEADER_SZ 3
#define MP3_ID3_TAG_SZ 31
#define MP3_ID3_TAG_YEAR_SZ 5

////////////////////////////////////
//
// Tags
//

typedef enum {
    TITLE = 0,
    ARTIST = 30,
    ALBUM = 60,
    YEAR = 90,
    COMMENT = 94

    #ifdef MP3_ID3_TAGS_USE_GENRES
    , GENRE = 124
    #endif // MP3_ID3_TAGS_USE_GENRES
} mp3_id3_tag;

typedef struct {
    char title[MP3_ID3_TAG_SZ];
    char artist[MP3_ID3_TAG_SZ];
    char album[MP3_ID3_TAG_SZ];
    char year[MP3_ID3_TAG_YEAR_SZ];
    char comment[MP3_ID3_TAG_SZ];
    
    #ifdef MP3_ID3_TAGS_USE_GENRES
    char genre[MP3_ID3_TAG_SZ];
    #endif // MP3_ID3_TAGS_USE_GENRES
} mp3_id3_tags;

////////////////////////////////////
//
// mp3_id3 interface
//

int mp3_id3_has_tags(const char *filename);
int mp3_id3_file_has_tags(FILE *f);

char *mp3_id3_read_tag(const char *filename, mp3_id3_tag tag);
char *mp3_id3_file_read_tag(FILE *f, mp3_id3_tag tag);

int mp3_id3_read_tags(const char *filename, mp3_id3_tags *tags);
int mp3_id3_file_read_tags(FILE *f, mp3_id3_tags *tags);

#endif // _MP3_ID3_TAGS_H

#ifdef MP3_ID3_TAGS_IMPLEMENTATION

#ifdef MP3_ID3_TAGS_USE_GENRES

#define MP3_ID3_TAGS_GENRE_COUNT 148

const char *mp3_id3_genres[MP3_ID3_TAGS_GENRE_COUNT] = {
    "Blues", "Classic Rock", "Country", "Dance",
    "Disco", "Funk", "Grunge", "Hip-Hop",
    "Jazz", "Metal", "New Age", "Oldies",
    "Other", "Pop", "R&B", "Rap",
    "Reggae", "Rock", "Techno", "Industrial",
    "Alternative", "Ska", "Death Metal", "Pranks",
    "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
    "Vocal", "Jazz+Funk", "Fusion", "Trance",
    "Classical", "Instrumental", "Acid", "House",
    "Game", "Sound Clip", "Gospel", "Noise",
    "AlternRock", "Bass", "Soul", "Punk",
    "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
    "Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
    "Electronic", "Pop-Folk", "Eurodance", "Dream",
    "Southern Rock", "Comedy", "Cult", "Gangsta",
    "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
    "Native American", "Cabaret", "New Wave", "Psychadelic",
    "Rave", "Showtunes", "Trailer", "Lo-Fi",
    "Tribal", "Acid Punk", "Acid Jazz", "Polka",
    "Retro", "Musical", "Rock & Roll", "Hard Rock",
    "Folk", "Folk/Rock", "National folk", "Swing",
    "Fast-fusion", "Bebob", "Latin", "Revival",
    "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
    "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
    "Big Band", "Chorus", "Easy Listening", "Acoustic",
    "Humour", "Speech", "Chanson", "Opera",
    "Chamber Music", "Sonata", "Symphony", "Booty Bass",
    "Primus", "Porn Groove", "Satire", "Slow Jam",
    "Club", "Tango", "Samba", "Folklore",
    "Ballad", "Powder Ballad", "Rhythmic Soul", "Freestyle",
    "Duet", "Punk Rock", "Drum Solo", "A Capella",
    "Euro-House", "Dance Hall", "Goa", "Drum & Bass",
    "Club House", "Hardcore", "Terror", "Indie",
    "BritPop", "NegerPunk", "Polsk Punk", "Beat",
    "Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover",
    "Contemporary C", "Christian Rock", "Merengue", "Salsa",
    "Thrash Metal", "Anime", "JPop", "SynthPop"
};

#endif // MP3_ID3_TAGS_USE_GENRES

const char *mp3_id3__failure_reason;

const char *mp3_id3_failure_reason(void) {
    return mp3_id3__failure_reason;
}

static void mp3_id3__err(const char *str) {
    mp3_id3__failure_reason = str;
}

int mp3_id3__read_id3(FILE *f, char *id3) {
    if (fseek(f, MP3_ID3_SEEK_OFFSET, SEEK_END)) {
        mp3_id3__err("Could not seek from the end of the file");
        return 0;
    }

    if (fread(id3, sizeof(char), MP3_ID3_BLOCK_SZ, f) < MP3_ID3_BLOCK_SZ) {
        mp3_id3__err("Read less than required amount of bytes from the file");
        return 0;
    }

    return 1;
}

int mp3_id3_has_tags(const char *filename) {
    FILE *f = fopen(filename, "rb");

    int ret = mp3_id3_file_has_tags(f);

    fclose(f);

    return ret;
}

int mp3_id3_file_has_tags(FILE *f) {
    if (!f) {
        mp3_id3__err("File pointer was NULL");
        return 0;
    } else {
        char id3[MP3_ID3_BLOCK_SZ];
        
        if (!mp3_id3__read_id3(f, id3)) {
            return 0;
        }

        if (strncmp(id3, "TAG", MP3_ID3_HEADER_SZ) == 0) {
            return 1;
        } else {
            return 0;
        }
    }
}

char *mp3_id3_read_tag(const char *filename, mp3_id3_tag tag) {
    FILE *f = fopen(filename, "rb");

    char *ret = mp3_id3_file_read_tag(f, tag);

    fclose(f);

    return ret;
}

char *mp3_id3_file_read_tag(FILE *f, mp3_id3_tag tag) {
    if (!f) {
        mp3_id3__err("File pointer was NULL");
        return NULL;
    } else if (!mp3_id3_file_has_tags(f)) {
        mp3_id3__err("File does not have tags");
        return NULL;
    } else {
        char id3[MP3_ID3_BLOCK_SZ];

        if (!mp3_id3__read_id3(f, id3)) {
            return 0;
        }
        
        char *ptr = id3 + MP3_ID3_HEADER_SZ + tag;
        
        size_t sz = (tag == YEAR) ? MP3_ID3_TAG_YEAR_SZ : MP3_ID3_TAG_SZ;
        char *str = (char*) malloc(sizeof(char) * sz);

        if (!str) {
            mp3_id3__err("Could not allocate memory for the tag string");
            return NULL;
        } else {
            strncpy(str, ptr, sz - 1);
            str[sz - 1] = '\0';

            return str;
        }
    }
}

int mp3_id3_read_tags(const char *filename, mp3_id3_tags *tags) {
    FILE *f = fopen(filename, "rb");

    int ret = mp3_id3_file_read_tags(f, tags);

    fclose(f);

    return ret;
}

int mp3_id3_file_read_tags(FILE *f, mp3_id3_tags *tags) {
    if (!f) {
        mp3_id3__err("File pointer was NULL");
        return 0;
    } else if (!mp3_id3_file_has_tags(f)) {
        mp3_id3__err("File does not have tags");
        return 0;
    } else if (!tags) {
        mp3_id3__err("mp3_id3_tags pointer was NULL");
        return 0;
    } else {
        char id3[MP3_ID3_BLOCK_SZ];

        if (!mp3_id3__read_id3(f, id3)) {
            return 0;
        }

        char *ptr = id3 + MP3_ID3_HEADER_SZ;

        strncpy(tags->title, ptr, MP3_ID3_TAG_SZ - 1);
        tags->title[MP3_ID3_TAG_SZ - 1] = '\0';
        ptr += MP3_ID3_TAG_SZ - 1;

        strncpy(tags->artist, ptr, MP3_ID3_TAG_SZ - 1);
        tags->artist[MP3_ID3_TAG_SZ - 1] = '\0';
        ptr += MP3_ID3_TAG_SZ - 1;

        strncpy(tags->album, ptr, MP3_ID3_TAG_SZ - 1);
        tags->album[MP3_ID3_TAG_SZ - 1] = '\0';
        ptr += MP3_ID3_TAG_SZ - 1;

        strncpy(tags->year, ptr, MP3_ID3_TAG_YEAR_SZ - 1);
        tags->year[MP3_ID3_TAG_YEAR_SZ - 1] = '\0';
        ptr += MP3_ID3_TAG_YEAR_SZ - 1;

        strncpy(tags->comment, ptr, MP3_ID3_TAG_SZ - 1);
        tags->comment[MP3_ID3_TAG_SZ - 1] = '\0';
        ptr += MP3_ID3_TAG_SZ - 1;

        #ifdef MP3_ID3_TAGS_USE_GENRES

        if (*ptr < 0 || *ptr >= MP3_ID3_TAGS_GENRE_COUNT) {
            strncpy(tags->genre, "Unknown", MP3_ID3_TAG_SZ - 1);
            tags->genre[MP3_ID3_TAG_SZ - 1] = '\0';
        } else {
            strncpy(tags->genre, mp3_id3_genres[*ptr], MP3_ID3_TAG_SZ - 1);
            tags->genre[MP3_ID3_TAG_SZ - 1] = '\0';
        }

        #endif // MP3_ID3_TAGS_USE_GENRES

        return 1;
    }
}

#endif // MP3_ID3_TAGS_IMPLEMENTATION

/*
-------------------------------------------------------------------------------
License
-------------------------------------------------------------------------------
Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------
*/