# chopping

Linux 802.11 channel hopping utility.

## Why?

Because when I'm doing research with 802.11 stuff and need to perform channel hopping using tools such as [Kismet](https://www.kismetwireless.net/) or [Aircrack-ng](https://www.aircrack-ng.org/) only for this task feels like killing a mosquito with a cannon.
And because sometimes I want to use tcpdump or tshark/wireshark and just need something simple to perform channel hopping with almost no memory or cpu consumption.

## Build

$ git clone https://github.com/6e726d/chopping.git  
$ cd chopping  
$ mkdir build  
$ cd build  
$ cmake ../  
$ make  

## Usage Examples

Perform channel hopping using interface wlp2s0 on all supported channels and hop every 5 seconds.

$ sudo chopping -i wlp2s0 -t 5000

Perform channel hopping using interface wlan0 on channels 1,6,11 and 56.

$ sudo chopping -i wlan0 -c 1,6,11,56

## Authors

* **Andrés Blanco** (6e726d)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
