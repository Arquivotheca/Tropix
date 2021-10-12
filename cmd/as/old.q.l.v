                        	.text
000000                  start0:
000000 EB26             	jmp	beta0
                        
                        	.align	4
000004                  alpha0:
000004 BC 0000 0003     	movl	#3,sp
                        
                        |*	.align	4
00000C                  gamma0:
00000C 0000 0000 0000   	.double	1.0
       F03F
                        
000014                  	.dsb	20
                        
000028                  beta0:
000028 BC 0000 0003     	movl	#3,sp
                        	
                        
                        
                        	.org	start0+512
000200                  start1:
000200 E9 0000 00DB     	jmp	beta1
                        
                        
                        	.align	4
000208                  alpha1:
000208 BC 0000 0003     	movl	#3,sp
                        
000210                  gamma1:
000210 0000 0000 0000   	.double	1.0
       F03F
                        
000218                  	.dsb	200
                        
0002E0                  beta1:
0002E0 BC 0000 0003     	movl	#3,sp



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

TEXT  start0                                   R       0000 0000
TEXT  beta0                                    R       0000 0028
TEXT  alpha0                                   R       0000 0004
TEXT  gamma0                                   R       0000 000C
TEXT  start1                                   R       0000 0200
TEXT  beta1                                    R       0000 02E0
TEXT  alpha1                                   R       0000 0208
TEXT  gamma1                                   R       0000 0210
