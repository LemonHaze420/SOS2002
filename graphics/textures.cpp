#include <windows.h>
#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>
#include <d3d8.h>
#include "MipRead.h"
#include <D3dx8tex.h.>

#undef MATCH_TEXTUREFORMAT

extern LPDIRECT3DDEVICE8	g_pd3dDevice;
extern LPDIRECT3D8		g_pD3D	;

extern void dprintf(char *,...);

class TextureFormats {
public:
	DWORD flags;
	IDirect3DTexture8 *surface565;
	IDirect3DTexture8 *surface1555;
	IDirect3DTexture8 *surface4444;
	IDirect3DTexture8 *surface555;
};

TextureFormats availableTextures;
//static void DeterminePackingShift (unsigned int mask, unsigned int *truncate_shift, unsigned int *up_shift);
static void DeterminePackingShift (int fmt, 
	unsigned int *tsr, unsigned int *usr,
	unsigned int *tsg, unsigned int *usg,
	unsigned int *tsb, unsigned int *usb,
	unsigned int *tsa, unsigned int *usa);

extern BOOL	g_bVoodoo;

#ifdef SJR
static HRESULT CALLBACK EnumPixelFormatsCallback (DDPIXELFORMAT * pddpf, LPVOID lpContext)
{
	unsigned long m;
	int a, r, g, b;

	TextureFormats *tf = (TextureFormats *)lpContext;

	// Setup the new surface desc
	DDSURFACEDESC2 ddsd;

	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
	ddsd.ddpfPixelFormat = *pddpf;
	ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwTextureStage  = 0;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED4)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXEDTO8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_RGB)
	{
		for (r = 0, m = pddpf->dwRBitMask; !(m & 1);
		     r++, m >>= 1);
		for (r = 0; m & 1; r++, m >>= 1);
		for (g = 0, m = pddpf->dwGBitMask; !(m & 1);
		     g++, m >>= 1);
		for (g = 0; m & 1; g++, m >>= 1);
			for (b = 0, m = pddpf->dwBBitMask; !(m & 1);
		     b++, m >>= 1);
		for (b = 0; m & 1; b++, m >>= 1);
	}
	a=0;
	if(pddpf->dwFlags &  DDPF_ALPHAPIXELS)
	{
		for (a = 0, m = pddpf->dwRGBAlphaBitMask; !(m & 1);
		     a++, m >>= 1);
		for (a = 0; m & 1; a++, m >>= 1);
	}
	if(a == 1 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface1555, &ddsd, sizeof(ddsd));
		tf->flags |= f1555;
	}
	if(a == 4 && r == 4 && g == 4 && b == 4)
	{
		memcpy(&tf->surface4444, &ddsd, sizeof(ddsd));
		tf->flags |= f4444;
	}
	if(a == 0 && r == 5 && g == 6 && b == 5)
	{
		memcpy(&tf->surface565, &ddsd, sizeof(ddsd));
		tf->flags |= f565;
	}
	if(a == 0 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface555, &ddsd, sizeof(ddsd));
		tf->flags |= f555;
	}
	return DDENUMRET_OK;
}

/*
 * D3DAppIEnumTextureFormats
 * Get a list of available texture map formats from the Direct3D driver by
 * enumeration.  Choose a default format (paletted is prefered).
 */
BOOL EnumTextureFormats (void)
{
	HRESULT hr;

	hr =	g_pd3dDevice->EnumTextureFormats (EnumPixelFormatsCallback, (LPVOID) & availableTextures);
	if (hr != DD_OK)
		return FALSE;
	return TRUE;
}
#endif
/*This calculates the maximum number of mipmaps a texture can have
   given its dimensions */
static int CalculateMipMapLevels (int width, int height)
{
	int LOD = 0;

	while (width > 0 && height > 0)
	{
		width /= 2;
		height /= 2;
		LOD++;
	}

	return (LOD);
}

/* heuristic to convert from a given texture format to
   one the hardware/D3D understands. 

 */


