.bp
.he 'GETOPT (libc)'TROPIX: Manual de Modificações'GETOPT (libc)'
.fo 'Atualizado em 13.7.88'Versão 2.1'Pag. %'

.b NOME
.in 5
.wo "getopt -"
Obtém as informações dadas a um programa.
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "13.07.88  1.0.1" 20
Agora quando ao fim da leitura das opções,
"getopt" além de retornar EOF, se reinicializa
permitindo a leitura de opções de um outro
vetor de argumentos.
