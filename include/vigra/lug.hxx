/************************************************************************/
/*                                                                      */
/*               Copyright 1998-2000 by Ullrich Koethe                  */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    You may use, modify, and distribute this software according       */
/*    to the terms stated in the LICENSE file included in               */
/*    the VIGRA distribution.                                           */
/*                                                                      */
/*    The VIGRA Website is                                              */
/*        http://kogs-www.informatik.uni-hamburg.de/~koethe/vigra/      */
/*    Please direct questions, bug reports, and contributions to        */
/*        koethe@informatik.uni-hamburg.de                              */
/*                                                                      */
/*  THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR          */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. */
/*                                                                      */
/************************************************************************/
 
 
#ifndef IMP_LUG_HXX
#define IMP_LUG_HXX

// lug includes 

// vigra includes 
#include "vigra/utilities.hxx"
#include "vigra/error.hxx"

#include <lug.h>

typedef bitmap_hdr LugImage;

extern "C"
{
    int allocatebitmap(bitmap_hdr*,int,int,int,int);
    int isagrayscaled(bitmap_hdr*);
    int write_lug_file(char*,bitmap_hdr*);
    int freebitmap(bitmap_hdr*);
    int read_lug_file(char*,bitmap_hdr*);
    int write_rgb_file(char*,bitmap_hdr*);
    int read_rgb_file(char*,bitmap_hdr*,int,int);
}

struct LugRGBEntry
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

inline 
byte *fill_bw_pallete(byte * buffer)
{
  int i;
  byte *ptr;

  /* I'll use a pointer */
  ptr = buffer;
  for (i = 0; i < 256; i++) {
    *ptr++= (byte) i;   /* R */
    *ptr++= (byte) i;   /* G */
    *ptr++= (byte) i;   /* B */
  }

  return buffer;
}

/** @name LUG file import and export
    LUG is a simple gateway for many common image
    data formats, including GIF, TIFF, JPEG and PostScript. The source code for LUG
    (Libraria de Utilidades Graficas - Graphics Utilities Library) is
    available at 
    \URL[http://www.uniovi.es/~rivero/LUG/]{http://www.uniovi.es/~rivero/LUG/}.
    LUG supports some formats (e.g. GIF, PostScript) on its own and uses 
    other public domain libraries for the rest (TIFF, JPEG). Unlike VIFF, 
    LUG can only handle 'byte' (scalar, RGB and mapped RGB) pixel types. 

    @memo supports GIF, TIFF, JPEG, PS, and other formats
*/
//@{

/********************************************************/
/*                                                      */
/*             read/write/delete LugImages              */
/*                                                      */
/********************************************************/

/** @name read/write/delete LUG images
    These functions are convenience functions to simplify LUG's usage 
    in the context of VIGRA. You can
    always use the original LUG functions for the same or related purposes 
    (see LUG documentation for details). 
    @memo
*/
//@{
    /** Create LugImage and read contents from given file.
    
        {\bf Declaration: }
	\begin{verbatim}
	LugImage * readLugImage(char * filename);
	\end{verbatim}
    
	Include-File:
	\URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
	
    */
inline
LugImage *
readLugImage(const char* filename)
{
	LugImage * image = new LugImage;
	if (!image)
		return 0;

	read_lug_file(const_cast<char*>(filename), image);
	return image;
}

    /** Write LugImage to given file. The output file format is determined 
        by the file name extension. For example, 'test.gif' will be a GIF file.
        You can override this convention by using alternative write functions.
        See the LUG manual for details. 
    
        {\bf Declaration: }
	\begin{verbatim}
	LugImage * writeLugImage(char * filename, LugImage* image);
	\end{verbatim}
    
	Include-File:
	\URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
	
    */
inline
void
writeLugImage(const char* filename, LugImage* image)
{
	if (!image)
		return;

	write_lug_file(const_cast<char*>(filename), image);
}

    /* Create LugImage if given size and color depth.
        Depth is either 8 or 24.
    
        {\bf Declaration: }
	\begin{verbatim}
	LugImage * createLugImage(int width, int height, int depth)
	\end{verbatim}
    
	Include-File:
	\URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
	
    */
