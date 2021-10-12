typedef union {
	int		value;
	char		*str;
	symbol_t	*sym;
	symbol_ref_t	sym_ref;
	expression_t	expression;
} YYSTYPE;
#define	T_REGISTER	258
#define	T_CONST	259
#define	T_DOWNLOAD	260
#define	T_SCB	261
#define	T_SRAM	262
#define	T_ALIAS	263
#define	T_SIZE	264
#define	T_ADDRESS	265
#define	T_ACCESS_MODE	266
#define	T_MODE	267
#define	T_BEGIN_CS	268
#define	T_END_CS	269
#define	T_BIT	270
#define	T_MASK	271
#define	T_NUMBER	272
#define	T_PATH	273
#define	T_CEXPR	274
#define	T_EOF	275
#define	T_INCLUDE	276
#define	T_SHR	277
#define	T_SHL	278
#define	T_ROR	279
#define	T_ROL	280
#define	T_MVI	281
#define	T_MOV	282
#define	T_CLR	283
#define	T_BMOV	284
#define	T_JMP	285
#define	T_JC	286
#define	T_JNC	287
#define	T_JE	288
#define	T_JNE	289
#define	T_JNZ	290
#define	T_JZ	291
#define	T_CALL	292
#define	T_ADD	293
#define	T_ADC	294
#define	T_INC	295
#define	T_DEC	296
#define	T_STC	297
#define	T_CLC	298
#define	T_CMP	299
#define	T_NOT	300
#define	T_XOR	301
#define	T_TEST	302
#define	T_AND	303
#define	T_OR	304
#define	T_RET	305
#define	T_NOP	306
#define	T_ACCUM	307
#define	T_ALLONES	308
#define	T_ALLZEROS	309
#define	T_NONE	310
#define	T_SINDEX	311
#define	T_A	312
#define	T_SYMBOL	313
#define	T_NL	314
#define	T_IF	315
#define	T_ELSE	316
#define	T_ELSE_IF	317
#define	T_ENDIF	318
#define	UMINUS	319


extern YYSTYPE yylval;
