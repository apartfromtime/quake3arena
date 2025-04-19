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
** QGL_WIN.C
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Quake3 you must implement the following
** two functions:
**
** QGL_Init() - loads libraries, assigns function pointers, etc.
** QGL_Shutdown() - unloads libraries, NULLs function pointers
*/

#include "../renderer/tr_local.h"
#include "../system/sdl_local.h"

void QGL_EnableLogging( bool enable );

// Extensions
PFNGLMULTITEXCOORD2FARB qglMultiTexCoord2fARB;
PFNGLACTIVETEXTUREARB qglActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARB qglClientActiveTextureARB;

PFNGLLOCKARRAYSEXT qglLockArraysEXT;
PFNGLUNLOCKARRAYSEXT qglUnlockArraysEXT;

// OpenGL 1
PFNGLACCUM qglAccum;
PFNGLALPHAFUNC qglAlphaFunc;
PFNGLARETEXTURESRESIDENT qglAreTexturesResident;
PFNGLARRAYELEMENT qglArrayElement;
PFNGLBEGIN qglBegin;
PFNGLBINDTEXTURE qglBindTexture;
PFNGLBITMAP qglBitmap;
PFNGLBLENDFUNC qglBlendFunc;
PFNGLCALLLIST qglCallList;
PFNGLCALLLISTS qglCallLists;
PFNGLCLEAR qglClear;
PFNGLCLEARACCUM qglClearAccum;
PFNGLCLEARCOLOR qglClearColor;
PFNGLCLEARDEPTH qglClearDepth;
PFNGLCLEARINDEX qglClearIndex;
PFNGLCLEARSTENCIL qglClearStencil;
PFNGLCLIPPLANE qglClipPlane;
PFNGLCOLOR3B qglColor3b;
PFNGLCOLOR3BV qglColor3bv;
PFNGLCOLOR3D qglColor3d;
PFNGLCOLOR3DV qglColor3dv;
PFNGLCOLOR3F qglColor3f;
PFNGLCOLOR3FV qglColor3fv;
PFNGLCOLOR3I qglColor3i;
PFNGLCOLOR3IV qglColor3iv;
PFNGLCOLOR3S qglColor3s;
PFNGLCOLOR3SV qglColor3sv;
PFNGLCOLOR3UB qglColor3ub;
PFNGLCOLOR3UBV qglColor3ubv;
PFNGLCOLOR3UI qglColor3ui;
PFNGLCOLOR3UIV qglColor3uiv;
PFNGLCOLOR3US qglColor3us;
PFNGLCOLOR3USV qglColor3usv;
PFNGLCOLOR4B qglColor4b;
PFNGLCOLOR4BV qglColor4bv;
PFNGLCOLOR4D qglColor4d;
PFNGLCOLOR4DV qglColor4dv;
PFNGLCOLOR4F qglColor4f;
PFNGLCOLOR4FV qglColor4fv;
PFNGLCOLOR4I qglColor4i;
PFNGLCOLOR4IV qglColor4iv;
PFNGLCOLOR4S qglColor4s;
PFNGLCOLOR4SV qglColor4sv;
PFNGLCOLOR4UB qglColor4ub;
PFNGLCOLOR4UBV qglColor4ubv;
PFNGLCOLOR4UI qglColor4ui;
PFNGLCOLOR4UIV qglColor4uiv;
PFNGLCOLOR4US qglColor4us;
PFNGLCOLOR4USV qglColor4usv;
PFNGLCOLORMASK qglColorMask;
PFNGLCOLORMATERIAL qglColorMaterial;
PFNGLCOLORPOINTER qglColorPointer;
PFNGLCOPYPIXELS qglCopyPixels;
PFNGLCOPYTEXIMAGE1D qglCopyTexImage1D;
PFNGLCOPYTEXIMAGE2D qglCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE1D qglCopyTexSubImage1D;
PFNGLCOPYTEXSUBIMAGE2D qglCopyTexSubImage2D;
PFNGLCULLFACE qglCullFace;
PFNGLDELETELISTS qglDeleteLists;
PFNGLDELETETEXTURES qglDeleteTextures;
PFNGLDEPTHFUNC qglDepthFunc;
PFNGLDEPTHMASK qglDepthMask;
PFNGLDEPTHRANGE qglDepthRange;
PFNGLDISABLE qglDisable;
PFNGLDISABLECLIENTSTATE qglDisableClientState;
PFNGLDRAWARRAYS qglDrawArrays;
PFNGLDRAWBUFFER qglDrawBuffer;
PFNGLDRAWELEMENTS qglDrawElements;
PFNGLDRAWPIXELS qglDrawPixels;
PFNGLEDGEFLAG qglEdgeFlag;
PFNGLEDGEFLAGPOINTER qglEdgeFlagPointer;
PFNGLEDGEFLAGV qglEdgeFlagv;
PFNGLENABLE qglEnable;
PFNGLENABLECLIENTSTATE qglEnableClientState;
PFNGLEND qglEnd;
PFNGLENDLIST qglEndList;
PFNGLEVALCOORD1D qglEvalCoord1d;
PFNGLEVALCOORD1DV qglEvalCoord1dv;
PFNGLEVALCOORD1F qglEvalCoord1f;
PFNGLEVALCOORD1FV qglEvalCoord1fv;
PFNGLEVALCOORD2D qglEvalCoord2d;
PFNGLEVALCOORD2DV qglEvalCoord2dv;
PFNGLEVALCOORD2F qglEvalCoord2f;
PFNGLEVALCOORD2FV qglEvalCoord2fv;
PFNGLEVALMESH1 qglEvalMesh1;
PFNGLEVALMESH2 qglEvalMesh2;
PFNGLEVALPOINT1 qglEvalPoint1;
PFNGLEVALPOINT2 qglEvalPoint2;
PFNGLFEEDBACKBUFFER qglFeedbackBuffer;
PFNGLFINISH qglFinish;
PFNGLFLUSH qglFlush;
PFNGLFOGF qglFogf;
PFNGLFOGFV qglFogfv;
PFNGLFOGI qglFogi;
PFNGLFOGIV qglFogiv;
PFNGLFRONTFACE qglFrontFace;
PFNGLFRUSTUM qglFrustum;
PFNGENLISTS qglGenLists;
PFNGLGENTEXTURES qglGenTextures;
PFNGLGETBOOLEANV qglGetBooleanv;
PFNGLGETCLIPPLANE qglGetClipPlane;
PFNGLGETDOUBLEV qglGetDoublev;
PFNGETERROR qglGetError;
PFNGLGETFLOATV qglGetFloatv;
PFNGLGETINTEGERV qglGetIntegerv;
PFNGLGETLIGHTFV qglGetLightfv;
PFNGLGETLIGHTIV qglGetLightiv;
PFNGLGETMAPDV qglGetMapdv;
PFNGLGETMAPFV qglGetMapfv;
PFNGLGETMAPIV qglGetMapiv;
PFNGLGETMATERIALFV qglGetMaterialfv;
PFNGLGETMATERIALIV qglGetMaterialiv;
PFNGLGETPIXELMAPFV qglGetPixelMapfv;
PFNGLGETPIXELMAPUIV qglGetPixelMapuiv;
PFNGLGETPIXELMAPUSV qglGetPixelMapusv;
PFNGLGETPOINTERV qglGetPointerv;
PFNGLGETPOLYGONSTIPPLE qglGetPolygonStipple;
PFNGLGETSTRING qglGetString;
PFNGLGETTEXENVFV qglGetTexEnvfv;
PFNGLGETTEXENVIV qglGetTexEnviv;
PFNGLGETTEXGENDV qglGetTexGendv;
PFNGLGETTEXGENFV qglGetTexGenfv;
PFNGLGETTEXGENIV qglGetTexGeniv;
PFNGLGETTEXIMAGE qglGetTexImage;
PFNGLGETTEXLEVELPARAMETERFV qglGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIV qglGetTexLevelParameteriv;
PFNGLGETTEXPARAMETERFV qglGetTexParameterfv;
PFNGLGETTEXPARAMETERIV qglGetTexParameteriv;
PFNGLHINT qglHint;
PFNGLINDEXMASK qglIndexMask;
PFNGLINDEXPOINTER qglIndexPointer;
PFNGLINDEXD qglIndexd;
PFNGLINDEXDV qglIndexdv;
PFNGLINDEXF qglIndexf;
PFNGLINDEXFV qglIndexfv;
PFNGLINDEXI qglIndexi;
PFNGLINDEXIV qglIndexiv;
PFNGLINDEXS qglIndexs;
PFNGLINDEXSV qglIndexsv;
PFNGLINDEXUB qglIndexub;
PFNGLINDEXUBV qglIndexubv;
PFNGLINITNAMES qglInitNames;
PFNGLINTERLEAVEDARRAYS qglInterleavedArrays;
PFNISENABLED qglIsEnabled;
PFNISLIST qglIsList;
PFNISTEXTURE qglIsTexture;
PFNGLLIGHTMODELF qglLightModelf;
PFNGLLIGHTMODELFV qglLightModelfv;
PFNGLLIGHTMODELI qglLightModeli;
PFNGLLIGHTMODELIV qglLightModeliv;
PFNGLLIGHTF qglLightf;
PFNGLLIGHTFV qglLightfv;
PFNGLLIGHTI qglLighti;
PFNGLLIGHTIV qglLightiv;
PFNGLLINESTIPPLE qglLineStipple;
PFNGLLINEWIDTH qglLineWidth;
PFNGLLISTBASE qglListBase;
PFNGLLOADIDENTITY qglLoadIdentity;
PFNGLLOADMATRIXD qglLoadMatrixd;
PFNGLLOADMATRIXF qglLoadMatrixf;
PFNGLLOADNAME qglLoadName;
PFNGLLOGICOP qglLogicOp;
PFNGLMAP1D qglMap1d;
PFNGLMAP1F qglMap1f;
PFNGLMAP2D qglMap2d;
PFNGLMAP2F qglMap2f;
PFNGLMAPGRID1D qglMapGrid1d;
PFNGLMAPGRID1F qglMapGrid1f;
PFNGLMAPGRID2D qglMapGrid2d;
PFNGLMAPGRID2F qglMapGrid2f;
PFNGLMATERIALF qglMaterialf;
PFNGLMATERIALFV qglMaterialfv;
PFNGLMATERIALI qglMateriali;
PFNGLMATERIALIV qglMaterialiv;
PFNGLMATRIXMODE qglMatrixMode;
PFNGLMULTMATRIXD qglMultMatrixd;
PFNGLMULTMATRIXF qglMultMatrixf;
PFNGLNEWLIST qglNewList;
PFNGLNORMAL3B qglNormal3b;
PFNGLNORMAL3BV qglNormal3bv;
PFNGLNORMAL3D qglNormal3d;
PFNGLNORMAL3DV qglNormal3dv;
PFNGLNORMAL3F qglNormal3f;
PFNGLNORMAL3FV qglNormal3fv;
PFNGLNORMAL3I qglNormal3i;
PFNGLNORMAL3IV qglNormal3iv;
PFNGLNORMAL3S qglNormal3s;
PFNGLNORMAL3SV qglNormal3sv;
PFNGLNORMALPOINTER qglNormalPointer;
PFNGLORTHO qglOrtho;
PFNGLPASSTHROUGH qglPassThrough;
PFNGLPIXELMAPFV qglPixelMapfv;
PFNGLPIXELMAPUIV qglPixelMapuiv;
PFNGLPIXELMAPUSV qglPixelMapusv;
PFNGLPIXELSTOREF qglPixelStoref;
PFNGLPIXELSTOREI qglPixelStorei;
PFNGLPIXELTRANSFERF qglPixelTransferf;
PFNGLPIXELTRANSFERI qglPixelTransferi;
PFNGLPIXELZOOM qglPixelZoom;
PFNGLPOINTSIZE qglPointSize;
PFNGLPOLYGONMODE qglPolygonMode;
PFNGLPOLYGONOFFSET qglPolygonOffset;
PFNGLPOLYGONSTIPPLE qglPolygonStipple;
PFNGLPOPATTRIB qglPopAttrib;
PFNGLPOPCLIENTATTRIB qglPopClientAttrib;
PFNGLPOPMATRIX qglPopMatrix;
PFNGLPOPNAME qglPopName;
PFNGLPRIORITIZETEXTURES qglPrioritizeTextures;
PFNGLPUSHATTRIB qglPushAttrib;
PFNGLPUSHCLIENTATTRIB qglPushClientAttrib;
PFNGLPUSHMATRIX qglPushMatrix;
PFNGLPUSHNAME qglPushName;
PFNGLRASTERPOS2D qglRasterPos2d;
PFNGLRASTERPOS2DV qglRasterPos2dv;
PFNGLRASTERPOS2F qglRasterPos2f;
PFNGLRASTERPOS2FV qglRasterPos2fv;
PFNGLRASTERPOS2I qglRasterPos2i;
PFNGLRASTERPOS2IV qglRasterPos2iv;
PFNGLRASTERPOS2S qglRasterPos2s;
PFNGLRASTERPOS2SV qglRasterPos2sv;
PFNGLRASTERPOS3D qglRasterPos3d;
PFNGLRASTERPOS3DV qglRasterPos3dv;
PFNGLRASTERPOS3F qglRasterPos3f;
PFNGLRASTERPOS3FV qglRasterPos3fv;
PFNGLRASTERPOS3I qglRasterPos3i;
PFNGLRASTERPOS3IV qglRasterPos3iv;
PFNGLRASTERPOS3S qglRasterPos3s;
PFNGLRASTERPOS3SV qglRasterPos3sv;
PFNGLRASTERPOS4D qglRasterPos4d;
PFNGLRASTERPOS4DV qglRasterPos4dv;
PFNGLRASTERPOS4F qglRasterPos4f;
PFNGLRASTERPOS4FV qglRasterPos4fv;
PFNGLRASTERPOS4I qglRasterPos4i;
PFNGLRASTERPOS4IV qglRasterPos4iv;
PFNGLRASTERPOS4S qglRasterPos4s;
PFNGLRASTERPOS4SV qglRasterPos4sv;
PFNGLREADBUFFER qglReadBuffer;
PFNGLREADPIXELS qglReadPixels;
PFNGLRECTD qglRectd;
PFNGLRECTDV qglRectdv;
PFNGLRECTF qglRectf;
PFNGLRECTFV qglRectfv;
PFNGLRECTI qglRecti;
PFNGLRECTIV qglRectiv;
PFNGLRECTS qglRects;
PFNGLRECTSV qglRectsv;
PFNRENDERMODE qglRenderMode;
PFNGLROTATED qglRotated;
PFNGLROTATEF qglRotatef;
PFNGLSCALED qglScaled;
PFNGLSCALEF qglScalef;
PFNGLSCISSOR qglScissor;
PFNGLSELECTBUFFER qglSelectBuffer;
PFNGLSHADEMODEL qglShadeModel;
PFNGLSTENCILFUNC qglStencilFunc;
PFNGLSTENCILMASK qglStencilMask;
PFNGLSTENCILOP qglStencilOp;
PFNGLTEXCOORD1D qglTexCoord1d;
PFNGLTEXCOORD1DV qglTexCoord1dv;
PFNGLTEXCOORD1F qglTexCoord1f;
PFNGLTEXCOORD1FV qglTexCoord1fv;
PFNGLTEXCOORD1I qglTexCoord1i;
PFNGLTEXCOORD1IV qglTexCoord1iv;
PFNGLTEXCOORD1S qglTexCoord1s;
PFNGLTEXCOORD1SV qglTexCoord1sv;
PFNGLTEXCOORD2D qglTexCoord2d;
PFNGLTEXCOORD2DV qglTexCoord2dv;
PFNGLTEXCOORD2F qglTexCoord2f;
PFNGLTEXCOORD2FV qglTexCoord2fv;
PFNGLTEXCOORD2I qglTexCoord2i;
PFNGLTEXCOORD2IV qglTexCoord2iv;
PFNGLTEXCOORD2S qglTexCoord2s;
PFNGLTEXCOORD2SV qglTexCoord2sv;
PFNGLTEXCOORD3D qglTexCoord3d;
PFNGLTEXCOORD3DV qglTexCoord3dv;
PFNGLTEXCOORD3F qglTexCoord3f;
PFNGLTEXCOORD3FV qglTexCoord3fv;
PFNGLTEXCOORD3I qglTexCoord3i;
PFNGLTEXCOORD3IV qglTexCoord3iv;
PFNGLTEXCOORD3S qglTexCoord3s;
PFNGLTEXCOORD3SV qglTexCoord3sv;
PFNGLTEXCOORD4D qglTexCoord4d;
PFNGLTEXCOORD4DV qglTexCoord4dv;
PFNGLTEXCOORD4F qglTexCoord4f;
PFNGLTEXCOORD4FV qglTexCoord4fv;
PFNGLTEXCOORD4I qglTexCoord4i;
PFNGLTEXCOORD4IV qglTexCoord4iv;
PFNGLTEXCOORD4S qglTexCoord4s;
PFNGLTEXCOORD4SV qglTexCoord4sv;
PFNGLTEXCOORDPOINTER qglTexCoordPointer;
PFNGLTEXENVF qglTexEnvf;
PFNGLTEXENVFV qglTexEnvfv;
PFNGLTEXENVI qglTexEnvi;
PFNGLTEXENVIV qglTexEnviv;
PFNGLTEXGEND qglTexGend;
PFNGLTEXGENDV qglTexGendv;
PFNGLTEXGENF qglTexGenf;
PFNGLTEXGENFV qglTexGenfv;
PFNGLTEXGENI qglTexGeni;
PFNGLTEXGENIV qglTexGeniv;
PFNGLTEXIMAGE1D qglTexImage1D;
PFNGLTEXIMAGE2D qglTexImage2D;
PFNGLTEXPARAMETERF qglTexParameterf;
PFNGLTEXPARAMETERFV qglTexParameterfv;
PFNGLTEXPARAMETERI qglTexParameteri;
PFNGLTEXPARAMETERIV qglTexParameteriv;
PFNGLTEXSUBIMAGE1D qglTexSubImage1D;
PFNGLTEXSUBIMAGE2D qglTexSubImage2D;
PFNGLTRANSLATED qglTranslated;
PFNGLTRANSLATEF qglTranslatef;
PFNGLVERTEX2D qglVertex2d;
PFNGLVERTEX2DV qglVertex2dv;
PFNGLVERTEX2F qglVertex2f;
PFNGLVERTEX2FV qglVertex2fv;
PFNGLVERTEX2I qglVertex2i;
PFNGLVERTEX2IV qglVertex2iv;
PFNGLVERTEX2S qglVertex2s;
PFNGLVERTEX2SV qglVertex2sv;
PFNGLVERTEX3D qglVertex3d;
PFNGLVERTEX3DV qglVertex3dv;
PFNGLVERTEX3F qglVertex3f;
PFNGLVERTEX3FV qglVertex3fv;
PFNGLVERTEX3I qglVertex3i;
PFNGLVERTEX3IV qglVertex3iv;
PFNGLVERTEX3S qglVertex3s;
PFNGLVERTEX3SV qglVertex3sv;
PFNGLVERTEX4D qglVertex4d;
PFNGLVERTEX4DV qglVertex4dv;
PFNGLVERTEX4F qglVertex4f;
PFNGLVERTEX4FV qglVertex4fv;
PFNGLVERTEX4I qglVertex4i;
PFNGLVERTEX4IV qglVertex4iv;
PFNGLVERTEX4S qglVertex4s;
PFNGLVERTEX4SV qglVertex4sv;
PFNGLVERTEXPOINTER qglVertexPointer;
PFNGLVIEWPORT qglViewport;