inline
LugImage *
createLugImage(int width, int height, int depth)
{
	LugImage * img = new LugImage;
	allocatebitmap(img, width, height, depth, 0);
	return img;
}

inline
LugImage *
createLugImage(int width, int height, int depth, int colors)
{
	LugImage * img = new LugImage;
	allocatebitmap(img, width, height, depth, colors);
	return img;
}

inline
LugImage *
createLugImage()
{
	LugImage * img = new LugImage;
	return img;
}

    /** Delete LugImage and free memory.
    
        {\bf Declaration: }
	\begin{verbatim}
	void freeLugImage(LugImage * image)
	\end{verbatim}
    
	Include-File:
	\URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
	
    */
inline
void
freeLugImage(LugImage * image)
{
	freebitmap(image);
	delete image;
}

    /** Test if LugImage is a grayscale image.
    
        {\bf Declaration: }
	\begin{verbatim}
	int isagrayscaled(LugImage * image)
	\end{verbatim}
    
	Include-File:
	\URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
	
    */
int
isagrayscaled(LugImage *);

//@}

/** @name Convert LugImages 
    @memo to/from any image supporting ImageIterators
*/
//@{
/********************************************************/
/*                                                      */
/*                     importLugImage                   */
/*                                                      */
/********************************************************/

/** Convert given lug into image specified by iterator range.
    Accessors are used to write the data.    
    This function calls \Ref{lugToScalarImage} or \Ref{lugToRGBImage}, depending on 
    the accessor's value_type.

    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    void
    importLugImage(LugImage * lug, ImageIterator iter, Accessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    void
    importLugImage(LugImage * lug, pair<ImageIterator, Accessor> dest)
    \end{verbatim}
    
    {\bf Usage:}

    Include-File:
    \URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
    
    \begin{verbatim}
    LugImage * lug = readimage("scalarimage.gif");
    
    BImage img(lug->xsize, lug->ysize);
    
    importLugImage(lug, destImage(img));
    
    freeLugImage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    see \Ref{lugToScalarImage} and \Ref{lugToRGBImage}
    
    {\bf Preconditions:}
    
    see \Ref{lugToScalarImage} and \Ref{lugToRGBImage}
    
    @memo
*/
template <class ImageIterator, class Accessor>
inline void
importLugImage(LugImage * lug, ImageIterator iter, Accessor a)
{
    typedef typename 
        NumericTraits<typename Accessor::value_type>::isScalar
        isScalar;
    importLugImage(lug, iter, a, isScalar());
}

template <class ImageIterator, class Accessor>
inline void
importLugImage(LugImage * lug, pair<ImageIterator, Accessor> dest)
{
    importLugImage(lug, dest.first, dest.second);
}

template <class ImageIterator, class Accessor>
inline void
importLugImage(LugImage * lug, ImageIterator iter, Accessor a, VigraTrueType)
{
    lugToScalarImage(lug, iter, a);
}

template <class ImageIterator, class Accessor>
inline void
importLugImage(LugImage * lug, ImageIterator iter, Accessor a, VigraFalseType)
{
    lugToRGBImage(lug, iter, a);
}

/********************************************************/
/*                                                      */
/*                   lugToScalarImage                   */
/*                                                      */
/********************************************************/

