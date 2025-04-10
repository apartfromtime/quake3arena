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
// tr_font.c
// 
//
// The font system uses FreeType 2.x to render TrueType fonts for use within the game.
// As of this writing ( Nov, 2000 ) Team Arena uses these fonts for all of the ui and 
// about 90% of the cgame presentation. A few areas of the CGAME were left uses the old 
// fonts since the code is shared with standard Q3A.
//
// If you include this font rendering code in a commercial product you MUST include the
// following somewhere with your product, see www.freetype.org for specifics or changes.
// The Freetype code also uses some hinting techniques that MIGHT infringe on patents 
// held by apple so be aware of that also.
//
// As of Q3A 1.25+ and Team Arena, we are shipping the game with the font rendering code
// disabled. This removes any potential patent issues and it keeps us from having to 
// distribute an actual TrueTrype font which is 1. expensive to do and 2. seems to require
// an act of god to accomplish. 
//
// What we did was pre-render the fonts using FreeType ( which is why we leave the FreeType
// credit in the credits ) and then saved off the glyph data and then hand touched up the 
// font bitmaps so they scale a bit better in GL.
//
// There are limitations in the way fonts are saved and reloaded in that it is based on 
// point size and not name. So if you pre-render Helvetica in 18 point and Impact in 18 point
// you will end up with a single 18 point data file and image set. Typically you will want to 
// choose 3 sizes to best approximate the scaling you will be doing in the ui scripting system
// 
// In the UI Scripting code, a scale of 1.0 is equal to a 48 point font. In Team Arena, we
// use three or four scales, most of them exactly equaling the specific rendered size. We 
// rendered three sizes in Team Arena, 12, 16, and 20. 
//
// To generate new font data you need to go through the following steps.
// 1. delete the fontImage_x_xx.tga files and fontImage_xx.dat files from the fonts path.
// 2. in a ui script, specificy a font, smallFont, and bigFont keyword with font name and 
//    point size. the original TrueType fonts must exist in fonts at this point.
// 3. run the game, you should see things normally.
// 4. Exit the game and there will be three dat files and at least three tga files. The 
//    tga's are in 256x256 pages so if it takes three images to render a 24 point font you 
//    will end up with fontImage_0_24.tga through fontImage_2_24.tga
// 5. You will need to flip the tga's in Photoshop as the tga output code writes them upside
//    down.
// 6. In future runs of the game, the system looks for these images and data files when a s
//    specific point sized font is rendered and loads them for use. 
// 7. Because of the original beta nature of the FreeType code you will probably want to hand
//    touch the font bitmaps.
// 
// Currently a define in the project turns on or off the FreeType code which is currently 
// defined out. To pre-render new fonts you need enable the define ( BUILD_FREETYPE ) and 
// uncheck the exclude from build check box in the FreeType2 area of the Renderer project. 


#include "tr_local.h"
#include "../qcommon/qcommon.h"

#ifdef BUILD_FREETYPE
#include <ft2build.h>
#include FT_OUTLINE_H

#define _FLOOR(x)  ((x) & -64)
#define _CEIL(x)   (((x)+63) & -64)
#define _TRUNC(x)  ((x) >> 6)

FT_Library ftLibrary = NULL;  
#endif // BUILD_FREETYPE

#define MAX_FONTS 6
static int registeredFontCount = 0;
static fontInfo_t registeredFont[MAX_FONTS];

#ifdef BUILD_FREETYPE
void
R_GetGlyphInfo(FT_GlyphSlot glyph, int* left, int* right, int* width, int* top,
    int* bottom, int* height, int* pitch)
{
    *left = _FLOOR(glyph->metrics.horiBearingX);
    *right = _CEIL(glyph->metrics.horiBearingX + glyph->metrics.width);
    *width = _TRUNC(*right - *left);

    *top = _CEIL(glyph->metrics.horiBearingY);
    *bottom = _FLOOR(glyph->metrics.horiBearingY - glyph->metrics.height);
    *height = _TRUNC(*top - *bottom);
    *pitch = (true ? (*width + 3) & -4 : (*width + 7) >> 3);
}

