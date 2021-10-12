.bp
.he 'GAR (cmd)'TROPIX: Manual de Modificações'GAR (cmd)'
.fo 'Atualizado em 20.07.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "gar -"
cria/atualiza coleções de arquivos
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "09.11.87  1.0.2" 20
Foi modificada a numeração dos volumes: agora começam de 1
(ao invés de 0). Esta modificação é apenas externa - internamente,
o formato da coleção não foi modificada, começando de 0.
Deste modo não foi criado nenhum problema de compatibilidade
de coleções velhas/novas.
.sp
Ao final de um volume, se o volume de continuação for no mesmo
dispositivo, não é mais necessário repetir o seu nome (desde que
não seja a entrada/saída padrão).

.ip "12.01.88  1.1.0" 20
Foi elaborado o conceito do "garfield", um arquivo que durante
a comparação é criado com o nome dos arquivos diferentes.
Durante a extração, este arquivo é consultado para extrair
apenas os arquivos diferentes.
.sp
A opção "-d" passou a significar, durante a escrita de uma
coleção, para não entrar em subdiretórios. 

.ip "15.01.88  1.1.1" 20
Foi alterado o módulo "out.c" para que, no caso interativo,
diretórios não desejados retornarem "1" para "ftw",
de modo a não percorrer estes diretórios.

.ip "15.02.88  1.1.2" 20
Foi feita uma pequena correção na leitura das linhas
do arquivo "garfield", que estava sendo lido em uma área automática.
Foi alterado o módulo "aux.c".

.ip "10.07.88  2.0.0" 20
Foi acrescida a opção ".". No modo normal, agora, arquivos
começando por ponto não são gravados na coleção.

.ip "12.07.88  2.0.1" 20
Agora, as coleções são abertas com o indicador de "O_LOCK",
para evitar a escrita simultanea de 2 "gars" no mesmo disquete (por exemplo).

.ip "01.12.88  2.1.0" 20
As partes não significativas do cabeçalhos de módulos objetos
são mascaradas, para simplificar a comparação de objetos.

.ip "17.05.89  2.2.0" 20
Nas leituras/escritas com erro agora são escritas as causas com "perror".
Alterado "aux.c".

.ip "19.08.89  2.3.0" 20
Foi corrigido o reconhecimento de padrões envolvendo ".." para
que mesmo sem a opção ".", arquivos do tipo "../a/b" e "a/../b"
sejam incluídos. 

.sp
Foi incluída o novo modo de operação "append" ("-a").

.ip "22.12.89  2.3.1" 20
Em caso de cabeçalho de volume inválido ou volume de continuação,
"gar" permite a busca de um arquivo GAR no meio da coleção.
Alterados "append.c", "aux.c", "gar.c", "in.c" e "tok.c".

.sp
Foi corrigido detalhe da criação de um link para um arquivo
quando este link já existia. Alterado "in.c". 

.ip "02.01.90  2.3.2" 20
Foi otimizado o programa para pular blocos (rotina "inskip")
rapidamente, através de "seek" ao invés de leitura. 
Com isto, os blocos pulados não têm o CRC checado, e por isto
foi alterada a semantica da opção "-r".
A blocagem de entrada para "-t" pode ser "1" (mais rápido).
Operações que ficaram bem mais rápidas: "-t",
"-x" (com garfield, padrões ou interativo) e "-a".
Alterados "aux.c", "gar.c", "gar.h",  "in.c" e "out.c".

.sp
Foi consertada a tabela, para imprimir o UID numérico
no caso do UID simbólico não puder ser obtido.
Alterado "in.c".

.sp
Foi consertado o número do volume de continuação, que estava defasado
de um. Alterado "aux.c".

.sp
A opção "-." faz com que os diretórios seja gravados.
Alterado "out.c".

.ip "16.04.90  2.3.3" 20
Foi estendido a função de saída ("-o") para gerar também coleções
no formato TAR. Modificados todos os módulos.

.sp
Foi consertada a seção de criação em "in.c", que emitia mensagens de
erro errôneas para criação de diretórios.

.sp
Foi criada a opção "-q" para a comparação rápida de módulos objeto.
Modificado "gar.c" e "in.c".

.ip "28.09.90  2.3.4" 20
Foi feita uma revisão geral, e introduzida a redução (opções "-C" e "-b").