/** Convert single-band LugImage to scalar image.
    This function uses accessors to write the data.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    void
    lugToScalarImage(LugImage * img, ImageIterator iter, Accessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    void
    lugToScalarImage(LugImage * img, pair<ImageIterator, Accessor> dest)
    \end{verbatim}
    
    {\bf Usage:}

    Include-File:
    \URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
    
    \begin{verbatim}
    LugImage * lug = readLugImage("scalarimage.tif");
    
    BImage img(lug->xsize, lug->ysize);
    
    lugToScalarImage(lug, destImage(img));
    
    freeLugImage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    \begin{verbatim}
    ImageIterator upperleft;
    unsigned char value;
    
    Accessor accessor;
		       
    accessor.set(value, upperleft);
    \end{verbatim}
    
    {\bf Preconditions:}
    
    \begin{verbatim}
    isagrayscaled(lug)
    \end{verbatim}
    
    @memo
*/
template <class ImageIterator, class Accessor>
void
lugToScalarImage(LugImage * img, ImageIterator iter, Accessor a)
{
    int w = img->xsize;
    int h = img->ysize;

    precondition(img,
		"lugToScalarImage(): " 
	      	"NULL pointer to input data.");
   
    precondition(isagrayscaled(img),
		"lugToScalarImage(): " 
	      	"Source image is not scalar.");

    // now we have a color map with 256 entries and r=g=b
    ImageIterator yd(iter);
	
    unsigned char * pindex = img->r;
    struct LugRGBEntry * cmap = (struct LugRGBEntry *) img->cmap;
    for(int y = 0; y < h; ++y, ++yd.y)
    {
	ImageIterator xd(yd);
	for(int x = 0; x < w; ++x, ++pindex, ++xd.x)
	{
	    a.set(cmap[*pindex].red, xd);
	}
    }
}

template <class ImageIterator, class Accessor>
inline
void
lugToScalarImage(LugImage * img, pair<ImageIterator, Accessor> dest)
{
    lugToScalarImage(img, dest.first, dest.second);
}

/********************************************************/
/*                                                      */
/*                     lugToRGBImage                    */
/*                                                      */
/********************************************************/

/** Convert LugImage to RGB image.
    This function uses accessors to write the data.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class RGBAccessor>
    void
    lugToRGBImage(LugImage * img, ImageIterator iter, RGBAccessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class RGBAccessor>
    void
    lugToRGBImage(LugImage * img, pair<ImageIterator, RGBAccessor> dest)
    \end{verbatim}
    
    {\bf Usage:}

    Include-File:
    \URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
    
    \begin{verbatim}
    LugImage * lug = readLugImage("rgbimage.gif");
    
    BRGBImage img(lug->xsize, lug->ysize);
    
    lugToRGBImage(lug, destImage(img));
    
    freeLugImage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    \begin{verbatim}
    ImageIterator upperleft;
    unsigned char value;
    
    RGBAccessor accessor;
		       
    accessor.setRed(value, upperleft);
    accessor.setGreen(value, upperleft);
    accessor.setBlue(value, upperleft);
    \end{verbatim}
    
    @memo
*/
template <class ImageIterator, class Accessor>
void
lugToRGBImage(LugImage * img, ImageIterator upperleft, Accessor a)
{
    precondition(img,
	    "lugToRGBImage(LugImage *, RGBImageIterator): " 
	    "NULL pointer to input data.");

    int x = 0;
    int y = 0;
    int w = img->xsize;
    int h = img->ysize;
	
    if(img->depth > 8)
    {
	// real multiband image
	precondition( (img->r && img->g && img->b),
		"lugToRGBImage(): " 
		"NULL pointer to pixel data.");

	ImageIterator yd(upperleft);

	unsigned char * pred = img->r;
	unsigned char * pgreen = img->g;
	unsigned char * pblue = img->b;
	for(y = 0; y < h; ++y, ++yd.y)
	{
	    ImageIterator xd(yd);
	    for(x = 0; x < w; ++x, ++pred, ++pgreen, ++pblue, ++xd.x)
	    {
		    a.setRed(*pred, xd);
		    a.setGreen(*pgreen, xd);
		    a.setBlue(*pblue, xd);
	    }
	}
    }
    else
    {
	// scalar data with color map
	precondition(img->r,
		"lugToRGBImage(): " 
		"NULL pointer to pixel data.");
	// data only in r-buffer
	unsigned char * ps = img->r;
	
	struct LugRGBEntry * cmap = (struct LugRGBEntry *) img->cmap;
    
	ImageIterator yd(upperleft);
    
	for(y = 0; y < h; ++y, ++yd.y)
	{
	    ImageIterator xd(yd);
	    for(x = 0; x < w; ++x, ++ps, ++xd.x)
	    {
		    a.setRed(cmap[*ps].red, xd);
		    a.setGreen(cmap[*ps].green, xd);
		    a.setBlue(cmap[*ps].blue, xd);
	    }
	}
    }
}