static PFNGLACCUM dllAccum;
static PFNGLALPHAFUNC dllAlphaFunc;
static PFNGLARETEXTURESRESIDENT dllAreTexturesResident;
static PFNGLARRAYELEMENT dllArrayElement;
static PFNGLBEGIN dllBegin;
static PFNGLBINDTEXTURE dllBindTexture;
static PFNGLBITMAP dllBitmap;
static PFNGLBLENDFUNC dllBlendFunc;
static PFNGLCALLLIST dllCallList;
static PFNGLCALLLISTS dllCallLists;
static PFNGLCLEAR dllClear;
static PFNGLCLEARACCUM dllClearAccum;
static PFNGLCLEARCOLOR dllClearColor;
static PFNGLCLEARDEPTH dllClearDepth;
static PFNGLCLEARINDEX dllClearIndex;
static PFNGLCLEARSTENCIL dllClearStencil;
static PFNGLCLIPPLANE dllClipPlane;
static PFNGLCOLOR3B dllColor3b;
static PFNGLCOLOR3BV dllColor3bv;
static PFNGLCOLOR3D dllColor3d;
static PFNGLCOLOR3DV dllColor3dv;
static PFNGLCOLOR3F dllColor3f;
static PFNGLCOLOR3FV dllColor3fv;
static PFNGLCOLOR3I dllColor3i;
static PFNGLCOLOR3IV dllColor3iv;
static PFNGLCOLOR3S dllColor3s;
static PFNGLCOLOR3SV dllColor3sv;
static PFNGLCOLOR3UB dllColor3ub;
static PFNGLCOLOR3UBV dllColor3ubv;
static PFNGLCOLOR3UI dllColor3ui;
static PFNGLCOLOR3UIV dllColor3uiv;
static PFNGLCOLOR3US dllColor3us;
static PFNGLCOLOR3USV dllColor3usv;
static PFNGLCOLOR4B dllColor4b;
static PFNGLCOLOR4BV dllColor4bv;
static PFNGLCOLOR4D dllColor4d;
static PFNGLCOLOR4DV dllColor4dv;
static PFNGLCOLOR4F dllColor4f;
static PFNGLCOLOR4FV dllColor4fv;
static PFNGLCOLOR4I dllColor4i;
static PFNGLCOLOR4IV dllColor4iv;
static PFNGLCOLOR4S dllColor4s;
static PFNGLCOLOR4SV dllColor4sv;
static PFNGLCOLOR4UB dllColor4ub;
static PFNGLCOLOR4UBV dllColor4ubv;
static PFNGLCOLOR4UI dllColor4ui;
static PFNGLCOLOR4UIV dllColor4uiv;
static PFNGLCOLOR4US dllColor4us;
static PFNGLCOLOR4USV dllColor4usv;
static PFNGLCOLORMASK dllColorMask;
static PFNGLCOLORMATERIAL dllColorMaterial;
static PFNGLCOLORPOINTER dllColorPointer;
static PFNGLCOPYPIXELS dllCopyPixels;
static PFNGLCOPYTEXIMAGE1D dllCopyTexImage1D;
static PFNGLCOPYTEXIMAGE2D dllCopyTexImage2D;
static PFNGLCOPYTEXSUBIMAGE1D dllCopyTexSubImage1D;
static PFNGLCOPYTEXSUBIMAGE2D dllCopyTexSubImage2D;
static PFNGLCULLFACE dllCullFace;
static PFNGLDELETELISTS dllDeleteLists;
static PFNGLDELETETEXTURES dllDeleteTextures;
static PFNGLDEPTHFUNC dllDepthFunc;
static PFNGLDEPTHMASK dllDepthMask;
static PFNGLDEPTHRANGE dllDepthRange;
static PFNGLDISABLE dllDisable;
static PFNGLDISABLECLIENTSTATE dllDisableClientState;
static PFNGLDRAWARRAYS dllDrawArrays;
static PFNGLDRAWBUFFER dllDrawBuffer;
static PFNGLDRAWELEMENTS dllDrawElements;
static PFNGLDRAWPIXELS dllDrawPixels;
static PFNGLEDGEFLAG dllEdgeFlag;
static PFNGLEDGEFLAGPOINTER dllEdgeFlagPointer;
static PFNGLEDGEFLAGV dllEdgeFlagv;
static PFNGLENABLE dllEnable;
static PFNGLENABLECLIENTSTATE dllEnableClientState;
static PFNGLEND dllEnd;
static PFNGLENDLIST dllEndList;
static PFNGLEVALCOORD1D dllEvalCoord1d;
static PFNGLEVALCOORD1DV dllEvalCoord1dv;
static PFNGLEVALCOORD1F dllEvalCoord1f;
static PFNGLEVALCOORD1FV dllEvalCoord1fv;
static PFNGLEVALCOORD2D dllEvalCoord2d;
static PFNGLEVALCOORD2DV dllEvalCoord2dv;
static PFNGLEVALCOORD2F dllEvalCoord2f;
static PFNGLEVALCOORD2FV dllEvalCoord2fv;
static PFNGLEVALMESH1 dllEvalMesh1;
static PFNGLEVALMESH2 dllEvalMesh2;
static PFNGLEVALPOINT1 dllEvalPoint1;
static PFNGLEVALPOINT2 dllEvalPoint2;
static PFNGLFEEDBACKBUFFER dllFeedbackBuffer;
static PFNGLFINISH dllFinish;
static PFNGLFLUSH dllFlush;
static PFNGLFOGF dllFogf;
static PFNGLFOGFV dllFogfv;
static PFNGLFOGI dllFogi;
static PFNGLFOGIV dllFogiv;
static PFNGLFRONTFACE dllFrontFace;
static PFNGLFRUSTUM dllFrustum;
static PFNGENLISTS dllGenLists;
static PFNGLGENTEXTURES dllGenTextures;
static PFNGLGETBOOLEANV dllGetBooleanv;
static PFNGLGETCLIPPLANE dllGetClipPlane;
static PFNGLGETDOUBLEV dllGetDoublev;
static PFNGETERROR dllGetError;
static PFNGLGETFLOATV dllGetFloatv;
static PFNGLGETINTEGERV dllGetIntegerv;
static PFNGLGETLIGHTFV dllGetLightfv;
static PFNGLGETLIGHTIV dllGetLightiv;
static PFNGLGETMAPDV dllGetMapdv;
static PFNGLGETMAPFV dllGetMapfv;
static PFNGLGETMAPIV dllGetMapiv;
static PFNGLGETMATERIALFV dllGetMaterialfv;
static PFNGLGETMATERIALIV dllGetMaterialiv;
static PFNGLGETPIXELMAPFV dllGetPixelMapfv;
static PFNGLGETPIXELMAPUIV dllGetPixelMapuiv;
static PFNGLGETPIXELMAPUSV dllGetPixelMapusv;
static PFNGLGETPOINTERV dllGetPointerv;
static PFNGLGETPOLYGONSTIPPLE dllGetPolygonStipple;
static PFNGLGETSTRING dllGetString;
static PFNGLGETTEXENVFV dllGetTexEnvfv;
static PFNGLGETTEXENVIV dllGetTexEnviv;
static PFNGLGETTEXGENDV dllGetTexGendv;
static PFNGLGETTEXGENFV dllGetTexGenfv;
static PFNGLGETTEXGENIV dllGetTexGeniv;
static PFNGLGETTEXIMAGE dllGetTexImage;
static PFNGLGETTEXLEVELPARAMETERFV dllGetTexLevelParameterfv;
static PFNGLGETTEXLEVELPARAMETERIV dllGetTexLevelParameteriv;
static PFNGLGETTEXPARAMETERFV dllGetTexParameterfv;
static PFNGLGETTEXPARAMETERIV dllGetTexParameteriv;
static PFNGLHINT dllHint;
static PFNGLINDEXMASK dllIndexMask;
static PFNGLINDEXPOINTER dllIndexPointer;
static PFNGLINDEXD dllIndexd;
static PFNGLINDEXDV dllIndexdv;
static PFNGLINDEXF dllIndexf;
static PFNGLINDEXFV dllIndexfv;
static PFNGLINDEXI dllIndexi;
static PFNGLINDEXIV dllIndexiv;
static PFNGLINDEXS dllIndexs;
static PFNGLINDEXSV dllIndexsv;
static PFNGLINDEXUB dllIndexub;
static PFNGLINDEXUBV dllIndexubv;
static PFNGLINITNAMES dllInitNames;
static PFNGLINTERLEAVEDARRAYS dllInterleavedArrays;
static PFNISENABLED dllIsEnabled;
static PFNISLIST dllIsList;
static PFNISTEXTURE dllIsTexture;
static PFNGLLIGHTMODELF dllLightModelf;
static PFNGLLIGHTMODELFV dllLightModelfv;
static PFNGLLIGHTMODELI dllLightModeli;
static PFNGLLIGHTMODELIV dllLightModeliv;
static PFNGLLIGHTF dllLightf;
static PFNGLLIGHTFV dllLightfv;
static PFNGLLIGHTI dllLighti;
static PFNGLLIGHTIV dllLightiv;
static PFNGLLINESTIPPLE dllLineStipple;
static PFNGLLINEWIDTH dllLineWidth;
static PFNGLLISTBASE dllListBase;
static PFNGLLOADIDENTITY dllLoadIdentity;
static PFNGLLOADMATRIXD dllLoadMatrixd;
static PFNGLLOADMATRIXF dllLoadMatrixf;
static PFNGLLOADNAME dllLoadName;
static PFNGLLOGICOP dllLogicOp;
static PFNGLMAP1D dllMap1d;
static PFNGLMAP1F dllMap1f;
static PFNGLMAP2D dllMap2d;
static PFNGLMAP2F dllMap2f;
static PFNGLMAPGRID1D dllMapGrid1d;
static PFNGLMAPGRID1F dllMapGrid1f;
static PFNGLMAPGRID2D dllMapGrid2d;
static PFNGLMAPGRID2F dllMapGrid2f;
static PFNGLMATERIALF dllMaterialf;
static PFNGLMATERIALFV dllMaterialfv;
static PFNGLMATERIALI dllMateriali;
static PFNGLMATERIALIV dllMaterialiv;
static PFNGLMATRIXMODE dllMatrixMode;
static PFNGLMULTMATRIXD dllMultMatrixd;
static PFNGLMULTMATRIXF dllMultMatrixf;
static PFNGLNEWLIST dllNewList;
static PFNGLNORMAL3B dllNormal3b;
static PFNGLNORMAL3BV dllNormal3bv;
static PFNGLNORMAL3D dllNormal3d;
static PFNGLNORMAL3DV dllNormal3dv;
static PFNGLNORMAL3F dllNormal3f;
static PFNGLNORMAL3FV dllNormal3fv;
static PFNGLNORMAL3I dllNormal3i;
static PFNGLNORMAL3IV dllNormal3iv;
static PFNGLNORMAL3S dllNormal3s;
static PFNGLNORMAL3SV dllNormal3sv;
static PFNGLNORMALPOINTER dllNormalPointer;
static PFNGLORTHO dllOrtho;
static PFNGLPASSTHROUGH dllPassThrough;
static PFNGLPIXELMAPFV dllPixelMapfv;
static PFNGLPIXELMAPUIV dllPixelMapuiv;
static PFNGLPIXELMAPUSV dllPixelMapusv;
static PFNGLPIXELSTOREF dllPixelStoref;
static PFNGLPIXELSTOREI dllPixelStorei;
static PFNGLPIXELTRANSFERF dllPixelTransferf;
static PFNGLPIXELTRANSFERI dllPixelTransferi;
static PFNGLPIXELZOOM dllPixelZoom;
static PFNGLPOINTSIZE dllPointSize;
static PFNGLPOLYGONMODE dllPolygonMode;
static PFNGLPOLYGONOFFSET dllPolygonOffset;
static PFNGLPOLYGONSTIPPLE dllPolygonStipple;
static PFNGLPOPATTRIB dllPopAttrib;
static PFNGLPOPCLIENTATTRIB dllPopClientAttrib;
static PFNGLPOPMATRIX dllPopMatrix;
static PFNGLPOPNAME dllPopName;
static PFNGLPRIORITIZETEXTURES dllPrioritizeTextures;
static PFNGLPUSHATTRIB dllPushAttrib;
static PFNGLPUSHCLIENTATTRIB dllPushClientAttrib;
static PFNGLPUSHMATRIX dllPushMatrix;
static PFNGLPUSHNAME dllPushName;
static PFNGLRASTERPOS2D dllRasterPos2d;
static PFNGLRASTERPOS2DV dllRasterPos2dv;
static PFNGLRASTERPOS2F dllRasterPos2f;
static PFNGLRASTERPOS2FV dllRasterPos2fv;
static PFNGLRASTERPOS2I dllRasterPos2i;
static PFNGLRASTERPOS2IV dllRasterPos2iv;
static PFNGLRASTERPOS2S dllRasterPos2s;
static PFNGLRASTERPOS2SV dllRasterPos2sv;
static PFNGLRASTERPOS3D dllRasterPos3d;
static PFNGLRASTERPOS3DV dllRasterPos3dv;
static PFNGLRASTERPOS3F dllRasterPos3f;
static PFNGLRASTERPOS3FV dllRasterPos3fv;
static PFNGLRASTERPOS3I dllRasterPos3i;
static PFNGLRASTERPOS3IV dllRasterPos3iv;
static PFNGLRASTERPOS3S dllRasterPos3s;
static PFNGLRASTERPOS3SV dllRasterPos3sv;
static PFNGLRASTERPOS4D dllRasterPos4d;
static PFNGLRASTERPOS4DV dllRasterPos4dv;
static PFNGLRASTERPOS4F dllRasterPos4f;
static PFNGLRASTERPOS4FV dllRasterPos4fv;
static PFNGLRASTERPOS4I dllRasterPos4i;
static PFNGLRASTERPOS4IV dllRasterPos4iv;
static PFNGLRASTERPOS4S dllRasterPos4s;
static PFNGLRASTERPOS4SV dllRasterPos4sv;
static PFNGLREADBUFFER dllReadBuffer;
static PFNGLREADPIXELS dllReadPixels;
static PFNGLRECTD dllRectd;
static PFNGLRECTDV dllRectdv;
static PFNGLRECTF dllRectf;
static PFNGLRECTFV dllRectfv;
static PFNGLRECTI dllRecti;
static PFNGLRECTIV dllRectiv;
static PFNGLRECTS dllRects;
static PFNGLRECTSV dllRectsv;
static PFNRENDERMODE dllRenderMode;
static PFNGLROTATED dllRotated;
static PFNGLROTATEF dllRotatef;
static PFNGLSCALED dllScaled;
static PFNGLSCALEF dllScalef;
static PFNGLSCISSOR dllScissor;
static PFNGLSELECTBUFFER dllSelectBuffer;
static PFNGLSHADEMODEL dllShadeModel;
static PFNGLSTENCILFUNC dllStencilFunc;
static PFNGLSTENCILMASK dllStencilMask;
static PFNGLSTENCILOP dllStencilOp;
static PFNGLTEXCOORD1D dllTexCoord1d;
static PFNGLTEXCOORD1DV dllTexCoord1dv;
static PFNGLTEXCOORD1F dllTexCoord1f;
static PFNGLTEXCOORD1FV dllTexCoord1fv;
static PFNGLTEXCOORD1I dllTexCoord1i;
static PFNGLTEXCOORD1IV dllTexCoord1iv;
static PFNGLTEXCOORD1S dllTexCoord1s;
static PFNGLTEXCOORD1SV dllTexCoord1sv;
static PFNGLTEXCOORD2D dllTexCoord2d;
static PFNGLTEXCOORD2DV dllTexCoord2dv;
static PFNGLTEXCOORD2F dllTexCoord2f;
static PFNGLTEXCOORD2FV dllTexCoord2fv;
static PFNGLTEXCOORD2I dllTexCoord2i;
static PFNGLTEXCOORD2IV dllTexCoord2iv;
static PFNGLTEXCOORD2S dllTexCoord2s;
static PFNGLTEXCOORD2SV dllTexCoord2sv;
static PFNGLTEXCOORD3D dllTexCoord3d;
static PFNGLTEXCOORD3DV dllTexCoord3dv;
static PFNGLTEXCOORD3F dllTexCoord3f;
static PFNGLTEXCOORD3FV dllTexCoord3fv;
static PFNGLTEXCOORD3I dllTexCoord3i;
static PFNGLTEXCOORD3IV dllTexCoord3iv;
static PFNGLTEXCOORD3S dllTexCoord3s;
static PFNGLTEXCOORD3SV dllTexCoord3sv;
static PFNGLTEXCOORD4D dllTexCoord4d;
static PFNGLTEXCOORD4DV dllTexCoord4dv;
static PFNGLTEXCOORD4F dllTexCoord4f;
static PFNGLTEXCOORD4FV dllTexCoord4fv;
static PFNGLTEXCOORD4I dllTexCoord4i;
static PFNGLTEXCOORD4IV dllTexCoord4iv;
static PFNGLTEXCOORD4S dllTexCoord4s;
static PFNGLTEXCOORD4SV dllTexCoord4sv;
static PFNGLTEXCOORDPOINTER dllTexCoordPointer;
static PFNGLTEXENVF dllTexEnvf;
static PFNGLTEXENVFV dllTexEnvfv;
static PFNGLTEXENVI dllTexEnvi;
static PFNGLTEXENVIV dllTexEnviv;
static PFNGLTEXGEND dllTexGend;
static PFNGLTEXGENDV dllTexGendv;
static PFNGLTEXGENF dllTexGenf;
static PFNGLTEXGENFV dllTexGenfv;
static PFNGLTEXGENI dllTexGeni;
static PFNGLTEXGENIV dllTexGeniv;
static PFNGLTEXIMAGE1D dllTexImage1D;
static PFNGLTEXIMAGE2D dllTexImage2D;
static PFNGLTEXPARAMETERF dllTexParameterf;
static PFNGLTEXPARAMETERFV dllTexParameterfv;
static PFNGLTEXPARAMETERI dllTexParameteri;
static PFNGLTEXPARAMETERIV dllTexParameteriv;
static PFNGLTEXSUBIMAGE1D dllTexSubImage1D;
static PFNGLTEXSUBIMAGE2D dllTexSubImage2D;
static PFNGLTRANSLATED dllTranslated;
static PFNGLTRANSLATEF dllTranslatef;
static PFNGLVERTEX2D dllVertex2d;
static PFNGLVERTEX2DV dllVertex2dv;
static PFNGLVERTEX2F dllVertex2f;
static PFNGLVERTEX2FV dllVertex2fv;
static PFNGLVERTEX2I dllVertex2i;
static PFNGLVERTEX2IV dllVertex2iv;
static PFNGLVERTEX2S dllVertex2s;
static PFNGLVERTEX2SV dllVertex2sv;
static PFNGLVERTEX3D dllVertex3d;
static PFNGLVERTEX3DV dllVertex3dv;
static PFNGLVERTEX3F dllVertex3f;
static PFNGLVERTEX3FV dllVertex3fv;
static PFNGLVERTEX3I dllVertex3i;
static PFNGLVERTEX3IV dllVertex3iv;
static PFNGLVERTEX3S dllVertex3s;
static PFNGLVERTEX3SV dllVertex3sv;
static PFNGLVERTEX4D dllVertex4d;
static PFNGLVERTEX4DV dllVertex4dv;
static PFNGLVERTEX4F dllVertex4f;
static PFNGLVERTEX4FV dllVertex4fv;
static PFNGLVERTEX4I dllVertex4i;
static PFNGLVERTEX4IV dllVertex4iv;
static PFNGLVERTEX4S dllVertex4s;
static PFNGLVERTEX4SV dllVertex4sv;
static PFNGLVERTEXPOINTER dllVertexPointer;
static PFNGLVIEWPORT dllViewport;

static const char* BooleanToString(GLboolean b)
{
	if (b == GL_FALSE)
		return "GL_FALSE";
	else if (b == GL_TRUE)
		return "GL_TRUE";
	else
		return "OUT OF RANGE FOR BOOLEAN";
}

static const char* FuncToString(GLenum f)
{
	switch (f)
	{
	case GL_ALWAYS:
		return "GL_ALWAYS";
	case GL_NEVER:
		return "GL_NEVER";
	case GL_LEQUAL:
		return "GL_LEQUAL";
	case GL_LESS:
		return "GL_LESS";
	case GL_EQUAL:
		return "GL_EQUAL";
	case GL_GREATER:
		return "GL_GREATER";
	case GL_GEQUAL:
		return "GL_GEQUAL";
	case GL_NOTEQUAL:
		return "GL_NOTEQUAL";
	default:
		return "!!! UNKNOWN !!!";
	}
}

static const char* PrimToString(GLenum mode)
{
	static char prim[1024];

	if (mode == GL_TRIANGLES)
		strcpy(prim, "GL_TRIANGLES");
	else if (mode == GL_TRIANGLE_STRIP)
		strcpy(prim, "GL_TRIANGLE_STRIP");
	else if (mode == GL_TRIANGLE_FAN)
		strcpy(prim, "GL_TRIANGLE_FAN");
	else if (mode == GL_QUADS)
		strcpy(prim, "GL_QUADS");
	else if (mode == GL_QUAD_STRIP)
		strcpy(prim, "GL_QUAD_STRIP");
	else if (mode == GL_POLYGON)
		strcpy(prim, "GL_POLYGON");
	else if (mode == GL_POINTS)
		strcpy(prim, "GL_POINTS");
	else if (mode == GL_LINES)
		strcpy(prim, "GL_LINES");
	else if (mode == GL_LINE_STRIP)
		strcpy(prim, "GL_LINE_STRIP");
	else if (mode == GL_LINE_LOOP)
		strcpy(prim, "GL_LINE_LOOP");
	else
		sprintf(prim, "0x%x", mode);

	return prim;
}

static const char* CapToString(GLenum cap)
{
	static char buffer[1024];

	switch (cap)
	{
	case GL_TEXTURE_2D:
		return "GL_TEXTURE_2D";
	case GL_BLEND:
		return "GL_BLEND";
	case GL_DEPTH_TEST:
		return "GL_DEPTH_TEST";
	case GL_CULL_FACE:
		return "GL_CULL_FACE";
	case GL_CLIP_PLANE0:
		return "GL_CLIP_PLANE0";
	case GL_COLOR_ARRAY:
		return "GL_COLOR_ARRAY";
	case GL_TEXTURE_COORD_ARRAY:
		return "GL_TEXTURE_COORD_ARRAY";
	case GL_VERTEX_ARRAY:
		return "GL_VERTEX_ARRAY";
	case GL_ALPHA_TEST:
		return "GL_ALPHA_TEST";
	case GL_STENCIL_TEST:
		return "GL_STENCIL_TEST";
	default:
		sprintf(buffer, "0x%x", cap);
	}

	return buffer;
}

static const char* TypeToString(GLenum t)
{
	switch (t)
	{
	case GL_BYTE:
		return "GL_BYTE";
	case GL_UNSIGNED_BYTE:
		return "GL_UNSIGNED_BYTE";
	case GL_SHORT:
		return "GL_SHORT";
	case GL_UNSIGNED_SHORT:
		return "GL_UNSIGNED_SHORT";
	case GL_INT:
		return "GL_INT";
	case GL_UNSIGNED_INT:
		return "GL_UNSIGNED_INT";
	case GL_FLOAT:
		return "GL_FLOAT";
	case GL_DOUBLE:
		return "GL_DOUBLE";
	default:
		return "!!! UNKNOWN !!!";
	}
}

static void GLAPIENTRY logAccum(GLenum op, GLfloat value)
{
	fprintf( glw_state.log_fp, "glAccum\n" );
	dllAccum( op, value );
}

static void GLAPIENTRY logAlphaFunc(GLenum func, GLclampf ref)
{
	fprintf( glw_state.log_fp, "glAlphaFunc( 0x%x, %f )\n", func, ref );
	dllAlphaFunc( func, ref );
}

static GLboolean GLAPIENTRY logAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences)
{
	fprintf( glw_state.log_fp, "glAreTexturesResident\n" );
	return dllAreTexturesResident( n, textures, residences );
}

static void GLAPIENTRY logArrayElement(GLint i)
{
	fprintf( glw_state.log_fp, "glArrayElement\n" );
	dllArrayElement( i );
}

static void GLAPIENTRY logBegin(GLenum mode)
{
	fprintf( glw_state.log_fp, "glBegin( %s )\n", PrimToString( mode ));
	dllBegin( mode );
}

static void GLAPIENTRY logBindTexture(GLenum target, GLuint texture)
{
	fprintf( glw_state.log_fp, "glBindTexture( 0x%x, %u )\n", target, texture );
	dllBindTexture( target, texture );
}

static void GLAPIENTRY logBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
	fprintf( glw_state.log_fp, "glBitmap\n" );
	dllBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
}

static void BlendToName( char *n, GLenum f )
{
	switch ( f )
	{
	case GL_ONE:
		strcpy( n, "GL_ONE" );
		break;
	case GL_ZERO:
		strcpy( n, "GL_ZERO" );
		break;
	case GL_SRC_ALPHA:
		strcpy( n, "GL_SRC_ALPHA" );
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		strcpy( n, "GL_ONE_MINUS_SRC_ALPHA" );
		break;
	case GL_DST_COLOR:
		strcpy( n, "GL_DST_COLOR" );
		break;
	case GL_ONE_MINUS_DST_COLOR:
		strcpy( n, "GL_ONE_MINUS_DST_COLOR" );
		break;
	case GL_DST_ALPHA:
		strcpy( n, "GL_DST_ALPHA" );
		break;
	default:
		sprintf( n, "0x%x", f );
	}
}
static void GLAPIENTRY logBlendFunc(GLenum sfactor, GLenum dfactor)
{
	char sf[128], df[128];

	BlendToName( sf, sfactor );
	BlendToName( df, dfactor );

	fprintf( glw_state.log_fp, "glBlendFunc( %s, %s )\n", sf, df );
	dllBlendFunc( sfactor, dfactor );
}

static void GLAPIENTRY logCallList(GLuint list)
{
	fprintf( glw_state.log_fp, "glCallList( %u )\n", list );
	dllCallList( list );
}

static void GLAPIENTRY logCallLists(GLsizei n, GLenum type, const void *lists)
{
	fprintf( glw_state.log_fp, "glCallLists\n" );
	dllCallLists( n, type, lists );
}

static void GLAPIENTRY logClear(GLbitfield mask)
{
	fprintf( glw_state.log_fp, "glClear( 0x%x = ", mask );

	if ( mask & GL_COLOR_BUFFER_BIT )
		fprintf( glw_state.log_fp, "GL_COLOR_BUFFER_BIT " );
	if ( mask & GL_DEPTH_BUFFER_BIT )
		fprintf( glw_state.log_fp, "GL_DEPTH_BUFFER_BIT " );
	if ( mask & GL_STENCIL_BUFFER_BIT )
		fprintf( glw_state.log_fp, "GL_STENCIL_BUFFER_BIT " );
	if ( mask & GL_ACCUM_BUFFER_BIT )
		fprintf( glw_state.log_fp, "GL_ACCUM_BUFFER_BIT " );

	fprintf( glw_state.log_fp, ")\n" );
	dllClear( mask );
}

static void GLAPIENTRY logClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	fprintf( glw_state.log_fp, "glClearAccum\n" );
	dllClearAccum( red, green, blue, alpha );
}

static void GLAPIENTRY logClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	fprintf( glw_state.log_fp, "glClearColor\n" );
	dllClearColor( red, green, blue, alpha );
}

static void GLAPIENTRY logClearDepth(GLclampd depth)
{
	fprintf( glw_state.log_fp, "glClearDepth( %f )\n", ( float ) depth );
	dllClearDepth( depth );
}

static void GLAPIENTRY logClearIndex(GLfloat c)
{
	fprintf( glw_state.log_fp, "glClearIndex\n" );
	dllClearIndex( c );
}

