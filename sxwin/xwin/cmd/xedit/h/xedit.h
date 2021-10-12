extern struct		_app_resources
{
	Boolean		enableBackups;
	char		*backupNamePrefix;
	char		*backupNameSuffix;
}	app_resources;

typedef enum
{
	NO_READ,
	READ_OK,
	WRITE_OK
}	FileAccess;

/*	externals in xedit.c 	*/

extern void		Feep ();

/*	externals in util.c 	*/

extern void		XeditPrintf ();
extern Widget		MakeCommandButton ();
extern Widget		MakeStringBox ();
extern String		GetString ();
extern FileAccess	MaybeCreateFile (), CheckFilePermissions ();

/*	externs in commands.c 	*/

extern void 		DoQuit ();
extern void		DoSave ();
extern void		DoLoad ();
