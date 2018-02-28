/*
 * Copyright (c) 2018 Andrés Blanco (6e726d)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include "interface.h"
#include "wireless.h"

/*
 * Returns wireless interface protocol name on protocol_name parameter.
 * On success zero is returned.
 */
int get_wireless_iface_protocol(const char* interface_name,
                                char* protocol_name)
{
    int sd;
    struct iwreq wrq;

    if(interface_name == NULL || protocol_name == NULL)
        return -1;

    memset(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, interface_name, IFNAMSIZ);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    if(ioctl(sd, SIOCGIWNAME, &wrq) != -1)
    {
        strncpy(protocol_name, wrq.u.name, IFNAMSIZ);
        close(sd);
        return 0;
    }

    close(sd);
    return -1;
}

/*
 * Returns wireless inferface mode on mode parameter. On success zero
 * is returned.
 */
int get_wireless_iface_mode(const char* interface_name, char* mode)
{
    int sd;
    struct iwreq wrq;

    if(interface_name == NULL || mode == NULL)
        return -1;

    memset(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, interface_name, IFNAMSIZ);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    if(ioctl(sd, SIOCGIWMODE, &wrq) != -1)
    {
        switch(wrq.u.mode)
        {
            case IW_MODE_AUTO:
                sprintf(mode, MODE_AUTO);
                break;
            case IW_MODE_ADHOC:
                sprintf(mode, MODE_ADHOC);
                break;
            case IW_MODE_INFRA:
                sprintf(mode, MODE_INFRA);
                break;
            case IW_MODE_MASTER:
                sprintf(mode, MODE_MASTER);
                break;
            case IW_MODE_REPEAT:
                sprintf(mode, MODE_REPEAT);
                break;
            case IW_MODE_SECOND:
                sprintf(mode, MODE_SECOND);
                break;
            case IW_MODE_MONITOR:
                sprintf(mode, MODE_MONITOR);
                break;
            case IW_MODE_MESH:
                sprintf(mode, MODE_MESH);
                break;
            default:
                close(sd);
                return -1;
        }
        close(sd);
        return 0;
    }

    close(sd);
    return -1;
}

/*
 * Returns wireless inferface frequency on freq parameter. On success
 * zero is returned.
 */
int get_wireless_iface_freq(const char* interface_name,
                            unsigned short int* freq)
{
    int sd;
    struct iwreq wrq;

    if(interface_name == NULL || freq == NULL)
        return -1;

    memset(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, interface_name, IFNAMSIZ);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    if(ioctl(sd, SIOCGIWFREQ, &wrq) != -1)
    {
        *freq = wrq.u.freq.m;
        close(sd);
        return 0;
    }

    close(sd);
    return -1;
}

/*
 * Set wireless interface channel defined on channel parameter. On success
 * zero is returned.
 */
int set_wireless_iface_channel(const char* interface_name,
                               const unsigned char channel)
{
    int sd;
    struct iwreq wrq;

    if(interface_name == NULL || channel == 0)
        return -1;

    memset(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, interface_name, IFNAMSIZ);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    wrq.u.freq.m = channel;

    if(ioctl(sd, SIOCSIWFREQ, &wrq) != -1)
    {
        close(sd);
        return 0;
    }

    close(sd);
    return -1;
}

/*
 * Returns wireless channels on channels parameter supported by the interface
 * defined on interface_name parameter. On success zero is returned.
 */
int get_wireless_iface_supported_channels(const char *interface_name,
                                          unsigned char* num_channels,
                                          unsigned char* channels)
{
    int i;
    int sd;
    struct iwreq wrq;
    char buffer[sizeof(struct iw_range) * 2];

    memset(buffer, 0, sizeof(buffer));
    memset(&wrq, 0, sizeof(struct iwreq));

    wrq.u.data.pointer = buffer;
    wrq.u.data.length = sizeof(buffer);
    wrq.u.data.flags = 0;

    strncpy(wrq.ifr_name, interface_name, IFNAMSIZ);

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sd == -1)
        return -1;

    if(ioctl(sd, SIOCGIWRANGE, &wrq) >= 0)
    {
        struct iw_range *range = (struct iw_range *)buffer;

        *num_channels = range->num_frequency;
        for(i = 0; i < range->num_frequency; i++)
            memcpy(channels + i, &range->freq[i].i, 1);
    }

    close(sd);

    return 0;
}

/*
 * Returns wireless channel for the frequency parameter.
 */
unsigned char get_channel_from_frequency(unsigned short int frequency)
{
    if(frequency >= BG_LOWER_FREQUENCY && frequency <= BG_UPPER_FREQUENCY)
        return (frequency - BG_BASE_FREQUENCY) / FREQUENCY_DELTA;
    else if(frequency == BG_CH14_FREQUENCY)
        return 14;
    else if(frequency >= A_LOWER_FREQUENCY && frequency <= A_UPPER_FREQUENCY)
        return (frequency - A_BASE_FREQUENCY) / FREQUENCY_DELTA;
    return 0;
}


/*
 * Verifies if iface_name is a in monitor mode.
 */
bool is_device_in_monitor_mode(const char *interface_name) {
    char mode[255];

    if(get_wireless_iface_mode(interface_name, mode) != 0)
        return false;

    if(memcmp(mode, MODE_MONITOR, strlen(MODE_MONITOR)) != 0)
        return false;

    return true;
}
