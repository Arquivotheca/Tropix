.bp
.he 'AS.486 (cmd)'TROPIX: Manual de Modificações'AS.486 (cmd)'
.fo 'Atualizado em 05.12.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "as.486 -"
montador para INTEL-80386/486/PENTIUM
.br

.in
.sp 2
.ip "07.07.94  3.0.0" 20
VERSÃO CORRENTE
.sp

.ip "25.07.94  3.0.1" 20
Agora só aceitando 2 dígitos hexadecimais em seqüências de "escape" do
tipo "....\x12....".

.ip "07.08.94  3.0.2" 20
Agora, as operações "<<" e ">>" são SEM sinal.

.ip "12.08.94  3.0.3" 20
Pequeno esquecimento de permitir relocação na seção CONST.

.ip "16.08.94  3.0.4" 20
Agora, podendo ter valores relocáveis do CONST para TEXT e vice-versa.

.ip "26.08.94  3.0.5" 20
Consertado o esquecimento das versões "far" de "ret".
.sp
Adicionados os sinônimos "enable", "disable", "inten" e "intdis".

.ip "04.09.94  3.0.6" 20
Acrescentados "callgw" e "callgl".
.sp
Trocado "enable" por "enint" e "disable" por "disint".
.sp
Corrigido os "pushs" e "pops" para registros de segmentação.
.sp
Criado o conceito de "NEEDED" para gerenciar imediatos e deslocamentos
indefinidos no primeiro passo.
.sp
Feita uma revisão no módulo "expr.c".

.ip "16.09.94  3.0.7" 20
Foram criadas as pseudos ".ifnz", ".ifdef", ".elif", ".else" e outras.
.sp
Foi acrescentada a opção "-D".
.sp
Agora "[" é sinônimo de "{".

.ip "26.09.94  3.0.8" 20
Módulo "if.c": as expressõs não são mais avaliadas se "false_level"
diferente de zero.
.sp
Módulos "regular1.c" e "regular2.c": a análise de
operandos agora permite termos operandos opcionais.
.sp
A Tabela "src_op_table.c"
foi alterada para refletir os operandos opcionais (em particular do
"muls").
.sp
Acrescentados os operadores relacionais "==", "!=", ">", ">=", "<" e "<=".
.sp
Acrescentados os operadores lógicos "&&", "||" e "!".

.ip "??.??.??  3.0.9" 20
.wo .............

.ip "??.??.??  3.0.10" 20
.wo .............

.ip "08.04.97  3.0.11" 20
.wo .............

.ip "07.08.97  3.0.12" 20
Introduzida a sintaxe "call *alpha" e "jmp *alpha".

.ip "23.01.00  3.2.3" 20
A versão do programa objeto passa a ser 0.

.ip "16.11.00  4.0.0" 20
Incorporadas as novas instruções de máquina:
"bswap", "cpuid", "invd", "invlpg", "rdmsr", "rsm", "wbinvd", wrmsr", "xadd", e "rdtsc".

.sp
No momento faltando apenas "cmpxchg" e "cmpxchg8b" para completar o
repertório do Pentium.

.ip "22.08.01  4.0.0" 20
Novo formato da tabela de símbolos SYM.

.ip "24.08.01  4.1.0" 20
Identificadores sem limite de tamanho.

.ip "20.08.02  4.3.0" 20
Novo cálculo (simplificado) dos SDIs.

.ip "17.03.03  4.4.0" 20
Agora vetor de indefinições do passo 1 dinâmico. Usando "malloc" da libc.

.ip "05.12.03  4.5.0" 20
Estendido o processamento de SDIs para ALIGNs.
