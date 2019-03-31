#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#define TBL24_SIZE 16777216
#define BLOCK_SIZE 256
#define CTE 1000


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

  uint16_t * tbl24 = (uint16_t *)calloc(TBL24_SIZE, sizeof(uint16_t));
  uint16_t * tblong = (uint16_t *)malloc(0);
  if(tbl24 == NULL || tblong == NULL){
    perror("malloc");
    return 0;
  }
  uint32_t  prefix = 0;
  int prefixLength = 0;
  int  outInterface = 0;
  uint16_t table_position = 0;
  uint16_t table_entry = 0;
  int n_options, i,j, aux, nblocks_tblong = 0;



  while(lectura != EOF){
    lectura =  readFIBLine(&prefix, &prefixLength, &outInterface);
	printf("Lectura de entrada - Prefijo:%d/%d >> %d\n", prefix, prefixLength, outInterface);
    if(lectura != 0){
      perror("readFIBLine");
      break;
    }
    table_position = prefix >> 8; // get rid of the 8 last bits we dont want (/24)


    if(prefixLength == 0){
      puts("Prefijo 0");
	  
    }else if(prefixLength <=  24){
		puts("Prefijo <= 24");
		table_entry = (uint16_t) outInterface;
		n_options = pow(2, 24-prefixLength);
		for(i = 0; i < n_options; i++){
		memcpy(&tbl24[table_position + i], &table_entry, sizeof(uint16_t));
		}
		//printf("ADDED TBL24 in Range[%d , %d] >> %d\n",table_position, table_position + n_options, tbl24[table_position] );

    }  else if(prefixLength > 24){
		puts("Prefijo > 24");
		
		if ( (tbl24[table_position] ^ (uint16_t) pow(2,15)) == (uint16_t)0  ){

			n_options = pow(2, 32-prefixLength);
			table_entry = outInterface;
			table_position = ((tbl24[table_position] ^ (uint16_t) pow(2,15)) - CTE) * BLOCK_SIZE;
			for(j = n_options - 1; j < 2*n_options -1; j++ ){
				memcpy(&tblong[table_position + j], &table_entry, sizeof(uint16_t));
			}

        }else{
			
          aux = tbl24[table_position];
          n_options = pow(2, 32-prefixLength);
		  
		  table_entry = CTE + nblocks_tblong++;
          table_entry = (uint16_t)(table_entry | (uint16_t) pow(2,15));
          memcpy(&tbl24[table_position], &table_entry, sizeof(uint16_t));
          table_entry = outInterface;
		  
          tblong = realloc(tblong, BLOCK_SIZE);
          if(tblong == NULL){
            perror("realloc");
          }

          for(i = 0; i < BLOCK_SIZE; i++){
            table_position = nblocks_tblong*BLOCK_SIZE;
            memcpy(&tblong[table_position + i], &aux, sizeof(uint16_t));

          }
          for(j = n_options - 1; j< 2*n_options - 1; j++ ){
            memcpy(&tblong[table_position + j], &table_entry, sizeof(uint16_t));
          }

          ////Esto es para mostrar en pantalla el resultado
          for(i = 0; i < BLOCK_SIZE; i++){
            table_position = nblocks_tblong*BLOCK_SIZE;
            //printf("Valor de la tabla----------------------%d\n", tblong[table_position +i ] );
          }
        }
	
    }else{
      puts("Reading prefix length failed");
      break;
    }
      
  }
  
  printf("hey lets delete memory\n" );
  //printf("%d, %d, %d\n", prefix, prefixLength, outInterface );
  free(tbl24);
  free(tblong);
  freeIO();
  printMemoryTimeUsage();
  return 0;
}
