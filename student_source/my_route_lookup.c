#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#define TBL24_Size 16777216;
#define TBLong_Size 256;




int main(int argc, char const *argv[]) {

  char * myself = basename(argv[0]);
  if (argc != 3) {
    printf("Uso: %s <FIB><InputPacketFile>\n", myself);
    printf("<FIB>: name of an ASCII file containing the FIB (Forwarding Information Base)\n");
    printf("<InputPacketFile>:  name of the ASCII file containing a list of destination IP addresses to be processed, separated by new line characters \n");
    exit(-1);
  }
  
/* Procesar los argumentos de la línea de comandos */
  uint16_t port = atoi(argv[1]);
  char filename[20];
  strcpy(filename,argv[2]);
  char filename1[20];
  strcpy(filename1,argv[3]);

  hash_entry_t *tbl24[TBL24_Size];
  hash_entry_t *tblong[TBLong_Size];



  return 0;
}