static int DetermineFormat (Mip * mip/*,DWORD *lplpFormat, int *srcBpp*/)
{
	int fmt;

	fmt = mip->format;

/*	if ((fmt == TEXFMT_ARGB_4444) && (!(availableTextures.flags & f4444)))
		fmt = TEXFMT_RGB_565;

	if ((fmt == TEXFMT_ARGB_1555) && (!(availableTextures.flags & f1555)))
		fmt = TEXFMT_RGB_565;

/*	switch (fmt)
	{
		case TEXFMT_RGB_565:
			*lplpFormat = &availableTextures.surface565;
			*srcBpp = 2;
			break;
		case TEXFMT_ARGB_1555:
			*lplpFormat = &availableTextures.surface1555;
			*srcBpp = 2;
			break;
		case TEXFMT_ARGB_4444:
			*lplpFormat = &availableTextures.surface4444;
			*srcBpp = 2;
			break;
	}*/

	return fmt;
}



static _D3DFORMAT DetermineD3DFmt(Mip *mip)
{
	int fmt;

#ifndef MATCH_TEXTUREFORMAT

	return D3DFMT_A8R8G8B8;
#endif

	fmt = mip->format;

	//if ((fmt == TEXFMT_ARGB_4444) && (!(availableTextures.flags & f4444)))
		//fmt = TEXFMT_RGB_565;

	//if ((fmt == TEXFMT_ARGB_1555) && (!(availableTextures.flags & f1555)))
	//	fmt = TEXFMT_RGB_565;

	switch (fmt)
	{
		case TEXFMT_RGB_565:
			return D3DFMT_R5G6B5;
			break;
		case TEXFMT_ARGB_1555:
			return D3DFMT_A1R5G5B5;
			break;
		case TEXFMT_ARGB_4444:
			return D3DFMT_A4R4G4B4;
			break;
	}
}




