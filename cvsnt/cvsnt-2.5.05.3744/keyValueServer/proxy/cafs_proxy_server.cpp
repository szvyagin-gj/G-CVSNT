#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/blobs_encryption.h"
#include "../include/blob_server.h"
#include "../include/blob_sockets.h"//move init to out of line
#define BLOB_LOG_LEVEL LOG_WARNING
#include "../sampleImplementation/def_log_printf.cpp"

void init_proxy(const char *url, int port, const char *cache, uint64_t sz, const char *encryption_secret, CafsClientAuthentication auth_on_master, bool update_mtimes, bool validate_blobs);
void close_proxy();
extern bool proxy_allow_push;

int main(int argc, const char **argv)
{
  if (argc < 3)
  {
    printf("Usage is cafs_proxy_server master_url cache_folder [validate_blobs_from_master] [update_mtime_on_access] [encryption/mandatory_encryption secret] [cache_soft_limit_size (mb). default is 102400 == 100Gb]\n");
    return 1;
  }
  if (!blob_init_sockets())
  {
    blob_logmessage(LOG_ERROR, "Can't init sockets, %d", blob_get_last_sock_error());
    return 1;
  }
  const char *encryption_secret = 0;
  CafsServerEncryption encryption = CafsServerEncryption::Local;
  CafsClientAuthentication auth_on_master = CafsClientAuthentication::AllowNoAuthPrivate;
  bool update_mtimes = false, validate_blobs = false;
  int pc = 3;
  for (; pc < argc;)
  {
    if (strcmp(argv[pc], "encryption") == 0 || strcmp(argv[pc], "mandatory_encryption") == 0)
    {
      if (argc <= pc + 1 || strlen(argv[pc+1]) < minimum_shared_secret_length)
      {
        fprintf(stderr, "encrypted server has to be started with secret of valid length > %d", (int)minimum_shared_secret_length);
        return 1;
      }
      encryption = strcmp(argv[pc], "encryption") == 0 ? CafsServerEncryption::Public : CafsServerEncryption::All;
      if (encryption == CafsServerEncryption::All)
      {
        //this is not required. Our proxy can be encrypting but still work with unencrypted old server. Though why bother, safer that way.
        auth_on_master = CafsClientAuthentication::RequiresAuth;
        proxy_allow_push = false;//we don't use write through on public proxies
      }
      encryption_secret = argv[pc+1];
      pc += 2;
    } else if (strcmp(argv[pc], "update_mtime_on_access") == 0)
    {
      update_mtimes = true;
      ++pc;
    } else if (strcmp(argv[pc], "validate_blobs_from_master") == 0)
    {
      validate_blobs = true;
      ++pc;
    }
    else break;
  }
  const int master_port = 2403;
  init_proxy(argv[1], master_port, argv[2], argc>pc ? atoi(argv[pc]) : 100*1024, encryption_secret, auth_on_master, update_mtimes, validate_blobs);
  printf("Starting %s%s server listening at port %d%s\n",
    encryption == CafsServerEncryption::Public ? "optional " : " ", encryption_secret ? "encrypted" : "local only", master_port,
    update_mtimes ? ", blob mtimes modified on access" : "");
  const bool result = start_push_server(master_port, 1024, nullptr, encryption_secret, encryption);
  printf("server quit %s", result ? "with error\n" :"normally\n");
  close_proxy();
  blob_close_sockets();
  return result ? 0 : 1;
}