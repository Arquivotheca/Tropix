.tp
.sp 10
.nf
      *******   *******  *******   *******    *    *     *
         *      *     *  *     *   *     *    *     *   *
         *      *     *  *     *   *     *    *      * *
         *      *******  *     *   *******    *       *
         *      *   *    *     *   *          *      * *
         *      *    *   *     *   *          *     *   *
         *      *     *  *******   *          *    *     *
.fi
.sp 10
.ce
VI - Editor Visual de Textos
.sp 2
.ce
Versão 3.0.1
.sp 4
.ce
Guia
.sp 10
.ce
Abril de 1997
.he ' '- ii - ' '
.bp
.sp 4
.ce
.b
APRESENTAÇÃO
.r
.sp 2

.pp
Este é o guia do Editor Visual de Textos "vi",
o editor de textos padrão do Sistema Operacional Multiprocessado TROPIX.

.pp
O editor "vi" foi desenvolvido a partir
das especificações da interface com o usuário
de um editor homônimo criado e implementado
na Universidade da California, em Berkeley,
e adotado pela AT&T para o Unix System V.

.pp
Desde a versão 2.0 o "vi" incorpora em seu modo Comandos
toda a funcionalidade do editor "ed"
(incluindo expressões regulares,
substituições e transferência/cópia de blocos)
e em seu modo Visual várias extensões
(incluindo a marcação de linhas,
a capacidade de repetição do último comando de edição
e um conjunto de novos comandos para a localização no texto).

.pp
Este manual descreve os comandos do editor "vi" em sua Versão 3.0.1.
O Capítulo 1 introduz os conceitos básicos
para a compreensão do funcionamento do editor "vi".
O Capítulo 2 descreve os comandos do modo Visual;
são apresentados os comandos para observação e localização no texto em edição,
os comandos para alteração do texto em edição e
as operações de cópia e transferência de blocos de texto.
O Capítulo 3 descreve o modo Comandos;
são apresentados os comandos e
como especificar a sua abrangência no texto em edição.
O Capítulo 4 descreve a inclusão de texto novo via teclado.
O Capítulo 5 descreve o subconjunto de expressões regulares
reconhecidas pelo editor.

.pp
As formas de evocação do editor "vi" a partir do Interpretador de Comandos
podem ser encontradas nas páginas do Manual de referência do Sistema TROPIX
referentes ao editor "vi" (cmd).

.he ' '- iii - ' '
.bp
.sp 10
.ce
CONTEÚDO
.sp 5
.in +3
Cap. 1 - Conceitos Fundamentais ............................ 1
.sp
Cap. 2 - Modo Visual ....................................... 3
.sp
.in +3
2.1 - Observação e Localização no Texto ................. 3
.sp
2.2 - Edição Básica do Texto ............................ 5
.sp
2.3 - Cópia e Transferência de Blocos de Texto .......... 7
.in -3
.sp
Cap. 3 - Modo Comandos ..................................... 8
.sp
.in +3
3.1 - Endereçamento ..................................... 8
.sp
3.2 - Comandos .......................................... 9
.in -3
.sp
Cap. 4 - Entrada e Inserção de Texto via Teclado ........... 15
.sp
Cap. 5 - Expressões Regulares .............................. 17
.in -3

.bp 1
.he 'Sistema TROPIX'Guia'VI'
.fo 'Atualizado em 30.04.97'Versão 3.0.1'Pag. %'

