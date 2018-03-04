#include <string.h>         //strcpy
#include <stdio.h>          //perror
#include <sys/ioctl.h>      //low-level access to Linux network devices
#include <net/if.h>         //low-level access to Linux network devices
#include <sys/types.h>      //Required for some historical BSD implementations
#include <sys/socket.h>     //socket file descriptors
#include <ifaddrs.h>        //getifaddrs()
#include <stdlib.h>         //exit()
#include <net/ethernet.h>   //the L2 protocols (e.g. ETH_P_802_3) UNUSED
#include <unistd.h>         //close()

#define ADDR_LEN_IPV4 6         //Number of bytes in physical address for IPv4
#define ADDR_LEN_IPV6 8         //Number of bytes in physical address for IPv6
#define ADDR_LEN ADDR_LEN_IPV4  //Set equal to IP version in use

//Linked list for hardware addresses & interface names/indices
struct hwaddr_ll
{
    int ifindex;                        // Index of interface
    char name[IFNAMSIZ];                // Name of interface
    unsigned char addr[ADDR_LEN];       // Physical-layer address
    struct hwaddr_ll *next;             // next ptr for linked list
};

/*  Function to find internet interfaces and store their names,
    interface index, as well as hardware addresses in a linked list. */

int hwaddr_extract(struct hwaddr_ll** head)
{
    struct hwaddr_ll* it = NULL;
    struct ifaddrs *ifap;

    /* Get interface addresses, store in ifap (linked list) */
    if(getifaddrs(&ifap) == -1)
        perror("hwaddr_extract: getifaddrs()");

    /* Open a throw-away socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0x00);
    if(sock == -1)
        perror("hwaddr_extract: socket()");

    /* Extract hardware address from ifap (linked list) */
    for(struct ifaddrs *ifa = ifap; ifa; ifa = ifa -> ifa_next) {
        if(ifa -> ifa_addr -> sa_family == AF_INET) {
            struct ifreq if_idx;
            struct hwaddr_ll *node = (struct hwaddr_ll*) malloc(
                sizeof(struct hwaddr_ll));
            node -> next = NULL;

            /* Get & store interface name */
            strcpy(node -> name, ifa->ifa_name);
            strcpy(if_idx.ifr_name, ifa->ifa_name);

            /* Get & store interface index */
            if( ioctl( sock, SIOCGIFINDEX, &if_idx ) != -1 ) {
                node -> ifindex = if_idx.ifr_ifindex;
            } else {
                perror("hwaddr_extract: ioctl()");
            }

            /* Get & store hardware address */
            if( ioctl( sock, SIOCGIFHWADDR, &if_idx ) != -1 ) {
                for(int i = 0; i < ADDR_LEN; i++)
                    node -> addr[i] = if_idx.ifr_ifru.ifru_hwaddr.sa_data[i];
            } else {
                perror("hwaddr_extract: ioctl()");
            }

            /* Traverse linked-list and add new node*/
            if(it == NULL) {
                *head = node;
                it = *head;
            } else {
                it -> next = node;
                it = node;
            }

        }
    }

    freeifaddrs(ifap);
    close(sock);
    return 0;
}

int main(int argc, char* argv[]) {

    /*  Note: This example assumes ADDR_LEN is set to ADDR_LEN_IPV4.
        If you want to use ADDR_LEN_IPV6, modify the printf() statement
        that prints the address to print two additional bytes. */

    struct hwaddr_ll *list = NULL;
    hwaddr_extract(&list);

    /* Iterate through the list and print  */
    for(struct hwaddr_ll *it = list; it != NULL; it = it -> next) {
        printf("Name: [%s]\t\t", it -> name);
        printf("Index [%d]\t", it -> ifindex);
        printf("Address [%X:%X:%X:%X:%X:%X]\n",
            it -> addr[0], it -> addr[1], it -> addr[2],
            it -> addr[3], it -> addr[4], it -> addr[5]);
    }

    /* Free the memory used by the linked list */
    for(struct hwaddr_ll *it = list; it;) {
        struct hwaddr_ll *jt = it;
        it = it -> next;
        free(jt);
    }
}
