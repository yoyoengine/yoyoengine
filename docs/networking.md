---
tags:
  - c
---

# Networking

yoyoengine extends the SDL_net API to provide wrappers around TCP and UDP sockets.

There is no planned Lua support for networking at this time.

Please note for P2P you will need to facilitate NAT punching yourself.

## Example

```c

/*
    This is an example of using the yoyoengine C
    scripting callbacks API to handle networking

    note: I haven't tested this exact snippet, so
    this is just a general idea (but im 99% certain
    this will work)
*/

#include <SDL_net.h>
#include <yoyoengine/yoyoengine.h>

UDPsocket udp_socket;
IPaddress ip_address;

void yoyo_post_init() {
    SDLNet_ResolveHost(&ip_address, "localhost", 1234);
    udp_socket = SDLNet_UDP_Open(0);
}

void yoyo_pre_frame() {

    // RECEIVE

    UDPpacket *packet = SDLNet_AllocPacket(512);
    if (SDLNet_UDP_Recv(udp_socket, packet)) {
        printf("Received packet from %s:%d\n", packet->address.host, packet->address.port);
        printf("Data: %s\n", (char *)packet->data);
    }

    SDLNet_FreePacket(packet);

    // SEND

    packet = SDLNet_AllocPacket(512);
    packet->address = ip_address;
    strcpy((char *)packet->data, "Hello, world!");
    packet->len = strlen((char *)packet->data) + 1;
    SDLNet_UDP_Send(udp_socket, -1, packet);
    SDLNet_FreePacket(packet);
}

```
