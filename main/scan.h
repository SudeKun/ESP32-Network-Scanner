#ifndef SCAN_H
#define SCAN_H

// device information
typedef struct deviceInfo{
    int online;
    uint32_t ip;
    uint8_t mac[6];
} deviceInfo;

// scan loop
void arpScan(void *);

// gets
uint32_t getMaxDevice(); // subnet maxium device
uint32_t getDeviceCount(); // onlie device count update after each loop
deviceInfo * getDeviceInfos(); // get an array of information

#endif
