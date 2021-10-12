.bp
.he 'EDBOOT (cmd)'TROPIX: Manual de Modificações'EDBOOT (cmd)'
.fo 'Atualizado em 29.08.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
.wo "edboot -"
gerencia "boot0" e "boot2"
.br

.in
.sp 2
.ip "26.07.96  3.0.0" 20
VERSÃO CORRENTE
.sp

.ip "09.09.97  3.0.1" 20
Adicionada a instalação do "boot0".

.ip "15.01.99  3.1.0" 20
Atualizado "askyesno" e testando agora a assinatura do
bloco 0.

.ip "02.02.99  3.1.6" 20
Agora, se não encontra "hda" tenta "sda".

.ip "12.07.00  4.0.0" 20
Agora processando também o "boot1". Verifica se as versões
de "boot0" e "boot1" são as atuais.

.ip "22.08.01  4.0.0" 20
Novo formato da tabela "nlist".

.ip "19.08.02  4.3.0" 20
Acrescentados os "boot1"s do T1.

.ip "23.10.04  4.6.0" 20
Agora procurando melhor "hda"/"sda".

.ip "29.08.06  4.9.0" 20
Acrescentada a opção "-V" para o modo de vídeo do "boot2".
