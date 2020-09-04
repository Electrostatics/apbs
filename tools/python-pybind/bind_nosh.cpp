#include "bind_nosh.hpp"

int parseInputFromString(NOsh *nosh, std::string str)
{
  int ret, bufsize;
  Vio *sock;

  startVio();

  VASSERT( bufsize <= VMAX_BUFSIZE );
  sock = Vio_ctor("BUFF","ASC",VNULL,"0","r");

  Vio_bufTake(sock, str.c_str(), str.size());

  ret = NOsh_parseInput(nosh, sock); 
  sock->VIObuffer = VNULL;
  Vio_dtor(&sock);
  return ret;
}
