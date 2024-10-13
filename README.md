
# systemctl-node

A simple control module from C bindings for service control with [Systemd](https://en.wikipedia.org/wiki/Systemd) - [systemctl](https://www.freedesktop.org/software/systemd/man/latest/systemctl.html) lib (created for Ubuntu/Debian systems)


## Installation

Install systemctl-node with npm

```bash
  npm install systemctl-node
```

Install systemctl-node with yarn

```bash
  yarn add systemctl-node
```
    
## Usage/Examples

Requires privilege escalation - (run with sudo)

```javascript
// app.js

const systemctl = require("systemctl-node");

/* Start service */
systemctl.start("nginx")

/* Stop service */
systemctl.stop("nginx")

/* Restart service */
systemctl.restart("nginx")
```

Run app
```bash
$ sudo node app.js
```


## Authors

- [Obediah Klopfenstein](https://www.github.com/obe711)


## License

[MIT](./LICENSE)