template <class ImageIterator, class Accessor>
inline
void
lugToRGBImage(LugImage * img, pair<ImageIterator, Accessor> dest)
{
    lugToRGBImage(img, dest.first, dest.second);
}

/********************************************************/
/*                                                      */
/*                     createLugImage                   */
/*                                                      */
/********************************************************/

/** Create a LUG image from the given iterator range.
    It is automatically determined whether a scalar or RGB image must 
    be created. This function uses accessors to read the data. 
    Note, however, that LUG images can only store 'unsigned char' pixel
    values, so all scalar types are converted to 'unsigned char' during conversion, 
    while all RGB types are converted to RGBValue<unsigned char>. Use the 
    \URL[VIFF]{VIFFrelatedfunctions.html} image format, if this is not 
    acceptable.
    
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    inline LugImage *
    createLugImage(ImageIterator upperleft, ImageIterator lowerright, 
                    Accessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    inline LugImage *
    createLugImage(triple<ImageIterator, ImageIterator, Accessor> src)
    \end{verbatim}

    {\bf Usage:}

    Include-File:
    \URL[vigra/viff.hxx]{../include/vigra/viff.hxx}
    
    \begin{verbatim}
    BImage img(width, height);
    
    ...
    
    LugImage * lug = createLugImage(srcImageRange(img));
    
    // output file format GIF is determined from the file name extension '.gif'
    writeLugImage("output.gif", lug); 
    
    freeimage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    \begin{verbatim}
    ImageIterator upperleft;
    Accessor accessor;
                           
    accessor(upperleft);   // result written into xvimage
    \end{verbatim}
    
    @memo
*/
template <class ImageIterator, class Accessor>
inline LugImage *
createLugImage(ImageIterator upperleft, ImageIterator lowerright, 
                      Accessor a)
{
    typedef typename 
           NumericTraits<typename Accessor::value_type>::isScalar 
           isScalar;
    return createLugImage(upperleft, lowerright, a, isScalar());
}

template <class ImageIterator, class Accessor>
inline LugImage *
createLugImage(triple<ImageIterator, ImageIterator, Accessor> src)
{
    return createLugImage(src.first, src.second, src.third);
}

template <class ImageIterator, class Accessor>
inline LugImage *
createLugImage(ImageIterator upperleft, ImageIterator lowerright, 
                      Accessor a, VigraFalseType)
{
    return createRGBLugImage(upperleft, lowerright, a);
}

template <class ImageIterator, class Accessor>
inline LugImage *
createLugImage(ImageIterator upperleft, ImageIterator lowerright, 
                      Accessor a, VigraTrueType)
{
    return createScalarLugImage(upperleft, lowerright, a);
}

/********************************************************/
/*                                                      */
/*                createScalarLugImage                  */
/*                                                      */
/********************************************************/