static void GLAPIENTRY logClearStencil(GLint s)
{
	fprintf( glw_state.log_fp, "glClearStencil( %d )\n", s );
	dllClearStencil( s );
}

static void GLAPIENTRY logClipPlane(GLenum plane, const GLdouble *equation)
{
	fprintf( glw_state.log_fp, "glClipPlane\n" );
	dllClipPlane( plane, equation );
}

static void GLAPIENTRY logColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
	fprintf( glw_state.log_fp, "glColor3b\n" );
	dllColor3b( red, green, blue );
}

static void GLAPIENTRY logColor3bv(const GLbyte *v)
{
	fprintf( glw_state.log_fp, "glColor3bv\n" );
	dllColor3bv( v );
}

static void GLAPIENTRY logColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
	fprintf( glw_state.log_fp, "glColor3d\n" );
	dllColor3d( red, green, blue );
}

static void GLAPIENTRY logColor3dv(const GLdouble *v)
{
	fprintf( glw_state.log_fp, "glColor3dv\n" );
	dllColor3dv( v );
}

static void GLAPIENTRY logColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
	fprintf( glw_state.log_fp, "glColor3f\n" );
	dllColor3f( red, green, blue );
}

static void GLAPIENTRY logColor3fv(const GLfloat *v)
{
	fprintf( glw_state.log_fp, "glColor3fv\n" );
	dllColor3fv( v );
}

static void GLAPIENTRY logColor3i(GLint red, GLint green, GLint blue)
{
	fprintf( glw_state.log_fp, "glColor3i\n" );
	dllColor3i( red, green, blue );
}

static void GLAPIENTRY logColor3iv(const GLint *v)
{
	fprintf( glw_state.log_fp, "glColor3iv\n" );
	dllColor3iv( v );
}

static void GLAPIENTRY logColor3s(GLshort red, GLshort green, GLshort blue)
{
	fprintf( glw_state.log_fp, "glColor3s\n" );
	dllColor3s( red, green, blue );
}

static void GLAPIENTRY logColor3sv(const GLshort *v)
{
	fprintf( glw_state.log_fp, "glColor3sv\n" );
	dllColor3sv( v );
}

static void GLAPIENTRY logColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
	fprintf( glw_state.log_fp, "glColor3ub\n" );
	dllColor3ub( red, green, blue );
}

static void GLAPIENTRY logColor3ubv(const GLubyte *v)
{
	fprintf( glw_state.log_fp, "glColor3ubv\n" );
	dllColor3ubv( v );
}

#define SIG( x ) fprintf( glw_state.log_fp, x "\n" )

static void GLAPIENTRY logColor3ui(GLuint red, GLuint green, GLuint blue)
{
	SIG( "glColor3ui" );
	dllColor3ui( red, green, blue );
}

static void GLAPIENTRY logColor3uiv(const GLuint *v)
{
	SIG( "glColor3uiv" );
	dllColor3uiv( v );
}

static void GLAPIENTRY logColor3us(GLushort red, GLushort green, GLushort blue)
{
	SIG( "glColor3us" );
	dllColor3us( red, green, blue );
}

static void GLAPIENTRY logColor3usv(const GLushort *v)
{
	SIG( "glColor3usv" );
	dllColor3usv( v );
}

static void GLAPIENTRY logColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
	SIG( "glColor4b" );
	dllColor4b( red, green, blue, alpha );
}

static void GLAPIENTRY logColor4bv(const GLbyte *v)
{
	SIG( "glColor4bv" );
	dllColor4bv( v );
}

static void GLAPIENTRY logColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	SIG( "glColor4d" );
	dllColor4d( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4dv(const GLdouble *v)
{
	SIG( "glColor4dv" );
	dllColor4dv( v );
}
static void GLAPIENTRY logColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	fprintf( glw_state.log_fp, "glColor4f( %f,%f,%f,%f )\n", red, green, blue, alpha );
	dllColor4f( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4fv(const GLfloat *v)
{
	fprintf( glw_state.log_fp, "glColor4fv( %f,%f,%f,%f )\n", v[0], v[1], v[2], v[3] );
	dllColor4fv( v );
}
static void GLAPIENTRY logColor4i(GLint red, GLint green, GLint blue, GLint alpha)
{
	SIG( "glColor4i" );
	dllColor4i( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4iv(const GLint *v)
{
	SIG( "glColor4iv" );
	dllColor4iv( v );
}
static void GLAPIENTRY logColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
	SIG( "glColor4s" );
	dllColor4s( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4sv(const GLshort *v)
{
	SIG( "glColor4sv" );
	dllColor4sv( v );
}
static void GLAPIENTRY logColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	SIG( "glColor4b" );
	dllColor4b( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4ubv(const GLubyte *v)
{
	SIG( "glColor4ubv" );
	dllColor4ubv( v );
}
static void GLAPIENTRY logColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
	SIG( "glColor4ui" );
	dllColor4ui( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4uiv(const GLuint *v)
{
	SIG( "glColor4uiv" );
	dllColor4uiv( v );
}
static void GLAPIENTRY logColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
	SIG( "glColor4us" );
	dllColor4us( red, green, blue, alpha );
}
static void GLAPIENTRY logColor4usv(const GLushort *v)
{
	SIG( "glColor4usv" );
	dllColor4usv( v );
}
static void GLAPIENTRY logColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	SIG( "glColorMask" );
	dllColorMask( red, green, blue, alpha );
}
static void GLAPIENTRY logColorMaterial(GLenum face, GLenum mode)
{
	SIG( "glColorMaterial" );
	dllColorMaterial( face, mode );
}

static void GLAPIENTRY logColorPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	fprintf( glw_state.log_fp, "glColorPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
	dllColorPointer( size, type, stride, pointer );
}

static void GLAPIENTRY logCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
	SIG( "glCopyPixels" );
	dllCopyPixels( x, y, width, height, type );
}

static void GLAPIENTRY logCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
	SIG( "glCopyTexImage1D" );
	dllCopyTexImage1D( target, level, internalFormat, x, y, width, border );
}

static void GLAPIENTRY logCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	SIG( "glCopyTexImage2D" );
	dllCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
}

static void GLAPIENTRY logCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
	SIG( "glCopyTexSubImage1D" );
	dllCopyTexSubImage1D( target, level, xoffset, x, y, width );
}

static void GLAPIENTRY logCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	SIG( "glCopyTexSubImage2D" );
	dllCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
}

static void GLAPIENTRY logCullFace(GLenum mode)
{
	fprintf( glw_state.log_fp, "glCullFace( %s )\n", ( mode == GL_FRONT ) ? "GL_FRONT" : "GL_BACK" );
	dllCullFace( mode );
}

static void GLAPIENTRY logDeleteLists(GLuint list, GLsizei range)
{
	SIG( "glDeleteLists" );
	dllDeleteLists( list, range );
}

static void GLAPIENTRY logDeleteTextures(GLsizei n, const GLuint *textures)
{
	SIG( "glDeleteTextures" );
	dllDeleteTextures( n, textures );
}

static void GLAPIENTRY logDepthFunc(GLenum func)
{
	fprintf( glw_state.log_fp, "glDepthFunc( %s )\n", FuncToString( func ) );
	dllDepthFunc( func );
}

static void GLAPIENTRY logDepthMask(GLboolean flag)
{
	fprintf( glw_state.log_fp, "glDepthMask( %s )\n", BooleanToString( flag ) );
	dllDepthMask( flag );
}

static void GLAPIENTRY logDepthRange(GLclampd zNear, GLclampd zFar)
{
	fprintf( glw_state.log_fp, "glDepthRange( %f, %f )\n", ( float ) zNear, ( float ) zFar );
	dllDepthRange( zNear, zFar );
}

static void GLAPIENTRY logDisable(GLenum cap)
{
	fprintf( glw_state.log_fp, "glDisable( %s )\n", CapToString( cap ) );
	dllDisable( cap );
}

static void GLAPIENTRY logDisableClientState(GLenum array)
{
	fprintf( glw_state.log_fp, "glDisableClientState( %s )\n", CapToString( array ) );
	dllDisableClientState( array );
}

static void GLAPIENTRY logDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	SIG( "glDrawArrays" );
	dllDrawArrays( mode, first, count );
}

static void GLAPIENTRY logDrawBuffer(GLenum mode)
{
	SIG( "glDrawBuffer" );
	dllDrawBuffer( mode );
}

static void GLAPIENTRY logDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	fprintf( glw_state.log_fp, "glDrawElements( %s, %d, %s, MEM )\n", PrimToString( mode ), count, TypeToString( type ) );
	dllDrawElements( mode, count, type, indices );
}

static void GLAPIENTRY logDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
	SIG( "glDrawPixels" );
	dllDrawPixels( width, height, format, type, pixels );
}

static void GLAPIENTRY logEdgeFlag(GLboolean flag)
{
	SIG( "glEdgeFlag" );
	dllEdgeFlag( flag );
}

static void GLAPIENTRY logEdgeFlagPointer(GLsizei stride, const void *pointer)
{
	SIG( "glEdgeFlagPointer" );
	dllEdgeFlagPointer( stride, pointer );
}

static void GLAPIENTRY logEdgeFlagv(const GLboolean *flag)
{
	SIG( "glEdgeFlagv" );
	dllEdgeFlagv( flag );
}

static void GLAPIENTRY logEnable(GLenum cap)
{
	fprintf( glw_state.log_fp, "glEnable( %s )\n", CapToString( cap ) );
	dllEnable( cap );
}

static void GLAPIENTRY logEnableClientState(GLenum array)
{
	fprintf( glw_state.log_fp, "glEnableClientState( %s )\n", CapToString( array ) );
	dllEnableClientState( array );
}

static void GLAPIENTRY logEnd(void)
{
	SIG( "glEnd" );
	dllEnd();
}

static void GLAPIENTRY logEndList(void)
{
	SIG( "glEndList" );
	dllEndList();
}

static void GLAPIENTRY logEvalCoord1d(GLdouble u)
{
	SIG( "glEvalCoord1d" );
	dllEvalCoord1d( u );
}

static void GLAPIENTRY logEvalCoord1dv(const GLdouble *u)
{
	SIG( "glEvalCoord1dv" );
	dllEvalCoord1dv( u );
}

static void GLAPIENTRY logEvalCoord1f(GLfloat u)
{
	SIG( "glEvalCoord1f" );
	dllEvalCoord1f( u );
}

static void GLAPIENTRY logEvalCoord1fv(const GLfloat *u)
{
	SIG( "glEvalCoord1fv" );
	dllEvalCoord1fv( u );
}
static void GLAPIENTRY logEvalCoord2d(GLdouble u, GLdouble v)
{
	SIG( "glEvalCoord2d" );
	dllEvalCoord2d( u, v );
}
static void GLAPIENTRY logEvalCoord2dv(const GLdouble *u)
{
	SIG( "glEvalCoord2dv" );
	dllEvalCoord2dv( u );
}
static void GLAPIENTRY logEvalCoord2f(GLfloat u, GLfloat v)
{
	SIG( "glEvalCoord2f" );
	dllEvalCoord2f( u, v );
}
static void GLAPIENTRY logEvalCoord2fv(const GLfloat *u)
{
	SIG( "glEvalCoord2fv" );
	dllEvalCoord2fv( u );
}

static void GLAPIENTRY logEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
	SIG( "glEvalMesh1" );
	dllEvalMesh1( mode, i1, i2 );
}
static void GLAPIENTRY logEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
	SIG( "glEvalMesh2" );
	dllEvalMesh2( mode, i1, i2, j1, j2 );
}
static void GLAPIENTRY logEvalPoint1(GLint i)
{
	SIG( "glEvalPoint1" );
	dllEvalPoint1( i );
}
static void GLAPIENTRY logEvalPoint2(GLint i, GLint j)
{
	SIG( "glEvalPoint2" );
	dllEvalPoint2( i, j );
}

static void GLAPIENTRY logFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer)
{
	SIG( "glFeedbackBuffer" );
	dllFeedbackBuffer( size, type, buffer );
}

static void GLAPIENTRY logFinish(void)
{
	SIG( "glFinish" );
	dllFinish();
}

static void GLAPIENTRY logFlush(void)
{
	SIG( "glFlush" );
	dllFlush();
}

static void GLAPIENTRY logFogf(GLenum pname, GLfloat param)
{
	SIG( "glFogf" );
	dllFogf( pname, param );
}

static void GLAPIENTRY logFogfv(GLenum pname, const GLfloat *params)
{
	SIG( "glFogfv" );
	dllFogfv( pname, params );
}

static void GLAPIENTRY logFogi(GLenum pname, GLint param)
{
	SIG( "glFogi" );
	dllFogi( pname, param );
}

static void GLAPIENTRY logFogiv(GLenum pname, const GLint *params)
{
	SIG( "glFogiv" );
	dllFogiv( pname, params );
}

static void GLAPIENTRY logFrontFace(GLenum mode)
{
	SIG( "glFrontFace" );
	dllFrontFace( mode );
}

static void GLAPIENTRY logFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	SIG( "glFrustum" );
	dllFrustum( left, right, bottom, top, zNear, zFar );
}

static GLuint GLAPIENTRY logGenLists(GLsizei range)
{
	SIG( "glGenLists" );
	return dllGenLists( range );
}

static void GLAPIENTRY logGenTextures(GLsizei n, GLuint *textures)
{
	SIG( "glGenTextures" );
	dllGenTextures( n, textures );
}

static void GLAPIENTRY logGetBooleanv(GLenum pname, GLboolean *params)
{
	SIG( "glGetBooleanv" );
	dllGetBooleanv( pname, params );
}

static void GLAPIENTRY logGetClipPlane(GLenum plane, GLdouble *equation)
{
	SIG( "glGetClipPlane" );
	dllGetClipPlane( plane, equation );
}

static void GLAPIENTRY logGetDoublev(GLenum pname, GLdouble *params)
{
	SIG( "glGetDoublev" );
	dllGetDoublev( pname, params );
}

static GLenum GLAPIENTRY logGetError(void)
{
	SIG( "glGetError" );
	return dllGetError();
}

static void GLAPIENTRY logGetFloatv(GLenum pname, GLfloat *params)
{
	SIG( "glGetFloatv" );
	dllGetFloatv( pname, params );
}

static void GLAPIENTRY logGetIntegerv(GLenum pname, GLint *params)
{
	SIG( "glGetIntegerv" );
	dllGetIntegerv( pname, params );
}

static void GLAPIENTRY logGetLightfv(GLenum light, GLenum pname, GLfloat *params)
{
	SIG( "glGetLightfv" );
	dllGetLightfv( light, pname, params );
}

static void GLAPIENTRY logGetLightiv(GLenum light, GLenum pname, GLint *params)
{
	SIG( "glGetLightiv" );
	dllGetLightiv( light, pname, params );
}

static void GLAPIENTRY logGetMapdv(GLenum target, GLenum query, GLdouble *v)
{
	SIG( "glGetMapdv" );
	dllGetMapdv( target, query, v );
}

static void GLAPIENTRY logGetMapfv(GLenum target, GLenum query, GLfloat *v)
{
	SIG( "glGetMapfv" );
	dllGetMapfv( target, query, v );
}

static void GLAPIENTRY logGetMapiv(GLenum target, GLenum query, GLint *v)
{
	SIG( "glGetMapiv" );
	dllGetMapiv( target, query, v );
}

static void GLAPIENTRY logGetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{
	SIG( "glGetMaterialfv" );
	dllGetMaterialfv( face, pname, params );
}

static void GLAPIENTRY logGetMaterialiv(GLenum face, GLenum pname, GLint *params)
{
	SIG( "glGetMaterialiv" );
	dllGetMaterialiv( face, pname, params );
}

static void GLAPIENTRY logGetPixelMapfv(GLenum map, GLfloat *values)
{
	SIG( "glGetPixelMapfv" );
	dllGetPixelMapfv( map, values );
}

static void GLAPIENTRY logGetPixelMapuiv(GLenum map, GLuint *values)
{
	SIG( "glGetPixelMapuiv" );
	dllGetPixelMapuiv( map, values );
}

static void GLAPIENTRY logGetPixelMapusv(GLenum map, GLushort *values)
{
	SIG( "glGetPixelMapusv" );
	dllGetPixelMapusv( map, values );
}

static void GLAPIENTRY logGetPointerv(GLenum pname, GLvoid* *params)
{
	SIG( "glGetPointerv" );
	dllGetPointerv( pname, params );
}

static void GLAPIENTRY logGetPolygonStipple(GLubyte *mask)
{
	SIG( "glGetPolygonStipple" );
	dllGetPolygonStipple( mask );
}

static const GLubyte * GLAPIENTRY logGetString(GLenum name)
{
	SIG( "glGetString" );
	return dllGetString( name );
}

static void GLAPIENTRY logGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params)
{
	SIG( "glGetTexEnvfv" );
	dllGetTexEnvfv( target, pname, params );
}

static void GLAPIENTRY logGetTexEnviv(GLenum target, GLenum pname, GLint *params)
{
	SIG( "glGetTexEnviv" );
	dllGetTexEnviv( target, pname, params );
}

static void GLAPIENTRY logGetTexGendv(GLenum coord, GLenum pname, GLdouble *params)
{
	SIG( "glGetTexGendv" );
	dllGetTexGendv( coord, pname, params );
}

static void GLAPIENTRY logGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params)
{
	SIG( "glGetTexGenfv" );
	dllGetTexGenfv( coord, pname, params );
}

static void GLAPIENTRY logGetTexGeniv(GLenum coord, GLenum pname, GLint *params)
{
	SIG( "glGetTexGeniv" );
	dllGetTexGeniv( coord, pname, params );
}

static void GLAPIENTRY logGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels)
{
	SIG( "glGetTexImage" );
	dllGetTexImage( target, level, format, type, pixels );
}
static void GLAPIENTRY logGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params )
{
	SIG( "glGetTexLevelParameterfv" );
	dllGetTexLevelParameterfv( target, level, pname, params );
}

static void GLAPIENTRY logGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
	SIG( "glGetTexLevelParameteriv" );
	dllGetTexLevelParameteriv( target, level, pname, params );
}

static void GLAPIENTRY logGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
	SIG( "glGetTexParameterfv" );
	dllGetTexParameterfv( target, pname, params );
}

static void GLAPIENTRY logGetTexParameteriv(GLenum target, GLenum pname, GLint *params)
{
	SIG( "glGetTexParameteriv" );
	dllGetTexParameteriv( target, pname, params );
}

static void GLAPIENTRY logHint(GLenum target, GLenum mode)
{
	fprintf( glw_state.log_fp, "glHint( 0x%x, 0x%x )\n", target, mode );
	dllHint( target, mode );
}

static void GLAPIENTRY logIndexMask(GLuint mask)
{
	SIG( "glIndexMask" );
	dllIndexMask( mask );
}

static void GLAPIENTRY logIndexPointer(GLenum type, GLsizei stride, const void *pointer)
{
	SIG( "glIndexPointer" );
	dllIndexPointer( type, stride, pointer );
}

static void GLAPIENTRY logIndexd(GLdouble c)
{
	SIG( "glIndexd" );
	dllIndexd( c );
}

static void GLAPIENTRY logIndexdv(const GLdouble *c)
{
	SIG( "glIndexdv" );
	dllIndexdv( c );
}

static void GLAPIENTRY logIndexf(GLfloat c)
{
	SIG( "glIndexf" );
	dllIndexf( c );
}

static void GLAPIENTRY logIndexfv(const GLfloat *c)
{
	SIG( "glIndexfv" );
	dllIndexfv( c );
}

static void GLAPIENTRY logIndexi(GLint c)
{
	SIG( "glIndexi" );
	dllIndexi( c );
}

static void GLAPIENTRY logIndexiv(const GLint *c)
{
	SIG( "glIndexiv" );
	dllIndexiv( c );
}

static void GLAPIENTRY logIndexs(GLshort c)
{
	SIG( "glIndexs" );
	dllIndexs( c );
}

static void GLAPIENTRY logIndexsv(const GLshort *c)
{
	SIG( "glIndexsv" );
	dllIndexsv( c );
}

static void GLAPIENTRY logIndexub(GLubyte c)
{
	SIG( "glIndexub" );
	dllIndexub( c );
}

static void GLAPIENTRY logIndexubv(const GLubyte *c)
{
	SIG( "glIndexubv" );
	dllIndexubv( c );
}

static void GLAPIENTRY logInitNames(void)
{
	SIG( "glInitNames" );
	dllInitNames();
}

static void GLAPIENTRY logInterleavedArrays(GLenum format, GLsizei stride, const void *pointer)
{
	SIG( "glInterleavedArrays" );
	dllInterleavedArrays( format, stride, pointer );
}

static GLboolean GLAPIENTRY logIsEnabled(GLenum cap)
{
	SIG( "glIsEnabled" );
	return dllIsEnabled( cap );
}
static GLboolean GLAPIENTRY logIsList(GLuint list)
{
	SIG( "glIsList" );
	return dllIsList( list );
}
static GLboolean GLAPIENTRY logIsTexture(GLuint texture)
{
	SIG( "glIsTexture" );
	return dllIsTexture( texture );
}

static void GLAPIENTRY logLightModelf(GLenum pname, GLfloat param)
{
	SIG( "glLightModelf" );
	dllLightModelf( pname, param );
}

static void GLAPIENTRY logLightModelfv(GLenum pname, const GLfloat *params)
{
	SIG( "glLightModelfv" );
	dllLightModelfv( pname, params );
}

static void GLAPIENTRY logLightModeli(GLenum pname, GLint param)
{
	SIG( "glLightModeli" );
	dllLightModeli( pname, param );

}

static void GLAPIENTRY logLightModeliv(GLenum pname, const GLint *params)
{
	SIG( "glLightModeliv" );
	dllLightModeliv( pname, params );
}

static void GLAPIENTRY logLightf(GLenum light, GLenum pname, GLfloat param)
{
	SIG( "glLightf" );
	dllLightf( light, pname, param );
}

static void GLAPIENTRY logLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
	SIG( "glLightfv" );
	dllLightfv( light, pname, params );
}

static void GLAPIENTRY logLighti(GLenum light, GLenum pname, GLint param)
{
	SIG( "glLighti" );
	dllLighti( light, pname, param );
}

static void GLAPIENTRY logLightiv(GLenum light, GLenum pname, const GLint *params)
{
	SIG( "glLightiv" );
	dllLightiv( light, pname, params );
}

static void GLAPIENTRY logLineStipple(GLint factor, GLushort pattern)
{
	SIG( "glLineStipple" );
	dllLineStipple( factor, pattern );
}

static void GLAPIENTRY logLineWidth(GLfloat width)
{
	SIG( "glLineWidth" );
	dllLineWidth( width );
}

static void GLAPIENTRY logListBase(GLuint base)
{
	SIG( "glListBase" );
	dllListBase( base );
}

static void GLAPIENTRY logLoadIdentity(void)
{
	SIG( "glLoadIdentity" );
	dllLoadIdentity();
}

static void GLAPIENTRY logLoadMatrixd(const GLdouble *m)
{
	SIG( "glLoadMatrixd" );
	dllLoadMatrixd( m );
}

static void GLAPIENTRY logLoadMatrixf(const GLfloat *m)
{
	SIG( "glLoadMatrixf" );
	dllLoadMatrixf( m );
}

static void GLAPIENTRY logLoadName(GLuint name)
{
	SIG( "glLoadName" );
	dllLoadName( name );
}

static void GLAPIENTRY logLogicOp(GLenum opcode)
{
	SIG( "glLogicOp" );
	dllLogicOp( opcode );
}

static void GLAPIENTRY logMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{
	SIG( "glMap1d" );
	dllMap1d( target, u1, u2, stride, order, points );
}