FT_Bitmap*
R_RenderGlyph(FT_GlyphSlot glyph, glyphInfo_t* glyphOut)
{
    FT_Bitmap* bitmap;
    int left, right, width, top, bottom, height, pitch, size;

    R_GetGlyphInfo(glyph, &left, &right, &width, &top, &bottom, &height, &pitch);

    if (glyph->format == ft_glyph_format_outline) {
        size = pitch * height;

        bitmap = Z_Malloc(sizeof(FT_Bitmap));

        bitmap->width = width;
        bitmap->rows = height;
        bitmap->pitch = pitch;
        bitmap->pixel_mode = ft_pixel_mode_grays;
        //bitmap->pixel_mode = ft_pixel_mode_mono;
        bitmap->buffer = Z_Malloc(pitch * height);
        bitmap->num_grays = 256;

        Com_Memset(bitmap->buffer, 0, size);

        FT_Outline_Translate(&glyph->outline, -left, -bottom);

        FT_Outline_Get_Bitmap(ftLibrary, &glyph->outline, bitmap);

        glyphOut->height = height;
        glyphOut->pitch = pitch;
        glyphOut->top = (glyph->metrics.horiBearingY >> 6) + 1;
        glyphOut->bottom = bottom;

        return bitmap;
    } else {
        ri.Printf(PRINT_ALL, "Non-outline fonts are not supported\n");
    }

    return NULL;
}

static glyphInfo_t*
RE_ConstructGlyphInfo(unsigned char* imageOut, int* xOut, int* yOut, int* maxHeight,
    FT_Face face, const unsigned char c, bool calcHeight)
{
    int i;
    static glyphInfo_t glyph;
    unsigned char* src, * dst;
    float scaled_width, scaled_height;
    FT_Bitmap* bitmap = NULL;

    Com_Memset(&glyph, 0, sizeof(glyphInfo_t));

    // make sure everything is here
    if (face != NULL) {
        
        FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_DEFAULT);
        bitmap = R_RenderGlyph(face->glyph, &glyph);

        if (bitmap) {
            glyph.xSkip = (face->glyph->metrics.horiAdvance >> 6) + 1;
        } else {
            return &glyph;
        }

        if (glyph.height > *maxHeight) {
            *maxHeight = glyph.height;
        }

        if (calcHeight) {
            Z_Free(bitmap->buffer);
            Z_Free(bitmap);
            return &glyph;
        }

        /*
            // need to convert to power of 2 sizes so we do not get
            // any scaling from the gl upload
            for (scaled_width = 1 ; scaled_width < glyph.pitch ; scaled_width<<=1)
                ;
            for (scaled_height = 1 ; scaled_height < glyph.height ; scaled_height<<=1)
                ;
        */

        scaled_width = glyph.pitch;
        scaled_height = glyph.height;

        // we need to make sure we fit
        if (*xOut + scaled_width + 1 >= 255) {
            if (*yOut + *maxHeight + 1 >= 255) {
                *yOut = -1;
                *xOut = -1;
                Z_Free(bitmap->buffer);
                Z_Free(bitmap);
                return &glyph;
            } else {
                *xOut = 0;
                *yOut += *maxHeight + 1;
            }
        } else if (*yOut + *maxHeight + 1 >= 255) {
            *yOut = -1;
            *xOut = -1;
            Z_Free(bitmap->buffer);
            Z_Free(bitmap);
            return &glyph;
        }

        src = bitmap->buffer;
        dst = imageOut + (*yOut * 256) + *xOut;

        if (bitmap->pixel_mode == ft_pixel_mode_mono) {
            for (i = 0; i < glyph.height; i++)
            {
                int j;
                unsigned char* _src = src;
                unsigned char* _dst = dst;
                unsigned char mask = 0x80;
                unsigned char val = *_src;
                for (j = 0; j < glyph.pitch; j++) {
                    if (mask == 0x80) {
                        val = *_src++;
                    }
                    if (val & mask) {
                        *_dst = 0xff;
                    }
                    mask >>= 1;

                    if (mask == 0) {
                        mask = 0x80;
                    }
                    _dst++;
                }

                src += glyph.pitch;
                dst += 256;

            }
        } else {
            for (i = 0; i < glyph.height; i++)
            {
                Com_Memcpy(dst, src, glyph.pitch);
                src += glyph.pitch;
                dst += 256;
            }
        }

        // we now have an 8 bit per pixel grey scale bitmap 
        // that is width wide and pf->ftSize->metrics.y_ppem tall

        glyph.cellH = scaled_height;
        glyph.cellW = scaled_width;
        glyph.s1 = (float)*xOut / 256;
        glyph.t1 = (float)*yOut / 256;
        glyph.s2 = glyph.s1 + (float)scaled_width / 256;
        glyph.t2 = glyph.t1 + (float)scaled_height / 256;

        *xOut += scaled_width + 1;
    }

    Z_Free(bitmap->buffer);
    Z_Free(bitmap);

    return &glyph;
}

