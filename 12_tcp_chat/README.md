# Cross-compile TCP chat for Raspberry Pi 4

Build `client.c` and `server.c` on an Ubuntu/Debian Linux development computer
(the **host**), then run the ARM executables on a Raspberry Pi 4 (the **Pi**).
Each program takes the server's numeric IPv4 address and TCP port as arguments.
The server accepts one client per run. These sources use `poll`, so no extra
libraries or `-pthread` option are required.

## 1. Check the Pi's OS architecture

Run on the **Pi**:

```sh
dpkg --print-architecture
hostname -I
```

Use `arm64` instructions for a 64-bit OS or `armhf` instructions for a 32-bit
OS. The Pi 4 CPU supports both; the installed OS determines which build to use.
Record the Pi's LAN IPv4 address. Examples below use `192.168.1.253`; replace
it if your Pi has a different address. Replace `YOUR_PI_USER` with its username.

## 2. Install tools on the host

For **arm64**, run on the **host**:

```sh
sudo apt update
sudo apt install gcc-aarch64-linux-gnu libc6-dev-arm64-cross file openssh-client
```

For **armhf**, run on the **host**:

```sh
sudo apt update
sudo apt install gcc-arm-linux-gnueabihf libc6-dev-armhf-cross file openssh-client
```

## 3. Cross-compile both programs on the host

From the repository root on the **host**:

```sh
cd 12_tcp_chat
mkdir -p build
```

For **arm64**:

```sh
aarch64-linux-gnu-gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -static client.c -o build/client
aarch64-linux-gnu-gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -static server.c -o build/server
```

For **armhf**, use these commands instead:

```sh
arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -static client.c -o build/client
arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -static server.c -o build/server
```

`-static` includes the required C library code in these executables, avoiding
a dependency on the Pi's installed glibc version. This example uses numeric
IPv4 addresses and does not require hostname resolution.

Verify on the **host**:

```sh
file build/client build/server
```

Expect `ELF 64-bit` and `ARM aarch64` for arm64, or `ELF 32-bit` and `ARM`
for armhf, along with `statically linked`.

## 4. Transfer both executables to the Pi

Enable SSH on the Pi if needed and connect both computers to the same LAN.
From `12_tcp_chat` on the **host**:

```sh
ssh YOUR_PI_USER@192.168.1.253 'mkdir -p ~/12_tcp_chat'
scp build/client build/server YOUR_PI_USER@192.168.1.253:~/12_tcp_chat/
```

## 5. Test both programs on the Pi

Open **two terminals** on the host and SSH into the Pi from each:

```sh
ssh YOUR_PI_USER@192.168.1.253
```

In the **first Pi session**, start the server:

```sh
cd ~/12_tcp_chat
chmod +x server client
./server 127.0.0.1 5050
```

Wait for `Listening on 127.0.0.1:5050; waiting for one chat client...`.

In the **second Pi session**, start the client:

```sh
cd ~/12_tcp_chat
./client 127.0.0.1 5050
```

Type a message and press Enter in either session; it appears in the other.
Here `127.0.0.1` works because both programs run on the same Pi.
Press Ctrl+C in both sessions to stop this test before the LAN example.

## 6. Chat between the host and Pi over the LAN

In a **Pi session**, start the ARM server using the Pi's LAN address:

```sh
cd ~/12_tcp_chat
./server 192.168.1.253 5050
```

On the **host**, open a local terminal (outside SSH) and build a native client.
Install the host compiler if needed:

```sh
sudo apt install build-essential
```

From `12_tcp_chat` on the **host**:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 client.c -o build/client_host
./build/client_host 192.168.1.253 5050
```

Use the native `client_host` on the host; use the ARM `client` on the Pi.
Both programs receive the **server's IP**, and both use port `5050`.
The host and Pi users can now type messages and press Enter in their terminals.
The traffic is plain TCP and is not encrypted.

Ctrl+D on an empty input line finishes sending but still allows receiving.
Both sides must finish sending for a normal exit. Ctrl+C exits immediately.
Restart the server before connecting a new client.

## Troubleshooting

- `Exec format error`: check the Pi's OS architecture and rebuild with the
  matching cross-compiler. ARM executables generally cannot run directly on
  an x86 host.
- `bind: Cannot assign requested address`: the server's IP must belong to
  the machine running the server. Check `hostname -I` on that machine.
- `bind: Address already in use`: stop the existing server or select another
  port in both commands.
- `connect: Connection refused`: start the server first and check its IP and
  port. For LAN use, bind to the Pi's LAN IP rather than `127.0.0.1`.
- Connection timeout: check network reachability, Wi-Fi client isolation,
  and firewall rules allowing TCP port `5050` between the two machines.
