/*
 * Copyright (C) 2017 Ben Cordero <bencord0@condi.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <inttypes.h>
#include <stdio.h>      
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>

#define IFNAMELEN 100
#define IFLINELEN 1000

void find_ip(char addressBuffer[]) {
    FILE *routes;
    char *line = NULL;
    size_t n = 0;
    uint32_t destination = 0, gateway = 0;
    char ifname[IFNAMELEN];
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    /* Find the interface with a gateway to the internet */
    routes = fopen("/proc/net/route", "r");
    if (!routes) {
        return;
    }

    while (getline(&line, &n, routes) != -1) {
        if (sscanf(line, "%s %d %d", ifname, &destination, &gateway) == 3) {
            if (destination == 0) { /* the internet */
                break;
            }
        }
    }
    fclose(routes);
    free(line);
    
    /* Find the IP on the interface */
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (!strcmp(ifa->ifa_name, ifname) && ifa->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        }
    }
    freeifaddrs(ifAddrStruct);
}

int main (int argc, char * const *argv) {
    FILE *ipdestfile = NULL;
    int writeip = 0, opt = 0;
    char addressBuffer[INET_ADDRSTRLEN];
    bzero(addressBuffer, INET_ADDRSTRLEN);
    
    while (!strncmp(addressBuffer, "", 1))
      find_ip(addressBuffer);

    while ((opt = getopt(argc, argv, "w:")) != -1) {
       switch (opt) {
           case 'w':
               writeip = 1;
               ipdestfile = fopen(optarg, "w");
               break;
           default:
               exit(1);
               break;
       }
    }
    
    if (writeip) {
        fprintf(ipdestfile, "MYIP=%s\n", addressBuffer);
        fclose(ipdestfile);
    } else {
        printf("%s\n", addressBuffer); 
    }
    return 0;
}
