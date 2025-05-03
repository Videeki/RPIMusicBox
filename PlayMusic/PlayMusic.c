/****************************************************************************************************/
/*                                                                                                  */
/*   Source: http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html                  */
/*   Help: https://stackoverflow.com/questions/26007700/play-mp3-on-raspberry-with-mpg123-and-c     */
/*                                                                                                  */
/*                                                                                                  */
/*   Compiling -> Linux: gcc -O2 PlayMusic.c -o PlayMusic -lmpg123 -lao                             */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
#include "PlayMusic.h"

static mpg123_handle *mh;
static unsigned char *buffer;
static size_t buffer_size;

static int driver;
static ao_device *dev;


int initMusic()
{
    // initializations //
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    int err;
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    return 0;
}

int playMusic(const char *path)
{
    // open the file and get the decoding format //
    int channels, encoding;
    long rate;
    mpg123_open(mh, path);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    // set the output format and open the output device //
    ao_sample_format format;
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);

    // decode and play //
    size_t done;
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, buffer, done);

    return 0;
}

int closeMusic()
{
    // clean up //
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();

    return 0;
}


/**********************************************     Main     **********************************************


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("There are less parameter added what required\n");
        return -1;
    }

    initMusic();

    playMusic(argv[1]);

    closeMusic();

    return 0;
}

**********************************************************************************************************/