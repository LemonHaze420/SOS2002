// $Header$

// $Log$
// Revision 1.4  2002-08-20 17:20:15+01  sjr
// <>
//
// Revision 1.3  2002-08-20 12:30:17+01  sjr
// Initial working DX8 version
//
// Revision 1.2  2002-07-22 10:06:36+01  jjs
// Test version of SOS2002
//
// Revision 1.2  1999/03/19 16:13:38  jjs
// Added 3D only and 2D only shutdown routine.
//
// Revision 1.1  1999/03/04 16:54:28  jjs
// Initial revision
//
#define D3D_OVERLOADS

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <math.h>
#include "rGraphics.h"
#include "MipRead.h"
#include "textures.h"
#include "bs2all.h"


#if defined(USE_CGVERTEXSHADER)// - in rgraphics.h
#include <cg\cgD3D.h>
#include "shaders\constants.h"


void ActivateSpecular(BorisTexture2 *bt);

cgDirect3D cg;

cgProgramContainer* m_Dot3SpecularVS;
cgProgramContainer* m_FogVS;
cgBindIter* m_Dot3SpecularVSWorldViewProj;
cgBindIter* m_Dot3SpecularVSLightPosition;
cgBindIter* m_Dot3SpecularVSEyePosition;
cgProgramContainer* m_Dot3SpecularPS;
cgProgramContainer* m_FogPS;
cgBindIter* m_Dot3SpecularPSNormalizationMap;

/*
Map:

  typedef struct _D3DVERTEXC {
    float    x;
    float    y;
    float    z;
    float    nx;
    float    ny;
    float    nz;
    float    color;          // Vertex color 
    float    tu;             // Texture coordinates 
    float    tv;
	
to:
	float3 Position; //in object space
	float3 Normal; //in object space
	float2 TexCoord0

*/
cgVertexAttribute vertex_attributes[] =
{
	{3, "Position", 0},
	{3, "Normal", 0},
	{1, "Colour", 0},
	{2, "TexCoord0", 0},
	{0, 0, 0}
};


#endif

#if defined (USE_NONCGVERTEXSHADER)

HRESULT LoadAndCreateShader(char *filename, const DWORD* pDeclaration, DWORD Usage, EBSHADERTYPE ShaderType, DWORD *pHandle);


DWORD dwDeclunlit[] = {
	D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3),
		D3DVSD_REG( 1, D3DVSDT_FLOAT3),
		D3DVSD_REG( 2, D3DVSDT_FLOAT2),
		D3DVSD_END()
};
DWORD dwDecllit[] = {
	D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3),
		D3DVSD_REG( 1, D3DVSDT_D3DCOLOR),
		D3DVSD_REG( 2, D3DVSDT_FLOAT2),
		D3DVSD_END()
};
DWORD dwDecldot3[] = {
	D3DVSD_STREAM( 0 ),
		D3DVSD_REG( 0, D3DVSDT_FLOAT3),
		D3DVSD_REG( 1, D3DVSDT_FLOAT3),
		D3DVSD_REG( 2, D3DVSDT_FLOAT2),
		D3DVSD_REG( 3, D3DVSDT_FLOAT3),
		D3DVSD_REG( 4, D3DVSDT_FLOAT3),
		D3DVSD_REG( 5, D3DVSDT_FLOAT3),
		D3DVSD_END()
};

	DWORD *VSDecl[NUM_VERTEXSHADERS];
	DWORD *VSFunc[NUM_VERTEXSHADERS];
	DWORD Shader[NUM_VERTEXSHADERS];


typedef struct _DOT3VERTEX
{
	float x,y,z;
	float nx,ny,nz;
	float tu1, tv1;
	float sx, sy, sz;
	float tx,ty,tz;
	float stx,sty,stz;
}DOT3VERTEX, *LPDOT3VERTEX;

DWORD				vShader;
DWORD				pShader;




#endif

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3DSURFACE8 g_pddsPrimary	 = NULL;
LPDIRECT3DSURFACE8 g_pddsBackBuffer = NULL;
LPDIRECT3D8			g_pD3D			 = NULL;
LPDIRECT3DDEVICE8			g_pd3dDevice	 = NULL;
//static LPDIRECT3DVIEWPORT7	g_pvViewport	 = NULL;
static RECT 				g_rcScreenRect;
static RECT 				g_rcViewportRect;
static HWND 				g_hWnd;
//-----------------------------------------------------------------------------
// Local variables for the Windows portion of the app
//-----------------------------------------------------------------------------
BOOL				g_bAppUseFullScreen = FALSE;
BOOL				g_bSoftwareOnly	   = FALSE;
BOOL				g_bVoodoo	   = FALSE;
BOOL				CurrentAlphaTest=FALSE;
BOOL				CurrentAlphaBlend=FALSE;
BOOL				CurrentBlend = FALSE;
BOOL				CurrentZWrite = TRUE;
BOOL				CurrentTClamp[3]={FALSE,FALSE};
BOOL				CurrentSClamp[3]={FALSE,FALSE};
LPDIRECT3DBASETEXTURE8	CurrentTexture[3];
D3DFILLMODE			CurrentFillMode=D3DFILL_SOLID;
D3DZBUFFERTYPE		zBuff=D3DZB_TRUE;
D3DMATRIX ProjectionMatrix, WorldMatrix, ViewMatrix;
D3DPRESENT_PARAMETERS d3dpp;
D3DFORMAT format;
D3DCAPS8 caps;
bool fullScreen;
bool has32depth;
int maxwidth;		// used for voodoo type cards
bool no32bitTextures, noStencilBuffer;
bool hasTnL;

bool isTnL;

//#define	PI	3.141592654f

D3DTLVERTEX vert[3];
D3DCOLOR CurrentColor;
BOOL TwoTextures = FALSE;
int TextureStages = 0;
#define TEXTURES_AVAILABLE 1000
BorisTexture2 textureHandles[TEXTURES_AVAILABLE]={NULL};
BorisTexture2 * Texture2 = NULL;
void SetTexture(int stage, LPDIRECT3DBASETEXTURE8/*LPDIRECT3DSURFACE8*/ tex);
BOOL SetAlphaBlend(BOOL flag);
static VIDEOMODES VideoModes[100]={0}; // should be enough!
int DisplayWidth = 800;
int DisplayHeight = 600;
int DisplayDepth = 16;
RGB_TYPE Format16;
DWORD Caps;

