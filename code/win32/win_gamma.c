/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*
** WIN_GAMMA.C
*/

#include <assert.h>
#include "../system/sdl_local.h"
#include "../renderer/tr_local.h"
#include "../qcommon/qcommon.h"

static unsigned short s_oldHardwareGamma[3][256];
HDC s_hDC;			// handle to device context

/*
** WG_CheckHardwareGamma
**
** Determines if the underlying hardware supports the Win32 gamma correction API.
*/
void WG_CheckHardwareGamma(void)
{
	HDC hDC;

	glConfig.deviceSupportsGamma = false;

	// non-3Dfx standalone drivers don't support gamma changes, period
	if (glConfig.driverType == GLDRV_STANDALONE) {
		return;
	}

	if (!r_ignorehwgamma->integer) {
		hDC = GetDC(GetDesktopWindow());
		glConfig.deviceSupportsGamma = GetDeviceGammaRamp(hDC, s_oldHardwareGamma);
		ReleaseDC(GetDesktopWindow(), hDC);

		if (glConfig.deviceSupportsGamma) {
			// do a sanity check on the gamma values
			if ((HIBYTE(s_oldHardwareGamma[0][255]) <= HIBYTE(s_oldHardwareGamma[0][0])) ||
				(HIBYTE(s_oldHardwareGamma[1][255]) <= HIBYTE(s_oldHardwareGamma[1][0])) ||
				(HIBYTE(s_oldHardwareGamma[2][255]) <= HIBYTE(s_oldHardwareGamma[2][0]))) {
				glConfig.deviceSupportsGamma = false;
				ri.Printf(PRINT_WARNING, "WARNING: device has broken gamma support, generated gamma.dat\n");
			}

			// make sure that we didn't have a prior crash in the game, and if so we need to
			// restore the gamma values to at least a linear value
			if ((HIBYTE(s_oldHardwareGamma[0][181]) == 255)) {

				int g;

				ri.Printf(PRINT_WARNING, "WARNING: suspicious gamma tables, using linear ramp for restoration\n");

				for (g = 0; g < 255; g++)
				{
					s_oldHardwareGamma[0][g] = g << 8;
					s_oldHardwareGamma[1][g] = g << 8;
					s_oldHardwareGamma[2][g] = g << 8;
				}
			}
		}
	}
}

/*
void mapGammaMax( void ) {
	int		i, j;
	unsigned short table[3][256];

	// try to figure out what win2k will let us get away with setting
	for ( i = 0 ; i < 256 ; i++ ) {
		if ( i >= 128 ) {
			table[0][i] = table[1][i] = table[2][i] = 0xffff;
		} else {
			table[0][i] = table[1][i] = table[2][i] = i<<9;
		}
	}

	for ( i = 0 ; i < 128 ; i++ ) {
		for ( j = i*2 ; j < 255 ; j++ ) {
			table[0][i] = table[1][i] = table[2][i] = j<<8;
			if ( !SetDeviceGammaRamp( s_hDC, table ) ) {
				break;
			}
		}
		table[0][i] = table[1][i] = table[2][i] = i<<9;
		Com_Printf( "index %i max: %i\n", i, j-1 );
	}
}
*/

/*
** GLimp_SetGamma
**
** This routine should only be called if glConfig.deviceSupportsGamma is TRUE
*/
void GLimp_SetGamma(unsigned char red[256], unsigned char green[256], unsigned char blue[256])
{
	unsigned short table[3][256];
	int		i, j;
	int		ret;
	OSVERSIONINFO	vinfo;

	// get a DC for our window if we don't already have one allocated
	if (s_hDC == NULL) {

		ri.Printf(PRINT_ALL, "...getting DC: ");

		SDL_PropertiesID properties = SDL_GetWindowProperties(g_wv.hWnd);

		if (properties) {
			if ((s_hDC = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HDC_POINTER,
				NULL)) == NULL) {
				ri.Printf(PRINT_ALL, "failed\n");
				return;
			}
		}

		ri.Printf(PRINT_ALL, "succeeded\n");
	}

	if (!glConfig.deviceSupportsGamma || r_ignorehwgamma->integer || !s_hDC) {
		return;
	}

	//mapGammaMax();

	for (i = 0; i < 256; i++)
	{
		table[0][i] = (((unsigned short)red[i]) << 8) | red[i];
		table[1][i] = (((unsigned short)green[i]) << 8) | green[i];
		table[2][i] = (((unsigned short)blue[i]) << 8) | blue[i];
	}

	// Win2K puts this odd restriction on gamma ramps...
	vinfo.dwOSVersionInfoSize = sizeof(vinfo);
	GetVersionEx(&vinfo);
	if (vinfo.dwMajorVersion == 5 && vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		Com_DPrintf("performing W2K gamma clamp.\n");
		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 128; i++)
			{
				if (table[j][i] > ((128 + i) << 8)) {
					table[j][i] = (128 + i) << 8;
				}
			}
			if (table[j][127] > 254 << 8) {
				table[j][127] = 254 << 8;
			}
		}
	} else {
		Com_DPrintf("skipping W2K gamma clamp.\n");
	}

	// enforce constantly increasing
	for (j = 0; j < 3; j++)
	{
		for (i = 1; i < 256; i++)
		{
			if (table[j][i] < table[j][i - 1]) {
				table[j][i] = table[j][i - 1];
			}
		}
	}

	ret = SetDeviceGammaRamp(s_hDC, table);
	if (!ret) {
		Com_Printf("SetDeviceGammaRamp failed.\n");
	}
}

/*
** WG_RestoreGamma
*/
void WG_RestoreGamma(void)
{
	if (glConfig.deviceSupportsGamma) {
		HDC hDC = GetDC(GetDesktopWindow());
		SetDeviceGammaRamp(hDC, s_oldHardwareGamma);
		ReleaseDC(GetDesktopWindow(), hDC);
	}
}