static void GLAPIENTRY logMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
	SIG( "glMap1f" );
	dllMap1f( target, u1, u2, stride, order, points );
}

static void GLAPIENTRY logMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{
	SIG( "glMap2d" );
	dllMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

static void GLAPIENTRY logMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{
	SIG( "glMap2f" );
	dllMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

static void GLAPIENTRY logMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
	SIG( "glMapGrid1d" );
	dllMapGrid1d( un, u1, u2 );
}

static void GLAPIENTRY logMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
	SIG( "glMapGrid1f" );
	dllMapGrid1f( un, u1, u2 );
}

static void GLAPIENTRY logMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
	SIG( "glMapGrid2d" );
	dllMapGrid2d( un, u1, u2, vn, v1, v2 );
}
static void GLAPIENTRY logMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
	SIG( "glMapGrid2f" );
	dllMapGrid2f( un, u1, u2, vn, v1, v2 );
}
static void GLAPIENTRY logMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	SIG( "glMaterialf" );
	dllMaterialf( face, pname, param );
}
static void GLAPIENTRY logMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
	SIG( "glMaterialfv" );
	dllMaterialfv( face, pname, params );
}

static void GLAPIENTRY logMateriali(GLenum face, GLenum pname, GLint param)
{
	SIG( "glMateriali" );
	dllMateriali( face, pname, param );
}

static void GLAPIENTRY logMaterialiv(GLenum face, GLenum pname, const GLint *params)
{
	SIG( "glMaterialiv" );
	dllMaterialiv( face, pname, params );
}

static void GLAPIENTRY logMatrixMode(GLenum mode)
{
	SIG( "glMatrixMode" );
	dllMatrixMode( mode );
}

static void GLAPIENTRY logMultMatrixd(const GLdouble *m)
{
	SIG( "glMultMatrixd" );
	dllMultMatrixd( m );
}

static void GLAPIENTRY logMultMatrixf(const GLfloat *m)
{
	SIG( "glMultMatrixf" );
	dllMultMatrixf( m );
}

static void GLAPIENTRY logNewList(GLuint list, GLenum mode)
{
	SIG( "glNewList" );
	dllNewList( list, mode );
}

static void GLAPIENTRY logNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
	SIG ("glNormal3b" );
	dllNormal3b( nx, ny, nz );
}

static void GLAPIENTRY logNormal3bv(const GLbyte *v)
{
	SIG( "glNormal3bv" );
	dllNormal3bv( v );
}

static void GLAPIENTRY logNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
	SIG( "glNormal3d" );
	dllNormal3d( nx, ny, nz );
}

static void GLAPIENTRY logNormal3dv(const GLdouble *v)
{
	SIG( "glNormal3dv" );
	dllNormal3dv( v );
}

static void GLAPIENTRY logNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	SIG( "glNormal3f" );
	dllNormal3f( nx, ny, nz );
}

static void GLAPIENTRY logNormal3fv(const GLfloat *v)
{
	SIG( "glNormal3fv" );
	dllNormal3fv( v );
}
static void GLAPIENTRY logNormal3i(GLint nx, GLint ny, GLint nz)
{
	SIG( "glNormal3i" );
	dllNormal3i( nx, ny, nz );
}
static void GLAPIENTRY logNormal3iv(const GLint *v)
{
	SIG( "glNormal3iv" );
	dllNormal3iv( v );
}
static void GLAPIENTRY logNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
	SIG( "glNormal3s" );
	dllNormal3s( nx, ny, nz );
}
static void GLAPIENTRY logNormal3sv(const GLshort *v)
{
	SIG( "glNormal3sv" );
	dllNormal3sv( v );
}
static void GLAPIENTRY logNormalPointer(GLenum type, GLsizei stride, const void *pointer)
{
	SIG( "glNormalPointer" );
	dllNormalPointer( type, stride, pointer );
}
static void GLAPIENTRY logOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	SIG( "glOrtho" );
	dllOrtho( left, right, bottom, top, zNear, zFar );
}

static void GLAPIENTRY logPassThrough(GLfloat token)
{
	SIG( "glPassThrough" );
	dllPassThrough( token );
}

static void GLAPIENTRY logPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values)
{
	SIG( "glPixelMapfv" );
	dllPixelMapfv( map, mapsize, values );
}

static void GLAPIENTRY logPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values)
{
	SIG( "glPixelMapuiv" );
	dllPixelMapuiv( map, mapsize, values );
}

static void GLAPIENTRY logPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values)
{
	SIG( "glPixelMapusv" );
	dllPixelMapusv( map, mapsize, values );
}
static void GLAPIENTRY logPixelStoref(GLenum pname, GLfloat param)
{
	SIG( "glPixelStoref" );
	dllPixelStoref( pname, param );
}
static void GLAPIENTRY logPixelStorei(GLenum pname, GLint param)
{
	SIG( "glPixelStorei" );
	dllPixelStorei( pname, param );
}
static void GLAPIENTRY logPixelTransferf(GLenum pname, GLfloat param)
{
	SIG( "glPixelTransferf" );
	dllPixelTransferf( pname, param );
}

static void GLAPIENTRY logPixelTransferi(GLenum pname, GLint param)
{
	SIG( "glPixelTransferi" );
	dllPixelTransferi( pname, param );
}

static void GLAPIENTRY logPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
	SIG( "glPixelZoom" );
	dllPixelZoom( xfactor, yfactor );
}

static void GLAPIENTRY logPointSize(GLfloat size)
{
	SIG( "glPointSize" );
	dllPointSize( size );
}

static void GLAPIENTRY logPolygonMode(GLenum face, GLenum mode)
{
	fprintf( glw_state.log_fp, "glPolygonMode( 0x%x, 0x%x )\n", face, mode );
	dllPolygonMode( face, mode );
}

static void GLAPIENTRY logPolygonOffset(GLfloat factor, GLfloat units)
{
	SIG( "glPolygonOffset" );
	dllPolygonOffset( factor, units );
}
static void GLAPIENTRY logPolygonStipple(const GLubyte *mask )
{
	SIG( "glPolygonStipple" );
	dllPolygonStipple( mask );
}
static void GLAPIENTRY logPopAttrib(void)
{
	SIG( "glPopAttrib" );
	dllPopAttrib();
}

static void GLAPIENTRY logPopClientAttrib(void)
{
	SIG( "glPopClientAttrib" );
	dllPopClientAttrib();
}

static void GLAPIENTRY logPopMatrix(void)
{
	SIG( "glPopMatrix" );
	dllPopMatrix();
}

static void GLAPIENTRY logPopName(void)
{
	SIG( "glPopName" );
	dllPopName();
}

static void GLAPIENTRY logPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
	SIG( "glPrioritizeTextures" );
	dllPrioritizeTextures( n, textures, priorities );
}

static void GLAPIENTRY logPushAttrib(GLbitfield mask)
{
	SIG( "glPushAttrib" );
	dllPushAttrib( mask );
}

static void GLAPIENTRY logPushClientAttrib(GLbitfield mask)
{
	SIG( "glPushClientAttrib" );
	dllPushClientAttrib( mask );
}

static void GLAPIENTRY logPushMatrix(void)
{
	SIG( "glPushMatrix" );
	dllPushMatrix();
}

static void GLAPIENTRY logPushName(GLuint name)
{
	SIG( "glPushName" );
	dllPushName( name );
}

static void GLAPIENTRY logRasterPos2d(GLdouble x, GLdouble y)
{
	SIG ("glRasterPot2d" );
	dllRasterPos2d( x, y );
}

static void GLAPIENTRY logRasterPos2dv(const GLdouble *v)
{
	SIG( "glRasterPos2dv" );
	dllRasterPos2dv( v );
}

static void GLAPIENTRY logRasterPos2f(GLfloat x, GLfloat y)
{
	SIG( "glRasterPos2f" );
	dllRasterPos2f( x, y );
}
static void GLAPIENTRY logRasterPos2fv(const GLfloat *v)
{
	SIG( "glRasterPos2dv" );
	dllRasterPos2fv( v );
}
static void GLAPIENTRY logRasterPos2i(GLint x, GLint y)
{
	SIG( "glRasterPos2if" );
	dllRasterPos2i( x, y );
}
static void GLAPIENTRY logRasterPos2iv(const GLint *v)
{
	SIG( "glRasterPos2iv" );
	dllRasterPos2iv( v );
}
static void GLAPIENTRY logRasterPos2s(GLshort x, GLshort y)
{
	SIG( "glRasterPos2s" );
	dllRasterPos2s( x, y );
}
static void GLAPIENTRY logRasterPos2sv(const GLshort *v)
{
	SIG( "glRasterPos2sv" );
	dllRasterPos2sv( v );
}
static void GLAPIENTRY logRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
	SIG( "glRasterPos3d" );
	dllRasterPos3d( x, y, z );
}
static void GLAPIENTRY logRasterPos3dv(const GLdouble *v)
{
	SIG( "glRasterPos3dv" );
	dllRasterPos3dv( v );
}
static void GLAPIENTRY logRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
	SIG( "glRasterPos3f" );
	dllRasterPos3f( x, y, z );
}
static void GLAPIENTRY logRasterPos3fv(const GLfloat *v)
{
	SIG( "glRasterPos3fv" );
	dllRasterPos3fv( v );
}
static void GLAPIENTRY logRasterPos3i(GLint x, GLint y, GLint z)
{
	SIG( "glRasterPos3i" );
	dllRasterPos3i( x, y, z );
}
static void GLAPIENTRY logRasterPos3iv(const GLint *v)
{
	SIG( "glRasterPos3iv" );
	dllRasterPos3iv( v );
}
static void GLAPIENTRY logRasterPos3s(GLshort x, GLshort y, GLshort z)
{
	SIG( "glRasterPos3s" );
	dllRasterPos3s( x, y, z );
}
static void GLAPIENTRY logRasterPos3sv(const GLshort *v)
{
	SIG( "glRasterPos3sv" );
	dllRasterPos3sv( v );
}
static void GLAPIENTRY logRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	SIG( "glRasterPos4d" );
	dllRasterPos4d( x, y, z, w );
}
static void GLAPIENTRY logRasterPos4dv(const GLdouble *v)
{
	SIG( "glRasterPos4dv" );
	dllRasterPos4dv( v );
}
static void GLAPIENTRY logRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	SIG( "glRasterPos4f" );
	dllRasterPos4f( x, y, z, w );
}
static void GLAPIENTRY logRasterPos4fv(const GLfloat *v)
{
	SIG( "glRasterPos4fv" );
	dllRasterPos4fv( v );
}
static void GLAPIENTRY logRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
	SIG( "glRasterPos4i" );
	dllRasterPos4i( x, y, z, w );
}
static void GLAPIENTRY logRasterPos4iv(const GLint *v)
{
	SIG( "glRasterPos4iv" );
	dllRasterPos4iv( v );
}
static void GLAPIENTRY logRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	SIG( "glRasterPos4s" );
	dllRasterPos4s( x, y, z, w );
}
static void GLAPIENTRY logRasterPos4sv(const GLshort *v)
{
	SIG( "glRasterPos4sv" );
	dllRasterPos4sv( v );
}
static void GLAPIENTRY logReadBuffer(GLenum mode)
{
	SIG( "glReadBuffer" );
	dllReadBuffer( mode );
}
static void GLAPIENTRY logReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
	SIG( "glReadPixels" );
	dllReadPixels( x, y, width, height, format, type, pixels );
}

static void GLAPIENTRY logRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
	SIG( "glRectd" );
	dllRectd( x1, y1, x2, y2 );
}

static void GLAPIENTRY logRectdv(const GLdouble *v1, const GLdouble *v2)
{
	SIG( "glRectdv" );
	dllRectdv( v1, v2 );
}

static void GLAPIENTRY logRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	SIG( "glRectf" );
	dllRectf( x1, y1, x2, y2 );
}

static void GLAPIENTRY logRectfv(const GLfloat *v1, const GLfloat *v2)
{
	SIG( "glRectfv" );
	dllRectfv( v1, v2 );
}
static void GLAPIENTRY logRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
	SIG( "glRecti" );
	dllRecti( x1, y1, x2, y2 );
}
static void GLAPIENTRY logRectiv(const GLint *v1, const GLint *v2)
{
	SIG( "glRectiv" );
	dllRectiv( v1, v2 );
}
static void GLAPIENTRY logRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
	SIG( "glRects" );
	dllRects( x1, y1, x2, y2 );
}
static void GLAPIENTRY logRectsv(const GLshort *v1, const GLshort *v2)
{
	SIG( "glRectsv" );
	dllRectsv( v1, v2 );
}
static GLint GLAPIENTRY logRenderMode(GLenum mode)
{
	SIG( "glRenderMode" );
	return dllRenderMode( mode );
}
static void GLAPIENTRY logRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	SIG( "glRotated" );
	dllRotated( angle, x, y, z );
}

static void GLAPIENTRY logRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	SIG( "glRotatef" );
	dllRotatef( angle, x, y, z );
}

static void GLAPIENTRY logScaled(GLdouble x, GLdouble y, GLdouble z)
{
	SIG( "glScaled" );
	dllScaled( x, y, z );
}

static void GLAPIENTRY logScalef(GLfloat x, GLfloat y, GLfloat z)
{
	SIG( "glScalef" );
	dllScalef( x, y, z );
}

static void GLAPIENTRY logScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	fprintf( glw_state.log_fp, "glScissor( %d, %d, %d, %d )\n", x, y, width, height );
	dllScissor( x, y, width, height );
}

static void GLAPIENTRY logSelectBuffer(GLsizei size, GLuint *buffer)
{
	SIG( "glSelectBuffer" );
	dllSelectBuffer( size, buffer );
}

static void GLAPIENTRY logShadeModel(GLenum mode)
{
	SIG( "glShadeModel" );
	dllShadeModel( mode );
}

static void GLAPIENTRY logStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	SIG( "glStencilFunc" );
	dllStencilFunc( func, ref, mask );
}

static void GLAPIENTRY logStencilMask(GLuint mask)
{
	SIG( "glStencilMask" );
	dllStencilMask( mask );
}

static void GLAPIENTRY logStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	SIG( "glStencilOp" );
	dllStencilOp( fail, zfail, zpass );
}

static void GLAPIENTRY logTexCoord1d(GLdouble s)
{
	SIG( "glTexCoord1d" );
	dllTexCoord1d( s );
}

static void GLAPIENTRY logTexCoord1dv(const GLdouble *v)
{
	SIG( "glTexCoord1dv" );
	dllTexCoord1dv( v );
}

static void GLAPIENTRY logTexCoord1f(GLfloat s)
{
	SIG( "glTexCoord1f" );
	dllTexCoord1f( s );
}
static void GLAPIENTRY logTexCoord1fv(const GLfloat *v)
{
	SIG( "glTexCoord1fv" );
	dllTexCoord1fv( v );
}
static void GLAPIENTRY logTexCoord1i(GLint s)
{
	SIG( "glTexCoord1i" );
	dllTexCoord1i( s );
}
static void GLAPIENTRY logTexCoord1iv(const GLint *v)
{
	SIG( "glTexCoord1iv" );
	dllTexCoord1iv( v );
}
static void GLAPIENTRY logTexCoord1s(GLshort s)
{
	SIG( "glTexCoord1s" );
	dllTexCoord1s( s );
}
static void GLAPIENTRY logTexCoord1sv(const GLshort *v)
{
	SIG( "glTexCoord1sv" );
	dllTexCoord1sv( v );
}
static void GLAPIENTRY logTexCoord2d(GLdouble s, GLdouble t)
{
	SIG( "glTexCoord2d" );
	dllTexCoord2d( s, t );
}

static void GLAPIENTRY logTexCoord2dv(const GLdouble *v)
{
	SIG( "glTexCoord2dv" );
	dllTexCoord2dv( v );
}
static void GLAPIENTRY logTexCoord2f(GLfloat s, GLfloat t)
{
	SIG( "glTexCoord2f" );
	dllTexCoord2f( s, t );
}
static void GLAPIENTRY logTexCoord2fv(const GLfloat *v)
{
	SIG( "glTexCoord2fv" );
	dllTexCoord2fv( v );
}
static void GLAPIENTRY logTexCoord2i(GLint s, GLint t)
{
	SIG( "glTexCoord2i" );
	dllTexCoord2i( s, t );
}
static void GLAPIENTRY logTexCoord2iv(const GLint *v)
{
	SIG( "glTexCoord2iv" );
	dllTexCoord2iv( v );
}
static void GLAPIENTRY logTexCoord2s(GLshort s, GLshort t)
{
	SIG( "glTexCoord2s" );
	dllTexCoord2s( s, t );
}
static void GLAPIENTRY logTexCoord2sv(const GLshort *v)
{
	SIG( "glTexCoord2sv" );
	dllTexCoord2sv( v );
}
static void GLAPIENTRY logTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
	SIG( "glTexCoord3d" );
	dllTexCoord3d( s, t, r );
}
static void GLAPIENTRY logTexCoord3dv(const GLdouble *v)
{
	SIG( "glTexCoord3dv" );
	dllTexCoord3dv( v );
}
static void GLAPIENTRY logTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
	SIG( "glTexCoord3f" );
	dllTexCoord3f( s, t, r );
}
static void GLAPIENTRY logTexCoord3fv(const GLfloat *v)
{
	SIG( "glTexCoord3fv" );
	dllTexCoord3fv( v );
}
static void GLAPIENTRY logTexCoord3i(GLint s, GLint t, GLint r)
{
	SIG( "glTexCoord3i" );
	dllTexCoord3i( s, t, r );
}
static void GLAPIENTRY logTexCoord3iv(const GLint *v)
{
	SIG( "glTexCoord3iv" );
	dllTexCoord3iv( v );
}
static void GLAPIENTRY logTexCoord3s(GLshort s, GLshort t, GLshort r)
{
	SIG( "glTexCoord3s" );
	dllTexCoord3s( s, t, r );
}
static void GLAPIENTRY logTexCoord3sv(const GLshort *v)
{
	SIG( "glTexCoord3sv" );
	dllTexCoord3sv( v );
}
static void GLAPIENTRY logTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	SIG( "glTexCoord4d" );
	dllTexCoord4d( s, t, r, q );
}
static void GLAPIENTRY logTexCoord4dv(const GLdouble *v)
{
	SIG( "glTexCoord4dv" );
	dllTexCoord4dv( v );
}
static void GLAPIENTRY logTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	SIG( "glTexCoord4f" );
	dllTexCoord4f( s, t, r, q );
}
static void GLAPIENTRY logTexCoord4fv(const GLfloat *v)
{
	SIG( "glTexCoord4fv" );
	dllTexCoord4fv( v );
}
static void GLAPIENTRY logTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
	SIG( "glTexCoord4i" );
	dllTexCoord4i( s, t, r, q );
}
static void GLAPIENTRY logTexCoord4iv(const GLint *v)
{
	SIG( "glTexCoord4iv" );
	dllTexCoord4iv( v );
}
static void GLAPIENTRY logTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
	SIG( "glTexCoord4s" );
	dllTexCoord4s( s, t, r, q );
}
static void GLAPIENTRY logTexCoord4sv(const GLshort *v)
{
	SIG( "glTexCoord4sv" );
	dllTexCoord4sv( v );
}
static void GLAPIENTRY logTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	fprintf( glw_state.log_fp, "glTexCoordPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
	dllTexCoordPointer( size, type, stride, pointer );
}

static void GLAPIENTRY logTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	fprintf( glw_state.log_fp, "glTexEnvf( 0x%x, 0x%x, %f )\n", target, pname, param );
	dllTexEnvf( target, pname, param );
}

static void GLAPIENTRY logTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
	SIG( "glTexEnvfv" );
	dllTexEnvfv( target, pname, params );
}

static void GLAPIENTRY logTexEnvi(GLenum target, GLenum pname, GLint param)
{
	fprintf( glw_state.log_fp, "glTexEnvi( 0x%x, 0x%x, 0x%x )\n", target, pname, param );
	dllTexEnvi( target, pname, param );
}
static void GLAPIENTRY logTexEnviv(GLenum target, GLenum pname, const GLint *params)
{
	SIG( "glTexEnviv" );
	dllTexEnviv( target, pname, params );
}

static void GLAPIENTRY logTexGend(GLenum coord, GLenum pname, GLdouble param)
{
	SIG( "glTexGend" );
	dllTexGend( coord, pname, param );
}

static void GLAPIENTRY logTexGendv(GLenum coord, GLenum pname, const GLdouble *params)
{
	SIG( "glTexGendv" );
	dllTexGendv( coord, pname, params );
}

static void GLAPIENTRY logTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
	SIG( "glTexGenf" );
	dllTexGenf( coord, pname, param );
}
static void GLAPIENTRY logTexGenfv(GLenum coord, GLenum pname, const GLfloat *params)
{
	SIG( "glTexGenfv" );
	dllTexGenfv( coord, pname, params );
}
static void GLAPIENTRY logTexGeni(GLenum coord, GLenum pname, GLint param)
{
	SIG( "glTexGeni" );
	dllTexGeni( coord, pname, param );
}
static void GLAPIENTRY logTexGeniv(GLenum coord, GLenum pname, const GLint *params)
{
	SIG( "glTexGeniv" );
	dllTexGeniv( coord, pname, params );
}
static void GLAPIENTRY logTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels)
{
	SIG( "glTexImage1D" );
	dllTexImage1D( target, level, internalformat, width, border, format, type, pixels );
}
static void GLAPIENTRY logTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
	SIG( "glTexImage2D" );
	dllTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}

static void GLAPIENTRY logTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	fprintf( glw_state.log_fp, "glTexParameterf( 0x%x, 0x%x, %f )\n", target, pname, param );
	dllTexParameterf( target, pname, param );
}

static void GLAPIENTRY logTexParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
	SIG( "glTexParameterfv" );
	dllTexParameterfv( target, pname, params );
}
static void GLAPIENTRY logTexParameteri(GLenum target, GLenum pname, GLint param)
{
	fprintf( glw_state.log_fp, "glTexParameteri( 0x%x, 0x%x, 0x%x )\n", target, pname, param );
	dllTexParameteri( target, pname, param );
}
static void GLAPIENTRY logTexParameteriv(GLenum target, GLenum pname, const GLint *params)
{
	SIG( "glTexParameteriv" );
	dllTexParameteriv( target, pname, params );
}
static void GLAPIENTRY logTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels)
{
	SIG( "glTexSubImage1D" );
	dllTexSubImage1D( target, level, xoffset, width, format, type, pixels );
}
static void GLAPIENTRY logTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
	SIG( "glTexSubImage2D" );
	dllTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}
static void GLAPIENTRY logTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	SIG( "glTranslated" );
	dllTranslated( x, y, z );
}

static void GLAPIENTRY logTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	SIG( "glTranslatef" );
	dllTranslatef( x, y, z );
}

static void GLAPIENTRY logVertex2d(GLdouble x, GLdouble y)
{
	SIG( "glVertex2d" );
	dllVertex2d( x, y );
}

