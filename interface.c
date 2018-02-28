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

#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "interface.h"

/*
 * Verifies if iface_name is a valid network interface using the
 * sys file system.
 */
bool is_interface_valid(const char* iface_name) {
    bool result = false;

    DIR* d;
    struct dirent *dir;

    d = opendir("/sys/class/net/");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(iface_name, dir->d_name) == 0)
                result = true;
        }
        closedir(d);
    }

    return result;
}

/*
 * Causes the driver for this interface to be shut down.
 */
bool interface_down(const char* iface_name)
{
    int sd;
    struct ifreq req;

    sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
        return false;

    memset(&req, 0, sizeof(req));

    strncpy(req.ifr_name, iface_name, strlen(iface_name));

    req.ifr_flags = IFF_BROADCAST | IFF_MULTICAST;
    if (ioctl(sd, SIOCSIFFLAGS, (char *)&req) < 0)
    {
        close(sd);
        return false;
    }

    close(sd);
    return true;
}

/*
 * Causes the interface to be activated.
 */
bool interface_up(const char* iface_name)
{
    int sd;
    struct ifreq req;

    sd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
        return false;

    memset(&req, 0, sizeof(req));

    strncpy(req.ifr_name, iface_name, strlen(iface_name));

    req.ifr_flags = IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST;
    if (ioctl(sd, SIOCSIFFLAGS, (char *)&req) < 0)
    {
        close(sd);
        return false;
    }

    close(sd);
    return true;
}
