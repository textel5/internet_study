/*
    tcp_sample sends (as a server) and gets (as a client) a string via tcp
    then shuts
....
    Copyright (C) 2021  Sergey Tokarev
....
    This file is part of tcp_sample.
    tcp_sample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    tcp_sample is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with tcp_sample.  If not, see <https://www.gnu.org/licenses/>.
*/

/*io headers*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*net headers*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tcp_sample.h"

#define UNSETTYPEPROG

#if 1
int main()
{
  int res, sockfd, port, opt, addrlen;
  char addr_str[] = "127.0.0.1";
  struct sockaddr_in addr;
#if defined(SERVER)
  int qlen, socktsfd;
  char msg[] = "hello!\n";
  qlen = 16;
  port = 2020;
#elif defined(CLIENT)
  char *buffin;
  const int buffin_size = 512;
  port = 2020;
  buffin = malloc(buffin_size * sizeof(char));
  addr.sin_port = htons(INADDR_ANY);
#else
# error UNSETTYPEPROG
#endif

  addr.sin_family = AF_INET;
#if defined(SERVER)
  addr.sin_port = htons(port);
# if 1
  res = inet_aton(addr_str, &(addr.sin_addr));
  if (!res) {
    perror("inet_aton");
    exit(1);
  }
# endif
# if 0
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
# endif
#elif defined(CLIENT)
# if 1
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
# endif
#else
# error UNSETTYPEPROG
#endif
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    perror("socket");
    exit(1);
  }
  opt = 1;
  res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (res == -1) {
    perror("setsockopt");
    exit(1);
  }
  res = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
  if (res == -1) {
    perror("bind");
    exit(1);
  }
  addrlen = sizeof(addr);
#if defined(SERVER)
  res = listen(sockfd, qlen);
  if (res == -1) {
    perror("listen");
    exit(1);
  }
  socktsfd = accept(sockfd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
  if (socktsfd == -1) {
    perror("accept");
    exit(1);
  }
  write(socktsfd, msg, sizeof(msg) - 1);
  shutdown(socktsfd, SHUT_WR);
#elif defined(CLIENT)
# if 1
  res = inet_aton(addr_str, &(addr.sin_addr));
  if (!res) {
    perror("inet_aton");
    exit(1);
  }
# endif
  addr.sin_port = htons(port);
  res = connect(sockfd, (struct sockaddr *)&addr, addrlen);
  if (res == -1) {
    perror("connect");
    exit(1);
  }
  read(sockfd, buffin, buffin_size);
  shutdown(sockfd, SHUT_RD);
  printf("%s\n", buffin);
#else
# error UNSETTYPEPROG
#endif
#if defined(SERVER)
  close(socktsfd);
#elif defined(CLIENT)
  free(buffin);
#else
# error UNSETTYPEPROG
#endif
  close(sockfd);
  return 0;
}
#endif