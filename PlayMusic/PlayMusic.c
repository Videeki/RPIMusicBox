/*************************************************************************************************/
/*                                                                                               */
/*   Source: http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html               */
/*   Help: https://stackoverflow.com/questions/26007700/play-mp3-on-raspberry-with-mpg123-and-c  */
/*                                                                                               */
/*                                                                                               */
/*   Compiling -> Linux: gcc -O2 PlayMusic.c -o PlayMusic -lmpg123 -lao                          */
/*   Compiling -> Windows: gcc PlayMusic.c -o PlayMusic.exe -lwinmm                              */
/*                                                                                               */
/*************************************************************************************************/
#include "PlayMusic.h"


#ifdef __linux__
mpg123_handle *mh;
unsigned char *buffer;
size_t buffer_size;
size_t done;
int err;

int driver;
ao_device *dev;

ao_sample_format format;
int channels, encoding;
long rate;
#endif

int initMusic()
{
    #ifdef _WIN32
        // Not neccesary the initialization //

    #elif __linux__
        // initializations //
        ao_initialize();
        driver = ao_default_driver_id();
        mpg123_init();
        mh = mpg123_new(NULL, &err);
        buffer_size = mpg123_outblock(mh);
        buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    #else
        printf("Sorry, the system are not implemented yet... :'(\n")

    #endif

    return 0;
}

int playMusic(char *path)
{
    #ifdef _WIN32
        char cmd[1024];
        sprintf(cmd, "open \"%s\" type mpegvideo alias mp3", path);
        
        mciSendString(cmd, NULL, 0, NULL);
    
        mciSendString("play mp3 wait", NULL, 0, NULL);

        mciSendString("close mp3", NULL, 0, NULL);

    #elif __linux__
        // open the file and get the decoding format //
        mpg123_open(mh, path);
        mpg123_getformat(mh, &rate, &channels, &encoding);

        // set the output format and open the output device //
        format.bits = mpg123_encsize(encoding) * BITS;
        format.rate = rate;
        format.channels = channels;
        format.byte_format = AO_FMT_NATIVE;
        format.matrix = 0;
        dev = ao_open_live(driver, &format, NULL);

        // decode and play //
        while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
            ao_play(dev, buffer, done);

    #else
        printf("Sorry, the system are not implemented yet... :'(\n")

    #endif

    return 0;
}

int closeMusic()
{
    #ifdef _WIN32
        // Not neccesary the initialization //

    #elif __linux__
        // clean up //
        free(buffer);
        ao_close(dev);
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        ao_shutdown();

    #else
        printf("Sorry, the system are not implemented yet... :'(\n")

    #endif

    return 0;
}
