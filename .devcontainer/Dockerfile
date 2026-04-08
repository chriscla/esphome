FROM node:20

ARG TZ
ENV TZ="$TZ"

ARG CLAUDE_CODE_VERSION=latest

# Base development tools, Python (for esphome), and network tools for the firewall
RUN apt-get update && apt-get install -y --no-install-recommends \
      less \
      git \
      procps \
      sudo \
      fzf \
      zsh \
      man-db \
      unzip \
      gnupg2 \
      curl \
      wget \
      ca-certificates \
      gh \
      iptables \
      ipset \
      iproute2 \
      dnsutils \
      aggregate \
      jq \
      nano \
      vim \
      python3 \
      python3-venv \
      python3-pip \
      protobuf-compiler \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# uv for fast Python package management
RUN pip install --break-system-packages uv

# chezmoi — pre-installed so VS Code's dotfiles install command
# (configured in user settings, not here) can run `chezmoi init --apply ...`
# without having to bootstrap chezmoi itself behind the firewall.
RUN sh -c "$(curl -fsLS https://get.chezmoi.io)" -- -b /usr/local/bin

# We use the built-in `node` user (UID 1000) from the node:20 image. The
# devcontainer feature `updateRemoteUserUID` (default ON) will rewrite this
# UID at first start to match the host user, which avoids permission
# headaches on bind mounts.

# npm global prefix accessible to the node user
RUN mkdir -p /usr/local/share/npm-global \
    && chown -R node:node /usr/local/share/npm-global

# Persist shell history across container rebuilds (mounted as a volume)
RUN mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R node /commandhistory

# Workspace, Claude config dir, and home subdirs that will be mounted (bind or
# named volumes) from devcontainer.json. Pre-creating them with the right
# ownership means the mounts inherit correct perms.
#
# Volume strategy:
#   ~/.cache         — chezmoi cache, uv, pip, etc. (re-download avoidance)
#   ~/.local         — chezmoi source, XDG-share externals (oh-my-zsh if XDG, etc.)
#   ~/.oh-my-zsh     — default OMZ install location (in case it's not under XDG)
#   ~/.tmux          — tmux plugin manager + plugins
#   ~/.platformio    — platformio toolchains/platforms (the big one)
#   ~/.espressif     — esp-idf tools
RUN mkdir -p \
      /esphome \
      /opt/esphome-venv \
      /home/node/.claude/agents \
      /home/node/.claude/skills \
      /home/node/.claude/commands \
      /home/node/.cache \
      /home/node/.local \
      /home/node/.oh-my-zsh \
      /home/node/.tmux \
      /home/node/.platformio \
      /home/node/.espressif \
    # Placeholder FILES (not dirs) so the named volume initializes with these
    # paths existing, which lets the host bind mounts in devcontainer.json
    # overlay them without Docker auto-creating them as directories.
    && touch \
      /home/node/.claude/settings.json \
      /home/node/.claude/CLAUDE.md \
    && chown -R node:node \
      /esphome \
      /opt/esphome-venv \
      /home/node/.claude \
      /home/node/.cache \
      /home/node/.local \
      /home/node/.oh-my-zsh \
      /home/node/.tmux \
      /home/node/.platformio \
      /home/node/.espressif

ENV DEVCONTAINER=true

# git-delta for nicer diffs
ARG GIT_DELTA_VERSION=0.18.2
RUN ARCH=$(dpkg --print-architecture) \
    && wget -q "https://github.com/dandavison/delta/releases/download/${GIT_DELTA_VERSION}/git-delta_${GIT_DELTA_VERSION}_${ARCH}.deb" \
    && dpkg -i "git-delta_${GIT_DELTA_VERSION}_${ARCH}.deb" \
    && rm "git-delta_${GIT_DELTA_VERSION}_${ARCH}.deb"

USER node

ENV NPM_CONFIG_PREFIX=/usr/local/share/npm-global
ENV PATH=$PATH:/usr/local/share/npm-global/bin
ENV SHELL=/bin/zsh
ENV EDITOR=nano
ENV VISUAL=nano
# zsh shell setup (.zshrc, oh-my-zsh, theme, plugins) is owned by chezmoi
# via the dotfiles install command in user settings — see devcontainer.json.

# Install Claude Code globally
RUN npm install -g @anthropic-ai/claude-code@${CLAUDE_CODE_VERSION}

# Python virtualenv with esphome + platformio for compiling/uploading firmware.
# Lives in /opt (not ~/.local) so the ~/.local volume mount can't shadow it.
ENV VIRTUAL_ENV=/opt/esphome-venv
RUN uv venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
ENV UV_SYSTEM_PYTHON=false

RUN uv pip install esphome platformio
# NOTE: platformio settings (telemetry off, etc.) are applied via
# postCreateCommand in devcontainer.json — they live under ~/.platformio,
# which is a mounted cache volume and would shadow anything written here.

# Firewall script — runs via sudo from postStartCommand. We grant the node
# user broad passwordless sudo because (a) we already need sudo for the
# firewall init and for chowning volume mount points at first start, and
# (b) this is a sandboxed dev container, not a production host.
USER root
COPY init-firewall.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/init-firewall.sh \
    && echo "node ALL=(root) NOPASSWD: ALL" > /etc/sudoers.d/node \
    && chmod 0440 /etc/sudoers.d/node

USER node
WORKDIR /esphome
