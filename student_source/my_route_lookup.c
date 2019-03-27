#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#define TBL24_Size 16777216
#define TBLong_Size 256



int main(int argc, char *argv[]) {
  int lectura = 0;
  int lectura2 = 0;
  int err;
  if (argc != 3) {
    printf("Uso: <FIB><InputPacketFile>\n");
    printf("<FIB>: name of an ASCII file containing the FIB (Forwarding Information Base)\n") ;
    printf("<InputPacketFile>:  name of the ASCII file containing a list of destination IP addresses to be processed, separated by new line characters  \n");
    exit(-1);
  }


  err = initializeIO(argv[1], argv[2]);
  if(err != 0){
    perror("initializeIO");
    printf("Not able to read the tables\n");
    return 0;
  }

  uint16_t * tbl24 = (uint16_t *)malloc(TBL24_Size);
  uint16_t * tblong = (uint16_t *)malloc(TBLong_Size);
  uint32_t  prefix = 0;
  int prefixLength = 0;
  int outInterface = 0;
  uint32_t IPAddress = 0;
  int numberOfTableAccesses = 0;

/*while(lectura1 != EOF){
  lectura1 = readInputPacketFileLine(&IPAddress);
  if(lectura != 0){
    perror("readInputPacketFileLine");
    break;
  }*/
  while(lectura != EOF){
    lectura =  readFIBLine(&prefix, &prefixLength, &outInterface);
    if(lectura != 0){
      perror("readFIBLine");
      //freeIO();
      break;
    }


    if(prefixLength == 0){
      //tbl24 = outInterface >> 0;
      puts("Prefijo 0");
    }else if(prefixLength <=  24){
      puts("Prefijo <=24");
      tbl24 =
    }else if(prefixLength >  24){
      puts("Prefijo >=24");
    }else{
      puts("Reading prefix length failed");
      break;
    }

      printf("%d, %d, %d, %d\n", prefix, prefixLength, outInterface, tbl24[0]);
  }

  //printf("%d, %d, %d\n", prefix, prefixLength, outInterface );
  free(tbl24);
  free(tblong);
  freeIO();
  return 0;
}