D3DTLVERTEXDARK darkvert[4];
BOOL DarkenSet=FALSE;

HRESULT SetProjectionMatrix(D3DMATRIX &mat, FLOAT fFOV, FLOAT fAspect,
                                     FLOAT fNearPlane, FLOAT fFarPlane )
{
	if( fabs(fFarPlane-fNearPlane) < 0.01f )
		return E_INVALIDARG;
	if( fabs(sin(fFOV/2)) < 0.01f )
		return E_INVALIDARG;
	
	FLOAT w = fAspect * (FLOAT)( cos(fFOV/2)/sin(fFOV/2) );
	FLOAT h =	1.0f  * (FLOAT)( cos(fFOV/2)/sin(fFOV/2) );
	FLOAT Q = fFarPlane / ( fFarPlane - fNearPlane );
	
	ZeroMemory( &mat, sizeof(D3DMATRIX) );
	mat._11 = w;
	mat._22 = h;
	mat._33 = Q;
	mat._34 = 1.0f;
	mat._43 = -Q*fNearPlane;

	return S_OK;
}

HRESULT SetWorldTransform(D3DMATRIX *mat)
{	
	memcpy(&WorldMatrix, mat, sizeof(D3DMATRIX));
   return g_pd3dDevice->SetTransform( D3DTS_WORLD,  mat );
}

void SetPixelFormat16(void)
{
	D3DFORMAT tformat;

	tformat = D3DFMT_R5G6B5;
	D3DXCheckTextureRequirements(g_pd3dDevice, NULL, NULL,NULL,NULL,&tformat, D3DPOOL_MANAGED);
	if(tformat == D3DFMT_R5G6B5)
		Format16 = RGB_565;
	else
	if(tformat == D3DFMT_X1R5G5B5 || tformat == D3DFMT_A1R5G5B5)
		Format16= RGB_555;
	else
		Format16=RGB_UNSUPPORTED;
}

void renderInitialise(HWND wnd, int width, int height, bool b_fullScreen)
{

    HRESULT hr;
	int numAdapters;
	int adapterID = 0;
	D3DVIEWPORT8 vp = {0,0,width,height,0.0f,1.0f};
	D3DFORMAT zformat = D3DFMT_D24S8;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	format = D3DFMT_A8R8G8B8;

	fullScreen = b_fullScreen;
	has32depth = true;
	g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

	numAdapters = g_pD3D->GetAdapterCount();
	if(numAdapters <= adapterID + 1)
		adapterID = numAdapters - 1;
	g_hWnd = wnd;
	
	hr = g_pD3D->GetDeviceCaps(adapterID,D3DDEVTYPE_HAL,&caps);

	maxwidth = caps.MaxTextureWidth;

	D3DDISPLAYMODE d3ddm;
	g_pD3D->GetAdapterDisplayMode(adapterID,&d3ddm);

	noStencilBuffer = false;
	
	if(!fullScreen && d3ddm.Format != D3DFMT_X8R8G8B8)
	{
		noStencilBuffer = true;
		zformat = D3DFMT_D16;
	}
	
	hasTnL = caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT? true : false;

	hr = g_pD3D->CheckDeviceFormat(adapterID, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8);
	
	if(FAILED(hr))
	{
		if(!fullScreen && d3ddm.Format == D3DFMT_X8R8G8B8)
		{
			MessageBox(g_hWnd,"Will not use 32bit desktop","Fatal Error",MB_OK | MB_ICONSTOP );
			exit(-1);
		}
		noStencilBuffer = true;
		zformat = D3DFMT_D16;
	}
	
	if(fullScreen)
	{
		d3dpp.Windowed					= FALSE;
		d3dpp.SwapEffect				= D3DSWAPEFFECT_FLIP;
		d3dpp.BackBufferWidth			= width;
		d3dpp.BackBufferHeight			= height;
		d3dpp.BackBufferCount			= 1;
		d3dpp.hDeviceWindow				= g_hWnd;
		d3dpp.BackBufferFormat			= format;
		d3dpp.EnableAutoDepthStencil	= TRUE;
		d3dpp.AutoDepthStencilFormat	= zformat;
		d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}
	else
	{
		d3dpp.Windowed					= TRUE;
		d3dpp.SwapEffect				= D3DSWAPEFFECT_COPY_VSYNC;
		d3dpp.BackBufferWidth			= width;
		d3dpp.BackBufferHeight			= height;
		d3dpp.BackBufferCount			= 1;
		d3dpp.hDeviceWindow				= g_hWnd;
		d3dpp.BackBufferFormat			= format = d3ddm.Format;
	    d3dpp.EnableAutoDepthStencil	= TRUE;
		d3dpp.AutoDepthStencilFormat	= zformat;
		d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}

	hr = g_pD3D->CreateDevice(adapterID, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_MULTITHREADED | (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT?D3DCREATE_HARDWARE_VERTEXPROCESSING:D3DCREATE_SOFTWARE_VERTEXPROCESSING),
		&d3dpp, &g_pd3dDevice);
	if(FAILED(hr))
	{
		format = d3dpp.BackBufferFormat			= D3DFMT_R5G6B5;		
		hr = g_pD3D->CreateDevice(adapterID, D3DDEVTYPE_HAL, g_hWnd,
			D3DCREATE_MULTITHREADED | (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT?D3DCREATE_HARDWARE_VERTEXPROCESSING:D3DCREATE_SOFTWARE_VERTEXPROCESSING),
			&d3dpp, &g_pd3dDevice);
		if(FAILED(hr))
			exit(0);
		has32depth = false;
	}

	g_pd3dDevice->SetViewport(&vp);
	D3DFORMAT tformat;

	tformat = D3DFMT_A8R8G8B8;
	D3DXCheckTextureRequirements(g_pd3dDevice,NULL,NULL,NULL,NULL,&tformat,D3DPOOL_MANAGED);
	if(tformat != D3DFMT_A8R8G8B8)
		no32bitTextures = true;
	else
		no32bitTextures = false;



	SetZBack(20000.0f);
	SetLightState(250, 250, 250);
	EnableZBuffer(D3DZB_TRUE);

	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER );



#if defined(USE_CGVERTEXSHADER)
	//Shaders
	cgContextContainer *pContextContainer = cg.CreateContextContainer(g_pd3dDevice);

	m_FogVS = pContextContainer->LoadCGProgramFromFile("Shaders\\SimpleFog_dx8vs.cg",
																"Fog Shader",
																cgDX8VertexProfile,
																vertex_attributes);
	if (m_FogVS == 0) {
		//m_strLastError = "Failed to load Cg vertex program";
		return;// E_FAIL;
	}

	m_FogPS = pContextContainer->LoadCGProgramFromFile("Shaders\\SimpleFog_dx8ps.cg",
																"Fog Pixel Shader",
																cgDX8PixelProfile);
	if (m_FogPS == 0) {
		//m_strLastError = "Failed to load Cg pixel program";
		return;// E_FAIL;
	}
	

//	m_Dot3SpecularPSNormalizationMap = m_Dot3SpecularPS->GetParameterBindByName("NormalizationMap");
#endif	

#if defined(USE_NONCGVERTEXSHADER)
	VSDecl[SHADER_UNLIT] = dwDecldot3;
	VSDecl[SHADER_UNLITSPECULAR] = dwDecldot3;
	VSDecl[SHADER_LIT] = dwDecllit;
	VSDecl[SHADER_DOT3] = dwDecldot3;
	VSDecl[SHADER_DOT3_NOSPEC] = dwDecldot3;
	LoadAndCreateShader("unlit.vso",VSDecl[SHADER_UNLIT],0,SHADERTYPE_VERTEXSHADER, &Shader[SHADER_UNLIT]);
	LoadAndCreateShader("unlitspec.vso",VSDecl[SHADER_UNLITSPECULAR],0,SHADERTYPE_VERTEXSHADER, &Shader[SHADER_UNLITSPECULAR]);
	LoadAndCreateShader("lit.vso",VSDecl[SHADER_LIT],0,SHADERTYPE_VERTEXSHADER, &Shader[SHADER_LIT]);
	LoadAndCreateShader("dot3.vso",VSDecl[SHADER_DOT3],0,SHADERTYPE_VERTEXSHADER, &Shader[SHADER_DOT3]);
	LoadAndCreateShader("dot3_nospec.vso",VSDecl[SHADER_DOT3_NOSPEC],0,SHADERTYPE_VERTEXSHADER, &Shader[SHADER_DOT3_NOSPEC]);
	LoadAndCreateShader("dot3.pso",0,0,SHADERTYPE_PIXELSHADER, &Shader[SHADER_DOT3_PIXEL]);
	g_pd3dDevice->SetVertexShaderConstant(C_ZERO_HALF,D3DXVECTOR4(0.0f,0.0f,0.0f,0.5f),1);
	g_pd3dDevice->SetVertexShaderConstant(C_ONE,D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f),1);
	// Illumination texture
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);	
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSW,	D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetRenderState( D3DRS_WRAP2, 0);
	
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);	
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSW,	D3DTADDRESS_CLAMP);	
	
	g_pd3dDevice->SetRenderState( D3DRS_WRAP3, 0);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	g_pd3dDevice->SetPixelShaderConstant(2,&light,1);
	pShader = vShader = NULL;
#endif
}

void renderSetCurrentMaterial(int r, int g, int b)
{
	CurrentColor = D3DCOLOR_XRGB(r, g, b);
	vert[0].color = vert[1].color = vert[2].color = CurrentColor;
}

void makeAndDrawVB(D3DTLVERTEX *data, int numverts, _D3DPRIMITIVETYPE type, int numprims)
{
	IDirect3DVertexBuffer8 *tribuf;
	unsigned char *ptr;
	HRESULT hr;

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEX) * numverts, D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, D3DPOOL_DEFAULT, &tribuf)))
		return;
	tribuf->Lock(0, 0, &ptr, 0);
	memcpy(ptr, data, sizeof(D3DTLVERTEX) * numverts);
	tribuf->Unlock();
	g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEX));
	g_pd3dDevice->SetVertexShader( D3DFVF_TLVERTEX );
	g_pd3dDevice->SetIndices(NULL, 0);
	hr = g_pd3dDevice->DrawPrimitive(type,  0, numprims);
	tribuf->Release();

}

void makeAndDrawVB(D3DTLVERTEX2 *data, int numverts, _D3DPRIMITIVETYPE type, int numprims)
{
	IDirect3DVertexBuffer8 *tribuf;
	unsigned char *ptr;

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEX2) * numverts, D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX2, D3DPOOL_DEFAULT, &tribuf)))
		return;
	tribuf->Lock(0, 0, &ptr, 0);
	memcpy(ptr, data, sizeof(D3DTLVERTEX2) * numverts);
	tribuf->Unlock();
	g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEX2));
	g_pd3dDevice->SetVertexShader( D3DFVF_TLVERTEX2 );
	g_pd3dDevice->DrawPrimitive(type,  0, numprims);
	tribuf->Release();

}

/*void makeAndDrawIVB(D3DTLVERTEX *data, int numverts, LPWORD indices, int numIndices, _D3DPRIMITIVETYPE type, int fvf)
{
	IDirect3DVertexBuffer8 *tribuf;
	IDirect3DIndexBuffer8 *triind;
	unsigned char *ptr;

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEX) * numverts, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &tribuf)))
		return;
	tribuf->Lock(0, 0, &ptr, 0);
	memcpy(ptr, data, sizeof(D3DTLVERTEX) * numverts);
	tribuf->Unlock();

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(numIndices, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &triind)))
		return;
	if (FAILED(hr = triind->Lock(0, 0, ptr, D3DLOCK_DISCARD)))
			dprintf("index buffer lock\n");
	memcpy(ptr, indices, numindices * 
	g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEX));
	g_pd3dDevice->DrawPrimitive(type,  0, numverts/3);
	tribuf->Release();

}*/