void Build_FreeTypeFont(fontInfo_t* font, const char* fontName, int pointSize)
{
    FT_Face face;
    int i, j, k, len, xOut, yOut, lastStart, imageNumber;
    int scaledSize, newSize, maxHeight, left;
    unsigned char* out, * imageBuff;
    glyphInfo_t* glyph;
    image_t* image;
    qhandle_t h;
    float max;
    void* faceData;
    char name[1024];
    float dpi = 72;
    float glyphScale = 72.0f / dpi; 		// change the scale to be relative to 1 based on 72 dpi ( so dpi of 144 means a scale of .5 )

    if (pointSize <= 0) {
        pointSize = 12;
    }
    
    // we also need to adjust the scale based on point size relative to 48 points as the ui scaling is based on a 48 point font
    glyphScale *= 48.0f / pointSize;

    if (ftLibrary == NULL) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: FreeType2 not initialized.\n");
        return;
    }

    len = ri.FS_ReadFile(fontName, &faceData);
    
    if (len <= 0) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: Unable to read font file\n");
        return;
    }

    // allocate on the stack first in case we fail
    if (FT_New_Memory_Face(ftLibrary, faceData, len, 0, &face)) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: FreeType2, unable to allocate new face.\n");
        return;
    }

    if (FT_Set_Char_Size(face, pointSize << 6, pointSize << 6, dpi, dpi)) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: FreeType2, Unable to set face char size.\n");
        return;
    }

    // make a 256x256 image buffer, once it is full, register it, clean it and keep going 
    // until all glyphs are rendered

    out = Z_Malloc(1024 * 1024);
    
    if (out == NULL) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: Z_Malloc failure during output image creation.\n");
        return;
    }

    Com_Memset(out, 0, 1024 * 1024);

    maxHeight = 0;

    for (i = GLYPH_START; i < GLYPH_END; i++)
    {
        glyph = RE_ConstructGlyphInfo(out, &xOut, &yOut, &maxHeight, face, (unsigned char)i, true);
    }

    xOut = 0;
    yOut = 0;
    i = GLYPH_START;
    lastStart = i;
    imageNumber = 0;

    while (i <= GLYPH_END)
    {
        glyph = RE_ConstructGlyphInfo(out, &xOut, &yOut, &maxHeight, face, (unsigned char)i, false);

        if (xOut == -1 || yOut == -1 || i == GLYPH_END) {

            // ran out of room
            // we need to create an image from the bitmap, set all the handles in the glyphs to this point

            scaledSize = 256 * 256;
            newSize = scaledSize * 4;
            imageBuff = Z_Malloc(newSize);
            left = 0;
            max = 0;

            for (k = 0; k < scaledSize; k++)
            {
                if (max < out[k]) {
                    max = out[k];
                }
            }

            if (max > 0) {
                max = 255 / max;
            }

            for (k = 0; k < scaledSize; k++)
            {
                imageBuff[left++] = 255;
                imageBuff[left++] = 255;
                imageBuff[left++] = 255;

                imageBuff[left++] = ((float)out[k] * max);
            }

            Com_sprintf(name, sizeof(name), "fonts/fontImage_%i_%i.tga", imageNumber++, pointSize);
            
            if (r_saveFontData->integer) {
                WriteTGA(name, imageBuff, 256, 256);
            }

            image = R_CreateImage(name, imageBuff, 256, 256, false, false, GL_CLAMP);
            h = RE_RegisterShaderFromImage(name, LIGHTMAP_2D, image, false);
            
            for (j = lastStart; j < i; j++)
            {
                font->glyphs[j].glyph = h;
                Q_strncpyz(font->glyphs[j].shaderName, name, sizeof(font->glyphs[j].shaderName));
            }
            
            lastStart = i;
            Com_Memset(out, 0, 1024 * 1024);
            xOut = 0;
            yOut = 0;
            Z_Free(imageBuff);
            i++;

        } else {
            
            Com_Memcpy(&font->glyphs[i], glyph, sizeof(glyphInfo_t));
            i++;
        }
    }

    registeredFont[registeredFontCount].glyphScale = glyphScale;
    font->glyphScale = glyphScale;
    Com_Memcpy(&registeredFont[registeredFontCount++], font, sizeof(fontInfo_t));

    if (r_saveFontData->integer) {
        ri.FS_WriteFile(va("fonts/fontImage_%i.dat", pointSize), font, sizeof(fontInfo_t));
    }

    Z_Free(out);
    ri.FS_FreeFile(faceData);
}
#endif // BUILD_FREETYPE

