# hwaddr_extract
Extracts internet interface names, indices, and hardware addresses and stores them in a linked list.

## What is hwaddr_extract?

hwaddr_extract is a small c program that performs system calls to learn all internet interfaces
names, indices and hardware addresses. It stores this information in a linked list pointed to by
a 'struct hwaddr_ll*' pointer.

### How to compile and run hwaddr_extract

Compiling the program is fairly simple.
Clone the repository, or download "main.c" and place it somewhere. Then,
```
gcc -std=gnu99 main.c -o hwaddr_extract
```
Note that the -std=gnu99 is important. I have forgotten why, but if you omit it, chances
are you will have "undefined"-errors. Once compiling is done, run it as you would any other program.
```
./hwaddr_extract
```

You should be presented with output that looks something like this:
```
Name: [lo]				Index [1]	Address [0:0:0:0:0:0]
Name: [wlp2s0]		Index [3]	Address [0:28:F8:3F:BA:B]
Name: [virbr0]		Index [4]	Address [52:54:0:B8:12:AD]
```

#### Why would i need this? Couln't I just use ifconfig to find this information?
Well, of course you could. I have uploaded this program in case some developer such
as yourself might find it useful for some task that they are working on.
I developed it because I plan to use this later in a different program to find
all interfaces which are Ethernet interfaces.