void renderTriangle(rTriangle* tri)
{
	vert[0].sx = tri->x0;
	vert[0].sy = tri->y0;
	vert[0].sz = tri->z0;
	vert[0].rhw = 1/vert[0].sz;
	vert[1].sx = tri->x1;
	vert[1].sy = tri->y1;
	vert[1].sz = tri->z1;
	vert[1].rhw = 1/vert[1].sz;
	vert[2].sx = tri->x2;
	vert[2].sy = tri->y2;
	vert[2].sz = tri->z2;
	vert[2].rhw = 1/vert[2].sz;

	makeAndDrawVB(&vert[0], 3, D3DPT_TRIANGLELIST, 1);
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX , vert, 3, 0);
	

}

void SetFillMode(D3DFILLMODE mode)
{

	if(CurrentFillMode != mode)
	{
		g_pd3dDevice->SetRenderState( D3DRS_FILLMODE , mode );
		CurrentFillMode = mode;
	}
	if(CurrentFillMode == D3DFILL_WIREFRAME)
	{
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}
}

void renderList(D3DTLVERTEX* list,int numVert,BOOL wireframe,BOOL clip)
{
	SetFillMode(wireframe?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	makeAndDrawVB(list, numVert, D3DPT_TRIANGLELIST, numVert/3); 
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX , list, numVert, 0);
}

void renderLine(D3DTLVERTEX* list,int numVert)
{
	//g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_TLVERTEX , list, numVert, 0);
	makeAndDrawVB(list, numVert, D3DPT_LINELIST, numVert/2); 
}

void renderFan(D3DTLVERTEX* list,int numVert)
{
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX , list, numVert, 0);
	makeAndDrawVB(list, numVert, D3DPT_TRIANGLEFAN, numVert-2);
}

void renderFan(D3DTLVERTEX2* list,int numVert)
{
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX2 , list, numVert, 0);
	makeAndDrawVB(list, numVert, D3DPT_TRIANGLEFAN, numVert-2);
}

void renderList(D3DTLVERTEX2* list,int numVert,BOOL wireframe,BOOL clip)
{
	SetFillMode(wireframe?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	if(TwoTextures)
	{
		BOOL tf;

		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert,0);
		makeAndDrawVB(list, numVert, D3DPT_TRIANGLEFAN, numVert-2);
	    tf = SetAlphaBlend( TRUE );

	    // Use the lightmap texture for the 2nd pass
	    SetTexture( 0, Texture2->lpTexture);//Surface );
	    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert, 0);
		makeAndDrawVB(list, numVert, D3DPT_TRIANGLEFAN, numVert-2);
	    SetAlphaBlend( tf );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	}
	else
	{
			//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert, 0);
		makeAndDrawVB(list, numVert, D3DPT_TRIANGLEFAN, numVert-2);
	}
}

#ifdef SJR
void renderIndexedList(D3DTLVERTEX* list,int numVert,LPWORD Indices, int numIndices, BOOL wireframe,BOOL clip)
{
	SetFillMode(wireframe?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX , list, numVert, Indices, numIndices, 0);
}


void renderIndexedList(D3DTLVERTEX2* list,int numVert,LPWORD Indices, int numIndices, BOOL wireframe,BOOL clip)
{
	SetFillMode(wireframe?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	if(TwoTextures)
	{
		BOOL tf;

		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert,Indices, numIndices, 0);
	    tf = SetAlphaBlend( TRUE );

	    // Use the lightmap texture for the 2nd pass
	    SetTexture( 0, Texture2->lpSurface );
	    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert, Indices, numIndices, 0);
	    SetAlphaBlend( tf );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	}
	else
	{
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert, Indices, numIndices, 0);
	}
}
#endif

void renderFlipSurface(void)
{
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

/*
void renderBeginScene(bool clear,DWORD rgb)
{
	DWORD flags;

	if(clear)
		flags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
	else
		flags = D3DCLEAR_ZBUFFER;
	g_pd3dDevice->Clear( 1UL,(D3DRECT *)&g_rcViewportRect, flags ,rgb,1.0f,0);
	g_pd3dDevice->BeginScene();
}
*/
void renderBeginScene(BOOL clear,DWORD rgb)
{
	DWORD flags;

//	if(clear)
		flags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
//	else
		//flags = D3DCLEAR_ZBUFFER;
	//HRESULT hr = g_pd3dDevice->Clear( 1UL,(D3DRECT *)&g_rcViewportRect, flags ,rgb,1.0f,0);
	HRESULT hr = g_pd3dDevice->Clear( 0UL,NULL, flags ,rgb,1.0f,0);
	g_pd3dDevice->BeginScene();
}

void renderEndScene(void)
{
	if(DarkenSet)
	{
		DarkenSet = FALSE;
		renderSetCurrentMaterial(NULL,NULL);
	//	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEXDARK , darkvert, 4, 0);

		IDirect3DVertexBuffer8 *tribuf;
		unsigned char *ptr;

		if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEXDARK ) * 4, D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEXDARK, D3DPOOL_DEFAULT, &tribuf)))
			return;
		tribuf->Lock(0, 0, &ptr, 0);
		memcpy(ptr, darkvert, sizeof(D3DTLVERTEXDARK) * 4);
		tribuf->Unlock();
		g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEXDARK));
		g_pd3dDevice->SetVertexShader( D3DFVF_TLVERTEXDARK );
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,  0, 2);
		tribuf->Release();



	}
	g_pd3dDevice->EndScene();
}

void renderDeInitialise(CLOSE_TYPE flag)
{
/*	if(flag & CLOSE_3D)
		Cleanup3DEnvironment();
	if(flag & CLOSE_2D)
	{
		CleanupSurfaces();
		Cleanup2DEnvironment();
	}*///sjr
}

void renderDeInitialise(void)
{
	renderDeInitialise(CLOSE_ALL);
}

void renderMove(LPARAM lParam)
{
	int x=LOWORD(lParam);
	int y=HIWORD(lParam); 
	DWORD dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
	DWORD dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
	SetRect( &g_rcScreenRect, x, y, x + dwWidth, y + dwHeight );
}

DWORD registerTexture(char *pointerToFile, int TextureStage,int *w,int *h)
{
	Mip mip;
	int myHandle=0;

	MipReadFromMem(&mip,pointerToFile,w,h);
	for(myHandle=0;myHandle<TEXTURES_AVAILABLE;++myHandle)
		if(textureHandles[myHandle].lpTexture == NULL)
			break;
	if(myHandle==TEXTURES_AVAILABLE)
		exit(0);
	RealizeImg(&textureHandles[myHandle],&mip,TextureStage);
	free(mip.data[0]);
	return myHandle+0x1000;
}

