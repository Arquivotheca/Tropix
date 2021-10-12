.bp
.he 'CPTREE (cmd)'TROPIX: Manual de Modificações'CPTREE (cmd)'
.fo 'Atualizado em 21.12.04'Versão 4.7.0'Pag. %'

.b NOME
.in 5
.wo "cptree -"
copia uma árvore
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "13.05.87  1.0.1" 20
O funcionamento  da opção "-o" foi alterado de modo a
manter também a data de criação dos arquivos.
.sp

.ip "06.07.87  1.0.2" 20
O percurso da árvore, originariamente em largura, passou a ser
em profundidade, sendo criada a opção "-b", para uma possível escolha
do modo de busca anterior.
.ip "" 20
O erro que havia ao copiar a árvore "/" foi corrigido.
.ip "" 20
Passou a ser possível o não percurso de uma determinada
subárvore através da opção "-i", que pedirá a confirmação também 
para diretórios.
.ip "" 20
Foi alterado o número de bits do modo do arquivo, passando para 24.
.sp

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

.ip "09.06.88  1.1.1" 20
O erro que havia ao copiar a árvore "/"
para um diretório da própria árvore "/" foi corrigido.

.ip "14.11.89  2.3.0" 20
O programa sofreu (ou deliciou-se) com uma revisão global.

.ip "28.01.90  2.3.1" 20
Foi consertado o detalhe da cópia do diretório ".". 

.ip "21.11.92  3.0.0" 20
Agora usando a chamada ao sistema "mkdir".

.ip "17.12.93  3.0.1" 20
Agora usando a chamada "inopen" e "ftw" nova. A opção "-b" foi substituída
pela "-L".

.ip "13.04.97  3.0.2" 20
Introduzida a lista para atualizar as datas dos diretórios no final.

.ip "03.05.97  3.0.3" 20
Corrigido o erro na composição dos nomes destino.

.ip "04.08.97  3.0.4" 20
Introduzido a opção "-P".

.ip "15.01.99  3.1.0" 20
Foi atualizada a chamada à função "askyesno".

.ip "08.01.01  4.0.0" 20
Foi adicionado o modo "de força" de "askyesno".

.ip "26.04.02  4.2.0" 20
Adicionado os elos simbólicos.

.ip "22.09.02  4.6.0" 20
Adicionada a opção "-l".

.ip "21.12.02  4.7.0" 20
Pequena correção na opção "-l".