static void GLAPIENTRY logVertex2dv(const GLdouble *v)
{
	SIG( "glVertex2dv" );
	dllVertex2dv( v );
}
static void GLAPIENTRY logVertex2f(GLfloat x, GLfloat y)
{
	SIG( "glVertex2f" );
	dllVertex2f( x, y );
}
static void GLAPIENTRY logVertex2fv(const GLfloat *v)
{
	SIG( "glVertex2fv" );
	dllVertex2fv( v );
}
static void GLAPIENTRY logVertex2i(GLint x, GLint y)
{
	SIG( "glVertex2i" );
	dllVertex2i( x, y );
}
static void GLAPIENTRY logVertex2iv(const GLint *v)
{
	SIG( "glVertex2iv" );
	dllVertex2iv( v );
}
static void GLAPIENTRY logVertex2s(GLshort x, GLshort y)
{
	SIG( "glVertex2s" );
	dllVertex2s( x, y );
}
static void GLAPIENTRY logVertex2sv(const GLshort *v)
{
	SIG( "glVertex2sv" );
	dllVertex2sv( v );
}
static void GLAPIENTRY logVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
	SIG( "glVertex3d" );
	dllVertex3d( x, y, z );
}
static void GLAPIENTRY logVertex3dv(const GLdouble *v)
{
	SIG( "glVertex3dv" );
	dllVertex3dv( v );
}
static void GLAPIENTRY logVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	SIG( "glVertex3f" );
	dllVertex3f( x, y, z );
}
static void GLAPIENTRY logVertex3fv(const GLfloat *v)
{
	SIG( "glVertex3fv" );
	dllVertex3fv( v );
}
static void GLAPIENTRY logVertex3i(GLint x, GLint y, GLint z)
{
	SIG( "glVertex3i" );
	dllVertex3i( x, y, z );
}
static void GLAPIENTRY logVertex3iv(const GLint *v)
{
	SIG( "glVertex3iv" );
	dllVertex3iv( v );
}
static void GLAPIENTRY logVertex3s(GLshort x, GLshort y, GLshort z)
{
	SIG( "glVertex3s" );
	dllVertex3s( x, y, z );
}
static void GLAPIENTRY logVertex3sv(const GLshort *v)
{
	SIG( "glVertex3sv" );
	dllVertex3sv( v );
}
static void GLAPIENTRY logVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	SIG( "glVertex4d" );
	dllVertex4d( x, y, z, w );
}
static void GLAPIENTRY logVertex4dv(const GLdouble *v)
{
	SIG( "glVertex4dv" );
	dllVertex4dv( v );
}
static void GLAPIENTRY logVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	SIG( "glVertex4f" );
	dllVertex4f( x, y, z, w );
}
static void GLAPIENTRY logVertex4fv(const GLfloat *v)
{
	SIG( "glVertex4fv" );
	dllVertex4fv( v );
}
static void GLAPIENTRY logVertex4i(GLint x, GLint y, GLint z, GLint w)
{
	SIG( "glVertex4i" );
	dllVertex4i( x, y, z, w );
}
static void GLAPIENTRY logVertex4iv(const GLint *v)
{
	SIG( "glVertex4iv" );
	dllVertex4iv( v );
}
static void GLAPIENTRY logVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	SIG( "glVertex4s" );
	dllVertex4s( x, y, z, w );
}
static void GLAPIENTRY logVertex4sv(const GLshort *v)
{
	SIG( "glVertex4sv" );
	dllVertex4sv( v );
}
static void GLAPIENTRY logVertexPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	fprintf( glw_state.log_fp, "glVertexPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
	dllVertexPointer( size, type, stride, pointer );
}
static void GLAPIENTRY logViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	fprintf( glw_state.log_fp, "glViewport( %d, %d, %d, %d )\n", x, y, width, height );
	dllViewport( x, y, width, height );
}

/*
** QGL_Shutdown
**
** Unloads the specified DLL then nulls out all the proc pointers.  This
** is only called during a hard shutdown of the OGL subsystem (e.g. vid_restart).
*/
void QGL_Shutdown(void)
{
	ri.Printf(PRINT_ALL, "...shutting down QGL\n");

	if (glw_state.hinstOpenGL) {
		ri.Printf(PRINT_ALL, "...unloading OpenGL DLL\n");
		SDL_UnloadObject(glw_state.hinstOpenGL);
	}

	glw_state.hinstOpenGL = NULL;

	qglAccum                     = NULL;
	qglAlphaFunc                 = NULL;
	qglAreTexturesResident       = NULL;
	qglArrayElement              = NULL;
	qglBegin                     = NULL;
	qglBindTexture               = NULL;
	qglBitmap                    = NULL;
	qglBlendFunc                 = NULL;
	qglCallList                  = NULL;
	qglCallLists                 = NULL;
	qglClear                     = NULL;
	qglClearAccum                = NULL;
	qglClearColor                = NULL;
	qglClearDepth                = NULL;
	qglClearIndex                = NULL;
	qglClearStencil              = NULL;
	qglClipPlane                 = NULL;
	qglColor3b                   = NULL;
	qglColor3bv                  = NULL;
	qglColor3d                   = NULL;
	qglColor3dv                  = NULL;
	qglColor3f                   = NULL;
	qglColor3fv                  = NULL;
	qglColor3i                   = NULL;
	qglColor3iv                  = NULL;
	qglColor3s                   = NULL;
	qglColor3sv                  = NULL;
	qglColor3ub                  = NULL;
	qglColor3ubv                 = NULL;
	qglColor3ui                  = NULL;
	qglColor3uiv                 = NULL;
	qglColor3us                  = NULL;
	qglColor3usv                 = NULL;
	qglColor4b                   = NULL;
	qglColor4bv                  = NULL;
	qglColor4d                   = NULL;
	qglColor4dv                  = NULL;
	qglColor4f                   = NULL;
	qglColor4fv                  = NULL;
	qglColor4i                   = NULL;
	qglColor4iv                  = NULL;
	qglColor4s                   = NULL;
	qglColor4sv                  = NULL;
	qglColor4ub                  = NULL;
	qglColor4ubv                 = NULL;
	qglColor4ui                  = NULL;
	qglColor4uiv                 = NULL;
	qglColor4us                  = NULL;
	qglColor4usv                 = NULL;
	qglColorMask                 = NULL;
	qglColorMaterial             = NULL;
	qglColorPointer              = NULL;
	qglCopyPixels                = NULL;
	qglCopyTexImage1D            = NULL;
	qglCopyTexImage2D            = NULL;
	qglCopyTexSubImage1D         = NULL;
	qglCopyTexSubImage2D         = NULL;
	qglCullFace                  = NULL;
	qglDeleteLists               = NULL;
	qglDeleteTextures            = NULL;
	qglDepthFunc                 = NULL;
	qglDepthMask                 = NULL;
	qglDepthRange                = NULL;
	qglDisable                   = NULL;
	qglDisableClientState        = NULL;
	qglDrawArrays                = NULL;
	qglDrawBuffer                = NULL;
	qglDrawElements              = NULL;
	qglDrawPixels                = NULL;
	qglEdgeFlag                  = NULL;
	qglEdgeFlagPointer           = NULL;
	qglEdgeFlagv                 = NULL;
	qglEnable                    = NULL;
	qglEnableClientState         = NULL;
	qglEnd                       = NULL;
	qglEndList                   = NULL;
	qglEvalCoord1d               = NULL;
	qglEvalCoord1dv              = NULL;
	qglEvalCoord1f               = NULL;
	qglEvalCoord1fv              = NULL;
	qglEvalCoord2d               = NULL;
	qglEvalCoord2dv              = NULL;
	qglEvalCoord2f               = NULL;
	qglEvalCoord2fv              = NULL;
	qglEvalMesh1                 = NULL;
	qglEvalMesh2                 = NULL;
	qglEvalPoint1                = NULL;
	qglEvalPoint2                = NULL;
	qglFeedbackBuffer            = NULL;
	qglFinish                    = NULL;
	qglFlush                     = NULL;
	qglFogf                      = NULL;
	qglFogfv                     = NULL;
	qglFogi                      = NULL;
	qglFogiv                     = NULL;
	qglFrontFace                 = NULL;
	qglFrustum                   = NULL;
	qglGenLists                  = NULL;
	qglGenTextures               = NULL;
	qglGetBooleanv               = NULL;
	qglGetClipPlane              = NULL;
	qglGetDoublev                = NULL;
	qglGetError                  = NULL;
	qglGetFloatv                 = NULL;
	qglGetIntegerv               = NULL;
	qglGetLightfv                = NULL;
	qglGetLightiv                = NULL;
	qglGetMapdv                  = NULL;
	qglGetMapfv                  = NULL;
	qglGetMapiv                  = NULL;
	qglGetMaterialfv             = NULL;
	qglGetMaterialiv             = NULL;
	qglGetPixelMapfv             = NULL;
	qglGetPixelMapuiv            = NULL;
	qglGetPixelMapusv            = NULL;
	qglGetPointerv               = NULL;
	qglGetPolygonStipple         = NULL;
	qglGetString                 = NULL;
	qglGetTexEnvfv               = NULL;
	qglGetTexEnviv               = NULL;
	qglGetTexGendv               = NULL;
	qglGetTexGenfv               = NULL;
	qglGetTexGeniv               = NULL;
	qglGetTexImage               = NULL;
	qglGetTexLevelParameterfv    = NULL;
	qglGetTexLevelParameteriv    = NULL;
	qglGetTexParameterfv         = NULL;
	qglGetTexParameteriv         = NULL;
	qglHint                      = NULL;
	qglIndexMask                 = NULL;
	qglIndexPointer              = NULL;
	qglIndexd                    = NULL;
	qglIndexdv                   = NULL;
	qglIndexf                    = NULL;
	qglIndexfv                   = NULL;
	qglIndexi                    = NULL;
	qglIndexiv                   = NULL;
	qglIndexs                    = NULL;
	qglIndexsv                   = NULL;
	qglIndexub                   = NULL;
	qglIndexubv                  = NULL;
	qglInitNames                 = NULL;
	qglInterleavedArrays         = NULL;
	qglIsEnabled                 = NULL;
	qglIsList                    = NULL;
	qglIsTexture                 = NULL;
	qglLightModelf               = NULL;
	qglLightModelfv              = NULL;
	qglLightModeli               = NULL;
	qglLightModeliv              = NULL;
	qglLightf                    = NULL;
	qglLightfv                   = NULL;
	qglLighti                    = NULL;
	qglLightiv                   = NULL;
	qglLineStipple               = NULL;
	qglLineWidth                 = NULL;
	qglListBase                  = NULL;
	qglLoadIdentity              = NULL;
	qglLoadMatrixd               = NULL;
	qglLoadMatrixf               = NULL;
	qglLoadName                  = NULL;
	qglLogicOp                   = NULL;
	qglMap1d                     = NULL;
	qglMap1f                     = NULL;
	qglMap2d                     = NULL;
	qglMap2f                     = NULL;
	qglMapGrid1d                 = NULL;
	qglMapGrid1f                 = NULL;
	qglMapGrid2d                 = NULL;
	qglMapGrid2f                 = NULL;
	qglMaterialf                 = NULL;
	qglMaterialfv                = NULL;
	qglMateriali                 = NULL;
	qglMaterialiv                = NULL;
	qglMatrixMode                = NULL;
	qglMultMatrixd               = NULL;
	qglMultMatrixf               = NULL;
	qglNewList                   = NULL;
	qglNormal3b                  = NULL;
	qglNormal3bv                 = NULL;
	qglNormal3d                  = NULL;
	qglNormal3dv                 = NULL;
	qglNormal3f                  = NULL;
	qglNormal3fv                 = NULL;
	qglNormal3i                  = NULL;
	qglNormal3iv                 = NULL;
	qglNormal3s                  = NULL;
	qglNormal3sv                 = NULL;
	qglNormalPointer             = NULL;
	qglOrtho                     = NULL;
	qglPassThrough               = NULL;
	qglPixelMapfv                = NULL;
	qglPixelMapuiv               = NULL;
	qglPixelMapusv               = NULL;
	qglPixelStoref               = NULL;
	qglPixelStorei               = NULL;
	qglPixelTransferf            = NULL;
	qglPixelTransferi            = NULL;
	qglPixelZoom                 = NULL;
	qglPointSize                 = NULL;
	qglPolygonMode               = NULL;
	qglPolygonOffset             = NULL;
	qglPolygonStipple            = NULL;
	qglPopAttrib                 = NULL;
	qglPopClientAttrib           = NULL;
	qglPopMatrix                 = NULL;
	qglPopName                   = NULL;
	qglPrioritizeTextures        = NULL;
	qglPushAttrib                = NULL;
	qglPushClientAttrib          = NULL;
	qglPushMatrix                = NULL;
	qglPushName                  = NULL;
	qglRasterPos2d               = NULL;
	qglRasterPos2dv              = NULL;
	qglRasterPos2f               = NULL;
	qglRasterPos2fv              = NULL;
	qglRasterPos2i               = NULL;
	qglRasterPos2iv              = NULL;
	qglRasterPos2s               = NULL;
	qglRasterPos2sv              = NULL;
	qglRasterPos3d               = NULL;
	qglRasterPos3dv              = NULL;
	qglRasterPos3f               = NULL;
	qglRasterPos3fv              = NULL;
	qglRasterPos3i               = NULL;
	qglRasterPos3iv              = NULL;
	qglRasterPos3s               = NULL;
	qglRasterPos3sv              = NULL;
	qglRasterPos4d               = NULL;
	qglRasterPos4dv              = NULL;
	qglRasterPos4f               = NULL;
	qglRasterPos4fv              = NULL;
	qglRasterPos4i               = NULL;
	qglRasterPos4iv              = NULL;
	qglRasterPos4s               = NULL;
	qglRasterPos4sv              = NULL;
	qglReadBuffer                = NULL;
	qglReadPixels                = NULL;
	qglRectd                     = NULL;
	qglRectdv                    = NULL;
	qglRectf                     = NULL;
	qglRectfv                    = NULL;
	qglRecti                     = NULL;
	qglRectiv                    = NULL;
	qglRects                     = NULL;
	qglRectsv                    = NULL;
	qglRenderMode                = NULL;
	qglRotated                   = NULL;
	qglRotatef                   = NULL;
	qglScaled                    = NULL;
	qglScalef                    = NULL;
	qglScissor                   = NULL;
	qglSelectBuffer              = NULL;
	qglShadeModel                = NULL;
	qglStencilFunc               = NULL;
	qglStencilMask               = NULL;
	qglStencilOp                 = NULL;
	qglTexCoord1d                = NULL;
	qglTexCoord1dv               = NULL;
	qglTexCoord1f                = NULL;
	qglTexCoord1fv               = NULL;
	qglTexCoord1i                = NULL;
	qglTexCoord1iv               = NULL;
	qglTexCoord1s                = NULL;
	qglTexCoord1sv               = NULL;
	qglTexCoord2d                = NULL;
	qglTexCoord2dv               = NULL;
	qglTexCoord2f                = NULL;
	qglTexCoord2fv               = NULL;
	qglTexCoord2i                = NULL;
	qglTexCoord2iv               = NULL;
	qglTexCoord2s                = NULL;
	qglTexCoord2sv               = NULL;
	qglTexCoord3d                = NULL;
	qglTexCoord3dv               = NULL;
	qglTexCoord3f                = NULL;
	qglTexCoord3fv               = NULL;
	qglTexCoord3i                = NULL;
	qglTexCoord3iv               = NULL;
	qglTexCoord3s                = NULL;
	qglTexCoord3sv               = NULL;
	qglTexCoord4d                = NULL;
	qglTexCoord4dv               = NULL;
	qglTexCoord4f                = NULL;
	qglTexCoord4fv               = NULL;
	qglTexCoord4i                = NULL;
	qglTexCoord4iv               = NULL;
	qglTexCoord4s                = NULL;
	qglTexCoord4sv               = NULL;
	qglTexCoordPointer           = NULL;
	qglTexEnvf                   = NULL;
	qglTexEnvfv                  = NULL;
	qglTexEnvi                   = NULL;
	qglTexEnviv                  = NULL;
	qglTexGend                   = NULL;
	qglTexGendv                  = NULL;
	qglTexGenf                   = NULL;
	qglTexGenfv                  = NULL;
	qglTexGeni                   = NULL;
	qglTexGeniv                  = NULL;
	qglTexImage1D                = NULL;
	qglTexImage2D                = NULL;
	qglTexParameterf             = NULL;
	qglTexParameterfv            = NULL;
	qglTexParameteri             = NULL;
	qglTexParameteriv            = NULL;
	qglTexSubImage1D             = NULL;
	qglTexSubImage2D             = NULL;
	qglTranslated                = NULL;
	qglTranslatef                = NULL;
	qglVertex2d                  = NULL;
	qglVertex2dv                 = NULL;
	qglVertex2f                  = NULL;
	qglVertex2fv                 = NULL;
	qglVertex2i                  = NULL;
	qglVertex2iv                 = NULL;
	qglVertex2s                  = NULL;
	qglVertex2sv                 = NULL;
	qglVertex3d                  = NULL;
	qglVertex3dv                 = NULL;
	qglVertex3f                  = NULL;
	qglVertex3fv                 = NULL;
	qglVertex3i                  = NULL;
	qglVertex3iv                 = NULL;
	qglVertex3s                  = NULL;
	qglVertex3sv                 = NULL;
	qglVertex4d                  = NULL;
	qglVertex4dv                 = NULL;
	qglVertex4f                  = NULL;
	qglVertex4fv                 = NULL;
	qglVertex4i                  = NULL;
	qglVertex4iv                 = NULL;
	qglVertex4s                  = NULL;
	qglVertex4sv                 = NULL;
	qglVertexPointer             = NULL;
	qglViewport                  = NULL;
}

#define GPA(pfn, a ) (pfn)SDL_LoadFunction(glw_state.hinstOpenGL, a)

