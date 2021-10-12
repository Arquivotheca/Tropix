.bp
.he 'PERROR (libc)'TROPIX: Manual de Modificações'PERROR (libc)'
.fo 'Atualizado em 28.07.91'Versão 2.3.2'Pag. %'

.b NOME
.in 5
Manipulação das mensagens de erro das chamadas ao sistema:
.sp
.wo "perror      -"
imprime a mensagem de erro
.br
.wo "errtostr    -"
obtém uma cadeia com o nome do erro 
.br
.wo "errno       -"
o número do erro
.br
.wo "sys_errlist -"
tabela de nomes de erros
.br
.wo "sys_nerr    -"
número de nomes de erros na tabela

.in
.sp 2
.ip "01.12.87  1.1.0" 20
VERSÃO CORRENTE
.sp

.ip "08.07.88  2.0.0" 20
Foi inserida a mensagem de "programa objeto de versão/modificação
descontinuada" em "errlst".

.ip "28.07.91  2.3.2" 20
Foi modificada toda a família de funções, para utilizar a tabela
agora residente no núcleo do sistema operacional, através de "kcntl".

.sp
As variáveis "sys_errlist" e "sys_nerr" não são mais suportadas.