DWORD registerTextureFile(char *File, int TextureStage,int *w,int *h)
{
	Mip mip;
	int myHandle=0;
	FILE *f1;
	char pointerToFile[131125];

	f1=fopen(File,"rb");
	fread(pointerToFile,131125,1,f1);
	fclose(f1);

	MipReadFromMem(&mip,pointerToFile,w,h);
	for(myHandle=0;myHandle<TEXTURES_AVAILABLE;++myHandle)
		if(textureHandles[myHandle].lpTexture == NULL)
			break;
	if(myHandle==TEXTURES_AVAILABLE)
		exit(0);
	RealizeImg(&textureHandles[myHandle],&mip,TextureStage);
	free(mip.data[0]);
	return myHandle+0x1000;
}

DWORD registerBump(char *pointerToFile,int *w,int *h)
{
	Mip mip;
	int myHandle=0;

	MipReadFromMem(&mip,pointerToFile,w,h);
	for(myHandle=0;myHandle<TEXTURES_AVAILABLE;++myHandle)
		if(textureHandles[myHandle].lpTexture == NULL)
			break;
	if(myHandle==TEXTURES_AVAILABLE)
		exit(0);
	RealizeBump(&textureHandles[myHandle],&mip);
	free(mip.data[0]);
	return myHandle+0x1000;
}

DWORD registerTexture8(char *pointerToFile, int TextureStage,int *w,int *h)
{
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmih;
	LONG pal[256];
	WORD pal16[256],*outp;
	Mip mip;
	int x,y,i,pitch;
	int myHandle=0;

	memcpy(&bmfh,pointerToFile,sizeof(bmfh));
	pointerToFile += sizeof(bmfh);
	memcpy(&bmih,pointerToFile,sizeof(bmih));
	pointerToFile += sizeof(bmih);
	memcpy(pal,pointerToFile,sizeof(pal));
	pointerToFile += sizeof(pal);

	pitch = bmih.biSizeImage / bmih.biHeight;
	for(i=0;i<256;++i)
		pal16[i] = ((pal[i]&0xff)>>3) | ((pal[i]&0xfc00)>>5) | ((pal[i]&0xf80000)>>8);

	mip.data[0] = malloc(bmih.biHeight * bmih.biWidth * 2);
	mip.depth = 1;
	mip.format = TEXFMT_RGB_565;
	mip.height = bmih.biHeight;
	mip.width = bmih.biWidth;
	mip.size = mip.height * mip.width * 2;
	
	outp = (WORD *)mip.data[0];
	for (y=bmih.biHeight-1; y>=0; y--)
		for (x=0; x<bmih.biWidth; x++)
			*outp++=pal16[pointerToFile[(y*pitch)+x]];

	for(myHandle=0;myHandle<TEXTURES_AVAILABLE;++myHandle)
		if(textureHandles[myHandle].lpTexture == NULL)
			break;
	if(myHandle==TEXTURES_AVAILABLE)
		exit(0);
	RealizeImg(&textureHandles[myHandle],&mip,TextureStage);
	free(mip.data[0]);
	return myHandle+0x1000;
}

void unregisterTexture(DWORD handle)
{
	int offset = handle - 0x1000;
	if(textureHandles[offset].lpTexture)
	{
		textureHandles[offset].lpTexture->Release();
		textureHandles[offset].lpTexture=NULL;
	}
}

void SetAlphaTest(BOOL flag)
{
	if(CurrentAlphaTest != flag)
	{
		g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, flag );
		CurrentAlphaTest = flag;
	}
}

BOOL SetAlphaBlend(BOOL flag)
{
	BOOL tf = CurrentAlphaBlend;
	if(CurrentAlphaBlend != flag)
	{
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, flag );
		CurrentAlphaBlend = flag;
	}
	return tf;
}

void SetTClamp(int stage, BOOL flag)
{
	if(CurrentTClamp[stage] != flag)
	{
		g_pd3dDevice->SetTextureStageState(stage, D3DTSS_ADDRESSV,flag ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
		CurrentTClamp[stage] = flag;
	}
}

void SetSClamp(int stage, BOOL flag)
{
	if(CurrentSClamp[stage] != flag)
	{
		g_pd3dDevice->SetTextureStageState(stage, D3DTSS_ADDRESSU,flag ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
		CurrentSClamp[stage] = flag;
	}
}

void SetBlend(BOOL flag)
{
	if(!g_bSoftwareOnly)
	{
		if(CurrentBlend != flag)
		{
			if(flag)
			{
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_NONE);//POINT);
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_NONE);//POINT);
			}
			else
			{
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			}
			CurrentBlend = flag;
		}
	}
}

void SetTexture(int stage, LPDIRECT3DBASETEXTURE8 tex)
{
	if(CurrentTexture[stage] != tex)
	{
		g_pd3dDevice->SetTexture (stage,tex);
		CurrentTexture[stage] = tex;
	}
}

DWORD isTextureAlpha(DWORD handle)
{
	BorisTexture2 *bt = &textureHandles[handle - 0x1000];

	switch(bt->format)
	{
		case TEXFMT_ARGB_1555:
			return 1;
		case TEXFMT_ARGB_4444:
			return 4;
		default:
			return 0;
	}
}

void renderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2, MAP_TYPE maptype,BorisMaterial *m3)
{
	static bool OneTextureLastTime = true;
	static bool VoodooTexture = false;

	if(m1 && m1->textureHandle)
	{
		BorisTexture2 *bt = &textureHandles[m1->textureHandle - 0x1000];
		SetBlend(m1->flags & MAT_NOBLEND);
		if(bt->format == TEXFMT_ARGB_1555 || bt->format == TEXFMT_ARGB_4444)
		{
			SetAlphaTest(TRUE);
			SetAlphaBlend(TRUE);
			EnableZWrite(TRUE);
//			EnableZWrite(FALSE);
		}
		else
		{
			SetAlphaTest(FALSE);
			SetAlphaBlend(FALSE);
			EnableZWrite(TRUE);
		}
		SetSClamp(0,m1->flags & MAT_SCLAMP);
		SetTClamp(0,m1->flags & MAT_TCLAMP);
		SetTexture (0,bt->lpTexture);
	}
	else
	{
		SetTexture (0,NULL);
		SetAlphaTest(FALSE);
		SetAlphaBlend(TRUE);
	}
	if(TextureStages>1)
	{
		if(m2 && m2->textureHandle && (maptype == MAP_ENVIRONMENT || maptype == MAP_DETAIL))
		{
			BorisTexture2 *bt = &textureHandles[m2->textureHandle - 0x1000];
			
			OneTextureLastTime = false;
			if(!m1->textureHandle)
			{
				SetSClamp(0,m2->flags & MAT_SCLAMP);
				SetTClamp(0,m2->flags & MAT_TCLAMP);
				SetTexture (0,bt->lpTexture);
				
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1);
				TwoTextures = FALSE;
			}
			else
			{
				if(TextureStages>1)
				{
					DWORD passes;
					
					SetSClamp(1,m2->flags & MAT_SCLAMP);
					SetTClamp(1,m2->flags & MAT_TCLAMP);
					SetTexture (1,bt->lpTexture);
					
					if(maptype == MAP_ENVIRONMENT)
					{
						g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE);
						if(Caps & CAP_HASBUMPING)
						{
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
							g_pd3dDevice->SetTexture( 2, NULL );
						}
					}
					else
					{
						if(Caps & CAP_HASBUMPING)
						{
							float _val;

							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
							g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

							// Bump Texture on stage 1
							//
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT00, *(DWORD *)(&(_val=1.0f)) );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT01, *(DWORD *)(&(_val=0.0f)) );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT10, *(DWORD *)(&(_val=0.0f)) );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT11, *(DWORD *)(&(_val=1.0f))  );

							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVLSCALE, *(DWORD *)(&(_val=1.0f))  );
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVLOFFSET, *(DWORD *)(&(_val=0.0f))  );

							bt = &textureHandles[m3->textureHandle - 0x1000];
							// Environment Texture on stage 2
							//
							g_pd3dDevice->SetTexture( 2, bt->lpTexture );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU , D3DTADDRESS_CLAMP  );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV , D3DTADDRESS_CLAMP  );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
							g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
						}
						else
							g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
					}
					g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
					if(VoodooTexture || FAILED(g_pd3dDevice->ValidateDevice(&passes)))	// deals with Voodoo
					{
						g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
						g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE);
						g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
						VoodooTexture = true;
					}
					TwoTextures = FALSE;
				}
				else
				{
					TwoTextures = TRUE;
					Texture2 = bt;
				}
			}
		}
		else
		{
			if(!OneTextureLastTime)
			{
				SetTexture(1,NULL);
				g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
				g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
					DWORD passes;
				if(FAILED(g_pd3dDevice->ValidateDevice(&passes)))
				{
					OneTextureLastTime = true;
				}

				TwoTextures = FALSE;
				OneTextureLastTime = true;
			}
		}
	}
}

void EnableZBuffer(BOOL flag)
{
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, flag?zBuff:D3DZB_FALSE );
}

void EnableZWrite(BOOL flag)
{
	if(CurrentZWrite!=flag)
	{
		EnableZBuffer(flag);
		g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, flag );
		CurrentZWrite = flag;
	}
}

LPVIDEOMODES GetVideoModes(void)
{
	return VideoModes;
}

BOOL SetVideoMode(int num)
{
	DisplayWidth = VideoModes[num].width;
	DisplayHeight = VideoModes[num].height;
	DisplayDepth = VideoModes[num].depth;
	return TRUE;
}

BOOL ResetVideoMode(int num)
{
	if(DisplayWidth != VideoModes[num].width || DisplayHeight != VideoModes[num].height || DisplayDepth != VideoModes[num].depth)
	{
	/*	DisplayWidth = VideoModes[num].width;
		DisplayHeight = VideoModes[num].height;
		DisplayDepth = VideoModes[num].depth;
		CleanupSurfaces();
		InitializeSurfaces(g_hWnd);*///sjr
	}
	return TRUE;
}

void SetFogColor(DWORD color)
{
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, color);
}

void renderFog(BOOL fog)
{

//	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, fog);

}

void DarkenScreen(int sx,int sy, int width, int height, D3DCOLOR val,float zval)
{
	DarkenSet = TRUE;

	darkvert[0].sx = sx;
	darkvert[0].sy = sy;
	darkvert[0].sz = zval;
	darkvert[0].rhw = 1.0f;
	darkvert[0].color = val;
	darkvert[1].sx = sx+width;
	darkvert[1].sy = sy;
	darkvert[1].sz = zval;
	darkvert[1].rhw = 1.0f;
	darkvert[1].color = val;
	darkvert[2].sx = sx+width;
	darkvert[2].sy = sy+height;
	darkvert[2].sz = zval;
	darkvert[2].rhw = 1.0f;
	darkvert[2].color = val;
	darkvert[3].sx = sx;
	darkvert[3].sy = sy+height;
	darkvert[3].sz = zval;
	darkvert[3].rhw = 1.0f;
	darkvert[3].color = val;
}

#define MIRRORZ 0.0000001f
void MirrorMask(int sx,int sy, int width, int height)
{
	const D3DCOLOR val = 0x01010101;
	D3DTLVERTEXDARK mirrorvert[4];

	mirrorvert[0].sx = sx;
	mirrorvert[0].sy = sy;
	mirrorvert[0].sz = MIRRORZ;
	mirrorvert[0].rhw = 1.0f;
	mirrorvert[0].color = val;
	mirrorvert[1].sx = sx+width;
	mirrorvert[1].sy = sy;
	mirrorvert[1].sz = MIRRORZ;
	mirrorvert[1].rhw = 1.0f;
	mirrorvert[1].color = val;
	mirrorvert[2].sx = sx+width;
	mirrorvert[2].sy = sy+height;
	mirrorvert[2].sz = MIRRORZ;
	mirrorvert[2].rhw = 1.0f;
	mirrorvert[2].color = val;
	mirrorvert[3].sx = sx;
	mirrorvert[3].sy = sy+height;
	mirrorvert[3].sz = MIRRORZ;
	mirrorvert[3].rhw = 1.0f;
	mirrorvert[3].color = val;

	renderSetCurrentMaterial(NULL,NULL);
	g_pd3dDevice->Clear( 1UL,(D3DRECT *)&g_rcViewportRect, D3DCLEAR_ZBUFFER ,0,1.0f,0);
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEXDARK , mirrorvert, 4, 0);
	IDirect3DVertexBuffer8 *tribuf;
	unsigned char *ptr;

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEXDARK ) * 4, D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEXDARK, D3DPOOL_DEFAULT, &tribuf)))
		return;
	tribuf->Lock(0, 0, &ptr, 0);
	memcpy(ptr, mirrorvert, sizeof(D3DTLVERTEXDARK) * 4);
	tribuf->Unlock();
	g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEXDARK));
	g_pd3dDevice->SetVertexShader( D3DFVF_TLVERTEXDARK );
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,  0, 2);
	tribuf->Release();

}

