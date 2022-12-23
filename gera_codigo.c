/* Eric Leao 2110694 3WB */
/* Pedro Machado Peçanha 2110535 3WB */

#include "gera_codigo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void error(const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
  int line = 1;
  int c;
  int codenum = 0; // guarda quantos bytes já colocamos no array
  int contFunc = 0;
  unsigned char *vetorEnderecos[300];

  // CODIGOS DE MAQUINA ABAIXO
  unsigned char inicio[] = {0x55, 0x48, 0x89, 0xe5};
  unsigned char ret[] = {0xc9, 0xc3};
  unsigned char movr$[] = {0xb8, 0x00, 0x00, 0x00, 0x00};
  unsigned char movrpilha[] = {0x8b, 0x45, 0x00};
  unsigned char movedieax[] = {0x89, 0xf8};
  unsigned char salvaedi[] = {0x89, 0x7d, 0xe8};
  unsigned char call[] = {0xe8, 0x00, 0x00, 0x00, 0x00};
  unsigned char restauraedi[] = {0x8b, 0x7d, 0xe8};
  unsigned char movedipilha[] = {0x8b, 0x7d, 0x00};
  unsigned char moveaxpilha[] = {0x89, 0x45, 0xfc};
  unsigned char mov$edi[] = {0xbf, 0x00, 0x00, 0x00, 0x00};
  unsigned char movr11$[] = {0x41, 0xbb, 0x00, 0x00, 0x00, 0x00};
  unsigned char movr11pilha[] = {0x44, 0x89, 0x5d, 0x00};
  unsigned char movpilhar11[] = {0x44, 0x8b, 0x5d, 0x00};
  unsigned char movr11edi[] = {0x41, 0x89, 0xfb};
  unsigned char add$[] = {0x41, 0x81, 0xc3, 0x00, 0x00, 0x00, 0x00};
  unsigned char addpilha[] = {0x44, 0x03, 0x5d, 0x00};
  unsigned char addedi[] = {0x41, 0x01, 0xfb};
  unsigned char sub$[] = {0x41, 0x81, 0xeb, 0x00, 0x00, 0x00, 0x00};
  unsigned char subpilha[] = {0x44, 0x2b, 0x5d, 0x00};
  unsigned char subedi[] = {0x41, 0x29, 0xfb};
  unsigned char imull$[] = {0x45, 0x69, 0xdb, 0x00, 0x00, 0x00, 0x00};
  unsigned char imullpilha[] = {0x44, 0x0f, 0xaf, 0x5d, 0xfc};
  unsigned char imulledi[] = {0x44, 0x0f, 0xaf, 0xdf};
  unsigned char subq32[] = {0x48, 0x83, 0xec, 0x20};
  unsigned char jneret[] = {0x75, 0x02, 0xc9, 0xc3};
  unsigned char cmpl0edi[] = {0x83, 0xff, 0x00};
  unsigned char cmpl0pilha[] = {0x83, 0x7d, 0xfc, 0x00};
  /*
  Cada array acima executa um código de máquina necessitado pelo programa.
  Por exemplo, o subq32[] subtrai 32 bytes da pilha.

  A variável codenum irá contar quantos bytes temos atualmente no vetor
code, que irá guardar o código de máquina gerado pelo nosso mini compilador.
 */
  /*
  Algoritmo de operações:

  - mover primeiro operando para um aux %r11d
  - operar o segundo operando armazenado nesse aux
  - mover o que está no aux para o que está sendo atribuído
  */
  while ((c = fgetc(f)) != EOF) {
    switch (c) {
    case 'f': { /* function */
      char c0;
      if (fscanf(f, "unction%c", &c0) != 1)
        error("comando invalido", line);
      *entry = (funcp)&code[codenum];
      /*
      Cada vez que tiver um input de function, atualizamos o ponteiro de entry,
      fazendo com que ele sempre aponte para a última chamada de função
      */
      vetorEnderecos[contFunc] = &code[codenum];
      contFunc++;
      /*
      Colocamos a função na posição em um vetor em que armazenaremos os endereços de início de todas as funções do programa. A função 0, na posição 0, a função 1, na posição 1, e assim em diante.
      */
      for (int c = 0; c < 4; c++) {
        code[codenum] = inicio[c];
        codenum++;
      }
      for (int c = 0; c < 4; c++) {
        code[codenum] = subq32[c];
        codenum++;
      }
      break;

      /*
      Após isso, executamos o código de máquina de início da função (pushq %rbp, movq %rsp, %rbp) e subtraímos 32 bytes da pilha, para podermos armazenar as 5 variáveis locais lá (v0, v1, v2, v3, v4). A v0 fica no -4(%rbp), a v1 fica no -8(%rbp), e assim em diante. 
      */
      
    }
    case 'e': { /* end */
      char c0;
      if (fscanf(f, "nd%c", &c0) != 1)
        error("comando invalido", line);
      for (int c = 0; c < 2; c++) {
        code[codenum] = ret[c];
        codenum++;
      }
      break;
      /*No comando de end, executamos a saída da função, isto é, a dupla de 
comandos "leave ret". Nesse momento, o retorno já foi movido para o %eax pelo ret
ou pelo zret.
      */
    }
    case 'r': { /* retorno incondicional */
      int idx0;
      char var0;
      if (fscanf(f, "et %c%d", &var0, &idx0) != 2)
        error("comando invalido", line);
      /*
      Aqui, separamos em três casos: quando queremos retornar 
      uma constante, uma variável, ou o parâmetro p0.
      */
      if (var0 == '$') {
        // quando é dólar, começa com b8
        for (int c = 0; c < 5; c++) {
          code[codenum] = movr$[c];
          codenum++;
        }
        codenum -= 4;
        for (int c = 0; c < 4; c++) {
          code[codenum] = (char)(idx0 >> 8 * (c));
          codenum++;
        }
        /*
        Aqui, colocamos o código de máquina para mover um número para
o %eax, que seria o b8 seguido dos 4 bytes do número desejado em little-endian.
O segundo for trabalha exatamente nisso, pegando o número fornecido no input e
colocando os bytes em little-endian no código de máquina.
*/
      }

      else if (var0 == 'v') {
      
        for (int c = 0; c < 3; c++) {
          code[codenum] = movrpilha[c];
          codenum++;
        }
        code[codenum - 1] = (char)(0xfc - idx0 * 4);
        /*
        Aqui, colocamos o código de máquina para mover uma posição da pilha para
o %eax. A conta feita ao final serve para exatamente definir qual é a posição na pilha que iremos mover para o %eax, ou seja, qual variável iremos mover para o registrador de retorno. A conta feita é reduzir o byte "fc", que caracteriza a posição -4(%rbp), por um múltiplo de 4 bytes, dependendo da variável escolhida.
        Por exemplo, se estamos fazendo a variável v1, diminuiremos 4 bytes de fc.

*/
      } else // caso do p
      {
        for (int c = 0; c < 2; c++) {
          code[codenum] = movedieax[c];
          codenum++;
        }
      }
      break;
    }
      /*
      Nesse caso, quando queremos retornar o p0, basta mover o %edi para o %eax. Movemos o código de máquina que faz isso que temos salvo para o array code.
*/

    case 'z': { /* retorno condicional, 'zret' varpc varpc */
      // Se o primeiro varpc for zero, retornar a função.
      int idx0, idx1;
      char var0, var1;
      if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4)
        error("comando invalido", line);
      if (var1 == '$') {
        /*
      Aqui, independentemente se retornamos ou não, posicionamos o que será ou não será retornado no %eax, visto que, caso não seja retornado, quando chamarmos uma
função que retorne, ela irá sobrescrever esse retorno que não ocorreu. O algoritmo
para colocar o varpc no %eax já foi exibido outras vezes no programa.
    
*/
        for (int c = 0; c < 5; c++) {
          code[codenum] = movr$[c];
          codenum++;
        }
        codenum -= 4;
        for (int c = 0; c < 4; c++) {
          code[codenum] = (char)(idx1 >> 8 * (c));
          codenum++;
        }
      }

      else if (var1 == 'v') {
       
        for (int c = 0; c < 3; c++) {
          code[codenum] = movrpilha[c];
          codenum++;
        }
        code[codenum - 1] = (char)(0xfc - idx1 * 4);
      } else // caso do p
      {
        for (int c = 0; c < 2; c++) {
          code[codenum] = movedieax[c];
          codenum++;
        }
      }
      /*
    Para a condição, são dois passos simples: comparamos o número com 0 e
colocamos o retorno no array code. Caso o número seja 0 mesmo, não fazemos nada e
deixamos o programa retornar. Caso a condição não se cumpra, fazemos um jump,
pulando os dois bytes que colocamos para fazer o retorno e, assim, seguimos no
programa.

*/
      if (var0 == 'p') {
        for (int c = 0; c < 3; c++) {
          code[codenum] = cmpl0edi[c];
          codenum++;
        }
      }
      if (var0 == 'v') {
        for (int c = 0; c < 4; c++) {
          code[codenum] = cmpl0pilha[c];
          codenum++;
        }
        code[codenum - 2] = (char)(0xfc - idx0 * 4);
      }
      if (var0 == '$') {
        if (idx0 == 0) {
          for (int c = 0; c < 2; c++) {
            code[codenum] = ret[c];
            codenum++;
          }
        } else {
          break;
        }
      }
      for (int c = 0; c < 4; c++) {
        code[codenum] = jneret[c];
        codenum++;
      }
      break;
    }
    case 'v': { /* atribuicao */ // ex: v0 = call [..]
      int idx0;
      char var0 = c, c0;
      if (fscanf(f, "%d = %c", &idx0, &c0) != 2)
        error("comando invalido", line);
    /*
    Aqui, temos o caso de atribuição, isto é, uma variável "vn" ser igual
a algo. A varíavel pode ser igual ao retorno de uma função que é chamada aqui,
ou a uma operação entre dois varpc (variável, parâmetro e constante) diferentes.
*/
      if (c0 == 'c') { /* call */
        int fint, idx1;
        char var1;
        if (fscanf(f, "all %d %c%d\n", &fint, &var1, &idx1) != 3)
          error("comando invalido", line);
        for (int c = 0; c < 3; c++) {
          code[codenum] = salvaedi[c];
          codenum++;
        }
        // No caso de um call, a primeira coisa que fazemos é preservar o %edi na pilha, na posição adequada.
        if (var1 == '$') {
          for (int c = 0; c < 5; c++) {
            code[codenum] = mov$edi[c];
            codenum++;
          }
          codenum -= 4;
          for (int c = 0; c < 4; c++) {
            code[codenum] = (char)(idx1 >> 8 * (c));
            codenum++;
          }
        }
        /*
        Aqui, passamos a analisar o que vai ser posto no parâmetro da chamada
da função. Caso seja uma constante, movemos ela para o %edi, colocando o opcode
que faz esse tipo de movimento e depois colocamos o número nos últimos 4 bytes
de acordo com um algoritmo já usado anteriormente.
*/
        if (var1 == 'v') {
          for (int c = 0; c < 3; c++) {
            code[codenum] = movedipilha[c];
            codenum++;
          }
          code[codenum - 1] = (char)(0xfc - idx1 * 4);
        }
        for (int c = 0; c < 5; c++) {
          code[codenum] = call[c];
          codenum++;
        }
        /*Caso seja uma variável, movemos a posição correta da pilha para o %edi,
usando o algoritmo já explicado anteriormente para pegarmos a variável correta.
*/
      
        unsigned long int offset = (unsigned long int)vetorEnderecos[fint] -
                                   (unsigned long int)&code[codenum];
        
        /*
        Para fazer um call, precisamos dos 4 bytes de offset, que posicionamos depois do e8. Para fazer o cálculo desse offset, precisamos fazer a seguinte operação: endereço da função que irá ser chamada - RIP. O RIP, no nosso caso, será sempre a instrução depois do call, ou seja, 5 bytes a frente do call. No nosso caso, na hora de calcular, o codenum já acaba se posicionando no RIP do
vetor code.
*/
        
        for (int c = 0; c < 4; c++) {
          code[codenum - 4 + c] = (offset) % 256;
          offset >>= 8;
        }
        // Aqui, posicionamos o offset em little-endian no vetor code.
        
        // Após call, precisamos restaurar o edi.
        for (int c = 0; c < 3; c++) {
          code[codenum] = restauraedi[c];
          codenum++;
        }
        // Aqui, finalizamos movendo o que esta no %eax para a variável desejada.
        for (int c = 0; c < 3; c++) {
          code[codenum] = moveaxpilha[c];
          codenum++;
        }
        code[codenum - 1] = (char)(0xfc - idx0 * 4);
      } else { /* operação aritmética */

        /*
  Algoritmo de operações:

  - Mover primeiro operando para um auxiliar %r11d.
  - Operar o segundo operando armazenando o resultado 
  dessa operação nesse auxiliar.
  - Mover o que está no aux para o que está sendo atribuído.

Assim, nos esquivamos da operação entre dois espaços de memória,
proibida em código de máquina. Isso aconteceria quando tivéssmos qualquer
operação entre 2 variáveis. Preferirmos padronizar as operações para exatamente
ter uma unidade no código.

Todos os algoritmos abaixo seguem essa linha, tendo alguns algoritmos já
usados no código anteriormente no meio, como a subtração para pegar a posição
correta da pilha, por exemplo.
  */

        
        int idx1, idx2;
        char var1 = c0, var2, op;
        if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4)
          error("comando invalido", line);
        if (var1 == '$') {
          for (int c = 0; c < 6; c++) {
            code[codenum] = movr11$[c];
            codenum++;
          }
          codenum -= 4;
          for (int c = 0; c < 4; c++) {
            code[codenum] = (char)(idx1 >> 8 * (c));
            codenum++;
          }
          code[codenum] = (char)(0xfc - idx1 * 4);
        } else if (var1 == 'v') {
          for (int c = 0; c < 4; c++) {
            code[codenum] = movpilhar11[c];
            codenum++;
          }
          code[codenum - 1] = (char)(0xfc - idx1 * 4);
        } else {
          for (int c = 0; c < 3; c++) {
            code[codenum] = movr11edi[c];
            codenum++;
          }
        }
        if (op == '+') {
          if (var2 == '$') {
            for (int c = 0; c < 7; c++) {
              code[codenum] = add$[c];
              codenum++;
            }
            codenum -= 4;
            for (int c = 0; c < 4; c++) {
              code[codenum] = (char)(idx2 >> 8 * (c));
              codenum++;
            }
          }
          if (var2 == 'v') {
            for (int c = 0; c < 4; c++) {
              code[codenum] = addpilha[c];
              codenum++;
            }
            code[codenum - 1] = (char)(0xfc - idx2 * 4);
          }
          if (var2 == 'p') {
            for (int c = 0; c < 3; c++) {
              code[codenum] = addedi[c];
              codenum++;
            }
          }
        } else if (op == '-') {
          if (var2 == '$') {
            for (int c = 0; c < 7; c++) {
              code[codenum] = sub$[c];
              codenum++;
            }
            codenum -= 4;
            for (int c = 0; c < 4; c++) {
              code[codenum] = (char)(idx2 >> 8 * (c));
              codenum++;
            }
          }
          if (var2 == 'v') {
            for (int c = 0; c < 4; c++) {
              code[codenum] = subpilha[c];
              codenum++;
            }
            code[codenum - 1] = (char)(0xfc - idx2 * 4);
          }
          if (var2 == 'p') {
            for (int c = 0; c < 3; c++) {
              code[codenum] = subedi[c];
              codenum++;
            }
          }
        } else {
          if (var2 == '$') {
            for (int c = 0; c < 7; c++) {
              code[codenum] = imull$[c];
              codenum++;
            }
            codenum -= 4;
            for (int c = 0; c < 4; c++) {
              code[codenum] = (char)(idx2 >> 8 * (c));
              codenum++;
            }
          }
          if (var2 == 'v') {
            for (int c = 0; c < 5; c++) {
              code[codenum] = imullpilha[c];
              codenum++;
            }
            code[codenum - 1] = (char)(0xfc - idx2 * 4);
          }
          if (var2 == 'p') {
            for (int c = 0; c < 4; c++) {
              code[codenum] = imulledi[c];
              codenum++;
            }
          }
        }
        for (int c = 0; c < 4; c++) {
          code[codenum] = movr11pilha[c];
          codenum++;
        }
        code[codenum - 1] = (char)(0xfc - idx0 * 4);
      }
      break;
    }
    default:
      error("comando desconhecido", line);
    }
    line++;
    fscanf(f, " ");
  }
  return;
}