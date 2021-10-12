.bp
.he 'BOOT0 (sys)'TROPIX: Manual de Modificações'BOOT0 (sys)'
.fo 'Atualizado em 02.01.08'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "boot0 -"
sistema de carga do sistema operacional TROPIX
.br

.in
.sp 2
.ip "01.01.96  3.0.0" 20
VERSÃO CORRENTE
.sp

.ip "26.07.96  3.0.1" 20
O módulo "boot0"foi estendido para
continuar com a fase seguinte no caso de nada ser teclado
após (aproximadamente) 15 segundos.

.ip "30.08.96  3.0.2" 20
O "boot0" agora só tenta ler um bloco 3 vezes, e escreve uma mensagem
mais detalhada do erro.

.ip "06.09.96  3.0.3" 20
O "boot0" usa também o "h0" para testar se um disco existe.
.sp
A função "get_param" foi colocada no setor 1.

.ip "19.09.96  3.0.4" 20
Foi acrescentado o "INT 13,15" no "boot0" para testar a existência de
um disco.

.ip "27.03.97  3.0.5" 20
Foi melhorada a distinção a entre discos SCSI e IDE com os esquemas
(64, 32) e (255, 63).

.ip "14.04.99  3.1.8" 20
Foi acrescentado o DOS FAT32.

.ip "02.05.00  3.2.3" 20
Foram acrescentados os códigos para > 8 GB (leitura linear).

.ip "26.07.00  4.0.0" 20
Procura a primeira entrada TROPIX (se o índice for NULO).
Retirado o "disint". Atualizados os nomes das partições.

.ip "28.07.00" 20
Usando agora o tempo obtido da CMOS para contar os 15 segundos.

.ip "27.08.02" 20
Procurando também sistemas de arquivos T1.

.ip "02.01.08  4.9.0" 20
Imprimindo os números dos blocos sem assinatura.