void MirrorMask(int* mVert)
{
	const D3DCOLOR val = 0x70203060;
	D3DTLVERTEXDARK mirrorvert[4];

	mirrorvert[0].sx = mVert[0];
	mirrorvert[0].sy = mVert[1];
	mirrorvert[0].sz = MIRRORZ;
	mirrorvert[0].rhw = 1.0f;
	mirrorvert[0].color = val;
	mirrorvert[1].sx = mVert[2];
	mirrorvert[1].sy = mVert[3];
	mirrorvert[1].sz = MIRRORZ;
	mirrorvert[1].rhw = 1.0f;
	mirrorvert[1].color = val;
	mirrorvert[2].sx = mVert[4];
	mirrorvert[2].sy = mVert[5];
	mirrorvert[2].sz = MIRRORZ;
	mirrorvert[2].rhw = 1.0f;
	mirrorvert[2].color = val;
	mirrorvert[3].sx = mVert[6];
	mirrorvert[3].sy = mVert[7];
	mirrorvert[3].sz = MIRRORZ;
	mirrorvert[3].rhw = 1.0f;
	mirrorvert[3].color = val;

	renderSetCurrentMaterial(NULL,NULL);
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEXDARK , mirrorvert, 4, 0);
	IDirect3DVertexBuffer8 *tribuf;
	unsigned char *ptr;

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(sizeof(D3DTLVERTEXDARK ) * 4, D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEXDARK, D3DPOOL_DEFAULT, &tribuf)))
		return;
	tribuf->Lock(0, 0, &ptr, 0);
	memcpy(ptr, mirrorvert, sizeof(D3DTLVERTEXDARK) * 4);
	tribuf->Unlock();
	g_pd3dDevice->SetStreamSource(0, tribuf, sizeof(D3DTLVERTEXDARK));
	g_pd3dDevice->SetVertexShader( D3DFVF_TLVERTEXDARK );
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,  0, 2);
	tribuf->Release();
}

void ClearZ(void)
{
	g_pd3dDevice->Clear( 1UL,(D3DRECT *)&g_rcViewportRect, D3DCLEAR_ZBUFFER ,0,1.0f,0);
}

LPDIRECT3DVERTEXBUFFER8 CreateVBuffer(DWORD num, DWORD dwFVF, int stride)
{
/*	D3DVERTEXBUFFERDESC vbdesc;
	LPDIRECT3DVERTEXBUFFER8 lpVB;

	ZeroMemory(&vbdesc,sizeof(D3DVERTEXBUFFERDESC));
	vbdesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps = D3DVBCAPS_WRITEONLY;
	vbdesc.dwFVF = dwFVF;
	vbdesc.dwNumVertices = num;

	if(FAILED(g_pD3D->CreateVertexBuffer(&vbdesc, &lpVB, 0)))
		lpVB = NULL;*/
	IDirect3DVertexBuffer8 *tribuf;
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(stride * num, D3DUSAGE_WRITEONLY, dwFVF, D3DPOOL_DEFAULT, &tribuf)))
		return NULL;

	return(tribuf);
}

void renderListVB(LPDIRECT3DVERTEXBUFFER8 lpVB, DWORD total, DWORD dwFVF, int stride)
{
	//g_pd3dDevice->DrawPrimitiveVB(D3DPT_TRIANGLELIST,lpVB,0,total,0);
	HRESULT hr;
	hr = g_pd3dDevice->SetStreamSource(0, lpVB, stride);
	hr = g_pd3dDevice->SetVertexShader( dwFVF );
	hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,  0, total/3);


}



DWORD renderGetCaps(void)
{
	return Caps;
}

void SetLightState(int r, int g, int b)
{
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(r, g, b));
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
}

BOOL AddLight(float rRed, float rGreen, float rBlue, float rX, float rY, float rZ)
{
    D3DLIGHT8 light;
	HRESULT hr;

    memset(&light, 0, sizeof(D3DLIGHT8));
 
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = rRed;
    light.Diffuse.g    = rGreen;
    light.Diffuse.b    = rBlue;
    light.Diffuse.a    = 1.0f;
    light.Direction.x = rX;
    light.Direction.y = rY;
    light.Direction.z = rZ;

    // Set the global light's information
    hr = g_pd3dDevice->SetLight(0, &light);

    // Add the global light to the global viewport
	hr = g_pd3dDevice->LightEnable(0, TRUE);
 
    return TRUE;
}

bool ReleaseLight(void)
{
	HRESULT hr;

	hr = g_pd3dDevice->LightEnable(0, FALSE);
	return true;
}

void SetLightDirection(float x,float z)
{
	D3DLIGHT8 light;

	g_pd3dDevice->GetLight(0, &light);

	light.Direction.x = x;
	light.Direction.y = -0.5f;
	light.Direction.z = z;
    
	g_pd3dDevice->SetLight(0,&light);
}

void SetLightColour(float r,float g,float b)
{
	D3DLIGHT8 light;

	g_pd3dDevice->GetLight(0, &light);

    light.Diffuse.r    = r;
    light.Diffuse.g    = g;
    light.Diffuse.b    = b;
    
	g_pd3dDevice->SetLight(0,&light);
}