/*
** QGL_Init
**
** This is responsible for binding our qgl function pointers to 
** the appropriate GL stuff.  In Windows this means doing a 
** LoadLibrary and a bunch of calls to GetProcAddress.  On other
** operating systems we need to do the right thing, whatever that
** might be.
*/
bool QGL_Init(const char* dllname)
{
	assert(glw_state.hinstOpenGL == 0);

	ri.Printf(PRINT_ALL, "...initializing QGL\n");
	ri.Printf(PRINT_ALL, "...calling SDL_LoadObject( '%s.dll' ): ", dllname);

	if ((glw_state.hinstOpenGL = SDL_LoadObject(dllname)) == 0) {
		ri.Printf(PRINT_ALL, "failed\n");
		return false;
	}

	ri.Printf( PRINT_ALL, "succeeded\n" );

	qglAccum                     = dllAccum						= GPA(PFNGLACCUM, "glAccum" );
	qglAlphaFunc                 = dllAlphaFunc					= GPA(PFNGLALPHAFUNC, "glAlphaFunc" );
	qglAreTexturesResident       = dllAreTexturesResident		= GPA(PFNGLARETEXTURESRESIDENT, "glAreTexturesResident" );
	qglArrayElement              = dllArrayElement				= GPA(PFNGLARRAYELEMENT, "glArrayElement" );
	qglBegin                     = dllBegin						= GPA(PFNGLBEGIN, "glBegin" );
	qglBindTexture               = dllBindTexture				= GPA(PFNGLBINDTEXTURE, "glBindTexture" );
	qglBitmap                    = dllBitmap					= GPA(PFNGLBITMAP, "glBitmap" );
	qglBlendFunc                 = dllBlendFunc					= GPA(PFNGLBLENDFUNC, "glBlendFunc" );
	qglCallList                  = dllCallList					= GPA(PFNGLCALLLIST, "glCallList" );
	qglCallLists                 = dllCallLists					= GPA(PFNGLCALLLISTS, "glCallLists" );
	qglClear                     = dllClear						= GPA(PFNGLCLEAR, "glClear" );
	qglClearAccum                = dllClearAccum				= GPA(PFNGLCLEARACCUM, "glClearAccum" );
	qglClearColor                = dllClearColor				= GPA(PFNGLCLEARCOLOR, "glClearColor" );
	qglClearDepth                = dllClearDepth				= GPA(PFNGLCLEARDEPTH, "glClearDepth" );
	qglClearIndex                = dllClearIndex				= GPA(PFNGLCLEARINDEX, "glClearIndex" );
	qglClearStencil              = dllClearStencil				= GPA(PFNGLCLEARSTENCIL, "glClearStencil" );
	qglClipPlane                 = dllClipPlane					= GPA(PFNGLCLIPPLANE, "glClipPlane" );
	qglColor3b                   = dllColor3b					= GPA(PFNGLCOLOR3B, "glColor3b" );
	qglColor3bv                  = dllColor3bv					= GPA(PFNGLCOLOR3BV, "glColor3bv" );
	qglColor3d                   = dllColor3d					= GPA(PFNGLCOLOR3D, "glColor3d" );
	qglColor3dv                  = dllColor3dv					= GPA(PFNGLCOLOR3DV, "glColor3dv" );
	qglColor3f                   = dllColor3f					= GPA(PFNGLCOLOR3F, "glColor3f" );
	qglColor3fv                  = dllColor3fv					= GPA(PFNGLCOLOR3FV, "glColor3fv" );
	qglColor3i                   = dllColor3i					= GPA(PFNGLCOLOR3I, "glColor3i" );
	qglColor3iv                  = dllColor3iv					= GPA(PFNGLCOLOR3IV, "glColor3iv" );
	qglColor3s                   = dllColor3s					= GPA(PFNGLCOLOR3S, "glColor3s" );
	qglColor3sv                  = dllColor3sv					= GPA(PFNGLCOLOR3SV, "glColor3sv" );
	qglColor3ub                  = dllColor3ub					= GPA(PFNGLCOLOR3UB, "glColor3ub" );
	qglColor3ubv                 = dllColor3ubv					= GPA(PFNGLCOLOR3UBV, "glColor3ubv" );
	qglColor3ui                  = dllColor3ui					= GPA(PFNGLCOLOR3UI, "glColor3ui" );
	qglColor3uiv                 = dllColor3uiv					= GPA(PFNGLCOLOR3UIV, "glColor3uiv" );
	qglColor3us                  = dllColor3us					= GPA(PFNGLCOLOR3US, "glColor3us" );
	qglColor3usv                 = dllColor3usv					= GPA(PFNGLCOLOR3USV, "glColor3usv" );
	qglColor4b                   = dllColor4b					= GPA(PFNGLCOLOR4B, "glColor4b" );
	qglColor4bv                  = dllColor4bv					= GPA(PFNGLCOLOR4BV, "glColor4bv" );
	qglColor4d                   = dllColor4d					= GPA(PFNGLCOLOR4D, "glColor4d" );
	qglColor4dv                  = dllColor4dv					= GPA(PFNGLCOLOR4DV, "glColor4dv" );
	qglColor4f                   = dllColor4f					= GPA(PFNGLCOLOR4F, "glColor4f" );
	qglColor4fv                  = dllColor4fv					= GPA(PFNGLCOLOR4FV, "glColor4fv" );
	qglColor4i                   = dllColor4i					= GPA(PFNGLCOLOR4I, "glColor4i" );
	qglColor4iv                  = dllColor4iv					= GPA(PFNGLCOLOR4IV, "glColor4iv" );
	qglColor4s                   = dllColor4s					= GPA(PFNGLCOLOR4S, "glColor4s" );
	qglColor4sv                  = dllColor4sv					= GPA(PFNGLCOLOR4SV, "glColor4sv" );
	qglColor4ub                  = dllColor4ub					= GPA(PFNGLCOLOR4UB, "glColor4ub" );
	qglColor4ubv                 = dllColor4ubv					= GPA(PFNGLCOLOR4UBV, "glColor4ubv" );
	qglColor4ui                  = dllColor4ui					= GPA(PFNGLCOLOR4UI, "glColor4ui" );
	qglColor4uiv                 = dllColor4uiv					= GPA(PFNGLCOLOR4UIV, "glColor4uiv" );
	qglColor4us                  = dllColor4us					= GPA(PFNGLCOLOR4US, "glColor4us" );
	qglColor4usv                 = dllColor4usv					= GPA(PFNGLCOLOR4USV, "glColor4usv" );
	qglColorMask                 = dllColorMask					= GPA(PFNGLCOLORMASK, "glColorMask" );
	qglColorMaterial             = dllColorMaterial				= GPA(PFNGLCOLORMATERIAL, "glColorMaterial" );
	qglColorPointer              = dllColorPointer				= GPA(PFNGLCOLORPOINTER, "glColorPointer" );
	qglCopyPixels                = dllCopyPixels				= GPA(PFNGLCOPYPIXELS, "glCopyPixels" );
	qglCopyTexImage1D            = dllCopyTexImage1D			= GPA(PFNGLCOPYTEXIMAGE1D, "glCopyTexImage1D" );
	qglCopyTexImage2D            = dllCopyTexImage2D			= GPA(PFNGLCOPYTEXIMAGE2D, "glCopyTexImage2D" );
	qglCopyTexSubImage1D         = dllCopyTexSubImage1D			= GPA(PFNGLCOPYTEXSUBIMAGE1D, "glCopyTexSubImage1D" );
	qglCopyTexSubImage2D         = dllCopyTexSubImage2D			= GPA(PFNGLCOPYTEXSUBIMAGE2D, "glCopyTexSubImage2D" );
	qglCullFace                  = dllCullFace					= GPA(PFNGLCULLFACE, "glCullFace" );
	qglDeleteLists               = dllDeleteLists				= GPA(PFNGLDELETELISTS, "glDeleteLists" );
	qglDeleteTextures            = dllDeleteTextures			= GPA(PFNGLDELETETEXTURES, "glDeleteTextures" );
	qglDepthFunc                 = dllDepthFunc					= GPA(PFNGLDEPTHFUNC, "glDepthFunc" );
	qglDepthMask                 = dllDepthMask					= GPA(PFNGLDEPTHMASK, "glDepthMask" );
	qglDepthRange                = dllDepthRange				= GPA(PFNGLDEPTHRANGE, "glDepthRange" );
	qglDisable                   = dllDisable					= GPA(PFNGLDISABLE, "glDisable" );
	qglDisableClientState        = dllDisableClientState		= GPA(PFNGLDISABLECLIENTSTATE, "glDisableClientState" );
	qglDrawArrays                = dllDrawArrays				= GPA(PFNGLDRAWARRAYS, "glDrawArrays" );
	qglDrawBuffer                = dllDrawBuffer				= GPA(PFNGLDRAWBUFFER, "glDrawBuffer" );
	qglDrawElements              = dllDrawElements				= GPA(PFNGLDRAWELEMENTS, "glDrawElements" );
	qglDrawPixels                = dllDrawPixels				= GPA(PFNGLDRAWPIXELS, "glDrawPixels" );
	qglEdgeFlag                  = dllEdgeFlag					= GPA(PFNGLEDGEFLAG, "glEdgeFlag" );
	qglEdgeFlagPointer           = dllEdgeFlagPointer			= GPA(PFNGLEDGEFLAGPOINTER, "glEdgeFlagPointer" );
	qglEdgeFlagv                 = dllEdgeFlagv					= GPA(PFNGLEDGEFLAGV, "glEdgeFlagv" );
	qglEnable                    = dllEnable                    = GPA(PFNGLENABLE, "glEnable" );
	qglEnableClientState         = dllEnableClientState         = GPA(PFNGLENABLECLIENTSTATE, "glEnableClientState" );
	qglEnd                       = dllEnd                       = GPA(PFNGLEND, "glEnd" );
	qglEndList                   = dllEndList                   = GPA(PFNGLENDLIST, "glEndList" );
	qglEvalCoord1d				 = dllEvalCoord1d				= GPA(PFNGLEVALCOORD1D, "glEvalCoord1d" );
	qglEvalCoord1dv              = dllEvalCoord1dv              = GPA(PFNGLEVALCOORD1DV, "glEvalCoord1dv" );
	qglEvalCoord1f               = dllEvalCoord1f               = GPA(PFNGLEVALCOORD1F, "glEvalCoord1f" );
	qglEvalCoord1fv              = dllEvalCoord1fv              = GPA(PFNGLEVALCOORD1FV, "glEvalCoord1fv" );
	qglEvalCoord2d               = dllEvalCoord2d               = GPA(PFNGLEVALCOORD2D, "glEvalCoord2d" );
	qglEvalCoord2dv              = dllEvalCoord2dv              = GPA(PFNGLEVALCOORD2DV, "glEvalCoord2dv" );
	qglEvalCoord2f               = dllEvalCoord2f               = GPA(PFNGLEVALCOORD2F, "glEvalCoord2f" );
	qglEvalCoord2fv              = dllEvalCoord2fv              = GPA(PFNGLEVALCOORD2FV, "glEvalCoord2fv" );
	qglEvalMesh1                 = dllEvalMesh1                 = GPA(PFNGLEVALMESH1, "glEvalMesh1" );
	qglEvalMesh2                 = dllEvalMesh2                 = GPA(PFNGLEVALMESH2, "glEvalMesh2" );
	qglEvalPoint1                = dllEvalPoint1                = GPA(PFNGLEVALPOINT1, "glEvalPoint1" );
	qglEvalPoint2                = dllEvalPoint2                = GPA(PFNGLEVALPOINT2, "glEvalPoint2" );
	qglFeedbackBuffer            = dllFeedbackBuffer            = GPA(PFNGLFEEDBACKBUFFER, "glFeedbackBuffer" );
	qglFinish                    = dllFinish                    = GPA(PFNGLFINISH, "glFinish" );
	qglFlush                     = dllFlush                     = GPA(PFNGLFLUSH, "glFlush" );
	qglFogf                      = dllFogf                      = GPA(PFNGLFOGF, "glFogf" );
	qglFogfv                     = dllFogfv                     = GPA(PFNGLFOGFV, "glFogfv" );
	qglFogi                      = dllFogi                      = GPA(PFNGLFOGI, "glFogi" );
	qglFogiv                     = dllFogiv                     = GPA(PFNGLFOGIV, "glFogiv" );
	qglFrontFace                 = dllFrontFace                 = GPA(PFNGLFRONTFACE, "glFrontFace" );
	qglFrustum                   = dllFrustum                   = GPA(PFNGLFRUSTUM, "glFrustum" );
	qglGenLists                  = dllGenLists                  = GPA(PFNGENLISTS, "glGenLists" );
	qglGenTextures               = dllGenTextures               = GPA(PFNGLGENTEXTURES, "glGenTextures" );
	qglGetBooleanv               = dllGetBooleanv               = GPA(PFNGLGETBOOLEANV, "glGetBooleanv" );
	qglGetClipPlane              = dllGetClipPlane              = GPA(PFNGLGETCLIPPLANE, "glGetClipPlane" );
	qglGetDoublev                = dllGetDoublev                = GPA(PFNGLGETDOUBLEV, "glGetDoublev" );
	qglGetError                  = dllGetError                  = GPA(PFNGETERROR, "glGetError" );
	qglGetFloatv                 = dllGetFloatv                 = GPA(PFNGLGETFLOATV, "glGetFloatv" );
	qglGetIntegerv               = dllGetIntegerv               = GPA(PFNGLGETINTEGERV, "glGetIntegerv" );
	qglGetLightfv                = dllGetLightfv                = GPA(PFNGLGETLIGHTFV, "glGetLightfv" );
	qglGetLightiv                = dllGetLightiv                = GPA(PFNGLGETLIGHTIV, "glGetLightiv" );
	qglGetMapdv                  = dllGetMapdv                  = GPA(PFNGLGETMAPDV, "glGetMapdv" );
	qglGetMapfv                  = dllGetMapfv                  = GPA(PFNGLGETMAPFV, "glGetMapfv" );
	qglGetMapiv                  = dllGetMapiv                  = GPA(PFNGLGETMAPIV, "glGetMapiv" );
	qglGetMaterialfv             = dllGetMaterialfv             = GPA(PFNGLGETMATERIALFV, "glGetMaterialfv" );
	qglGetMaterialiv             = dllGetMaterialiv             = GPA(PFNGLGETMATERIALIV, "glGetMaterialiv" );
	qglGetPixelMapfv             = dllGetPixelMapfv             = GPA(PFNGLGETPIXELMAPFV, "glGetPixelMapfv" );
	qglGetPixelMapuiv            = dllGetPixelMapuiv            = GPA(PFNGLGETPIXELMAPUIV, "glGetPixelMapuiv" );
	qglGetPixelMapusv            = dllGetPixelMapusv            = GPA(PFNGLGETPIXELMAPUSV, "glGetPixelMapusv" );
	qglGetPointerv               = dllGetPointerv               = GPA(PFNGLGETPOINTERV, "glGetPointerv" );
	qglGetPolygonStipple         = dllGetPolygonStipple         = GPA(PFNGLGETPOLYGONSTIPPLE, "glGetPolygonStipple" );
	qglGetString                 = dllGetString                 = GPA(PFNGLGETSTRING, "glGetString" );
	qglGetTexEnvfv               = dllGetTexEnvfv               = GPA(PFNGLGETTEXENVFV, "glGetTexEnvfv" );
	qglGetTexEnviv               = dllGetTexEnviv               = GPA(PFNGLGETTEXENVIV, "glGetTexEnviv" );
	qglGetTexGendv               = dllGetTexGendv               = GPA(PFNGLGETTEXGENDV, "glGetTexGendv" );
	qglGetTexGenfv               = dllGetTexGenfv               = GPA(PFNGLGETTEXGENFV, "glGetTexGenfv" );
	qglGetTexGeniv               = dllGetTexGeniv               = GPA(PFNGLGETTEXGENIV, "glGetTexGeniv" );
	qglGetTexImage               = dllGetTexImage               = GPA(PFNGLGETTEXIMAGE, "glGetTexImage" );
	qglGetTexLevelParameterfv	 = dllGetTexLevelParameterfv	= GPA(PFNGLGETTEXLEVELPARAMETERFV, "glGetTexLevelParameterfv");
	qglGetTexLevelParameteriv	 = dllGetTexLevelParameteriv	= GPA(PFNGLGETTEXLEVELPARAMETERIV, "glGetTexLevelParameteriv");
	qglGetTexParameterfv         = dllGetTexParameterfv         = GPA(PFNGLGETTEXPARAMETERFV, "glGetTexParameterfv" );
	qglGetTexParameteriv         = dllGetTexParameteriv         = GPA(PFNGLGETTEXPARAMETERIV, "glGetTexParameteriv" );
	qglHint                      = dllHint                      = GPA(PFNGLHINT, "glHint" );
	qglIndexMask                 = dllIndexMask                 = GPA(PFNGLINDEXMASK, "glIndexMask" );
	qglIndexPointer              = dllIndexPointer              = GPA(PFNGLINDEXPOINTER, "glIndexPointer" );
	qglIndexd                    = dllIndexd                    = GPA(PFNGLINDEXD, "glIndexd" );
	qglIndexdv                   = dllIndexdv                   = GPA(PFNGLINDEXDV, "glIndexdv" );
	qglIndexf                    = dllIndexf                    = GPA(PFNGLINDEXF, "glIndexf" );
	qglIndexfv                   = dllIndexfv                   = GPA(PFNGLINDEXFV, "glIndexfv" );
	qglIndexi                    = dllIndexi                    = GPA(PFNGLINDEXI, "glIndexi" );
	qglIndexiv                   = dllIndexiv                   = GPA(PFNGLINDEXIV, "glIndexiv" );
	qglIndexs                    = dllIndexs                    = GPA(PFNGLINDEXS, "glIndexs" );
	qglIndexsv                   = dllIndexsv                   = GPA(PFNGLINDEXSV, "glIndexsv" );
	qglIndexub                   = dllIndexub                   = GPA(PFNGLINDEXUB, "glIndexub" );
	qglIndexubv                  = dllIndexubv                  = GPA(PFNGLINDEXUBV, "glIndexubv" );
	qglInitNames                 = dllInitNames                 = GPA(PFNGLINITNAMES, "glInitNames" );
	qglInterleavedArrays         = dllInterleavedArrays         = GPA(PFNGLINTERLEAVEDARRAYS, "glInterleavedArrays" );
	qglIsEnabled                 = dllIsEnabled                 = GPA(PFNISENABLED, "glIsEnabled" );
	qglIsList                    = dllIsList                    = GPA(PFNISLIST, "glIsList" );
	qglIsTexture                 = dllIsTexture                 = GPA(PFNISTEXTURE, "glIsTexture" );
	qglLightModelf               = dllLightModelf               = GPA(PFNGLLIGHTMODELF, "glLightModelf" );
	qglLightModelfv              = dllLightModelfv              = GPA(PFNGLLIGHTMODELFV, "glLightModelfv" );
	qglLightModeli               = dllLightModeli               = GPA(PFNGLLIGHTMODELI, "glLightModeli" );
	qglLightModeliv              = dllLightModeliv              = GPA(PFNGLLIGHTMODELIV, "glLightModeliv" );
	qglLightf                    = dllLightf                    = GPA(PFNGLLIGHTF, "glLightf" );
	qglLightfv                   = dllLightfv                   = GPA(PFNGLLIGHTFV, "glLightfv" );
	qglLighti                    = dllLighti                    = GPA(PFNGLLIGHTI, "glLighti" );
	qglLightiv                   = dllLightiv                   = GPA(PFNGLLIGHTIV, "glLightiv" );
	qglLineStipple               = dllLineStipple               = GPA(PFNGLLINESTIPPLE, "glLineStipple" );
	qglLineWidth                 = dllLineWidth                 = GPA(PFNGLLINEWIDTH, "glLineWidth" );
	qglListBase                  = dllListBase                  = GPA(PFNGLLISTBASE, "glListBase" );
	qglLoadIdentity              = dllLoadIdentity              = GPA(PFNGLLOADIDENTITY, "glLoadIdentity" );
	qglLoadMatrixd               = dllLoadMatrixd               = GPA(PFNGLLOADMATRIXD, "glLoadMatrixd" );
	qglLoadMatrixf               = dllLoadMatrixf               = GPA(PFNGLLOADMATRIXF, "glLoadMatrixf" );
	qglLoadName                  = dllLoadName                  = GPA(PFNGLLOADNAME, "glLoadName" );
	qglLogicOp                   = dllLogicOp                   = GPA(PFNGLLOGICOP, "glLogicOp" );
	qglMap1d                     = dllMap1d                     = GPA(PFNGLMAP1D, "glMap1d" );
	qglMap1f                     = dllMap1f                     = GPA(PFNGLMAP1F, "glMap1f" );
	qglMap2d                     = dllMap2d                     = GPA(PFNGLMAP2D, "glMap2d" );
	qglMap2f                     = dllMap2f                     = GPA(PFNGLMAP2F, "glMap2f" );
	qglMapGrid1d                 = dllMapGrid1d                 = GPA(PFNGLMAPGRID1D, "glMapGrid1d" );
	qglMapGrid1f                 = dllMapGrid1f                 = GPA(PFNGLMAPGRID1F, "glMapGrid1f" );
	qglMapGrid2d                 = dllMapGrid2d                 = GPA(PFNGLMAPGRID2D, "glMapGrid2d" );
	qglMapGrid2f                 = dllMapGrid2f                 = GPA(PFNGLMAPGRID2F, "glMapGrid2f" );
	qglMaterialf                 = dllMaterialf                 = GPA(PFNGLMATERIALF, "glMaterialf" );
	qglMaterialfv                = dllMaterialfv                = GPA(PFNGLMATERIALFV, "glMaterialfv" );
	qglMateriali                 = dllMateriali                 = GPA(PFNGLMATERIALI, "glMateriali" );
	qglMaterialiv                = dllMaterialiv                = GPA(PFNGLMATERIALIV, "glMaterialiv" );
	qglMatrixMode                = dllMatrixMode                = GPA(PFNGLMATRIXMODE, "glMatrixMode" );
	qglMultMatrixd               = dllMultMatrixd               = GPA(PFNGLMULTMATRIXD, "glMultMatrixd" );
	qglMultMatrixf               = dllMultMatrixf               = GPA(PFNGLMULTMATRIXF, "glMultMatrixf" );
	qglNewList                   = dllNewList                   = GPA(PFNGLNEWLIST, "glNewList" );
	qglNormal3b                  = dllNormal3b                  = GPA(PFNGLNORMAL3B, "glNormal3b" );
	qglNormal3bv                 = dllNormal3bv                 = GPA(PFNGLNORMAL3BV, "glNormal3bv" );
	qglNormal3d                  = dllNormal3d                  = GPA(PFNGLNORMAL3D, "glNormal3d" );
	qglNormal3dv                 = dllNormal3dv                 = GPA(PFNGLNORMAL3DV, "glNormal3dv" );
	qglNormal3f                  = dllNormal3f                  = GPA(PFNGLNORMAL3F, "glNormal3f" );
	qglNormal3fv                 = dllNormal3fv                 = GPA(PFNGLNORMAL3FV, "glNormal3fv" );
	qglNormal3i                  = dllNormal3i                  = GPA(PFNGLNORMAL3I, "glNormal3i" );
	qglNormal3iv                 = dllNormal3iv                 = GPA(PFNGLNORMAL3IV, "glNormal3iv" );
	qglNormal3s                  = dllNormal3s                  = GPA(PFNGLNORMAL3S, "glNormal3s" );
	qglNormal3sv                 = dllNormal3sv                 = GPA(PFNGLNORMAL3SV, "glNormal3sv" );
	qglNormalPointer             = dllNormalPointer             = GPA(PFNGLNORMALPOINTER, "glNormalPointer" );
	qglOrtho                     = dllOrtho                     = GPA(PFNGLORTHO, "glOrtho" );
	qglPassThrough               = dllPassThrough               = GPA(PFNGLPASSTHROUGH, "glPassThrough" );
	qglPixelMapfv                = dllPixelMapfv                = GPA(PFNGLPIXELMAPFV, "glPixelMapfv" );
	qglPixelMapuiv               = dllPixelMapuiv               = GPA(PFNGLPIXELMAPUIV, "glPixelMapuiv" );
	qglPixelMapusv               = dllPixelMapusv               = GPA(PFNGLPIXELMAPUSV, "glPixelMapusv" );
	qglPixelStoref               = dllPixelStoref               = GPA(PFNGLPIXELSTOREF, "glPixelStoref" );
	qglPixelStorei               = dllPixelStorei               = GPA(PFNGLPIXELSTOREI, "glPixelStorei" );
	qglPixelTransferf            = dllPixelTransferf            = GPA(PFNGLPIXELTRANSFERF, "glPixelTransferf" );
	qglPixelTransferi            = dllPixelTransferi            = GPA(PFNGLPIXELTRANSFERI, "glPixelTransferi" );
	qglPixelZoom                 = dllPixelZoom                 = GPA(PFNGLPIXELZOOM, "glPixelZoom" );
	qglPointSize                 = dllPointSize                 = GPA(PFNGLPOINTSIZE, "glPointSize" );
	qglPolygonMode               = dllPolygonMode               = GPA(PFNGLPOLYGONMODE, "glPolygonMode" );
	qglPolygonOffset             = dllPolygonOffset             = GPA(PFNGLPOLYGONOFFSET, "glPolygonOffset" );
	qglPolygonStipple            = dllPolygonStipple            = GPA(PFNGLPOLYGONSTIPPLE, "glPolygonStipple" );
	qglPopAttrib                 = dllPopAttrib                 = GPA(PFNGLPOPATTRIB, "glPopAttrib" );
	qglPopClientAttrib           = dllPopClientAttrib           = GPA(PFNGLPOPCLIENTATTRIB, "glPopClientAttrib" );
	qglPopMatrix                 = dllPopMatrix                 = GPA(PFNGLPOPMATRIX, "glPopMatrix" );
	qglPopName                   = dllPopName                   = GPA(PFNGLPOPNAME, "glPopName" );
	qglPrioritizeTextures        = dllPrioritizeTextures        = GPA(PFNGLPRIORITIZETEXTURES, "glPrioritizeTextures" );
	qglPushAttrib                = dllPushAttrib                = GPA(PFNGLPUSHATTRIB, "glPushAttrib" );
	qglPushClientAttrib          = dllPushClientAttrib          = GPA(PFNGLPUSHCLIENTATTRIB, "glPushClientAttrib" );
	qglPushMatrix                = dllPushMatrix                = GPA(PFNGLPUSHMATRIX, "glPushMatrix" );
	qglPushName                  = dllPushName                  = GPA(PFNGLPUSHNAME, "glPushName" );
	qglRasterPos2d               = dllRasterPos2d               = GPA(PFNGLRASTERPOS2D, "glRasterPos2d" );
	qglRasterPos2dv              = dllRasterPos2dv              = GPA(PFNGLRASTERPOS2DV, "glRasterPos2dv" );
	qglRasterPos2f               = dllRasterPos2f               = GPA(PFNGLRASTERPOS2F, "glRasterPos2f" );
	qglRasterPos2fv              = dllRasterPos2fv              = GPA(PFNGLRASTERPOS2FV, "glRasterPos2fv" );
	qglRasterPos2i               = dllRasterPos2i               = GPA(PFNGLRASTERPOS2I, "glRasterPos2i" );
	qglRasterPos2iv              = dllRasterPos2iv              = GPA(PFNGLRASTERPOS2IV, "glRasterPos2iv" );
	qglRasterPos2s               = dllRasterPos2s               = GPA(PFNGLRASTERPOS2S, "glRasterPos2s" );
	qglRasterPos2sv              = dllRasterPos2sv              = GPA(PFNGLRASTERPOS2SV, "glRasterPos2sv" );
	qglRasterPos3d               = dllRasterPos3d               = GPA(PFNGLRASTERPOS3D, "glRasterPos3d" );
	qglRasterPos3dv              = dllRasterPos3dv              = GPA(PFNGLRASTERPOS3DV, "glRasterPos3dv" );
	qglRasterPos3f               = dllRasterPos3f               = GPA(PFNGLRASTERPOS3F, "glRasterPos3f" );
	qglRasterPos3fv              = dllRasterPos3fv              = GPA(PFNGLRASTERPOS3FV, "glRasterPos3fv" );
	qglRasterPos3i               = dllRasterPos3i               = GPA(PFNGLRASTERPOS3I, "glRasterPos3i" );
	qglRasterPos3iv              = dllRasterPos3iv              = GPA(PFNGLRASTERPOS3IV, "glRasterPos3iv" );
	qglRasterPos3s               = dllRasterPos3s               = GPA(PFNGLRASTERPOS3S, "glRasterPos3s" );
	qglRasterPos3sv              = dllRasterPos3sv              = GPA(PFNGLRASTERPOS3SV, "glRasterPos3sv" );
	qglRasterPos4d               = dllRasterPos4d               = GPA(PFNGLRASTERPOS4D, "glRasterPos4d" );
	qglRasterPos4dv              = dllRasterPos4dv              = GPA(PFNGLRASTERPOS4DV, "glRasterPos4dv" );
	qglRasterPos4f               = dllRasterPos4f               = GPA(PFNGLRASTERPOS4F, "glRasterPos4f" );
	qglRasterPos4fv              = dllRasterPos4fv              = GPA(PFNGLRASTERPOS4FV, "glRasterPos4fv" );
	qglRasterPos4i               = dllRasterPos4i               = GPA(PFNGLRASTERPOS4I, "glRasterPos4i" );
	qglRasterPos4iv              = dllRasterPos4iv              = GPA(PFNGLRASTERPOS4IV, "glRasterPos4iv" );
	qglRasterPos4s               = dllRasterPos4s               = GPA(PFNGLRASTERPOS4S, "glRasterPos4s" );
	qglRasterPos4sv              = dllRasterPos4sv              = GPA(PFNGLRASTERPOS4SV, "glRasterPos4sv" );
	qglReadBuffer                = dllReadBuffer                = GPA(PFNGLREADBUFFER, "glReadBuffer" );
	qglReadPixels                = dllReadPixels                = GPA(PFNGLREADPIXELS, "glReadPixels" );
	qglRectd                     = dllRectd                     = GPA(PFNGLRECTD, "glRectd" );
	qglRectdv                    = dllRectdv                    = GPA(PFNGLRECTDV, "glRectdv" );
	qglRectf                     = dllRectf                     = GPA(PFNGLRECTF, "glRectf" );
	qglRectfv                    = dllRectfv                    = GPA(PFNGLRECTFV, "glRectfv" );
	qglRecti                     = dllRecti                     = GPA(PFNGLRECTI, "glRecti" );
	qglRectiv                    = dllRectiv                    = GPA(PFNGLRECTIV, "glRectiv" );
	qglRects                     = dllRects                     = GPA(PFNGLRECTS, "glRects" );
	qglRectsv                    = dllRectsv                    = GPA(PFNGLRECTSV, "glRectsv" );
	qglRenderMode                = dllRenderMode                = GPA(PFNRENDERMODE, "glRenderMode" );
	qglRotated                   = dllRotated                   = GPA(PFNGLROTATED, "glRotated" );
	qglRotatef                   = dllRotatef                   = GPA(PFNGLROTATEF, "glRotatef" );
	qglScaled                    = dllScaled                    = GPA(PFNGLSCALED, "glScaled" );
	qglScalef                    = dllScalef                    = GPA(PFNGLSCALEF, "glScalef" );
	qglScissor                   = dllScissor                   = GPA(PFNGLSCISSOR, "glScissor" );
	qglSelectBuffer              = dllSelectBuffer              = GPA(PFNGLSELECTBUFFER, "glSelectBuffer" );
	qglShadeModel                = dllShadeModel                = GPA(PFNGLSHADEMODEL, "glShadeModel" );
	qglStencilFunc               = dllStencilFunc               = GPA(PFNGLSTENCILFUNC, "glStencilFunc" );
	qglStencilMask               = dllStencilMask               = GPA(PFNGLSTENCILMASK, "glStencilMask" );
	qglStencilOp                 = dllStencilOp                 = GPA(PFNGLSTENCILOP, "glStencilOp" );
	qglTexCoord1d                = dllTexCoord1d                = GPA(PFNGLTEXCOORD1D, "glTexCoord1d" );
	qglTexCoord1dv               = dllTexCoord1dv               = GPA(PFNGLTEXCOORD1DV, "glTexCoord1dv" );
	qglTexCoord1f                = dllTexCoord1f                = GPA(PFNGLTEXCOORD1F, "glTexCoord1f" );
	qglTexCoord1fv               = dllTexCoord1fv               = GPA(PFNGLTEXCOORD1FV, "glTexCoord1fv" );
	qglTexCoord1i                = dllTexCoord1i                = GPA(PFNGLTEXCOORD1I, "glTexCoord1i" );
	qglTexCoord1iv               = dllTexCoord1iv               = GPA(PFNGLTEXCOORD1IV, "glTexCoord1iv" );
	qglTexCoord1s                = dllTexCoord1s                = GPA(PFNGLTEXCOORD1S, "glTexCoord1s" );
	qglTexCoord1sv               = dllTexCoord1sv               = GPA(PFNGLTEXCOORD1SV, "glTexCoord1sv" );
	qglTexCoord2d                = dllTexCoord2d                = GPA(PFNGLTEXCOORD2D, "glTexCoord2d" );
	qglTexCoord2dv               = dllTexCoord2dv               = GPA(PFNGLTEXCOORD2DV, "glTexCoord2dv" );
	qglTexCoord2f                = dllTexCoord2f                = GPA(PFNGLTEXCOORD2F, "glTexCoord2f" );
	qglTexCoord2fv               = dllTexCoord2fv               = GPA(PFNGLTEXCOORD2FV, "glTexCoord2fv" );
	qglTexCoord2i                = dllTexCoord2i                = GPA(PFNGLTEXCOORD2I, "glTexCoord2i" );
	qglTexCoord2iv               = dllTexCoord2iv               = GPA(PFNGLTEXCOORD2IV, "glTexCoord2iv" );
	qglTexCoord2s                = dllTexCoord2s                = GPA(PFNGLTEXCOORD2S, "glTexCoord2s" );
	qglTexCoord2sv               = dllTexCoord2sv               = GPA(PFNGLTEXCOORD2SV, "glTexCoord2sv" );
	qglTexCoord3d                = dllTexCoord3d                = GPA(PFNGLTEXCOORD3D, "glTexCoord3d" );
	qglTexCoord3dv               = dllTexCoord3dv               = GPA(PFNGLTEXCOORD3DV, "glTexCoord3dv" );
	qglTexCoord3f                = dllTexCoord3f                = GPA(PFNGLTEXCOORD3F, "glTexCoord3f" );
	qglTexCoord3fv               = dllTexCoord3fv               = GPA(PFNGLTEXCOORD3FV, "glTexCoord3fv" );
	qglTexCoord3i                = dllTexCoord3i                = GPA(PFNGLTEXCOORD3I, "glTexCoord3i" );
	qglTexCoord3iv               = dllTexCoord3iv               = GPA(PFNGLTEXCOORD3IV, "glTexCoord3iv" );
	qglTexCoord3s                = dllTexCoord3s                = GPA(PFNGLTEXCOORD3S, "glTexCoord3s" );
	qglTexCoord3sv               = dllTexCoord3sv               = GPA(PFNGLTEXCOORD3SV, "glTexCoord3sv" );
	qglTexCoord4d                = dllTexCoord4d                = GPA(PFNGLTEXCOORD4D, "glTexCoord4d" );
	qglTexCoord4dv               = dllTexCoord4dv               = GPA(PFNGLTEXCOORD4DV, "glTexCoord4dv" );
	qglTexCoord4f                = dllTexCoord4f                = GPA(PFNGLTEXCOORD4F, "glTexCoord4f" );
	qglTexCoord4fv               = dllTexCoord4fv               = GPA(PFNGLTEXCOORD4FV, "glTexCoord4fv" );
	qglTexCoord4i                = dllTexCoord4i                = GPA(PFNGLTEXCOORD4I, "glTexCoord4i" );
	qglTexCoord4iv               = dllTexCoord4iv               = GPA(PFNGLTEXCOORD4IV, "glTexCoord4iv" );
	qglTexCoord4s                = dllTexCoord4s                = GPA(PFNGLTEXCOORD4S, "glTexCoord4s" );
	qglTexCoord4sv               = dllTexCoord4sv               = GPA(PFNGLTEXCOORD4SV, "glTexCoord4sv" );
	qglTexCoordPointer           = dllTexCoordPointer           = GPA(PFNGLTEXCOORDPOINTER, "glTexCoordPointer" );
	qglTexEnvf                   = dllTexEnvf                   = GPA(PFNGLTEXENVF, "glTexEnvf" );
	qglTexEnvfv                  = dllTexEnvfv                  = GPA(PFNGLTEXENVFV, "glTexEnvfv" );
	qglTexEnvi                   = dllTexEnvi                   = GPA(PFNGLTEXENVI, "glTexEnvi" );
	qglTexEnviv                  = dllTexEnviv                  = GPA(PFNGLTEXENVIV, "glTexEnviv" );
	qglTexGend                   = dllTexGend                   = GPA(PFNGLTEXGEND, "glTexGend" );
	qglTexGendv                  = dllTexGendv                  = GPA(PFNGLTEXGENDV, "glTexGendv" );
	qglTexGenf                   = dllTexGenf                   = GPA(PFNGLTEXGENF, "glTexGenf" );
	qglTexGenfv                  = dllTexGenfv                  = GPA(PFNGLTEXGENFV, "glTexGenfv" );
	qglTexGeni                   = dllTexGeni                   = GPA(PFNGLTEXGENI, "glTexGeni" );
	qglTexGeniv                  = dllTexGeniv                  = GPA(PFNGLTEXGENIV, "glTexGeniv" );
	qglTexImage1D                = dllTexImage1D                = GPA(PFNGLTEXIMAGE1D, "glTexImage1D" );
	qglTexImage2D                = dllTexImage2D                = GPA(PFNGLTEXIMAGE2D, "glTexImage2D" );
	qglTexParameterf             = dllTexParameterf             = GPA(PFNGLTEXPARAMETERF, "glTexParameterf" );
	qglTexParameterfv            = dllTexParameterfv            = GPA(PFNGLTEXPARAMETERFV, "glTexParameterfv" );
	qglTexParameteri             = dllTexParameteri             = GPA(PFNGLTEXPARAMETERI, "glTexParameteri" );
	qglTexParameteriv            = dllTexParameteriv            = GPA(PFNGLTEXPARAMETERIV, "glTexParameteriv" );
	qglTexSubImage1D             = dllTexSubImage1D             = GPA(PFNGLTEXSUBIMAGE1D, "glTexSubImage1D" );
	qglTexSubImage2D             = dllTexSubImage2D             = GPA(PFNGLTEXSUBIMAGE2D, "glTexSubImage2D" );
	qglTranslated                = dllTranslated                = GPA(PFNGLTRANSLATED, "glTranslated" );
	qglTranslatef                = dllTranslatef                = GPA(PFNGLTRANSLATEF, "glTranslatef" );
	qglVertex2d                  = dllVertex2d                  = GPA(PFNGLVERTEX2D, "glVertex2d" );
	qglVertex2dv                 = dllVertex2dv                 = GPA(PFNGLVERTEX2DV, "glVertex2dv" );
	qglVertex2f                  = dllVertex2f                  = GPA(PFNGLVERTEX2F, "glVertex2f" );
	qglVertex2fv                 = dllVertex2fv                 = GPA(PFNGLVERTEX2FV, "glVertex2fv" );
	qglVertex2i                  = dllVertex2i                  = GPA(PFNGLVERTEX2I, "glVertex2i" );
	qglVertex2iv                 = dllVertex2iv                 = GPA(PFNGLVERTEX2IV, "glVertex2iv" );
	qglVertex2s                  = dllVertex2s                  = GPA(PFNGLVERTEX2S, "glVertex2s" );
	qglVertex2sv                 = dllVertex2sv                 = GPA(PFNGLVERTEX2SV, "glVertex2sv" );
	qglVertex3d                  = dllVertex3d                  = GPA(PFNGLVERTEX3D, "glVertex3d" );
	qglVertex3dv                 = dllVertex3dv                 = GPA(PFNGLVERTEX3DV, "glVertex3dv" );
	qglVertex3f                  = dllVertex3f                  = GPA(PFNGLVERTEX3F, "glVertex3f" );
	qglVertex3fv                 = dllVertex3fv                 = GPA(PFNGLVERTEX3FV, "glVertex3fv" );
	qglVertex3i                  = dllVertex3i                  = GPA(PFNGLVERTEX3I, "glVertex3i" );
	qglVertex3iv                 = dllVertex3iv                 = GPA(PFNGLVERTEX3IV, "glVertex3iv" );
	qglVertex3s                  = dllVertex3s                  = GPA(PFNGLVERTEX3S, "glVertex3s" );
	qglVertex3sv                 = dllVertex3sv                 = GPA(PFNGLVERTEX3SV, "glVertex3sv" );
	qglVertex4d                  = dllVertex4d                  = GPA(PFNGLVERTEX4D, "glVertex4d" );
	qglVertex4dv                 = dllVertex4dv                 = GPA(PFNGLVERTEX4DV, "glVertex4dv" );
	qglVertex4f                  = dllVertex4f                  = GPA(PFNGLVERTEX4F, "glVertex4f" );
	qglVertex4fv                 = dllVertex4fv                 = GPA(PFNGLVERTEX4FV, "glVertex4fv" );
	qglVertex4i                  = dllVertex4i                  = GPA(PFNGLVERTEX4I, "glVertex4i" );
	qglVertex4iv                 = dllVertex4iv                 = GPA(PFNGLVERTEX4IV, "glVertex4iv" );
	qglVertex4s                  = dllVertex4s                  = GPA(PFNGLVERTEX4S, "glVertex4s" );
	qglVertex4sv                 = dllVertex4sv                 = GPA(PFNGLVERTEX4SV, "glVertex4sv" );
	qglVertexPointer             = dllVertexPointer             = GPA(PFNGLVERTEXPOINTER, "glVertexPointer" );
	qglViewport                  = dllViewport                  = GPA(PFNGLVIEWPORT, "glViewport" );

	// check logging
	QGL_EnableLogging(r_logFile->integer);

	return true;
}

