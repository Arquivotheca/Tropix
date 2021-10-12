.bp
.he 'LD (cmd)'TROPIX: Manual de Modificações'LD (cmd)'
.fo 'Atualizado em 12.07.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
ld - editor-ligador de módulos objeto
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "13.06.88  2.0.1" 20
Foi retirado do "ld" o uso de "bit-fields", que não é
eficiente. Alterados os módulos "pass1.c", "pass2.c" e "sub.c".
.sp

.ip "21.07.88  2.0.4" 20
Foi alterada a opção "-u" para que liste apenas os símbolos
não referenciados dos módulos dados explicitamente.
Se foi dada duas vezes ("-uu"), lista todos os símbolos
não referenciados, inclusive dos módulos das bibliotecas.

.sp
Foi intruduzida o contrôle da versão dos módulos.
Se um módulo não for da versão corrente, nem da anterior,
é emitido uma advertencia.

.ip "20.07.89  2.2.0" 20
Foi corrigido a relocação em relação a símbolos absolutos
que estava incorreta com o uso da opção "-r".
Alterado "pass2.c".

.sp
Utilizando-se a opção "-r" agora não são mais impressas as
variáveis indefinidas.
Alterado "pass1.c".

.ip "04.08.89  2.2.1" 20
Foi alterado o critério de superposição de TEXT com DATA,
para possibilitar termos DATA abaixo do TEXT.

.ip "01.04.90  2.3.0" 20
Foram feitas diversas alterações em todos módulos para substituir
todas as tabelas de alocação estática para alocação dinâmica.
A tabela "hash" foi dobrada de tamanho.

.sp
Se não for dada a opção "-e", será usado o símbolo "start".

.ip "19.12.90  2.3.1" 20
Alterado para trabalhar com símbolos externos de 31 caracteres.

.ip "08.01.91  2.3.2" 20
Introduzida a opção "-.".

.ip "11.02.91  2.3.3" 20
O Algoritmo de "hash" dos símbolos foi convertido de colisões internas
para externas, o que remove o limite do número de
símbolos que podem ser armazenados.

.ip "13.04.95  3.0.0" 20
Adaptação para o PC.

.ip "20.05.97  3.0.1" 20
O programa sofreu (ou deliciou-se com) uma revisão global.

.sp
Além disto, consertado o problema do "-r", que não funcionava.

.ip "22.08.01  4.0.0" 20
Usando o novo formato da tabela de símbolos SYM.
.sp
Usando o novo formato da tabela de sinopse .LIBSYM.

.ip "26.09.01  4.2.0" 20
Alterado  passo 1, compondo inicialmente uma tabela de todos os símbolos
definidos em todas as bibliotecas.

.ip "11.06.02  4.3.0" 20
O programa foi completamente revisto, devido à inclusão da tabela de
referências externas. Foi suprimida a opção '-a'.

.ip "12.07.02" 20
Foi novamente totalmente revisto, para entender a opção "-r", e
criar bibliotecas compartilhadas (opção "-b").