BOOL PutImg (LPDIRECT3DSURFACE8 lpDDS, const Mip * mip, int LOD, int width, int height, _D3DFORMAT texFmt)
{
	unsigned char *dst, *src, *srcLine, *dstLine;
	size_t srcStride, dstStride;
	int srcBpp, dstBpp;
//	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	unsigned int r_up_shift, g_up_shift, b_up_shift, a_up_shift;
	unsigned int r_truncate_shift, g_truncate_shift, b_truncate_shift,
	  a_truncate_shift;
	unsigned int packed_pixel;
	unsigned int red, green, blue, alpha;
	WORD tmp16;
	DWORD row, col;
	D3DLOCKED_RECT lockedSurface;
	D3DSURFACE_DESC ddsd;
	LPDIRECT3DSURFACE8 workSurface;

	/* check for valid source format and determine pixel size */

	switch (mip->format)
	{
		case TEXFMT_RGB_565:
		case TEXFMT_ARGB_4444:
		case TEXFMT_ARGB_1555:
			srcBpp = 2;
			break;
	}

	srcStride = srcBpp * width;
	srcLine = (unsigned char *)mip->data[LOD];

	/* Build a descriptor to lock the surface */

/*	memset (&ddsd, 0, sizeof (DDSURFACEDESC2));
	ddsd.dwSize = sizeof (DDSURFACEDESC2);
	ddrval = lpDDS->Lock (NULL, &ddsd, 0, NULL);

	dstBpp = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
	dstStride = ddsd.lPitch;*/

	//Create work surface with nice simple format
#ifdef MATCH_TEXTUREFORMAT
	if (FAILED(g_pd3dDevice->CreateImageSurface(width, height, D3DFMT_A8R8G8B8, &workSurface)))
		dprintf("worksurface\n");

	if (FAILED(workSurface->LockRect(&lockedSurface, NULL, 0)))
		dprintf("failed LockRect\n");
#else
	if (FAILED(lpDDS->LockRect(&lockedSurface, NULL, 0)))
		dprintf("failed LockRect\n");

#endif

	dstBpp= 4;
	dstStride = lockedSurface.Pitch;


	dstLine = (unsigned char *) lockedSurface.pBits;


	/*DeterminePackingShift (mip->format,
		&r_truncate_shift, &r_up_shift,
		&g_truncate_shift, &g_up_shift,
		&b_truncate_shift, &b_up_shift,
		&a_truncate_shift, &a_up_shift);
/*	DeterminePackingShift (ddsd.ddpfPixelFormat.dwRBitMask,
			       &r_truncate_shift, &r_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwGBitMask,
			       &g_truncate_shift, &g_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwBBitMask,
			       &b_truncate_shift, &b_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask,
			       &a_truncate_shift, &a_up_shift);*/

	/* Slow code is good, or something */

	for (row = 0; row < height; row++)
	{
		dst = dstLine;
		dstLine += dstStride;
		src = srcLine;
		srcLine += srcStride;
		for (col = 0; col < width; col++)
		{

			/* extract the pixel */

			switch (mip->format)
			{
				case TEXFMT_RGB_565:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					green = (tmp16 & 0x3f) << 2;
					tmp16 >>= 6;
					red = (tmp16 & 0x1f) << 3;
					tmp16 >>= 6;
					alpha = 0xff;
					break;
				case TEXFMT_ARGB_4444:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					green = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					red = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					alpha = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					break;
				case TEXFMT_ARGB_1555:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					green = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					red = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					alpha = (tmp16 & 0xf) ? 0xff : 0x0;
					break;
			}

			/* Pack the pixel */

			/*packed_pixel = ((blue >> b_truncate_shift) << b_up_shift);
			packed_pixel |= ((green >> g_truncate_shift) << g_up_shift);
			packed_pixel |= ((red >> r_truncate_shift) << r_up_shift);
			/* TBD: compiler error!!, shifting by 32 is a NOOP */
			/*if (a_truncate_shift < 32)
				packed_pixel |= ((alpha >> a_truncate_shift) << a_up_shift);*/
			
			//if (a_truncate_shift < 32)
				packed_pixel = (alpha << 24) | (red << 16) | (green << 8) | blue;
			
			//else
				//packed_pixel = (red << 16) | (green << 8) | blue;


			*((unsigned long *) dst) = (unsigned long) packed_pixel;
			dst += dstBpp;
			src += srcBpp;
		}
	}

#ifdef MATCH_TEXTUREFORMAT
	workSurface->UnlockRect();
	//copy work surface back to real surface
	HRESULT hr = D3DXLoadSurfaceFromSurface(lpDDS, NULL, NULL, workSurface, NULL, NULL, D3DX_FILTER_NONE , 0);
	if (FAILED(hr))
		dprintf("Copy workSurface back\n");
	workSurface->Release();
#else
	lpDDS->UnlockRect();
#endif
	return TRUE;
}


