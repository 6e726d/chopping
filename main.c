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
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>

#include "interface.h"
#include "wireless.h"

#define PROGRAM_NAME "chopping"
#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define MAX_CHANNELS 255
#define CHANNEL_HOPPING_RATE 1000

/*
 * Prints program usage information.
 */
void show_usage(void) {
    fprintf(stdout,
            "%s %d.%d\n\n"
            "Usage:\n"
            "\t%s -i <interface> -c <channel list> -h <hop> -t <timeout>\n\n"
            "Example:\n"
            "\t%s -i wlp2s0 -c 1,6,11 -t 100\n",
            PROGRAM_NAME,
            VERSION_MAJOR,
            VERSION_MINOR,
            PROGRAM_NAME,
            PROGRAM_NAME);
}

/*
 * Prints all available wireless interfaces.
 */
void show_wireless_ifaces(void) {
    DIR* d;
    struct dirent *dir;

    d = opendir("/sys/class/net/");
    if (d) {
        printf("\nAvailable Wireless Interfaces:\n");
        while ((dir = readdir(d)) != NULL) {
            char protocol_name[PROTOCOL_NAME_MAX_LENGTH];
            if(get_wireless_iface_protocol(dir->d_name, protocol_name) == 0) {
                char iface_mode[255];
                unsigned char num_channels;
                unsigned char sup_channels[MAX_CHANNELS];

                if(get_wireless_iface_mode(dir->d_name, iface_mode) != 0)
                    continue;

                if(get_wireless_iface_supported_channels(dir->d_name,
                                                         &num_channels,
                                                         sup_channels) != 0)
                    continue;

                printf(" - %s [%s]\n", dir->d_name, protocol_name);
                printf("\tMode: %s\n", iface_mode);

                if(is_device_in_monitor_mode(dir->d_name) == true) {
                    unsigned char channel;
                    unsigned short int freq;
                    if(get_wireless_iface_freq(dir->d_name, &freq) == 0) {
                        channel = get_channel_from_frequency(freq);
                        printf("\tChannel: %d\n", channel);
                    }
                }

                printf("\tSupported Channels: ");
                for(int i = 0; i < num_channels; i++) {
                    printf("%d", sup_channels[i]);
                    if(i < num_channels - 1)
                        printf(",");
                }
                printf("\n\n");
            }
        }
        closedir(d);
    }
}

/*
 * Process channels argument comma separated values string.
 */
int process_channels_argument(char* channel_list,
                              unsigned char* num_channels,
                              unsigned char* channels) {
    char* token;
    unsigned char idx = 0;
    const char* delimiter = ",";

    token = strtok(channel_list, delimiter);
    while(token != NULL) {
        if(idx >= MAX_CHANNELS)
            return -1;
        unsigned char channel = atoi(token);
        if(channel > 0 || channel < 255)
            memcpy(channels + idx, &channel, 1);
            idx++;
        token = strtok(NULL, delimiter);
    }

    if(idx > 0)
        *num_channels = idx + 1;

    return 0;
}

/*
 * Perform channel hopping based on function parameters.
 */
int perform_channel_hopping(char* iface_name,
                            unsigned char num_channels,
                            unsigned char* sup_channels,
                            unsigned int hop,
                            unsigned int timeout) {
    printf("Channels: ");
    for(int i = 0; i < num_channels - 1; i++) {
        printf("%d", sup_channels[i]);
        if(i < num_channels - 2)
            printf(",");
    }
    printf("\n");

    unsigned char idx = 0;

    while(true) {
        if(idx >= num_channels - 1)
            idx = 0;
        if(set_wireless_iface_channel(iface_name, sup_channels[idx]) != 0) {
            unsigned char channel;
            unsigned short int freq;
            if(get_wireless_iface_freq(iface_name, &freq) == 0) {
                channel = get_channel_from_frequency(freq);
                printf("Error: unable to set %s to channel %d -> %d\n",
                       iface_name,
                       sup_channels[idx],
                       channel);
            }
        } else {
            printf("%s -> %d\n", iface_name, sup_channels[idx]);
        }
        usleep(CHANNEL_HOPPING_RATE * timeout);
        idx += hop;
    }
}

int main(int argc, char** argv) {
    if(geteuid() != 0) {
        fprintf(stderr, "Error: Need root privileges.\n");
        return -1;
    }

    int option = 0;
    unsigned int hop = 1;
    char* iface_name = NULL;
    char* channels_arg = NULL;
    unsigned int timeout = CHANNEL_HOPPING_RATE;

    while ((option = getopt(argc, argv,"i:c:h:t:")) != -1) {
        switch (option) {
            case 'i':
                iface_name = optarg;
                break;
            case 'c':
                channels_arg = optarg;
                break;
            case 'h':
                hop = atoi(optarg);
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            default:
                show_usage();
                show_wireless_ifaces();
                exit(EXIT_FAILURE);
        }
    }

    if(iface_name == NULL) {
        show_usage();
        show_wireless_ifaces();
        exit(EXIT_FAILURE);
    }

    unsigned char num_channels = 0;
    unsigned char channels[MAX_CHANNELS];
    memset(channels, 0, sizeof(channels));

    if(channels_arg != NULL) {
        if(process_channels_argument(channels_arg, &num_channels, channels) != 0) {
            printf("Error: Too many channels. List must be less that %d.\n", MAX_CHANNELS);
            exit(EXIT_FAILURE);
        }
    } else {
        if(get_wireless_iface_supported_channels(iface_name, &num_channels, channels) != 0) {
            printf("Error: Unable to get supported channels for %s.", iface_name);
            return -1;
        }
    }

    if(is_interface_valid(iface_name) == false)
    {
        fprintf(stderr, "Error: %s interface not found.\n", iface_name);
        return -1;
    }

    char protocol_name[PROTOCOL_NAME_MAX_LENGTH];

    if(get_wireless_iface_protocol(iface_name, protocol_name) != 0) {
        fprintf(stderr, "Error: %s is not a wireless interface.", iface_name);
        exit(EXIT_FAILURE);
    }

    if(!is_device_in_monitor_mode(iface_name)) {
        fprintf(stderr, "Error: %s is not in monitor mode.\n", iface_name);
        exit(EXIT_FAILURE);
    }

    printf("Using Interface: %s\n", iface_name);
    printf("Hop: %d\n", hop);
    printf("Hop Timeout: %d\n", timeout);

    perform_channel_hopping(iface_name, num_channels, channels, hop, timeout);

    return 0;
}
