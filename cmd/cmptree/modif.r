.bp
.he 'CMPTREE (cmd)'TROPIX: Manual de Modificações'CMPTREE (cmd)'
.fo 'Atualizado em 28.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "cmptree -"
compara árvores
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

.ip "09.07.87  1.0.2" 20
O percurso da árvore, originariamente em largura, passou a ser
em profundidade, sendo criada a opção "-b", para uma possível escolha
do modo de busca anterior.
.ip "" 20
O erro que havia ao copiar a árvore "/" foi corrigido.
.ip "" 20
Foi alterado o número de bits do modo do arquivo, passando para 24.
.sp

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

.ip "04.03.88  1.1.1" 20
As subárvores da árvore origem, cujo diretório raiz não existir
na árvore destino e não for criado durante a execução (opções "-u" e "-i"), 
não serão mais percorridas.
.sp

.ip "09.06.88  1.1.2" 20
O erro que havia ao copiar a árvore "/"
para um diretório da própria árvore "/" foi corrigido.
.sp
Foi alterado o funcionamento para detectar quando
uma subárvore fonte é a raiz da árvore destino.

.ip "24.11.89  2.3.0" 20
O programa sofreu (ou deliciou-se) com uma revisão global.

.ip "28.01.90  2.3.1" 20
Foi consertado o detalhe da comparação do diretório ".". 

.ip "19.11.91  2.3.2" 20
Quando fonte e destino são vazios, estava se esquecendo de fechar os arquivos.

.sp
Foi consertado o erro de comparação de módulos-objeto
com cabeçalhos inválidos.

.ip "21.11.92  3.0.0" 20
Agora usando a chamada ao sistema "mkdir".

.ip "17.12.93  3.0.1" 20
Agora usando a chamada "inopen" e "ftw" nova. A opção "-b" foi substituída
pela "-L".

.ip "02.02.96  3.0.2" 20
Revisão para o PC. Introduzido o "garfield".

.ip "03.06.96  3.0.3" 20
Trocada a opção "-i" por "-f".

.ip "14.04.97  3.0.4" 20
Introduzida a lista para atualizar as datas dos diretórios no final.

.ip "03.05.97  3.0.5" 20
Corrigido o erro na geração dos nomes destino.

.ip "04.08.97  3.0.6" 20
Introduzido a opção "-P".

.ip "10.01.99  3.1.0" 20
Atualizado para a nova versão de "askyesno".

.ip "24.01.00  3.2.3" 20
Introduzida a tabela de comparação de módulos objetos.

.ip "29.03.00  ....." 20
Feita uma revisão no código de retorno.

.ip "05.08.01  4.0.0" 20
Acrescentada a possibilidade da remoção do arquivo fonte quando o destino não existe.

.ip "26.04.02  4.2.0" 20
Acrescentados os elos simbólicos.

.ip "06.06.02  4.3.0" 20
Acrescentada a comparação das tabelas de referências externas.

.ip "28.10.03  4.5.0" 20
Acrescentada a opção "-d".
