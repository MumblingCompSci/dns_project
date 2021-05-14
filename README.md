# Dug: the DNS query generator

Dug was a project for my Intro to Networks class. Our goal was to create a program that could generate a DNS request packet, send it, and recieve a correct response from the specified server.

---

This project uses the Boost C libraries for debugging outputs. The version I was using can be found at [the Boost website](https://www.boost.org/users/history/version_1_76_0.html). To install on linux, just extract the tarball into your `/usr/include/` directory.

To build the project, run `make`.

To run the project, use this format in the directory where dug is:<br>
> `./dug [-v] (website to query) (dns server IP)`

The verbose verion of dug will provide information such as sent and recieved packet sizes along with some of the data that was processed from the inputs.

---

TODO: add processing for DNS responses