BOOL RealizeImg (BorisTexture2 *tex, Mip* mip, int TextureStage)
{
	LPDIRECT3DSURFACE8 tptr;
	int LOD, MAX_LOD;
	int width, height;
	_D3DFORMAT textureFormat;
	
	MAX_LOD = CalculateMipMapLevels (mip->width, mip->height);
	MAX_LOD = min (MAX_LOD, mip->depth);

	textureFormat = DetermineD3DFmt(mip);
	if (FAILED(g_pd3dDevice->CreateTexture(mip->width, mip->height, MAX_LOD, 0, textureFormat, D3DPOOL_MANAGED, &tex->lpTexture)))
		dprintf("tex\n");

	
	MAX_LOD = tex->lpTexture->GetLevelCount();

	width = mip->width;
	height= mip->height;

	for (LOD = 0; LOD < MAX_LOD; LOD++)
	{
		
		tex->lpTexture->GetSurfaceLevel(LOD, &tptr);
		PutImg (tptr, mip, LOD, width, height, textureFormat);

		width >>= 1;
		height >>= 1;
	}

	//tex->lpTexture = tmpTexture;
	tex->format = DetermineFormat (mip);

		/* Get the next mipmap level */

	/*	ddrval = lpDDS->GetAttachedSurface (&ddscaps, &lpDDS);

		if (ddrval != DD_OK && lpDDS != NULL)
		{
			lpDDS->Release ();
			return FALSE;
		}
	}


/*	LPDDSURFACEDESC2 lpFormat;
	LPDIRECTDRAWSURFACE7 lpDDS;
	LPDIRECTDRAWSURFACE7 lpSurface = NULL;
	LPDIRECTDRAWPALETTE lpPalette = NULL;
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	DDSCAPS2 ddscaps;
	int LOD, MAX_LOD;
	int target_width, target_height, target_format,width,height;
	int srcBpp;

	/* determine what the final width, height and format the
	   image should be that can be supported by D3D
	 */

/*	target_width = mip->width;
	target_height = mip->height;
	target_format = DetermineFormat (mip, &lpFormat, &srcBpp);

	MAX_LOD = CalculateMipMapLevels (target_width, target_height);

	/* honor the input images # of LOD's */
/*	MAX_LOD = min (MAX_LOD, mip->depth);

	/* Setup the descriptor to create the surface */

/*	memcpy (&ddsd, lpFormat, sizeof (DDSURFACEDESC2));
	ddsd.dwSize = sizeof (DDSURFACEDESC2);

	if (MAX_LOD != 1)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
			| DDSD_MIPMAPCOUNT;
		ddsd.dwMipMapCount = MAX_LOD;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX |
			DDSCAPS_MIPMAP;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	}
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwHeight = target_height;
	ddsd.dwWidth = target_width;
	if(g_bVoodoo)
	{
		ddsd.dwFlags |= DDSD_TEXTURESTAGE;
		ddsd.dwTextureStage = TextureStage;
	}

	ddrval = g_pDD7->CreateSurface (&ddsd, &lpDDS, NULL);
	if(ddrval != DD_OK)
		return FALSE;
	lpSurface = lpDDS;

	/* fill the MipMaps by looping through each Level of Detail */


/*	ddscaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	ddscaps.dwCaps2 = 0;
	ddscaps.dwCaps3 = 0;
	ddscaps.dwCaps4 = 0;

	width = mip->width;
	height= mip->height;

	for (LOD = 0; LOD < MAX_LOD; LOD++)
	{
		PutImg (lpDDS, mip, LOD, width, height);

		width >>= 1;
		height >>= 1;

		/* Get the next mipmap level */

	/*	ddrval = lpDDS->GetAttachedSurface (&ddscaps, &lpDDS);

		if (ddrval != DD_OK && lpDDS != NULL)
		{
			lpDDS->Release ();
			return FALSE;
		}
	}

	tex->lpSurface = lpSurface;
	tex->format = target_format;*/

	return TRUE;
}

/*
   This packs and copies data from a source image to a destination image.
   dstBpp means bytes per pixel, not bpp
   size is the size of the data source in bytes
   The masks specify where the bits for each channel should go
 */
#ifdef SJR
static void PackAndCopy (unsigned char *dest, unsigned char *src, int size, int dstBpp,
		int src_fmt, int r_mask, int g_mask, int b_mask, int a_mask)
{
	unsigned int dest_index;
	unsigned int r_up_shift, g_up_shift, b_up_shift, a_up_shift;
	unsigned int r_truncate_shift, g_truncate_shift, b_truncate_shift,
	  a_truncate_shift;
	unsigned int packed_pixel;
	unsigned int red, green, blue, alpha;
	BYTE *ptr, *end;
	WORD tmp16;

	/* Get packing shift information */

	DeterminePackingShift (r_mask, &r_truncate_shift, &r_up_shift);
	DeterminePackingShift (g_mask, &g_truncate_shift, &g_up_shift);
	DeterminePackingShift (b_mask, &b_truncate_shift, &b_up_shift);
	DeterminePackingShift (a_mask, &a_truncate_shift, &a_up_shift);

	/* Slow code is good, or something */

	for (ptr = src, end = src + size, dest_index = 0;
	     ptr < end;
	     dest_index += dstBpp)
	{

		/* extract the pixel */

		switch (src_fmt)
		{
			case TEXFMT_RGB_565:
				tmp16 = *(WORD *) ptr;
				ptr += 2;
				blue = (tmp16 & 0x1f) << 3;
				tmp16 >>= 5;
				green = (tmp16 & 0x3f) << 2;
				tmp16 >>= 6;
				red = (tmp16 & 0x1f) << 3;
				tmp16 >>= 6;
				alpha = 0xff;
				break;
			case TEXFMT_ARGB_4444:
				tmp16 = *(WORD *) ptr;
				ptr += 2;
				blue = (tmp16 & 0xf) << 4;
				tmp16 >>= 4;
				green = (tmp16 & 0xf) << 4;
				tmp16 >>= 4;
				red = (tmp16 & 0xf) << 4;
				tmp16 >>= 4;
				alpha = 0xff;
				break;
		}

		/* Pack the pixel */

		packed_pixel = ((blue >> b_truncate_shift) << b_up_shift);
		packed_pixel |= ((green >> g_truncate_shift) << g_up_shift);
		packed_pixel |= ((red >> r_truncate_shift) << r_up_shift);

		/* TBD: compiler error!!, shifting by 32 is a NOOP */

		if (a_truncate_shift < 32)
			packed_pixel |= ((alpha >> a_truncate_shift) << a_up_shift);

		switch (dstBpp)
		{
			case 4:
				*((unsigned int *) (&dest[dest_index])) = packed_pixel;
				break;
			case 2:
				*((unsigned short *) (&dest[dest_index])) =
					(unsigned short) packed_pixel;
				break;
			case 1:
				dest[dest_index] = (unsigned char) packed_pixel;
				break;
		}
	}
}

