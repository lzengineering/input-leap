# Network and security

## Input Leap

PC B (client) initiates TCP to PC A (server) on the default port 24800. Keyboard/mouse transport does not require UDP. Bonjour/mDNS discovery is optional and normally does not cross routed subnets; use a fixed address or internal DNS.

Restrictive policy:

- allow PC B only to PC A TCP 24800;
- allow matching inbound TCP 24800 on Windows Firewall at PC A only from PC B;
- rely on stateful return traffic;
- do not enable mDNS reflection, broad inter-subnet access, or public port forwarding.

TLS fingerprints must be verified after certificate changes. Do not silently accept a new server fingerprint after an address change.

## OpenClaw

Windows nodes initiate their Gateway connection to `192.168.10.10:18789/TCP`. No inbound Windows-node listener is required for normal operation. Restrict the Gateway rule to the exact DHCP-reserved node addresses rather than all of `192.168.20.0/24` once the addresses are final.

Input Leap and OpenClaw rules are independent. Neither requires a public NAT/port-forward rule.
