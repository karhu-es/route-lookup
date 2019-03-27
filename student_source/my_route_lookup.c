#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#define TBL24_Size 16777216
#define TBLong_Size 256



int main(int argc, char *argv[]) {
  int lectura = 0;

  if (argc != 3) {
    printf("Uso: <FIB><InputPacketFile>\n");
    printf("<FIB>: name of an ASCII file containing the FIB (Forwarding Information Base)\n") ;
    printf("<InputPacketFile>:  name of the ASCII file containing a list of destination IP addresses to be processed, separated by new line characters  \n");
    exit(-1);
  }

  if(initializeIO(argv[1], argv[2]) != 0){
    perror("initializeIO");
    printf("Not able to read the tables\n");
    return 0;
  }

  uint16_t * tbl24 = (uint16_t *)malloc(0);
  uint16_t * tblong = (uint16_t *)malloc(0);
  if(tbl24 == NULL || tblong == NULL){
    perror("malloc");
    return 0;
  }
  uint32_t  prefix = 0;
  int prefixLength = 0;
  int outInterface = 0;
  uint32_t IPAddress = 0;
  int numberOfTableAccesses = 0;
  uint16_t tbl24_entries = 0;


  while(lectura != EOF){
    lectura =  readFIBLine(&prefix, &prefixLength, &outInterface);
    if(lectura != 0){
      perror("readFIBLine");
      break;
    }


    if(prefixLength == 0){
      //tbl24 = outInterface >> 0;
      puts("Prefijo 0");
    }else if(prefixLength <=  24){
      puts("Prefijo <=24");

      tbl24 = realloc(tbl24, ++tbl24_entries * sizeof(uint16_t));
      memcpy((tbl24 + tbl24_entries), &outInterface, sizeof(uint16_t));

    }else if(prefixLength >  24){
      puts("Prefijo >=24");
      tblong = realloc(tblong, sizeof(tblong) + sizeof(uint16_t));
    }else{
      puts("Reading prefix length failed");
      break;
    }

      printf("%d, %d, %d, %d\n", prefix, prefixLength, outInterface, tbl24[0]);
  }
  uint16_t i;
  for(i = 0; i <= tbl24_entries; i++){
    printf("%d\n", tbl24[i]);
  }

  printf("hey lets delete memory\n" );
  //printf("%d, %d, %d\n", prefix, prefixLength, outInterface );
  free(tbl24);
  free(tblong);
  freeIO();
  return 0;
}
