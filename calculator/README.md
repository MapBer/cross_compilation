# Cross-compile a command-line calculator for Raspberry Pi 4

This example reads two integers from command-line arguments and prints their
sum using `printf`. It demonstrates argument handling, input validation, and
error exit codes in a cross-compiled C program.

Use an Ubuntu/Debian development computer as the **host**, and Raspberry Pi OS
on the **Pi**. Install the matching cross-compiler using the
[Hello_world setup guide](../hello_world/README.md).

## Build on the host

First, check the OS architecture on the **Pi**:

```sh
dpkg --print-architecture
```

On the **host**, from the repository root:

```sh
cd calculator
mkdir -p build
```

If the Pi reports `arm64`, build with:

```sh
aarch64-linux-gnu-gcc -std=c11 -Wall -Wextra -Werror -O2 -static main.c -o build/calculator
```

If the Pi reports `armhf`, build with:

```sh
arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -Werror -O2 -static main.c -o build/calculator
```

Both commands link statically, avoiding a dependency on the Pi's installed
glibc version for this example. Inspect the result on the **host**:

```sh
file build/calculator
```

Expect an ARM aarch64 executable for `arm64`, or a 32-bit ARM executable for
`armhf`. The compiler runs on your host and generates machine code for the Pi.

## Transfer and run

On the **host**, replace `YOUR_PI_USER` with your Pi username and the IP with
your Pi's current address:

```sh
scp build/calculator YOUR_PI_USER@192.168.1.253:~/calculator
ssh YOUR_PI_USER@192.168.1.253
```

In the SSH session on the **Pi**:

```sh
chmod +x ~/calculator
~/calculator 12 30
```

Output:

```text
12 + 30 = 42
```

Try negative numbers:

```sh
~/calculator -7 3
```

Output:

```text
-7 + 3 = -4
```

Invalid input such as `~/calculator abc 3`, missing arguments, and a sum
outside the signed 64-bit range produce an error and a nonzero exit code.
Run `echo $?` immediately after the command to inspect its exit code.
