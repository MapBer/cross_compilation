# Cross-compile Hello_world for Raspberry Pi 4

This guide builds `main.c` on an Ubuntu/Debian Linux development computer
(the host), then copies the executable to a Raspberry Pi 4 running Raspberry
Pi OS (the target). Commands labelled **host** run on your development
computer; commands labelled **Pi** run on the Raspberry Pi.

## 1. Check the Pi's operating system architecture

On the **Pi**, run:

```sh
dpkg --print-architecture
```

- `arm64`: follow the 64-bit instructions.
- `armhf`: follow the 32-bit instructions.

Choose according to the installed OS, since the Pi 4 supports both. Checking
the package architecture avoids confusing a 64-bit kernel with 32-bit user space.

## 2. Install the cross-compiler on the host

For **64-bit Raspberry Pi OS**, run on the **host**:

```sh
sudo apt update
sudo apt install gcc-aarch64-linux-gnu libc6-dev-arm64-cross file openssh-client
```

For **32-bit Raspberry Pi OS**, run on the **host**:

```sh
sudo apt update
sudo apt install gcc-arm-linux-gnueabihf libc6-dev-armhf-cross file openssh-client
```

## 3. Cross-compile on the host

From the repository root on the **host**:

```sh
cd hello_world
mkdir -p build
```

For **64-bit Raspberry Pi OS**:

```sh
aarch64-linux-gnu-gcc -std=c11 -Wall -Wextra -Werror -O2 -static main.c -o build/hello_world
```

For **32-bit Raspberry Pi OS**:

```sh
arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -Werror -O2 -static main.c -o build/hello_world
```

The example links the C library statically so this simple program does not
depend on the Pi having the host toolchain's glibc version. For larger programs
using shared libraries, use a toolchain and target sysroot matching the Pi's OS
and dependencies rather than assuming the host's libraries are compatible.

Check the executable on the **host**:

```sh
file build/hello_world
```

Expect `ELF 64-bit` and `ARM aarch64` for arm64, or `ELF 32-bit` and `ARM`
for armhf, with `statically linked` in the output. An x86-64 result means you
used the host compiler instead of the ARM cross-compiler.

## 4. Copy to the Pi and run

Enable SSH on the Pi through `sudo raspi-config` under **Interface Options → SSH**.
Make sure the host can reach the Pi over your network. Replace `YOUR_PI_USER`
and `PI_IP_ADDRESS` below with your actual Pi username and IP address.

From the `hello_world` directory on the **host**:

```sh
scp build/hello_world YOUR_PI_USER@PI_IP_ADDRESS:~/hello_world
ssh YOUR_PI_USER@PI_IP_ADDRESS
```

Then, in the SSH session on the **Pi**:

```sh
chmod +x ~/hello_world
~/hello_world
```

Expected output:

```text
Hello_world
```

An `Exec format error` usually indicates a binary built for the wrong
architecture. Recheck step 1 and rebuild using the matching compiler. An ARM
executable generally cannot run directly on an x86 host; run it on the Pi.

## References

- [Arm GNU cross-compiler installation guide](https://learn.arm.com/install-guides/gcc/cross/)
- [Raspberry Pi OS documentation](https://www.raspberrypi.com/documentation/computers/os.html)