void RE_RegisterFont(const char* fontName, int pointSize, fontInfo_t* font)
{
    int fdOffset;
    const byte* fdFile;
    void* fileData;
    int i, len;
    char name[1024];

    // make sure the render thread is stopped
    R_SyncRenderThread();

    if (registeredFontCount >= MAX_FONTS) {
        ri.Printf(PRINT_ALL, "RE_RegisterFont: Too many fonts registered already.\n");
        return;
    }

    Com_sprintf(name, sizeof(name), "fonts/fontImage_%i.dat", pointSize);
    
    for (i = 0; i < registeredFontCount; i++)
    {
        if (Q_stricmp(name, registeredFont[i].name) == 0) {
            Com_Memcpy(font, &registeredFont[i], sizeof(fontInfo_t));
            return;
        }
    }

    len = ri.FS_ReadFile(name, NULL);

    if (len == sizeof(fontInfo_t)) {

        ri.FS_ReadFile(name, &fileData);
        fdOffset = 0;
        fdFile = fileData;

        for (i = 0; i < GLYPHS_PER_FONT; i++)
        {
            font->glyphs[i].height      = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].top         = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].bottom      = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].pitch       = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].xSkip       = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].cellW       = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].cellH       = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].s1          = ReadFltLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].t1          = ReadFltLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].s2          = ReadFltLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].t2          = ReadFltLE(&fdFile[fdOffset]); fdOffset += 4;
            font->glyphs[i].glyph       = ReadIntLE(&fdFile[fdOffset]); fdOffset += 4;
            Com_Memcpy(font->glyphs[i].shaderName, &fdFile[fdOffset], 32); fdOffset += 32;
        }

        font->glyphScale = ReadFltLE(&fdFile[fdOffset]); fdOffset += 4;
        Com_Memcpy(font->name, &fdFile[fdOffset], MAX_QPATH);
        Q_strncpyz(font->name, name, sizeof(font->name));

        for (i = GLYPH_START; i < GLYPH_END; i++)
        {
            font->glyphs[i].glyph = RE_RegisterShaderNoMip(font->glyphs[i].shaderName);
        }

        Com_Memcpy(&registeredFont[registeredFontCount++], font, sizeof(fontInfo_t));

        return;
    }

#ifndef BUILD_FREETYPE
    ri.Printf(PRINT_ALL, "RE_RegisterFont: FreeType code not available\n");
#else
    Build_FreeTypeFont(font, fontName, pointSize);
#endif // BUILD_FREETYPE
}

void R_InitFont()
{
#ifdef BUILD_FREETYPE
    if (FT_Init_FreeType(&ftLibrary)) {
        ri.Printf(PRINT_ALL, "R_InitFreeType: Unable to initialize FreeType.\n");
    }
#endif // BUILD_FREETYPE
    registeredFontCount = 0;
}

void R_DoneFont()
{
#ifdef BUILD_FREETYPE
    if (ftLibrary) {
        FT_Done_FreeType(ftLibrary);
        ftLibrary = NULL;
    }
#endif // BUILD_FREETYPE
    registeredFontCount = 0;
}
