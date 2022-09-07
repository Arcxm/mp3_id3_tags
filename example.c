#include <stdio.h>
#include <stdlib.h>

#define MP3_ID3_TAGS_USE_GENRES
#define MP3_ID3_TAGS_IMPLEMENTATION
#include "mp3_id3_tags.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("usage: %s filename\n", argv[0]);
    } else if (argc == 2) {
        FILE *f = fopen(argv[1], "rb");

        if (f) {
            mp3_id3_tags tags;
            if (mp3_id3_file_read_tags(f, &tags)) {
                printf("MP3: %s\n\n", argv[1]);

                printf("Title: %s\n", tags.title);
                printf("Artist: %s\n", tags.artist);
                printf("Album: %s\n", tags.album);
                printf("Year: %s\n", tags.year);
                printf("Comment: %s\n", tags.comment);
                printf("Genre: %s\n", tags.genre);
            } else {
                fprintf(stderr, "error: %s\n", mp3_id3_failure_reason());
            }

            fclose(f);
        } else {
            printf("failed to open/read '%s'\n", argv[1]);
        }
    }

    return EXIT_SUCCESS;
}