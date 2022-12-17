# About mp3_id3_tags

`mp3_id3_tags` is a public domain single header C/C++ library to read ID3 Tags from an MP3.

After looking at multiple single header C/C++ libraries and using them (especially the [stb](https://github.com/nothings/stb) ones), i decided to try and make one myself.

## Supported tags

- Title
- Artist
- Album
- Year
- Comment
- Genre

## Defines

These need to be defined before the `mp3_id3_tags.h` file is included:

- `MP3_ID3_TAGS_IMPLEMENTATION` in *one* C or C++ file to create the implementation
- `MP3_ID3_TAGS_USE_GENRES` to be able to read the genre tag

## Documentation

### Example

A full example program can be found [here](example.c).

### Function variants

Each function comes in two variants:

1. Takes a filename and opens a file by itself
2. Takes a file pointer and uses the already opened file ('file' in function name)

### mp3_id3_has_tags and mp3_id3_file_has_tags

Checks if an mp3 file has tag information.<br/>
Returns 1 on success, 0 on failure.

```c
if (mp3_id3_has_tags("song.mp3")) {
  fprintf(stdout, "Song has tags!\n");
} else {
  fprintf(stdout, "Song has no tags!\n");
}
```

### mp3_id3_read_tag and mp3_id3_file_read_tag

Read a specific tag from an mp3 file.<br/>
Returns a null terminated string which contains the data of the requested tag or `NULL` on failure.

Note that the returned string has to be freed after usage.

```c
char *artist = mp3_id3_read_tag("song.mp3", ARTIST);
if (artist) {
  fprintf(stdout, "Artist: %s\n", artist);
  free(artist);
  artist = NULL;
} else {
  fprintf(stderr, "error: %s\n", mp3_id3_failure_reason());
}
```

### mp3_id3_read_tags and mp3_id3_file_read_tags

Read all tags from an mp3 file.<br/>
Returns 1 on success, 0 on failure.<br/>
The supplied mp3_id3_tags structure will contain the tag information.

```c
mp3_id3_tags tags;
if (mp3_id3_read_tags("song.mp3", &tags)) {
  fprintf(stdout, "%s by %s\n", tags.title, tags.artist);
} else {
  fprintf(stderr, "error: %s\n", mp3_id3_failure_reason());
}
```