float SetZBack(float zback)
{
	static float lastZback=0.0f;
	float retval;

	SetProjectionMatrix(ProjectionMatrix, PI/5.5f, 3.0f/4.0f, 1.0f, zback );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &ProjectionMatrix );
	retval = lastZback;
	lastZback = zback;
	return retval;
}

#if defined(USE_CGVERTEXSHADER)

D3DXVECTOR4 m_vHtFogColor;

void SetFog(BOOL on)
{
	if (on) {
		D3DXMATRIX m, t;
		float m_fFogStart, m_fFogEnd, m_fFogRange;

		m_fFogStart = 10.0f;
		m_fFogEnd = 15.0f;
		m_fFogRange = 5.0f;

		D3DXVECTOR4 vFog(m_fFogStart, m_fFogEnd, m_fFogRange, 0.5f);
		g_pd3dDevice->SetVertexShaderConstant(CV_FOGPARAMS, &vFog, 1);
		
		//attenuation
		D3DXVECTOR4 vHtFog(0.4f, 0.1f, 0.1f, 0.5f);
		g_pd3dDevice->SetVertexShaderConstant(CV_HEIGHT_FOG_PARAMS, &vHtFog, 1);

		//Get view matrix
		D3DXMatrixIdentity(&m);
		D3DXMatrixTranslation(&m, boris2->currentcamera->position.x, boris2->currentcamera->position.y, boris2->currentcamera->position.z);
		D3DXMatrixRotationX(&m, boris2->currentcamera->angle.x);
		D3DXMatrixRotationY(&m, boris2->currentcamera->angle.y);
		D3DXMatrixRotationZ(&m, boris2->currentcamera->angle.z);

		D3DXMatrixMultiply(&t, &m, &D3DXMATRIX(WorldMatrix));
		D3DXMatrixMultiply(&t, &t, &D3DXMATRIX(ProjectionMatrix));
		D3DXMatrixTranspose(&t, &t);

		g_pd3dDevice->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &t(0, 0), 4);
		g_pd3dDevice->SetVertexShaderConstant(CV_ZEROONE, D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f), 1);

		// CV_LIGHT_CONST -> x = ambient
		//   y = ambient for back facing illumination - transparency
    	//   z = back facing const attenuation

		g_pd3dDevice->SetVertexShaderConstant(CV_LIGHT_CONST, D3DXVECTOR4(0.45f, 0.1f, 0.7f, 0.0f), 1);	
		g_pd3dDevice->SetVertexShaderConstant(CV_LIGHT_CONST, D3DXVECTOR4(0.15f, 0.1f, 0.7f, 0.0f), 1);


		m_vHtFogColor = D3DXVECTOR4(0.90f, 0.90f, 1.0f, 0.0f);		// RGBA
	//m_vHtFogColor = D3DXVECTOR4(0.20f, 0.20f, 0.5f, 0.0f);		// RGBA

	g_pd3dDevice->SetPixelShaderConstant(CP_HEIGHT_FOG_COLOR, &m_vHtFogColor, 1);
		g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);
		g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
		g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0x00aaaaaa);
		m_FogVS->SetShaderActive();
	} else {
		g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
	}


}

void ActivateSpecular(BorisTexture2 *bt)
{
	// Set transform from object space to projection space
/*	D3DXMATRIX worldViewProj = WorldMatrix * ProjectionMatrix;//m_View * m_Projection;
	D3DXMatrixTranspose(&worldViewProj, &worldViewProj);
	m_Dot3SpecularVS->SetShaderConstant(m_Dot3SpecularVSWorldViewProj, &worldViewProj(0, 0));

	// Send light position in object space
	D3DXMATRIX world2obj;
	D3DXMatrixInverse(&world2obj, 0, &WorldMatrix);
	D3DXVECTOR3 lightPositionInObjectSpace;
	D3DXVec3TransformCoord(&lightPositionInObjectSpace, &m_LightPosition, &world2obj);
	m_Dot3SpecularVS->SetShaderConstant(m_Dot3SpecularVSLightPosition, lightPositionInObjectSpace);

	// Send eye position in object space
	D3DXMATRIX view2world;
	D3DXMatrixInverse(&view2world, 0, &ProjectionMatrix);//m_View);
	D3DXVECTOR3 eyePositionInWorldSpace(view2world._41, view2world._42, view2world._43);
	D3DXVECTOR3 eyePositionInObjectSpace;
	D3DXVec3TransformCoord(&eyePositionInObjectSpace, &eyePositionInWorldSpace, &world2obj);
	m_Dot3SpecularVS->SetShaderConstant(m_Dot3SpecularVSEyePosition, eyePositionInObjectSpace);


	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Activate shader
	m_Dot3SpecularVS->SetShaderActive();
	m_Dot3SpecularPS->SetShaderActive();
	m_Dot3SpecularPS->SetTexture(m_Dot3SpecularPSNormalizationMap, bt->lpTexture);
*/
}
#endif

#if defined(USE_NONCGVERTEXSHADER)

HRESULT LoadAndCreateShader(char *filename, const DWORD* pDeclaration, DWORD Usage, EBSHADERTYPE ShaderType, DWORD *pHandle)
{
	HANDLE hFile;
	HRESULT hr;

	hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 1;
	DWORD dwFileSize = GetFileSize(hFile,NULL);

	const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
	ReadFile(hFile, (void *)pShader, dwFileSize, &dwFileSize, NULL);
	CloseHandle(hFile);

	if(ShaderType == SHADERTYPE_VERTEXSHADER)
	{
		hr = g_pd3dDevice->CreateVertexShader(pDeclaration, pShader,pHandle, Usage);
	}
	else
	if(ShaderType == SHADERTYPE_PIXELSHADER)
	{
		hr = g_pd3dDevice->CreatePixelShader(pShader,pHandle);
	}
	HeapFree(GetProcessHeap(), 0, (void*)pShader);

	return S_OK;
}
	
void SetVertexShader(DWORD handle)
{
	if(handle != vShader)
	{
		g_pd3dDevice->SetVertexShader(handle);
		vShader = handle;
	}
}
void SetPixelShader(DWORD handle)
{
	if(handle != pShader)
	{
		g_pd3dDevice->SetPixelShader(handle);
		pShader = handle;
	}
}

DWORD GetShader(int shader)
{
		return Shader[shader];
}
#endif
