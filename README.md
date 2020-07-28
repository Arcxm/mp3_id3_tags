# About mp3_id3_tags

`mp3_id3_tags` is a public domain single header C/C++ library to read ID3 Tags from a MP3.

After looking at multiple single header C/C++ libraries and using them (especially the [stb](https://github.com/nothings/stb) ones), I decided to try and make one myself.

# Supported tags:
Title, Artist, Album, Year, Comment and Genre.

# Defines:

These need to be defined before the `mp3_id3_tags.h` file is included.

Define `MP3_ID3_TAGS_IMPLEMENTATION` in *one* C or C++ file to create the implementation. <br />
Define `MP3_ID3_TAGS_USE_GENRES` to be able to read the Genre tag.

# Documentation

Note: Each function comes in two variants.

The first one opens a file handle by itself and closes it afterwards. E.g. `mp3_id3_has_tags`. <br />
The second one (with 'file' in the function name) uses an already existing file handle. E.g. `mp3_id3_file_has_tags`.

### mp3_id3_has_tags and mp3_id3_file_has_tags

Check, whether or not the file has ID3 Tags.

Returns 1 on success, or 0 on failure.

```c
if (mp3_id3_has_tags("mySong.mp3")) {
  printf("ID3 Tags found!");
} else {
  printf("No ID3 Tags found!");
}
```

### mp3_id3_read_tag and mp3_id3_file_read_tag

Read a specific single tag from a MP3. <br />
Note: the returned char* has to be freed after it's usage to avoid a memory leak.

Returns a newly allocated char* which contains the tag's data, or NULL if it fails.

```c
char *artist = mp3_id3_read_tag("mySong.mp3", MP3_ID3_TAG_ARTIST);
if (artist) {
  printf("Artist: %s", artist);
  free(artist);
  artist = NULL;
} else {
  perror("Failed to read artist tag");
}
```

### mp3_id3_read_tags and mp3_id3_file_read_tags

Read the ID3 tags from a MP3.
The supplied mp3_id3_tags structure will contain the tag data.

Returns 1 on success, or 0 on failure.

```c
mp3_id3_tags tags;
if (mp3_id3_read_tags("mySong.mp3", &tags)) {
  // Do something with the tag data
} else {
  perror("Failed to read tag data");
}
```
