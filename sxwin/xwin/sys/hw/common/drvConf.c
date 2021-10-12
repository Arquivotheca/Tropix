#include "xf86.h"
#include "xf86Xinput.h"

extern DriverRec ATI, NV, VGA, S3VIRGE, TRIDENT;

DriverPtr xf86DriverList[] =
{
	&ATI,
	&NV,
	&TRIDENT,
	&S3VIRGE,
	&VGA
};

int xf86NumDrivers = sizeof(xf86DriverList) / sizeof(xf86DriverList[0]);

extern InputDriverRec xf86KEYBOARD, MOUSE;

InputDriverPtr xf86InputDriverList[] =
{
	&MOUSE
};

int xf86NumInputDrivers = sizeof(xf86InputDriverList) / sizeof(xf86InputDriverList[0]);