/*
   This determines how the bits from the unpacked data must be shifted
   around to fit into the packed format
 */
#endif
static void DeterminePackingShift (int fmt, 
	unsigned int *tsr, unsigned int *usr,
	unsigned int *tsg, unsigned int *usg,
	unsigned int *tsb, unsigned int *usb,
	unsigned int *tsa, unsigned int *usa)
{
	switch(fmt) {
	case TEXFMT_RGB_565:
		*usr = 11;
		*usg = 5;
		*usb = 0;
		*usa = 0;
		*tsr = 3;
		*tsg = 2;
		*tsb = 3;
		*tsa = 32;
		break;
	case TEXFMT_ARGB_1555:
		*usr = 10;
		*usg = 5;
		*usb = 0;
		*usa = 15;
		*tsr = 3;
		*tsg = 3;
		*tsb = 3;
		*tsa = 7;
		break;
	case TEXFMT_ARGB_4444:
		*usr = 8;
		*usg = 4;
		*usb = 0;
		*usa = 12;
		*tsr = 4;
		*tsg = 4;
		*tsb = 4;
		*tsa = 4;
		break;
	}
}


#ifdef SJR
static void DeterminePackingShift (unsigned int mask, unsigned int *truncate_shift,
			    unsigned int *up_shift)
{
	unsigned int size;

	/* Determine how much the channel needs to be shifted up */

	if (mask == 0)
	{
		*up_shift = 0;
		*truncate_shift = 32;
	}
	else
	{
		*up_shift = 0;
		while ((mask & 1) == 0)
		{
			(*up_shift)++;
			mask >>= 1;
		}

		/* Determine how many bits the channel is */

		size = 0;
		while (mask & 1)
		{
			size++;
			mask >>= 1;
		}

		/* Calculate how many bits need to be chopped off */

		*truncate_shift = 8 - size;
	}
}
/* Given a D3D channel bit mask, determine the size of the channel in bits */

static int DetermineDepth (int a)
{
	int count = 0;
	while (!(a & 1) && a)
		a = a >> 1;

	while (a & 1)
	{
		a = a >> 1;
		count++;
	}

	return (count);
}
#endif

