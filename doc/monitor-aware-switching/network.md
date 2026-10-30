# Network and security

## Input Leap

PC B (client) initiates TCP to PC A (server) on the default port 24800. Keyboard/mouse transport does not require UDP. Bonjour/mDNS discovery is optional and normally does not cross routed subnets; use a fixed address or internal DNS.

Restrictive policy:

- allow PC B only to PC A TCP 24800;
- allow matching inbound TCP 24800 on Windows Firewall at PC A only from PC B;
- rely on stateful return traffic;
- do not enable mDNS reflection, broad inter-subnet access, or public port forwarding.

TLS fingerprints must be verified after certificate changes. Do not silently accept a new server fingerprint after an address change.

## Remote development automation

If remote build or test agents are used, they should initiate encrypted connections to their controller. Restrict controller access to the exact agent addresses; do not expose agent control ports publicly or permit entire local subnets by default.

Input Leap and development-automation rules are independent. Neither requires a public NAT/port-forward rule.