/** Create a gray-scaled LugImage from the given scalar image.
    The size of the LugImage is determined by the input image.
    This function uses accessors to read the data. All pixel data types are 
    converted to 'unsigned char' during conversion.  Use the 
    \URL[VIFF]{VIFFrelatedfunctions.html} image format, if this is not 
    acceptable.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    LugImage *
    createScalarLugImage(ImageIterator upperleft, ImageIterator lowerright, Accessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class Accessor>
    LugImage *
    createScalarLugImage(triple<ImageIterator, ImageIterator, Accessor> src)
    \end{verbatim}
    
    {\bf Usage:}

    Include-File:
    \URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
    
    \begin{verbatim}
    BImage img(width, height);
    ...
    
    LugImage * lug = createScalarLugImage(srcImageRange(img));
    
    writeLugImage("scalarimage.tif", lug);
    
    freeLugImage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    \begin{verbatim}
    ImageIterator upperleft;
    unsigned char value;
    
    Accessor accessor;
		       
    value = accessor(upperleft);
    \end{verbatim}
    
    @memo
*/
template <class ImageIterator, class Accessor>
LugImage *
createScalarLugImage(ImageIterator upperleft, ImageIterator lowerright, Accessor a)
{

    int x = 0;
    int y = 0;
    int w = lowerright.x - upperleft.x;
    int h = lowerright.y - upperleft.y;
    

    LugImage * img = createLugImage(w, h, 8, 256);
    if (!img)
	    return 0;
    
    // create the gray scaled palette
    fill_bw_pallete(img->cmap);

    unsigned char * pindex = img->r;
    ImageIterator yd(upperleft);

    for(y = 0; y < h; ++y, ++yd.y)
    {
	ImageIterator xd(yd);
	for(x = 0; x < w; ++x, ++pindex, ++xd.x)
	{
	    *pindex = a(xd);
	}
    }
    return img;
}

template <class ImageIterator, class Accessor>
inline
LugImage *
createScalarLugImage(triple<ImageIterator, ImageIterator, Accessor> src)
{
    return createScalarLugImage(src.first, src.second, src.third);
}

/********************************************************/
/*                                                      */
/*                  createRGBLugImage                   */
/*                                                      */
/********************************************************/

/** Create a RGB LugImage from the given RGB image.
    The size of the LugImage is determined by the input image.
    This function uses accessors to read the data. All pixel data types are 
    converted to 'RGBValue<unsigned char>' during conversion. Use the 
    \URL[VIFF]{VIFFrelatedfunctions.html} image format, if this is not 
    acceptable.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    template <class ImageIterator, class RGBAccessor>
    LugImage *
    createRGBLugImage(ImageIterator upperleft, ImageIterator lowerright, RGBAccessor a)
    \end{verbatim}

    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    template <class ImageIterator, class RGBAccessor>
    LugImage *
    createRGBLugImage(triple<ImageIterator, ImageIterator, RGBAccessor> src)
    \end{verbatim}
    
    {\bf Usage:}

    Include-File:
    \URL[vigra/lug.hxx]{../include/vigra/lug.hxx}
    
    \begin{verbatim}
    BRGBImage img(width, height);
    ...
    
    LugImage * lug = createRGBLugImage(srcImageRange(img));
    
    writeLugImage("rgbimage.gif", lug);
    
    freeLugImage(lug);
    \end{verbatim}
    
    {\bf Required Interface:}
    
    \begin{verbatim}
    ImageIterator upperleft;
    unsigned char value;
    
    RGBAccessor accessor;
		       
    value = accessor.red(upperleft);
    value = accessor.green(upperleft);
    value = accessor.blue(upperleft);
    \end{verbatim}
    
    @memo
*/
template <class ImageIterator, class Accessor>
LugImage *
createRGBLugImage(ImageIterator upperleft, ImageIterator lowerright, Accessor a)
{

    int x = 0;
    int y = 0;
    int w = lowerright.x - upperleft.x;
    int h = lowerright.y - upperleft.y;

    LugImage * img = createLugImage(w, h, 24, 0);
    if (!img)
	    return 0;

    unsigned char * pred = img->r;
    unsigned char * pgreen = img->g;
    unsigned char * pblue = img->b;
    ImageIterator yd(upperleft);

    for(y = 0; y < h; ++y, ++yd.y)
    {
	ImageIterator xd(yd);
	for(x = 0; x < w; ++x, ++pred, ++pgreen, ++pblue, ++xd.x)
	{
	    *pred   = a.red(xd);
	    *pgreen = a.green(xd);
	    *pblue  = a.blue(xd);
	}
    }
    return img;
}

template <class ImageIterator, class Accessor>
inline
LugImage *
createRGBLugImage(triple<ImageIterator, ImageIterator, Accessor> src)
{
    return createRGBLugImage(src.first, src.second, src.third);
}

//@}

//@}

#endif // IMP_LUG_HXX


