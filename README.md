# Asymetrically Encrypted Custom Shell Imeplementation

A custom remote shell implementation, designed to work between my laptop and raspi. I have no idea if it'll work on your hardware.

## Installation

1. Install [OpenSSL](https://github.com/openssl/openssl/releases) 3.5.4 or later.
2. Generate a public/private key pair with OpenSSL and place them in /keys/, called "public.pem" and "private.pem" respectively.
3. run `make`
4. run `./exec` to boot

By default, `exec` boots the current device as the server. To boot as client, run `./exec -c`

### Other Terminal Flags

- `-c` : Boot as client
- `-v` : Verbose
- `-a` : Specify a custom IP with which to connect (*used when booting as client, and when the server IP is different from the hard-coded default of* `192.168.20.30`)
- `-h` : show these options