void QGL_EnableLogging(bool enable)
{
	static bool isEnabled;

	// return if we're already active
	if (isEnabled && enable) {
		// decrement log counter and stop if it has reached 0
		ri.Cvar_Set("r_logFile", va("%d", r_logFile->integer - 1));
		if (r_logFile->integer) {
			return;
		}
		enable = false;
	}

	// return if we're already disabled
	if (!enable && !isEnabled) {
		return;
	}

	isEnabled = enable;

	if (enable) {
		if (!glw_state.log_fp) {

			struct tm* newtime;
			time_t aclock;
			char buffer[1024];
			cvar_t* basedir;

			time(&aclock);
			newtime = localtime(&aclock);

			asctime(newtime);

			basedir = ri.Cvar_Get("fs_basepath", "", 0);
			Com_sprintf(buffer, sizeof(buffer), "%s/gl.log", basedir->string);
			glw_state.log_fp = fopen(buffer, "wt");

			fprintf(glw_state.log_fp, "%s\n", asctime(newtime));
		}

		qglAccum                     = logAccum;
		qglAlphaFunc                 = logAlphaFunc;
		qglAreTexturesResident       = logAreTexturesResident;
		qglArrayElement              = logArrayElement;
		qglBegin                     = logBegin;
		qglBindTexture               = logBindTexture;
		qglBitmap                    = logBitmap;
		qglBlendFunc                 = logBlendFunc;
		qglCallList                  = logCallList;
		qglCallLists                 = logCallLists;
		qglClear                     = logClear;
		qglClearAccum                = logClearAccum;
		qglClearColor                = logClearColor;
		qglClearDepth                = logClearDepth;
		qglClearIndex                = logClearIndex;
		qglClearStencil              = logClearStencil;
		qglClipPlane                 = logClipPlane;
		qglColor3b                   = logColor3b;
		qglColor3bv                  = logColor3bv;
		qglColor3d                   = logColor3d;
		qglColor3dv                  = logColor3dv;
		qglColor3f                   = logColor3f;
		qglColor3fv                  = logColor3fv;
		qglColor3i                   = logColor3i;
		qglColor3iv                  = logColor3iv;
		qglColor3s                   = logColor3s;
		qglColor3sv                  = logColor3sv;
		qglColor3ub                  = logColor3ub;
		qglColor3ubv                 = logColor3ubv;
		qglColor3ui                  = logColor3ui;
		qglColor3uiv                 = logColor3uiv;
		qglColor3us                  = logColor3us;
		qglColor3usv                 = logColor3usv;
		qglColor4b                   = logColor4b;
		qglColor4bv                  = logColor4bv;
		qglColor4d                   = logColor4d;
		qglColor4dv                  = logColor4dv;
		qglColor4f                   = logColor4f;
		qglColor4fv                  = logColor4fv;
		qglColor4i                   = logColor4i;
		qglColor4iv                  = logColor4iv;
		qglColor4s                   = logColor4s;
		qglColor4sv                  = logColor4sv;
		qglColor4ub                  = logColor4ub;
		qglColor4ubv                 = logColor4ubv;
		qglColor4ui                  = logColor4ui;
		qglColor4uiv                 = logColor4uiv;
		qglColor4us                  = logColor4us;
		qglColor4usv                 = logColor4usv;
		qglColorMask                 = logColorMask;
		qglColorMaterial             = logColorMaterial;
		qglColorPointer              = logColorPointer;
		qglCopyPixels                = logCopyPixels;
		qglCopyTexImage1D            = logCopyTexImage1D;
		qglCopyTexImage2D            = logCopyTexImage2D;
		qglCopyTexSubImage1D         = logCopyTexSubImage1D;
		qglCopyTexSubImage2D         = logCopyTexSubImage2D;
		qglCullFace                  = logCullFace;
		qglDeleteLists               = logDeleteLists;
		qglDeleteTextures            = logDeleteTextures;
		qglDepthFunc                 = logDepthFunc;
		qglDepthMask                 = logDepthMask;
		qglDepthRange                = logDepthRange;
		qglDisable                   = logDisable;
		qglDisableClientState        = logDisableClientState;
		qglDrawArrays                = logDrawArrays;
		qglDrawBuffer                = logDrawBuffer;
		qglDrawElements              = logDrawElements;
		qglDrawPixels                = logDrawPixels;
		qglEdgeFlag                  = logEdgeFlag;
		qglEdgeFlagPointer           = logEdgeFlagPointer;
		qglEdgeFlagv                 = logEdgeFlagv;
		qglEnable                    = logEnable;
		qglEnableClientState         = logEnableClientState;
		qglEnd                       = logEnd;
		qglEndList                   = logEndList;
		qglEvalCoord1d				 = logEvalCoord1d;
		qglEvalCoord1dv              = logEvalCoord1dv;
		qglEvalCoord1f               = logEvalCoord1f;
		qglEvalCoord1fv              = logEvalCoord1fv;
		qglEvalCoord2d               = logEvalCoord2d;
		qglEvalCoord2dv              = logEvalCoord2dv;
		qglEvalCoord2f               = logEvalCoord2f;
		qglEvalCoord2fv              = logEvalCoord2fv;
		qglEvalMesh1                 = logEvalMesh1;
		qglEvalMesh2                 = logEvalMesh2;
		qglEvalPoint1                = logEvalPoint1;
		qglEvalPoint2                = logEvalPoint2;
		qglFeedbackBuffer            = logFeedbackBuffer;
		qglFinish                    = logFinish;
		qglFlush                     = logFlush;
		qglFogf                      = logFogf;
		qglFogfv                     = logFogfv;
		qglFogi                      = logFogi;
		qglFogiv                     = logFogiv;
		qglFrontFace                 = logFrontFace;
		qglFrustum                   = logFrustum;
		qglGenLists                  = logGenLists;
		qglGenTextures               = logGenTextures;
		qglGetBooleanv               = logGetBooleanv;
		qglGetClipPlane              = logGetClipPlane;
		qglGetDoublev                = logGetDoublev;
		qglGetError                  = logGetError;
		qglGetFloatv                 = logGetFloatv;
		qglGetIntegerv               = logGetIntegerv;
		qglGetLightfv                = logGetLightfv;
		qglGetLightiv                = logGetLightiv;
		qglGetMapdv                  = logGetMapdv;
		qglGetMapfv                  = logGetMapfv;
		qglGetMapiv                  = logGetMapiv;
		qglGetMaterialfv             = logGetMaterialfv;
		qglGetMaterialiv             = logGetMaterialiv;
		qglGetPixelMapfv             = logGetPixelMapfv;
		qglGetPixelMapuiv            = logGetPixelMapuiv;
		qglGetPixelMapusv            = logGetPixelMapusv;
		qglGetPointerv               = logGetPointerv;
		qglGetPolygonStipple         = logGetPolygonStipple;
		qglGetString                 = logGetString;
		qglGetTexEnvfv               = logGetTexEnvfv;
		qglGetTexEnviv               = logGetTexEnviv;
		qglGetTexGendv               = logGetTexGendv;
		qglGetTexGenfv               = logGetTexGenfv;
		qglGetTexGeniv               = logGetTexGeniv;
		qglGetTexImage               = logGetTexImage;
		qglGetTexLevelParameterfv    = logGetTexLevelParameterfv;
		qglGetTexLevelParameteriv    = logGetTexLevelParameteriv;
		qglGetTexParameterfv         = logGetTexParameterfv;
		qglGetTexParameteriv         = logGetTexParameteriv;
		qglHint                      = logHint;
		qglIndexMask                 = logIndexMask;
		qglIndexPointer              = logIndexPointer;
		qglIndexd                    = logIndexd;
		qglIndexdv                   = logIndexdv;
		qglIndexf                    = logIndexf;
		qglIndexfv                   = logIndexfv;
		qglIndexi                    = logIndexi;
		qglIndexiv                   = logIndexiv;
		qglIndexs                    = logIndexs;
		qglIndexsv                   = logIndexsv;
		qglIndexub                   = logIndexub;
		qglIndexubv                  = logIndexubv;
		qglInitNames                 = logInitNames;
		qglInterleavedArrays         = logInterleavedArrays;
		qglIsEnabled                 = logIsEnabled;
		qglIsList                    = logIsList;
		qglIsTexture                 = logIsTexture;
		qglLightModelf               = logLightModelf;
		qglLightModelfv              = logLightModelfv;
		qglLightModeli               = logLightModeli;
		qglLightModeliv              = logLightModeliv;
		qglLightf                    = logLightf;
		qglLightfv                   = logLightfv;
		qglLighti                    = logLighti;
		qglLightiv                   = logLightiv;
		qglLineStipple               = logLineStipple;
		qglLineWidth                 = logLineWidth;
		qglListBase                  = logListBase;
		qglLoadIdentity              = logLoadIdentity;
		qglLoadMatrixd               = logLoadMatrixd;
		qglLoadMatrixf               = logLoadMatrixf;
		qglLoadName                  = logLoadName;
		qglLogicOp                   = logLogicOp;
		qglMap1d                     = logMap1d;
		qglMap1f                     = logMap1f;
		qglMap2d                     = logMap2d;
		qglMap2f                     = logMap2f;
		qglMapGrid1d                 = logMapGrid1d;
		qglMapGrid1f                 = logMapGrid1f;
		qglMapGrid2d                 = logMapGrid2d;
		qglMapGrid2f                 = logMapGrid2f;
		qglMaterialf                 = logMaterialf;
		qglMaterialfv                = logMaterialfv;
		qglMateriali                 = logMateriali;
		qglMaterialiv                = logMaterialiv;
		qglMatrixMode                = logMatrixMode;
		qglMultMatrixd               = logMultMatrixd;
		qglMultMatrixf               = logMultMatrixf;
		qglNewList                   = logNewList;
		qglNormal3b                  = logNormal3b;
		qglNormal3bv                 = logNormal3bv;
		qglNormal3d                  = logNormal3d;
		qglNormal3dv                 = logNormal3dv;
		qglNormal3f                  = logNormal3f;
		qglNormal3fv                 = logNormal3fv;
		qglNormal3i                  = logNormal3i;
		qglNormal3iv                 = logNormal3iv;
		qglNormal3s                  = logNormal3s;
		qglNormal3sv                 = logNormal3sv;
		qglNormalPointer             = logNormalPointer;
		qglOrtho                     = logOrtho;
		qglPassThrough               = logPassThrough;
		qglPixelMapfv                = logPixelMapfv;
		qglPixelMapuiv               = logPixelMapuiv;
		qglPixelMapusv               = logPixelMapusv;
		qglPixelStoref               = logPixelStoref;
		qglPixelStorei               = logPixelStorei;
		qglPixelTransferf            = logPixelTransferf;
		qglPixelTransferi            = logPixelTransferi;
		qglPixelZoom                 = logPixelZoom;
		qglPointSize                 = logPointSize;
		qglPolygonMode               = logPolygonMode;
		qglPolygonOffset             = logPolygonOffset;
		qglPolygonStipple            = logPolygonStipple;
		qglPopAttrib                 = logPopAttrib;
		qglPopClientAttrib           = logPopClientAttrib;
		qglPopMatrix                 = logPopMatrix;
		qglPopName                   = logPopName;
		qglPrioritizeTextures        = logPrioritizeTextures;
		qglPushAttrib                = logPushAttrib;
		qglPushClientAttrib          = logPushClientAttrib;
		qglPushMatrix                = logPushMatrix;
		qglPushName                  = logPushName;
		qglRasterPos2d               = logRasterPos2d;
		qglRasterPos2dv              = logRasterPos2dv;
		qglRasterPos2f               = logRasterPos2f;
		qglRasterPos2fv              = logRasterPos2fv;
		qglRasterPos2i               = logRasterPos2i;
		qglRasterPos2iv              = logRasterPos2iv;
		qglRasterPos2s               = logRasterPos2s;
		qglRasterPos2sv              = logRasterPos2sv;
		qglRasterPos3d               = logRasterPos3d;
		qglRasterPos3dv              = logRasterPos3dv;
		qglRasterPos3f               = logRasterPos3f;
		qglRasterPos3fv              = logRasterPos3fv;
		qglRasterPos3i               = logRasterPos3i;
		qglRasterPos3iv              = logRasterPos3iv;
		qglRasterPos3s               = logRasterPos3s;
		qglRasterPos3sv              = logRasterPos3sv;
		qglRasterPos4d               = logRasterPos4d;
		qglRasterPos4dv              = logRasterPos4dv;
		qglRasterPos4f               = logRasterPos4f;
		qglRasterPos4fv              = logRasterPos4fv;
		qglRasterPos4i               = logRasterPos4i;
		qglRasterPos4iv              = logRasterPos4iv;
		qglRasterPos4s               = logRasterPos4s;
		qglRasterPos4sv              = logRasterPos4sv;
		qglReadBuffer                = logReadBuffer;
		qglReadPixels                = logReadPixels;
		qglRectd                     = logRectd;
		qglRectdv                    = logRectdv;
		qglRectf                     = logRectf;
		qglRectfv                    = logRectfv;
		qglRecti                     = logRecti;
		qglRectiv                    = logRectiv;
		qglRects                     = logRects;
		qglRectsv                    = logRectsv;
		qglRenderMode                = logRenderMode;
		qglRotated                   = logRotated;
		qglRotatef                   = logRotatef;
		qglScaled                    = logScaled;
		qglScalef                    = logScalef;
		qglScissor                   = logScissor;
		qglSelectBuffer              = logSelectBuffer;
		qglShadeModel                = logShadeModel;
		qglStencilFunc               = logStencilFunc;
		qglStencilMask               = logStencilMask;
		qglStencilOp                 = logStencilOp;
		qglTexCoord1d                = logTexCoord1d;
		qglTexCoord1dv               = logTexCoord1dv;
		qglTexCoord1f                = logTexCoord1f;
		qglTexCoord1fv               = logTexCoord1fv;
		qglTexCoord1i                = logTexCoord1i;
		qglTexCoord1iv               = logTexCoord1iv;
		qglTexCoord1s                = logTexCoord1s;
		qglTexCoord1sv               = logTexCoord1sv;
		qglTexCoord2d                = logTexCoord2d;
		qglTexCoord2dv               = logTexCoord2dv;
		qglTexCoord2f                = logTexCoord2f;
		qglTexCoord2fv               = logTexCoord2fv;
		qglTexCoord2i                = logTexCoord2i;
		qglTexCoord2iv               = logTexCoord2iv;
		qglTexCoord2s                = logTexCoord2s;
		qglTexCoord2sv               = logTexCoord2sv;
		qglTexCoord3d                = logTexCoord3d;
		qglTexCoord3dv               = logTexCoord3dv;
		qglTexCoord3f                = logTexCoord3f;
		qglTexCoord3fv               = logTexCoord3fv;
		qglTexCoord3i                = logTexCoord3i;
		qglTexCoord3iv               = logTexCoord3iv;
		qglTexCoord3s                = logTexCoord3s;
		qglTexCoord3sv               = logTexCoord3sv;
		qglTexCoord4d                = logTexCoord4d;
		qglTexCoord4dv               = logTexCoord4dv;
		qglTexCoord4f                = logTexCoord4f;
		qglTexCoord4fv               = logTexCoord4fv;
		qglTexCoord4i                = logTexCoord4i;
		qglTexCoord4iv               = logTexCoord4iv;
		qglTexCoord4s                = logTexCoord4s;
		qglTexCoord4sv               = logTexCoord4sv;
		qglTexCoordPointer           = logTexCoordPointer;
		qglTexEnvf                   = logTexEnvf;
		qglTexEnvfv                  = logTexEnvfv;
		qglTexEnvi                   = logTexEnvi;
		qglTexEnviv                  = logTexEnviv;
		qglTexGend                   = logTexGend;
		qglTexGendv                  = logTexGendv;
		qglTexGenf                   = logTexGenf;
		qglTexGenfv                  = logTexGenfv;
		qglTexGeni                   = logTexGeni;
		qglTexGeniv                  = logTexGeniv;
		qglTexImage1D                = logTexImage1D;
		qglTexImage2D                = logTexImage2D;
		qglTexParameterf             = logTexParameterf;
		qglTexParameterfv            = logTexParameterfv;
		qglTexParameteri             = logTexParameteri;
		qglTexParameteriv            = logTexParameteriv;
		qglTexSubImage1D             = logTexSubImage1D;
		qglTexSubImage2D             = logTexSubImage2D;
		qglTranslated                = logTranslated;
		qglTranslatef                = logTranslatef;
		qglVertex2d                  = logVertex2d;
		qglVertex2dv                 = logVertex2dv;
		qglVertex2f                  = logVertex2f;
		qglVertex2fv                 = logVertex2fv;
		qglVertex2i                  = logVertex2i;
		qglVertex2iv                 = logVertex2iv;
		qglVertex2s                  = logVertex2s;
		qglVertex2sv                 = logVertex2sv;
		qglVertex3d                  = logVertex3d;
		qglVertex3dv                 = logVertex3dv;
		qglVertex3f                  = logVertex3f;
		qglVertex3fv                 = logVertex3fv;
		qglVertex3i                  = logVertex3i;
		qglVertex3iv                 = logVertex3iv;
		qglVertex3s                  = logVertex3s;
		qglVertex3sv                 = logVertex3sv;
		qglVertex4d                  = logVertex4d;
		qglVertex4dv                 = logVertex4dv;
		qglVertex4f                  = logVertex4f;
		qglVertex4fv                 = logVertex4fv;
		qglVertex4i                  = logVertex4i;
		qglVertex4iv                 = logVertex4iv;
		qglVertex4s                  = logVertex4s;
		qglVertex4sv                 = logVertex4sv;
		qglVertexPointer             = logVertexPointer;
		qglViewport                  = logViewport;

	} else {
		
		if (glw_state.log_fp) {
			fprintf(glw_state.log_fp, "*** CLOSING LOG ***\n");
			fclose(glw_state.log_fp);
			glw_state.log_fp = NULL;
		}

		qglAccum                     = dllAccum;
		qglAlphaFunc                 = dllAlphaFunc;
		qglAreTexturesResident       = dllAreTexturesResident;
		qglArrayElement              = dllArrayElement;
		qglBegin                     = dllBegin;
		qglBindTexture               = dllBindTexture;
		qglBitmap                    = dllBitmap;
		qglBlendFunc                 = dllBlendFunc;
		qglCallList                  = dllCallList;
		qglCallLists                 = dllCallLists;
		qglClear                     = dllClear;
		qglClearAccum                = dllClearAccum;
		qglClearColor                = dllClearColor;
		qglClearDepth                = dllClearDepth;
		qglClearIndex                = dllClearIndex;
		qglClearStencil              = dllClearStencil;
		qglClipPlane                 = dllClipPlane;
		qglColor3b                   = dllColor3b;
		qglColor3bv                  = dllColor3bv;
		qglColor3d                   = dllColor3d;
		qglColor3dv                  = dllColor3dv;
		qglColor3f                   = dllColor3f;
		qglColor3fv                  = dllColor3fv;
		qglColor3i                   = dllColor3i;
		qglColor3iv                  = dllColor3iv;
		qglColor3s                   = dllColor3s;
		qglColor3sv                  = dllColor3sv;
		qglColor3ub                  = dllColor3ub;
		qglColor3ubv                 = dllColor3ubv;
		qglColor3ui                  = dllColor3ui;
		qglColor3uiv                 = dllColor3uiv;
		qglColor3us                  = dllColor3us;
		qglColor3usv                 = dllColor3usv;
		qglColor4b                   = dllColor4b;
		qglColor4bv                  = dllColor4bv;
		qglColor4d                   = dllColor4d;
		qglColor4dv                  = dllColor4dv;
		qglColor4f                   = dllColor4f;
		qglColor4fv                  = dllColor4fv;
		qglColor4i                   = dllColor4i;
		qglColor4iv                  = dllColor4iv;
		qglColor4s                   = dllColor4s;
		qglColor4sv                  = dllColor4sv;
		qglColor4ub                  = dllColor4ub;
		qglColor4ubv                 = dllColor4ubv;
		qglColor4ui                  = dllColor4ui;
		qglColor4uiv                 = dllColor4uiv;
		qglColor4us                  = dllColor4us;
		qglColor4usv                 = dllColor4usv;
		qglColorMask                 = dllColorMask;
		qglColorMaterial             = dllColorMaterial;
		qglColorPointer              = dllColorPointer;
		qglCopyPixels                = dllCopyPixels;
		qglCopyTexImage1D            = dllCopyTexImage1D;
		qglCopyTexImage2D            = dllCopyTexImage2D;
		qglCopyTexSubImage1D         = dllCopyTexSubImage1D;
		qglCopyTexSubImage2D         = dllCopyTexSubImage2D;
		qglCullFace                  = dllCullFace;
		qglDeleteLists               = dllDeleteLists;
		qglDeleteTextures            = dllDeleteTextures;
		qglDepthFunc                 = dllDepthFunc;
		qglDepthMask                 = dllDepthMask;
		qglDepthRange                = dllDepthRange;
		qglDisable                   = dllDisable;
		qglDisableClientState        = dllDisableClientState;
		qglDrawArrays                = dllDrawArrays;
		qglDrawBuffer                = dllDrawBuffer;
		qglDrawElements              = dllDrawElements;
		qglDrawPixels                = dllDrawPixels;
		qglEdgeFlag                  = dllEdgeFlag;
		qglEdgeFlagPointer           = dllEdgeFlagPointer;
		qglEdgeFlagv                 = dllEdgeFlagv;
		qglEnable                    = dllEnable;
		qglEnableClientState         = dllEnableClientState;
		qglEnd                       = dllEnd;
		qglEndList                   = dllEndList;
		qglEvalCoord1d				 = dllEvalCoord1d;
		qglEvalCoord1dv              = dllEvalCoord1dv;
		qglEvalCoord1f               = dllEvalCoord1f;
		qglEvalCoord1fv              = dllEvalCoord1fv;
		qglEvalCoord2d               = dllEvalCoord2d;
		qglEvalCoord2dv              = dllEvalCoord2dv;
		qglEvalCoord2f               = dllEvalCoord2f;
		qglEvalCoord2fv              = dllEvalCoord2fv;
		qglEvalMesh1                 = dllEvalMesh1;
		qglEvalMesh2                 = dllEvalMesh2;
		qglEvalPoint1                = dllEvalPoint1;
		qglEvalPoint2                = dllEvalPoint2;
		qglFeedbackBuffer            = dllFeedbackBuffer;
		qglFinish                    = dllFinish;
		qglFlush                     = dllFlush;
		qglFogf                      = dllFogf;
		qglFogfv                     = dllFogfv;
		qglFogi                      = dllFogi;
		qglFogiv                     = dllFogiv;
		qglFrontFace                 = dllFrontFace;
		qglFrustum                   = dllFrustum;
		qglGenLists                  = dllGenLists;
		qglGenTextures               = dllGenTextures;
		qglGetBooleanv               = dllGetBooleanv;
		qglGetClipPlane              = dllGetClipPlane;
		qglGetDoublev                = dllGetDoublev;
		qglGetError                  = dllGetError;
		qglGetFloatv                 = dllGetFloatv;
		qglGetIntegerv               = dllGetIntegerv;
		qglGetLightfv                = dllGetLightfv;
		qglGetLightiv                = dllGetLightiv;
		qglGetMapdv                  = dllGetMapdv;
		qglGetMapfv                  = dllGetMapfv;
		qglGetMapiv                  = dllGetMapiv;
		qglGetMaterialfv             = dllGetMaterialfv;
		qglGetMaterialiv             = dllGetMaterialiv;
		qglGetPixelMapfv             = dllGetPixelMapfv;
		qglGetPixelMapuiv            = dllGetPixelMapuiv;
		qglGetPixelMapusv            = dllGetPixelMapusv;
		qglGetPointerv               = dllGetPointerv;
		qglGetPolygonStipple         = dllGetPolygonStipple;
		qglGetString                 = dllGetString;
		qglGetTexEnvfv               = dllGetTexEnvfv;
		qglGetTexEnviv               = dllGetTexEnviv;
		qglGetTexGendv               = dllGetTexGendv;
		qglGetTexGenfv               = dllGetTexGenfv;
		qglGetTexGeniv               = dllGetTexGeniv;
		qglGetTexImage               = dllGetTexImage;
		qglGetTexLevelParameterfv    = dllGetTexLevelParameterfv;
		qglGetTexLevelParameteriv    = dllGetTexLevelParameteriv;
		qglGetTexParameterfv         = dllGetTexParameterfv;
		qglGetTexParameteriv         = dllGetTexParameteriv;
		qglHint                      = dllHint;
		qglIndexMask                 = dllIndexMask;
		qglIndexPointer              = dllIndexPointer;
		qglIndexd                    = dllIndexd;
		qglIndexdv                   = dllIndexdv;
		qglIndexf                    = dllIndexf;
		qglIndexfv                   = dllIndexfv;
		qglIndexi                    = dllIndexi;
		qglIndexiv                   = dllIndexiv;
		qglIndexs                    = dllIndexs;
		qglIndexsv                   = dllIndexsv;
		qglIndexub                   = dllIndexub;
		qglIndexubv                  = dllIndexubv;
		qglInitNames                 = dllInitNames;
		qglInterleavedArrays         = dllInterleavedArrays;
		qglIsEnabled                 = dllIsEnabled;
		qglIsList                    = dllIsList;
		qglIsTexture                 = dllIsTexture;
		qglLightModelf               = dllLightModelf;
		qglLightModelfv              = dllLightModelfv;
		qglLightModeli               = dllLightModeli;
		qglLightModeliv              = dllLightModeliv;
		qglLightf                    = dllLightf;
		qglLightfv                   = dllLightfv;
		qglLighti                    = dllLighti;
		qglLightiv                   = dllLightiv;
		qglLineStipple               = dllLineStipple;
		qglLineWidth                 = dllLineWidth;
		qglListBase                  = dllListBase;
		qglLoadIdentity              = dllLoadIdentity;
		qglLoadMatrixd               = dllLoadMatrixd;
		qglLoadMatrixf               = dllLoadMatrixf;
		qglLoadName                  = dllLoadName;
		qglLogicOp                   = dllLogicOp;
		qglMap1d                     = dllMap1d;
		qglMap1f                     = dllMap1f;
		qglMap2d                     = dllMap2d;
		qglMap2f                     = dllMap2f;
		qglMapGrid1d                 = dllMapGrid1d;
		qglMapGrid1f                 = dllMapGrid1f;
		qglMapGrid2d                 = dllMapGrid2d;
		qglMapGrid2f                 = dllMapGrid2f;
		qglMaterialf                 = dllMaterialf;
		qglMaterialfv                = dllMaterialfv;
		qglMateriali                 = dllMateriali;
		qglMaterialiv                = dllMaterialiv;
		qglMatrixMode                = dllMatrixMode;
		qglMultMatrixd               = dllMultMatrixd;
		qglMultMatrixf               = dllMultMatrixf;
		qglNewList                   = dllNewList;
		qglNormal3b                  = dllNormal3b;
		qglNormal3bv                 = dllNormal3bv;
		qglNormal3d                  = dllNormal3d;
		qglNormal3dv                 = dllNormal3dv;
		qglNormal3f                  = dllNormal3f;
		qglNormal3fv                 = dllNormal3fv;
		qglNormal3i                  = dllNormal3i;
		qglNormal3iv                 = dllNormal3iv;
		qglNormal3s                  = dllNormal3s;
		qglNormal3sv                 = dllNormal3sv;
		qglNormalPointer             = dllNormalPointer;
		qglOrtho                     = dllOrtho;
		qglPassThrough               = dllPassThrough;
		qglPixelMapfv                = dllPixelMapfv;
		qglPixelMapuiv               = dllPixelMapuiv;
		qglPixelMapusv               = dllPixelMapusv;
		qglPixelStoref               = dllPixelStoref;
		qglPixelStorei               = dllPixelStorei;
		qglPixelTransferf            = dllPixelTransferf;
		qglPixelTransferi            = dllPixelTransferi;
		qglPixelZoom                 = dllPixelZoom;
		qglPointSize                 = dllPointSize;
		qglPolygonMode               = dllPolygonMode;
		qglPolygonOffset             = dllPolygonOffset;
		qglPolygonStipple            = dllPolygonStipple;
		qglPopAttrib                 = dllPopAttrib;
		qglPopClientAttrib           = dllPopClientAttrib;
		qglPopMatrix                 = dllPopMatrix;
		qglPopName                   = dllPopName;
		qglPrioritizeTextures        = dllPrioritizeTextures;
		qglPushAttrib                = dllPushAttrib;
		qglPushClientAttrib          = dllPushClientAttrib;
		qglPushMatrix                = dllPushMatrix;
		qglPushName                  = dllPushName;
		qglRasterPos2d               = dllRasterPos2d;
		qglRasterPos2dv              = dllRasterPos2dv;
		qglRasterPos2f               = dllRasterPos2f;
		qglRasterPos2fv              = dllRasterPos2fv;
		qglRasterPos2i               = dllRasterPos2i;
		qglRasterPos2iv              = dllRasterPos2iv;
		qglRasterPos2s               = dllRasterPos2s;
		qglRasterPos2sv              = dllRasterPos2sv;
		qglRasterPos3d               = dllRasterPos3d;
		qglRasterPos3dv              = dllRasterPos3dv;
		qglRasterPos3f               = dllRasterPos3f;
		qglRasterPos3fv              = dllRasterPos3fv;
		qglRasterPos3i               = dllRasterPos3i;
		qglRasterPos3iv              = dllRasterPos3iv;
		qglRasterPos3s               = dllRasterPos3s;
		qglRasterPos3sv              = dllRasterPos3sv;
		qglRasterPos4d               = dllRasterPos4d;
		qglRasterPos4dv              = dllRasterPos4dv;
		qglRasterPos4f               = dllRasterPos4f;
		qglRasterPos4fv              = dllRasterPos4fv;
		qglRasterPos4i               = dllRasterPos4i;
		qglRasterPos4iv              = dllRasterPos4iv;
		qglRasterPos4s               = dllRasterPos4s;
		qglRasterPos4sv              = dllRasterPos4sv;
		qglReadBuffer                = dllReadBuffer;
		qglReadPixels                = dllReadPixels;
		qglRectd                     = dllRectd;
		qglRectdv                    = dllRectdv;
		qglRectf                     = dllRectf;
		qglRectfv                    = dllRectfv;
		qglRecti                     = dllRecti;
		qglRectiv                    = dllRectiv;
		qglRects                     = dllRects;
		qglRectsv                    = dllRectsv;
		qglRenderMode                = dllRenderMode;
		qglRotated                   = dllRotated;
		qglRotatef                   = dllRotatef;
		qglScaled                    = dllScaled;
		qglScalef                    = dllScalef;
		qglScissor                   = dllScissor;
		qglSelectBuffer              = dllSelectBuffer;
		qglShadeModel                = dllShadeModel;
		qglStencilFunc               = dllStencilFunc;
		qglStencilMask               = dllStencilMask;
		qglStencilOp                 = dllStencilOp;
		qglTexCoord1d                = dllTexCoord1d;
		qglTexCoord1dv               = dllTexCoord1dv;
		qglTexCoord1f                = dllTexCoord1f;
		qglTexCoord1fv               = dllTexCoord1fv;
		qglTexCoord1i                = dllTexCoord1i;
		qglTexCoord1iv               = dllTexCoord1iv;
		qglTexCoord1s                = dllTexCoord1s;
		qglTexCoord1sv               = dllTexCoord1sv;
		qglTexCoord2d                = dllTexCoord2d;
		qglTexCoord2dv               = dllTexCoord2dv;
		qglTexCoord2f                = dllTexCoord2f;
		qglTexCoord2fv               = dllTexCoord2fv;
		qglTexCoord2i                = dllTexCoord2i;
		qglTexCoord2iv               = dllTexCoord2iv;
		qglTexCoord2s                = dllTexCoord2s;
		qglTexCoord2sv               = dllTexCoord2sv;
		qglTexCoord3d                = dllTexCoord3d;
		qglTexCoord3dv               = dllTexCoord3dv;
		qglTexCoord3f                = dllTexCoord3f;
		qglTexCoord3fv               = dllTexCoord3fv;
		qglTexCoord3i                = dllTexCoord3i;
		qglTexCoord3iv               = dllTexCoord3iv;
		qglTexCoord3s                = dllTexCoord3s;
		qglTexCoord3sv               = dllTexCoord3sv;
		qglTexCoord4d                = dllTexCoord4d;
		qglTexCoord4dv               = dllTexCoord4dv;
		qglTexCoord4f                = dllTexCoord4f;
		qglTexCoord4fv               = dllTexCoord4fv;
		qglTexCoord4i                = dllTexCoord4i;
		qglTexCoord4iv               = dllTexCoord4iv;
		qglTexCoord4s                = dllTexCoord4s;
		qglTexCoord4sv               = dllTexCoord4sv;
		qglTexCoordPointer           = dllTexCoordPointer;
		qglTexEnvf                   = dllTexEnvf;
		qglTexEnvfv                  = dllTexEnvfv;
		qglTexEnvi                   = dllTexEnvi;
		qglTexEnviv                  = dllTexEnviv;
		qglTexGend                   = dllTexGend;
		qglTexGendv                  = dllTexGendv;
		qglTexGenf                   = dllTexGenf;
		qglTexGenfv                  = dllTexGenfv;
		qglTexGeni                   = dllTexGeni;
		qglTexGeniv                  = dllTexGeniv;
		qglTexImage1D                = dllTexImage1D;
		qglTexImage2D                = dllTexImage2D;
		qglTexParameterf             = dllTexParameterf;
		qglTexParameterfv            = dllTexParameterfv;
		qglTexParameteri             = dllTexParameteri;
		qglTexParameteriv            = dllTexParameteriv;
		qglTexSubImage1D             = dllTexSubImage1D;
		qglTexSubImage2D             = dllTexSubImage2D;
		qglTranslated                = dllTranslated;
		qglTranslatef                = dllTranslatef;
		qglVertex2d                  = dllVertex2d;
		qglVertex2dv                 = dllVertex2dv;
		qglVertex2f                  = dllVertex2f;
		qglVertex2fv                 = dllVertex2fv;
		qglVertex2i                  = dllVertex2i;
		qglVertex2iv                 = dllVertex2iv;
		qglVertex2s                  = dllVertex2s;
		qglVertex2sv                 = dllVertex2sv;
		qglVertex3d                  = dllVertex3d;
		qglVertex3dv                 = dllVertex3dv;
		qglVertex3f                  = dllVertex3f;
		qglVertex3fv                 = dllVertex3fv;
		qglVertex3i                  = dllVertex3i;
		qglVertex3iv                 = dllVertex3iv;
		qglVertex3s                  = dllVertex3s;
		qglVertex3sv                 = dllVertex3sv;
		qglVertex4d                  = dllVertex4d;
		qglVertex4dv                 = dllVertex4dv;
		qglVertex4f                  = dllVertex4f;
		qglVertex4fv                 = dllVertex4fv;
		qglVertex4i                  = dllVertex4i;
		qglVertex4iv                 = dllVertex4iv;
		qglVertex4s                  = dllVertex4s;
		qglVertex4sv                 = dllVertex4sv;
		qglVertexPointer             = dllVertexPointer;
		qglViewport                  = dllViewport;
	}
}
