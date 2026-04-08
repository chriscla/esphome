#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

# Extract Docker's embedded DNS NAT rules (targeting 127.0.0.11) before we
# flush iptables, so we can selectively restore them after.
DOCKER_DNS_RULES=$(iptables-save -t nat | grep "127\.0\.0\.11" || true)

# Flush existing rules and delete existing ipsets
iptables -F
iptables -X
iptables -t nat -F
iptables -t nat -X
iptables -t mangle -F
iptables -t mangle -X
ipset destroy allowed-domains 2>/dev/null || true

# 2. Selectively restore ONLY internal Docker DNS resolution
if [ -n "$DOCKER_DNS_RULES" ]; then
    echo "Restoring Docker DNS rules..."
    iptables -t nat -N DOCKER_OUTPUT 2>/dev/null || true
    iptables -t nat -N DOCKER_POSTROUTING 2>/dev/null || true
    echo "$DOCKER_DNS_RULES" | xargs -L 1 iptables -t nat
else
    echo "No Docker DNS rules to restore"
fi

# First allow DNS and localhost before any restrictions
iptables -A OUTPUT -p udp --dport 53 -j ACCEPT
iptables -A INPUT  -p udp --sport 53 -j ACCEPT
iptables -A OUTPUT -p tcp --dport 53 -j ACCEPT
iptables -A INPUT  -p tcp --sport 53 -j ACCEPT
# Allow outbound SSH (git over ssh)
iptables -A OUTPUT -p tcp --dport 22 -j ACCEPT
iptables -A INPUT  -p tcp --sport 22 -m state --state ESTABLISHED -j ACCEPT
# Allow localhost
iptables -A INPUT  -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

# Create ipset with CIDR support
ipset create allowed-domains hash:net

# Fetch GitHub meta information and add their IP ranges
echo "Fetching GitHub IP ranges..."
gh_ranges=$(curl -s https://api.github.com/meta)
if [ -z "$gh_ranges" ]; then
    echo "ERROR: Failed to fetch GitHub IP ranges"
    exit 1
fi

if ! echo "$gh_ranges" | jq -e '.web and .api and .git' >/dev/null; then
    echo "ERROR: GitHub API response missing required fields"
    exit 1
fi

echo "Processing GitHub IPs..."
while read -r cidr; do
    if [[ ! "$cidr" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}/[0-9]{1,2}$ ]]; then
        echo "ERROR: Invalid CIDR range from GitHub meta: $cidr"
        exit 1
    fi
    echo "Adding GitHub range $cidr"
    ipset add -exist allowed-domains "$cidr"
done < <(echo "$gh_ranges" | jq -r '(.web + .api + .git)[]' | aggregate -q)

# Resolve and add other allowed domains.
#
# Buckets:
#   * Claude Code / Anthropic infra
#   * VS Code marketplace + updates
#   * npm
#   * Python packaging (PyPI + Fastly CDN host)
#   * ESPHome (esphome.io and the dashboard's update channel)
#   * PlatformIO (registry, API, dl, collector)
#   * Espressif (toolchains, firmware downloads used by platformio for ESP targets)
DOMAINS=(
    "api.anthropic.com"
    "sentry.io"
    "statsig.anthropic.com"
    "statsig.com"
    "marketplace.visualstudio.com"
    "vscode.blob.core.windows.net"
    "update.code.visualstudio.com"
    "registry.npmjs.org"
    "pypi.org"
    "files.pythonhosted.org"
    "pypi.python.org"
    "esphome.io"
    "platformio.org"
    "api.platformio.org"
    "registry.platformio.org"
    "api.registry.platformio.org"
    "collector.platformio.org"
    "dl.espressif.com"
    "raw.githubusercontent.com"
    "objects.githubusercontent.com"
)

# Resolve all domains in parallel (dig each in the background, then wait)
# and cache results per-domain, so the subsequent validation loop stays
# simple and serial. Cuts postStart latency from ~20*RTT to ~1*RTT.
RESOLVE_TMP=$(mktemp -d)
trap 'rm -rf "$RESOLVE_TMP"' EXIT

echo "Resolving ${#DOMAINS[@]} domains in parallel..."
for domain in "${DOMAINS[@]}"; do
    dig +noall +answer +time=3 +tries=2 A "$domain" \
        | awk '$4 == "A" {print $5}' > "$RESOLVE_TMP/$domain" &
done
wait

for domain in "${DOMAINS[@]}"; do
    ips=$(cat "$RESOLVE_TMP/$domain")
    if [ -z "$ips" ]; then
        echo "ERROR: Failed to resolve $domain"
        exit 1
    fi

    while read -r ip; do
        if [[ ! "$ip" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
            echo "ERROR: Invalid IP from DNS for $domain: $ip"
            exit 1
        fi
        echo "Adding $ip for $domain"
        ipset add -exist allowed-domains "$ip"
    done < <(echo "$ips")
done

# Derive the LAN subnet from the actual kernel routing table rather than
# hard-coding a /24: read the scope-link route on the same interface as
# the default route. This is needed to reach ESP devices on the host LAN.
DEFAULT_IFACE=$(ip -4 route show default | awk '{print $5; exit}')
if [ -z "$DEFAULT_IFACE" ]; then
    echo "ERROR: Failed to detect default interface"
    exit 1
fi
HOST_NETWORK=$(ip -4 route show dev "$DEFAULT_IFACE" scope link | awk '{print $1; exit}')
if [ -z "$HOST_NETWORK" ]; then
    echo "ERROR: Failed to detect host network on $DEFAULT_IFACE"
    exit 1
fi
echo "Host network detected as: $HOST_NETWORK (via $DEFAULT_IFACE)"

# Allow LAN traffic (host network) — needed to reach ESP devices on the local network
iptables -A INPUT  -s "$HOST_NETWORK" -j ACCEPT
iptables -A OUTPUT -d "$HOST_NETWORK" -j ACCEPT

# Set default policies to DROP
iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT DROP

# Allow established connections for already approved traffic
iptables -A INPUT  -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Then allow only specific outbound traffic to allowed domains
iptables -A OUTPUT -m set --match-set allowed-domains dst -j ACCEPT

# Explicitly REJECT all other outbound traffic for immediate feedback
iptables -A OUTPUT -j REJECT --reject-with icmp-admin-prohibited

echo "Firewall configuration complete"
echo "Verifying firewall rules..."
if curl --connect-timeout 5 https://example.com >/dev/null 2>&1; then
    echo "ERROR: Firewall verification failed - was able to reach https://example.com"
    exit 1
else
    echo "Firewall verification passed - unable to reach https://example.com as expected"
fi

# Verify GitHub API access
if ! curl --connect-timeout 5 https://api.github.com/zen >/dev/null 2>&1; then
    echo "ERROR: Firewall verification failed - unable to reach https://api.github.com"
    exit 1
else
    echo "Firewall verification passed - able to reach https://api.github.com as expected"
fi
