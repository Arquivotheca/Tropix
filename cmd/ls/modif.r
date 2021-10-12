.bp
.he 'LS (CMD)'TROPIX: Manual de Modificações'LS (CMD)'
.fo 'Atualizado em 26.04.07'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "ls -"
lista características de arquivos e diretórios
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "22.06.87  1.0.1" 20
Foi adaptado para mostrar os 8 novos bites do modo.
Agora são também mostrados os bites "m" (mantém o texto na memória),
e os provisóriamente chamados de 1, 2, 3, 4, 5, 6 e 7, mas ainda
com funcionalidade indefinida. Para tanto a opção "-l" foi alterada,
assim como foi criada a opção "-L". Agora a opção "-l" mostra apenas
os bites de proteção dos arquivos listados. A nova opção "-L" mostra
além da proteção os bites do modo dos arquivos listados.

.in
.sp 2
.ip "01.01.88  1.1.0" 20
VERSÃO CORRENTE

.ip "25.02.90  2.3.0" 20
Foi realizada uma revisão global, e inserindo a nova opção "-R".

.ip "17.09.90  2.3.1" 20
Ordena os diretórios alfabeticamente (na opção "-R").

.ip "13.12.93  3.0.0" 20
Usando agora as chamadas "inopen" e "instat".

.sp
Usando "strerror" ao invés de "perror".

.ip "03.03.95  3.0.1" 20
Adaptação para o PC

.ip "22.05.97  3.0.2" 20
Mais uma revisão.

.ip "10.02.99  3.1.7" 20
Inseridas as cores.

.ip "07.08.01  4.0.0" 20
Usando "readdir".

.ip "17.04.02  4.2.0" 20
Adicionados os elos simbólicos e as cores brilhantes.

.ip "06.09.02  4.3.0" 20
Colocado o cálculo da largura dos campos, para ficar mais estreito.

.ip "27.03.03  4.4.0" 20
Consertado o problema do "-N".

.ip "02.10.03  4.5.0" 20
Usando a comparação FOLD.

.ip "18.10.03" 20
Desfeito o cálculo da largura dos campos.

.ip "18.09.05  4.8.0" 20
Recolocado o cálculo da largura dos campos, para ficar mais estreito
(que indecisão!).

.ip "26.04.07  4.9.0" 20
O Tamanho agora é impresso como "sem sinal".
