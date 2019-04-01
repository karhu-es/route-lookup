#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#define TBL24_Size 16777216
#define TBLong_Block_Size 256
#define Entry_Size 16
#define CTE 1000
#define MISS 0

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

  uint16_t * tbl24 = (uint16_t *)calloc(TBL24_Size, Entry_Size);
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

  int n_ip_dest_processed = 0;
  int n_tables_accessed = 0;
  float avg_table_accessed = 0;
  time_t t_start = 0;

  while(lectura != EOF){
    lectura =  readFIBLine(&prefix, &prefixLength, &outInterface);
    if(lectura != 0){
      perror("readFIBLine");
      break;
    }
    table_position = prefix >> 8; // get rid of the 8 last bits we dont want (/24)


    if(prefixLength == 0){
      //tbl24 = outInterface >> 0;
      puts("Prefijo 0");
    }else if(prefixLength <=  24){
      table_entry = (uint16_t) outInterface;
      n_options = pow(2, 24-prefixLength);
      for(i = 0; i < n_options; i++){
        memcpy(&tbl24[table_position + i], &table_entry, sizeof(uint16_t));
      }
      printf("Valor de la tabla----------------------%d\n", tbl24[table_position] );

    }else if(prefixLength >  24){
      puts("Prefijo > 24");
      if ( (tbl24[table_position]   | (uint16_t) pow(2,15)) == tbl24[table_position]  ){
          /// Esta opcion es cuando tenemos un 1 + puerto en la tabla. Se supone que no necesitamos crear table, sino escribir sobre la que ya teniamos escrita
          n_options = pow(2, 32-prefixLength);
          table_entry = outInterface;
          table_position = (tbl24[table_position] ^ (uint16_t) pow(2,15)) - CTE;
          for(j = n_options - 1; j < 2*n_options -1; j++ ){
            memcpy(&tblong[table_position + j], &table_entry, sizeof(uint16_t));
          }

          ////Esto es para mostrar en pantalla el resultado
          for(i = 0; i < TBLong_Block_Size; i++){
            printf("Valor de la tabla----------------------%d\n", tblong[table_position +i ] );
          }

        }else{
          puts("Aqui en la opcion 0 con algo");
          aux = tbl24[table_position];
          n_options = pow(2, 32-prefixLength);
          tblong = realloc(tblong, TBLong_Block_Size);

          if(tblong == NULL){
            perror("realloc");
          }

          table_entry = CTE + nblocks_tblong*TBLong_Block_Size;
          table_entry = (uint16_t)(table_entry | (uint16_t) pow(2,15));
          nblocks_tblong ++;
          memcpy(&tbl24[table_position], &table_entry, sizeof(uint16_t));
          printf("Entrada escrita en posicion %d >> %d\n",table_position, tbl24[table_position] );
          table_entry = outInterface;

          for(i = 0; i < TBLong_Block_Size; i++){
            table_position = nblocks_tblong*TBLong_Block_Size;
            memcpy(&tblong[table_position + i], &aux, sizeof(uint16_t));

          }
          for(j = n_options - 1; j< 2*n_options - 1; j++ ){
            memcpy(&tblong[table_position + j], &table_entry, sizeof(uint16_t));
          }

          ////Esto es para mostrar en pantalla el resultado
          for(i = 0; i < TBLong_Block_Size; i++){
            table_position = nblocks_tblong*TBLong_Block_Size;
            printf("Valor de la tabla----------------------%d\n", tblong[table_position +i ] );


          }
        }
        //printf("Valor de la tabla----------------------%d\n", tblong[table_position] );

    }else{
      puts("Reading prefix length failed");
      break;
    }

      printf("%d, %d, %d, %d\n", prefix, prefixLength, outInterface, tbl24[table_position]);
  }

  ///Look up for the incoming Ip addresses
  uint32_t IPAddress;
  time_t t_end = 0;
  time_t time_consumed = 0;
  time_t total_time = 0;
  lectura = 0;
  while(lectura == 0){
    time_consumed = 0;
    t_start = 0;
    t_end = 0;
    n_ip_dest_processed++;

    lectura = readInputPacketFileLine(&IPAddress);

    if(lectura != 0){
      perror("readInputPacketFileLine");
      //return 0;
    }
    t_start = time(NULL);
    table_position = IPAddress >> 8;

    if(tbl24[table_position] == 0){

      outInterface = MISS;
      printf("Aqui estamos\n");
      n_tables_accessed++;
      printf("Interfazzzzzzzzzzzzzzzzzzzzzzzzzzzzzz %d\n", outInterface);

    }else if((tbl24[table_position]   | (uint16_t) pow(2,15)) == tbl24[table_position] ){

      printf("Estamos en la segunda tabla\n");
      table_position = (tbl24[table_position]   ^ (uint16_t) pow(2,15)) - CTE;
      outInterface = tblong[table_position];
      n_tables_accessed++;
      printf("Interfazzzzzzzzzzzzzzzzzzzzzzzzzzzzzz %d\n", outInterface);

    }else{

      printf("Hey, que no llegamos a la segunda tabla\n");
      outInterface = tbl24[table_position];
      n_tables_accessed++;
      printf("Interfazzzzzzzzzzzzzzzzzzzzzzzzzzzzzz %d\n", outInterface);
    }
    t_end = time(NULL);
    time_consumed = t_end - t_start;
    printf("total time: %d\n", time_consumed);
    total_time = total_time + time_consumed;
  }

printf("acesses %d, ips: %d\n", n_tables_accessed, n_ip_dest_processed);

  printf("hey lets delete memory\n" );
  //printf("%d, %d, %d\n", prefix, prefixLength, outInterface );
  free(tbl24);
  free(tblong);
  freeIO();
  return 0;
}
