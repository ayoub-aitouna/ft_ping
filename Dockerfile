FROM debian:bookworm

RUN apt-get -y update
RUN apt-get install -y inetutils-ftp build-essential libpcap-dev net-tools
RUN apt-get install -y zsh curl git wget
RUN apt-get install iputils-ping -y

RUN rm -rf /var/lib/apt/lists/*
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

WORKDIR /app
SHELL [ "/usr/bin/zsh" ]
# ENTRYPOINT [""]
