/*

mp3_id3_tags: v1.1 by Marc Specht
no warranty implied; use at your own risk

See end of file for license information.

Do this:
    #define MP3_ID3_TAGS_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define MP3_ID3_TAGS_IMPLEMENTATION
#include "mp3_id3_tags.h" 

You can #define MP3_ID3_TAGS_USE_GENRES before the #include to also be able to read the genre.

Revision history:
    v1.1 (2020-07-27) Added mp3_id3_read_tag and mp3_id3_file_read_tag, updated documentation
    v1.0 (2020-07-26) First release

*/

#ifndef _MP3_ID3_TAGS_H
#define _MP3_ID3_TAGS_H

// DOCUMENTATION
// 
// Supported tags:
//     - Title
//     - Artist
//     - Album
//     - Year
//     - Comment
//     - Genre (when MP3_ID3_TAGS_USE_GENRES is defined)
// 
// Functions:
//     mp3_id3_has_tags and mp3_id3_file_has_tags:
//         checks if a mp3 file has tag information
//             return: 1 on success, 0 on failure
// 
//     mp3_id3_read_tag and mp3_id3_file_read_tag:
//         read a specific tag from a mp3 file
//             return: null terminated string which contains the requested tags data or NULL
//             note: the returned char* has to be freed
// 
//     mp3_id3_read_tags and mp3_id3_file_read_tags:
//         read all tags from a mp3 file
//             return: 1 on success, 0 on failure
//             if the function succeeds, the supplied mp3_id3_tags structure will contain the tag information
// 
// Example:
//     if (mp3_id3_has_tags(fileName)) {
//         mp3_id3_tags tags;
//         if (mp3_id3_read_tags(fileName, &tags)) {
//             (use tags)
//         } else {
//             (print error)
//         }
//     }
// 

#include <stdio.h> // for fclose, fopen, fread, fseek
#include <string.h> // for malloc, strncmp, strcpy, strncpy

enum
{

    MP3_ID3_TAG_TITLE = 0,
    MP3_ID3_TAG_ARTIST = 30,
    MP3_ID3_TAG_ALBUM = 60,
    MP3_ID3_TAG_YEAR = 90,
    MP3_ID3_TAG_COMMENT = 94

    #ifdef MP3_ID3_TAGS_USE_GENRES
    , MP3_ID3_TAG_GENRE = 124
    #endif // MP3_ID3_TAGS_USE_GENRES

};

typedef struct
{
    
    char title[31];
    char artist[31];
    char album[31];
    char year[5];
    char comment[31];
    
    #ifdef MP3_ID3_TAGS_USE_GENRES
    char genre[31];
    #endif // MP3_ID3_TAGS_USE_GENRES

} mp3_id3_tags;

int mp3_id3_has_tags(const char *fileName);
int mp3_id3_file_has_tags(FILE *f);

char *mp3_id3_read_tag(const char *fileName, int tagId);
char *mp3_id3_file_read_tag(FILE *f, int tagId);

int mp3_id3_read_tags(const char *fileName, mp3_id3_tags *tags);
int mp3_id3_file_read_tags(FILE *f, mp3_id3_tags *tags);

#endif // _MP3_ID3_TAGS_H

#ifdef MP3_ID3_TAGS_IMPLEMENTATION

#ifdef MP3_ID3_TAGS_USE_GENRES

#define MP3_ID3_TAGS_GENRE_COUNT 148

char *mp3_id3_genres[MP3_ID3_TAGS_GENRE_COUNT] =
{

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

int mp3_id3_has_tags(const char *fileName)
{

    FILE *f = fopen(fileName, "rb");

    int ret = mp3_id3_file_has_tags(f);

    fclose(f);

    return ret;

}

int mp3_id3_file_has_tags(FILE *f)
{

    if (!f)
        return 0;
    else
    {

        char id3[128];
        if (fseek(f, -128, SEEK_END) || fread(id3, sizeof(char), 128, f) < 128)
            return 0;

        if (strncmp(id3, "TAG", 3))
            return 0;
        else
            return 1;

    }

}

char *mp3_id3_read_tag(const char *fileName, int tagId)
{

    FILE *f = fopen(fileName, "rb");

    char *ret = mp3_id3_file_read_tag(f, tagId);

    fclose(f);

    return ret;

}

char *mp3_id3_file_read_tag(FILE *f, int tagId)
{

    if (!f)
        return NULL;
    else if (!mp3_id3_file_has_tags(f))
        return NULL;
    else
    {

        char id3[128];

        if (fseek(f, -128, SEEK_END) || fread(id3, sizeof(char), 128, f) < 128)
            return 0;
        
        unsigned char *ptr = id3 + 3 + tagId;
        
        size_t sz = (tagId == MP3_ID3_TAG_YEAR) ? 5 : 31;
        char *str = (char*) malloc(sizeof(char) * sz);

        if (!str)
            return NULL;
        else
        {

            strncpy(str, ptr, sz - 1);
            str[sz - 1] = '\0';

            return str;

        }

    }

}

int mp3_id3_read_tags(const char *fileName, mp3_id3_tags *tags)
{

    FILE *f = fopen(fileName, "rb");

    int ret = mp3_id3_file_read_tags(f, tags);

    fclose(f);

    return ret;

}

int mp3_id3_file_read_tags(FILE *f, mp3_id3_tags *tags)
{

    if (!f)
        return 0;
    else if (!mp3_id3_file_has_tags(f))
        return 0;
    else if (!tags)
        return 0;
    else
    {

        char id3[128];

        if (fseek(f, -128, SEEK_END) || fread(id3, sizeof(char), 128, f) < 128)
            return 0;

        unsigned char *ptr = id3 + 3;

        strncpy(tags->title, ptr, 30);
        tags->title[30] = '\0';
        ptr += 30;

        strncpy(tags->artist, ptr, 30);
        tags->artist[30] = '\0';
        ptr += 30;

        strncpy(tags->album, ptr, 30);
        tags->album[30] = '\0';
        ptr += 30;

        strncpy(tags->year, ptr, 4);
        tags->year[4] = '\0';
        ptr += 4;

        strncpy(tags->comment, ptr, 30);
        tags->comment[30] = '\0';
        ptr += 30;

        #ifdef MP3_ID3_TAGS_USE_GENRES

        if (*ptr >= MP3_ID3_TAGS_GENRE_COUNT)
            strcpy(tags->genre, "Unknown");
        else
            strcpy(tags->genre, mp3_id3_genres[*ptr]);

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