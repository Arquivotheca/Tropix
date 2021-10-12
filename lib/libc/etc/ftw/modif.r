.bp
.he 'FTW (libc)'TROPIX: Manual de Modificações'FTW (libc)'
.fo 'Atualizado em 25.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "ftw -"
caminha em uma árvore do sistema de arquivos
.br

.in
.sp 2
.ip "01.04.87  1.1" 20
VERSÃO CORRENTE
.sp

.ip "14.01.88  1.1.1" 20
O problema existente na interrupção do percurso
de subárvores, ocorrido em consequência do não 
fechamento dos respectivos diretórios, foi corrigido.
.sp

.ip "08.05.93  3.0.0" 20
Agora os nomes dos arquivos são ordenados.

.ip "14.12.93  3.0.1" 20
Nova implementação, recursiva, muito mais rápida, usando "alloca".
Desvantagem: não caminha por largura.

.ip "22.06.95  3.0.2" 20
Pequena melhoria: alocando menos memória através de "alloca".

.ip "12.04.97  3.0.3" 20
Introduzida as opções de "pré" ou "pós" ordem.

.ip "05.08.01  4.0.0" 20
Usando a função "readdir".

.ip "25.04.02  4.2.0" 20
Adaptada para elos simbólicos.