.sp
Foi consertado o problema da emissão do "max_code" justamente quando
o tamanho do código aumentava.

.ip "24.10.90  2.3.5" 20
Introduzida a comparação com atualização (opção "-u").

.sp
Foi melhorada a alocação do arquivo temporário de compactação.
Agora escolhe "/tmp/...", "/usr/tmp/..." e "...".

.ip "21.11.90  2.3.6" 20
Foi resolvido o problema da emissão de "max_code" justamente quando
o tamanho do código mudava.

.ip "26.11.90  2.3.7" 20
Foi introduzida a opção "-S", com a criação de coleções incrementais,
através de "garstamp". 
Alterados "src/gar.c" e "src/out.c".

.sp
Foi alterado o formato de impressão de diagnósticos. Agora os diagnósticos
vem na linha seguinte à linha contendo o nome do arquivo.
Alterados "src/append.c", "src/aux.c", "src/gar.c", "src/in.c",
"src/out.c" e "src/tok.c".

.ip "13.12.90  2.3.8" 20
Novamente, mais uma vêz foi resolvido o problema da emissão de "max_code"
justamente quando o tamanho do código mudava, o que também não foi o caso
na versão 2.3.6.
Alterados "src/reduce1.c".

.sp
A área de códigos de redução foi passada de "short" para "int".

.ip "14.01.91  2.3.9" 20
Foi acrescentada o teste "bit_sz < MAX_BIT_SZ" para certificar-se de que o
limite não é passado.

.ip "12.03.91  2.3.10" 20
Foi acrescentado a busca de arquivos no formato TAR no meio de uma coleção.
Alterados "src/gar.c" e "src/tok.c".

.ip "19.10.91  2.3.11" 20
Foi eliminada a mensagem de "link espúrio" quando os nomes são iguais
(decorrentes de uma operação de "append").

.sp
Foi introduzida a opção "-M".

.ip "22.03.92  2.3.12" 20
Foram introduzidos os padrões de exclusão (opção "-P").

.ip "20.11.92  3.0.00" 20
Agora usando a chamada ao sistema "mkdir".

.ip "21.07.93  3.1.00" 20
Introduzido o novo método de compressão, a "deflação".

.ip "21.07.93  3.1.01" 20
Agora, subárvores do tipo ".old/..." nem são mais caminhadas.

.ip "07.04.94  3.1.02" 20
Consertado pequeno detalhe de criação dos diretórios (colocado "0777").

.ip "07.04.95  3.1.03" 20
???.

.ip "11.07.95  3.1.04" 20
Extendida a opção "-g" para a saída.

.ip "11.03.97  3.1.05" 20
Agora, os diretórios são sempre incluídos na coleção.

.ip "04.05.97  3.1.06" 20
O comando sofreu (mais) uma revisão.

.ip "09.05.97  3.1.07" 20
Introduzida a lista para atualizar as datas dos diretórios no final.

.ip "25.05.97  3.1.08" 20
Agora temos a opção "-F" para dar o dispositivo.

.ip "03.06.97  3.1.09" 20
Pequena correção no formato TAR: este formato processa apenas arquivos
regulares.

.ip "04.08.97  3.1.10" 20
Agora, lendo nomes da entrada padrão serão interpretados como árvores.

.ip "05.04.98  3.1.11" 20
Foram colocados alguns "close" faltando no módulo "out.c".

.ip "17.04.98  3.1.11" 20
Introduzidos os códigos de dispositivo '-2' e '-3', para o ZIP.

.ip "16.07.98  3.1.12" 20
Consertado o esquecimento da criação de arquivos novos para "-F".

.ip "12.01.99  3.1.13" 20
Foi introduzida a nova versão de "askyesno".

.ip "08.05.02  4.2.0" 20
Introduzido o elo simbólico e as cores dos nomes dos arquivos.

.ip "19.03.03  4.4.0" 20
Alterado o cabeçalho dos arquivos para poder comportar os
novos tamanhos (32 bits) do DEV e INO.

.ip "14.10.04  4.6.0" 20
O indicador "-f" agora pode ser usado também para a saída.

.ip "17.07.05  4.8.0" 20
Foi atualizada a versão TAR (mas ainda não concluída).

.ip "20.07.06  4.9.0" 20
A opção "-d" na saída passa a significar "não incluir diretórios".