BOOL RealizeBump (BorisTexture2 *tex, Mip* mip)
{
/*	LPDDSURFACEDESC2 lpFormat;
	LPDIRECTDRAWSURFACE7 lpDDS;
	LPDIRECTDRAWSURFACE7 lpSurface = NULL;
	LPDIRECTDRAWPALETTE lpPalette = NULL;
	DDSURFACEDESC2 ddsd;*/
	HRESULT ddrval;
	//DDSCAPS2 ddscaps;
	int LOD, MAX_LOD;
	int target_width, target_height, target_format,width,height;
	int srcBpp;
	_D3DFORMAT textureFormat;
	LPDIRECT3DTEXTURE8 tmpTexture;
	D3DLOCKED_RECT ddsd;
	LPDIRECT3DSURFACE8 workSurface;

	/* determine what the final width, height and format the
	   image should be that can be supported by D3D
	 */

	textureFormat = DetermineD3DFmt(mip);
	if (FAILED(g_pd3dDevice->CreateTexture(mip->width, mip->height, 1, 0, D3DFMT_V8U8, D3DPOOL_MANAGED, &tmpTexture)))
		dprintf("tex\n");

	target_width = mip->width;
	target_height = mip->height;
	target_format = DetermineFormat (mip/*, &lpFormat, &srcBpp*/);

	//MAX_LOD = CalculateMipMapLevels (target_width, target_height);

	/* honor the input images # of LOD's */
	//MAX_LOD = min (MAX_LOD, mip->depth);
	MAX_LOD = min (tmpTexture->GetLevelCount(), mip->depth);
	tmpTexture->GetSurfaceLevel(0, &workSurface);

	/* Setup the descriptor to create the surface */

/*	memset (&ddsd, 0, sizeof (DDSURFACEDESC2));
	ddsd.dwSize = sizeof (DDSURFACEDESC2);

	if (MAX_LOD != 1)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
			| DDSD_MIPMAPCOUNT;
		ddsd.dwMipMapCount = MAX_LOD;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX |
			DDSCAPS_MIPMAP;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	}
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwHeight = target_height;
	ddsd.dwWidth = target_width;

	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags=DDPF_BUMPDUDV;
	ddsd.ddpfPixelFormat.dwBumpBitCount=16;
	ddsd.ddpfPixelFormat.dwBumpDuBitMask=0x00ff;
	ddsd.ddpfPixelFormat.dwBumpDvBitMask=0xff00;
	ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask=0x0000;

	ddrval = g_pDD7->CreateSurface (&ddsd, &lpDDS, NULL);
	if(ddrval != DD_OK)
		return FALSE;
	lpSurface = lpDDS;

	/* fill the MipMaps by looping through each Level of Detail */

	//ddscaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

	width = mip->width;
	height= mip->height;

	WORD *heightmap = (WORD *)mip->data[0];

//	ddrval = lpDDS->Lock(NULL,&ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR, NULL);
	if (FAILED(workSurface->LockRect(&ddsd, NULL, 0)))
		dprintf("failed LockRect\n");

	for(DWORD y=0;y<height;++y)
	{
		BYTE *pDst = (BYTE*)ddsd.pBits+y*ddsd.Pitch;
		for( DWORD x = 0; x < width; x++ )
		{
			LONG v00, v01, v10;
#if 0			
			v00 = (heightmap[(y*width+x)]&0x7e0)>>4; 
			if( x == width-1 ) 
				v01 = (heightmap[(y*width+x)]&0x7e0)>>4; 
			else
				v01 = (heightmap[(y*width+x+1)]&0x7e0)>>4;
			
			if( y == height-1 ) 
				v10 = (heightmap[((y)*width+x)]&0x7e0)>>4;
			else
				v10 = (heightmap[((y+1)*width+x)]&0x7e0)>>4; 
#else
			v00 = (heightmap[(y*width+x)]&0x0f)<<3; 
			if( x == width-1 ) 
				v01 = (heightmap[(y*width+x)]&0x0f)<<3; 
			else
				v01 = (heightmap[(y*width+x+1)]&0x0f)<<3;
			
			if( y == height-1 ) 
				v10 = (heightmap[((y)*width+x)]&0x0f)<<3;
			else
				v10 = (heightmap[((y+1)*width+x)]&0x0f)<<3; 
#endif			
			LONG iDu = v00 - v01; 
			LONG iDv = v00 - v10; 
			
			*pDst++ = (BYTE)iDu;
			*pDst++ = (BYTE)iDv;
		}
	}

	ddrval = workSurface->UnlockRect();

	tex->lpTexture = tmpTexture;
	tex->format = target_format;

	return TRUE;
}


