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

#ifndef CHOPPING_WIRELESS_H
#define CHOPPING_WIRELESS_H

#define PROTOCOL_NAME_MAX_LENGTH 510

#define MODE_AUTO    "Auto"
#define MODE_ADHOC   "Ad-Hoc"
#define MODE_INFRA   "Infrastructure"
#define MODE_MASTER  "Master"
#define MODE_REPEAT  "Repeater"
#define MODE_SECOND  "Secondary"
#define MODE_MONITOR "Monitor"
#define MODE_MESH    "Mesh"

#define FREQUENCY_DELTA 5 // MHz
#define BG_BASE_FREQUENCY 2407
#define BG_LOWER_FREQUENCY 2412 // Channel 1
#define BG_UPPER_FREQUENCY 2472 // Channle 13
#define BG_CH14_FREQUENCY 2484 // Channel 14
#define A_BASE_FREQUENCY 5000
#define A_LOWER_FREQUENCY 5170 // Channel 34
#define A_UPPER_FREQUENCY 5825 // Channel 165

// Returns wireless interface protocol name.
int get_wireless_iface_protocol(const char*, char*);
// Returns wireless interface mode.
int get_wireless_iface_mode(const char*, char*);
// Returns wireless interface frequency.
int get_wireless_iface_freq(const char*, unsigned short int*);
// Set wireless interface channel.
int set_wireless_iface_channel(const char*, unsigned char);
// Returns wireless interface supported channels.
int get_wireless_iface_supported_channels(const char*,
                                          unsigned char*,
                                          unsigned char*);
// Returns channel from frequency.
unsigned char get_channel_from_frequency(unsigned short int);

// Verifies if wireless interface is in monitor mode.
bool is_device_in_monitor_mode(const char*);

#endif //CHOPPING_WIRELESS_H
