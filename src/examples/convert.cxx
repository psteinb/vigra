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
 

#include <iostream>
#include "vigra/stdimage.hxx"
#include "vigra/impex.hxx"


int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " infile outfile" << std::endl;
        std::cout << "(supported fomats: " << vigraImpexListFormats() << ")" << std::endl;
        
        return 1;
    }
    
    try
    {
        // read image given as first argument
        // file type is determined automatically
        ImageImportInfo info(argv[1]);
        
        if(info.isGrayscale())
        {
            // create a gray scale image of appropriate size
            BImage in(info.width(), info.height());
            
            // import the image just read
            importImage(info, destImage(in));
            
            // write the image to the file given as second argument
            // the file type will be determined from the file name's extension
            exportImage(srcImageRange(in), ImageExportInfo(argv[2]));
        }
        else
        {
            // create a RGB image of appropriate size
            BRGBImage in(info.width(), info.height());
            
            // import the image just read
            importImage(info, destImage(in));
            
            // write the image to the file given as second argument
            // the file type will be determined from the file name's extension
            exportImage(srcImageRange(in), ImageExportInfo(argv[2]));
        }
    }
    catch (VigraStdException & e)
    {
        // catch any errors that might have occured and pront their reason
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
