/* Eric Leao 2110694 3WB */
/* Pedro Machado Peçanha 2110535 3WB */

#include "execpage.h"
#include "gera_codigo.h"

int main(int argc, char *argv[]) {
  FILE *fp;
  funcp funcLBS;
  int res;
  unsigned char code[300];
  /* Abre o arquivo para leitura */
  fp = fopen("1teste.txt", "r");
  /* Gera o codigo */
  gera_codigo(fp, code, &funcLBS);
  if (funcLBS == NULL) {
    printf("Erro na geracao\n");
  }
  for (int c = 0; c < 300; c++) {
    //printf("%x\n", code[c]);
  }
  /* Chama a função gerada */
  execpage(code, 300);
  res = (*funcLBS)(4);
  printf("%d\n", res);
}