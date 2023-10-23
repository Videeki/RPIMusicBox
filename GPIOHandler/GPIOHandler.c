// Compiling: gcc GPIOHandler.c -o Builds/GPIOHandler
#include "GPIOHandler.h"

int initPIN(int pinNr)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    char initPin[33];
    FILE *export;
    export = fopen("/sys/class/gpio/export", "w");

    if (export == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(export, "%d", pinNr);
    fclose(export);

    sleep(1);

    #endif

    return 0;
}

int setupPIN(int pinNr, char *mode)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    char setupPin[50];
    FILE *direction;
    sprintf(setupPin, "/sys/class/gpio/gpio%d/direction", pinNr);
    direction = fopen(setupPin, "w");

    if (direction == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(direction, "%s", mode);
    fclose(direction);
    
    #endif

    return 0;
}

int writePIN(int pinNr, int value)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    char pinPath[50];
    FILE *writePin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    writePin = fopen(pinPath, "w");

    if (writePin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(writePin, "%d", value);
    fclose(writePin);
    
    #endif

    return 0;
}

int readPIN(int pinNr)
{
    #ifdef _WIN32
    int value;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    int value;
    char pinPath[50];
    FILE *readPin;
    sprintf(pinPath, "/sys/class/gpio/gpio%d/value", pinNr);
    
    readPin = fopen(pinPath, "r");
    
    if (readPin == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fscanf(readPin, "%d", &value);
    
    fclose(readPin);
    
    #endif

    return value;
}

int deinitPIN(int pinNr)
{
    #ifdef _WIN32
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    char deinitPin[35];
    FILE *unexport;
    unexport = fopen("/sys/class/gpio/unexport", "w");

    if (unexport == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(unexport, "%d", pinNr);
    fclose(unexport);

    #endif

    return 0;
}

int initGPIO(struct gpiohandle_request* rq, int pins[], int nrOfPins, int direction)
{
    #ifdef _WIN32
    int ret = 0;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    int fd, ret, i;
    // open the device
    fd = open(DEV_NAME, O_RDONLY);
    if (fd < 0)
    {
        printf("Unabled to open %s: %s", DEV_NAME, strerror(errno));
        return fd;
    }
    
    for(i = 0; i < nrOfPins; i++)
    {
        rq->lineoffsets[i] = pins[i];
    }

    (direction == INPUT) ? (rq->flags = GPIOHANDLE_REQUEST_INPUT) : (rq->flags = GPIOHANDLE_REQUEST_OUTPUT);
    rq->lines = nrOfPins;
    ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, rq);
    close(fd);

    if (ret == -1)
    {
        printf("Unable to get line handle from ioctl : %s", strerror(errno));
        return ret;
    }

    #endif

    return ret;
}

int writeGPIO(struct gpiohandle_request* rq, int* values)
{
    #ifdef _WIN32
    int ret = 0;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    struct gpiohandle_data data;
    int ret, i;

    for(i = 0; i < rq->lines; i++)
    {
        data.values[i] = values[i];
    }

    ret = ioctl(rq->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (ret == -1)
    {
        printf("Unable to set line value using ioctl : %%s", strerror(errno));
        return ret;
    }
    else
    {
         usleep(2000000);
    }
    
    #endif

    return ret;
}

int readGPIO(struct gpiohandle_request* rq, int* values)
{
    #ifdef _WIN32
    int ret = 0;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    struct gpiohandle_data data;
    int nrOfPins = rq->lines;
    
    int ret, i;

    ret = ioctl(rq->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
    if (ret == -1)
    {
        printf("Unable to get line value using ioctl : %s", strerror(errno));
        return ret;
    }
    else
    {
        for(i = 0; i < nrOfPins; i++)
        {
            values[i] = data.values[i];
        }
    }

    #endif

    return ret;
}

int pollGPIO(int offset)
{
    #ifdef _WIN32
    int ret = 0;
	printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    struct gpioevent_request gpioevent_rq;
    struct pollfd pfd;
    int fd, ret;

    fd = open(DEV_NAME, O_RDONLY);
    if (fd < 0)
    {
        printf("Unabled to open %s: %s\n", DEV_NAME, strerror(errno));
        return fd;
    }

    gpioevent_rq.lineoffset = offset;
    gpioevent_rq.handleflags = GPIOHANDLE_REQUEST_INPUT;
    gpioevent_rq.eventflags = GPIOEVENT_EVENT_RISING_EDGE;
    ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &gpioevent_rq);
    close(fd);
    if (ret == -1)
    {
        printf("Unable to get line event from ioctl : %s\n", strerror(errno));
        return ret;
    }
    
    pfd.fd = gpioevent_rq.fd;
    pfd.events = POLLIN;
    ret = poll(&pfd, 1, -1);
    if (ret == -1)
    {
        printf("Error while polling event from GPIO: %s\n", strerror(errno));
    }
    else if (pfd.revents & POLLIN)
    {
        printf("Rising edge event on GPIO offset: %d, of %s\n", offset, DEV_NAME);
    }
    else
    {
        printf("Polling On: %d, of %s\n", offset, DEV_NAME);     
    }
        
    close(gpioevent_rq.fd);

    #endif

    return ret;
}

int closeGPIO(struct gpiohandle_request* rq)
{
    #ifdef _WIN32
    printf("The GPIO Handling has not implemented yet on Windows.\n");

    #elif __linux__
    close(rq->fd);

    #endif

    return 0;
}