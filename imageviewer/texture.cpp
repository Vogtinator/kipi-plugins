/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using
 *               an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#undef PERFORMANCE_ANALYSIS

#include "texture.h"

// Qt includes

#include <QMatrix>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>

// Local includes
#ifdef PERFORMANCE_ANALYSIS
#include "timer.h"
#endif

namespace KIPIviewer
{

Texture::Texture(KIPI::Interface* i)
{
    kipiInterface  = i;
    rotate_list[0] = 90;
    rotate_list[1] = 180;
    rotate_list[2] = 270;
    rotate_list[3] = 180;
    rotate_idx     = 0;
    reset();
}

Texture::~Texture()
{
}

/*!
    \fn Texture::height()
 */
int Texture::height() const
{
    return glimage.height();
}

/*!
    \fn Texture::width()
 */
int Texture::width() const
{
    return glimage.width();
}

/*!
    \fn Texture::load(QString fn, QSize size, GLuint tn)
    \brief load file from disc and save it in texture
    \param fn filename to load
    \param size size of image which is downloaded to texture mem
    \param tn texture id generated by glGenTexture
    if "size" is set to image size, scaling is only performed by the GPU but not
    by the CPU, however the AGP usage to texture memory is increased (20MB for a 5mp image)
 */
bool Texture::load(const QString& fn, const QSize& size, GLuint tn)
{
    filename     = fn;
    initial_size = size;
    _texnr       = tn;

    // check if its a RAW file.
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QFileInfo fileInfo(fn);
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
    {
        // it's a RAW file, use the libkdcraw loader
        KDcrawIface::KDcraw::loadDcrawPreview(qimage, filename);
    }
    else
    {
        // use the standard loader
        qimage = QImage(filename);
    }

    //handle rotation
    KIPI::ImageInfo info = kipiInterface->info(filename);
    if (info.angle() != 0)
    {
        QMatrix r;
        r.rotate(info.angle());
        qimage = qimage.transformed(r);
        kDebug() << "image rotated by " << info.angle() << " degree";
    }

    if (qimage.isNull())
    {
        return false;
    }

    _load();
    reset();
    rotate_idx = 0;
    return true;
}

/*!
    \fn Texture::load(QImage im, QSize size, GLuint tn)
    \brief copy file from QImage to texture
    \param im Qimage to be copied from
    \param size size of image which is downloaded to texture mem
    \param tn texture id generated by glGenTexture
    if "size" is set to image size, scaling is only performed by the GPU but not
    by the CPU, however the AGP usage to texture memory is increased (20MB for a 5mp image)
 */
bool Texture::load(const QImage& im, const QSize& size, GLuint tn)
{
    qimage       = im;
    initial_size = size;
    _texnr       = tn;
    _load();
    reset();
    rotate_idx   = 0;
    return true;
}

/*!
    \fn Texture::load()
    internal load function
    rt[xy] <= 1
 */
bool Texture::_load()
{
    int w = initial_size.width();
    int h = initial_size.height();

    if (w == 0 || w > qimage.width() || h > qimage.height())
    {
        glimage = QGLWidget::convertToGLFormat(qimage);
    }
    else
    {
        glimage = QGLWidget::convertToGLFormat(qimage.scaled(w,h,Qt::KeepAspectRatio,Qt::FastTransformation));
    }

    w = glimage.width();
    h = glimage.height();
    if (h < w)
    {
        rtx = 1;
        rty = float(h)/float(w);
    }
    else
    {
        rtx = float(w)/float(h);
        rty = 1;
    }
    return true;
}

/*!
    \fn Texture::data()
 */
GLvoid* Texture::data()
{
    return glimage.bits();
}

/*!
    \fn Texture::texnr()
 */
GLuint Texture::texnr()
{
    return _texnr;
}

/*!
    \fn Texture::zoom(float delta, QPoint mousepos)
    \brief calculate new tex coords on zooming
    \param delta delta between previous zoom and current zoom
    \param mousepos mouse position returned by QT
    \TODO rename mousepos to something more generic
*/
void Texture::zoom(float delta, const QPoint& mousepos)
//u: start in texture, u=[0..1], u=0 is begin, u=1 is end of texture
//z=[0..1], z=1 -> no zoom
//l: length of tex in glFrustum coordinate system
//rt: ratio of tex, rt<=1, see _load() for definition
//rd: ratio of display, rd>=1
//m: mouse pos normalized, cd=[0..rd]
//c:  mouse pos normalized to zoom*l, c=[0..1]
{
    z        *= delta;
    delta    =  z*(1.0/delta-1.0); //convert to real delta=z_old-z_new

    float mx = mousepos.x()/(float)display_x*rdx;
    float cx = (mx-rdx/2.0+rtx/2.0)/rtx;
    float vx = ux+cx*z;
    ux       = ux+(vx-ux)*delta/z;

    float my = mousepos.y()/(float)display_y*rdy;
    float cy = (my-rdy/2.0+rty/2.0)/rty;
    cy       = 1-cy;
    float vy = uy+cy*z;
    uy       = uy+(vy-uy)*delta/z;

    calcVertex();
}

/*!
    \fn Texture::calcVertex()
    Calculate vertices according internal state variables
    z, ux, uy are calculated in Texture::zoom()
 */
void Texture::calcVertex()
// rt: ratio of tex, rt<=1, see _load() for definition
// u: start in texture, u=[0..1], u=0 is begin, u=1 is end of texture
// l: length of tex in glFrustum coordinate system
// halftexel: the color of a texel is determined by a corner of the texel and not its center point
//                  this seems to introduce a visible jump on changing the tex-size.
//
// the glFrustum coord-sys is visible in [-rdx..rdx] ([-1..1] for square screen) for z=1 (no zoom)
// the tex coord-sys goes from [-rtx..rtx] ([-1..1] for square texture)
{
    // x part
    float lx          = 2*rtx/z;  //length of tex
    float tsx         = lx/(float)glimage.width(); //texelsize in glFrustum coordinates
    float halftexel_x = tsx/2.0;
    float wx          = lx*(1-ux-z);
    vleft             = -rtx-ux*lx - halftexel_x;  //left
    vright            = rtx+wx - halftexel_x;      //right

    // y part
    float ly          = 2*rty/z;
    float tsy         = ly/(float)glimage.height(); //texelsize in glFrustum coordinates
    float halftexel_y = tsy/2.0;
    float wy          = ly*(1-uy-z);
    vbottom           = -rty - uy*ly + halftexel_y; //bottom
    vtop              = rty + wy + halftexel_y;     //top
}

/*!
    \fn Texture::vertex_bottom()
 */
GLfloat Texture::vertex_bottom()
{
    return (GLfloat) vbottom;
}

/*!
    \fn Texture::vertex_top()
 */
GLfloat Texture::vertex_top()
{
    return (GLfloat) vtop;
}

/*!
    \fn Texture::vertex_left()
 */
GLfloat Texture::vertex_left()
{
    return (GLfloat) vleft;
}

/*!
    \fn Texture::vertex_right()
 */
GLfloat Texture::vertex_right()
{
    return (GLfloat) vright;
}

/*!
    \fn Texture::setViewport(int w, int h)
    \param w width of window
    \param h height of window
    Set widget's viewport. Ensures that rdx & rdy are always > 1
 */
void Texture::setViewport(int w, int h)
{
    if (h>w)
    {
        rdx = 1.0;
        rdy = h/float(w);
    }
    else
    {
        rdx = w/float(h);
        rdy = 1.0;
    }
    display_x = w;
    display_y = h;
}

/*!
    \fn Texture::move(QPoint diff)
    new tex coordinates have to be calculated if the view is panned
 */
void Texture::move(const QPoint& diff)
{
    ux = ux-diff.x()/float(display_x)*z*rdx/rtx;
    uy = uy+diff.y()/float(display_y)*z*rdy/rty;
    calcVertex();
}

/*!
    \fn Texture::reset()
 */
void Texture::reset()
{
    ux              = 0;
    uy              = 0;
    z               = 1.0;
    float zoomdelta = 0;

    if ((rtx < rty) && (rdx < rdy) && (rtx/rty < rdx/rdy))
    {
        zoomdelta = z-rdx/rdy;
    }

    if ((rtx < rty) && (rtx/rty > rdx/rdy))
    {
        zoomdelta = z-rtx;
    }

    if ((rtx >= rty) && (rdy < rdx) && (rty/rtx < rdy/rdx))
    {
        zoomdelta = z-rdy/rdx;
    }

    if ((rtx >= rty) && (rty/rtx > rdy/rdx))
    {
        zoomdelta = z-rty;
    }

    QPoint p =  QPoint(display_x/2,display_y/2);
    zoom(1.0-zoomdelta,p);

    calcVertex();
}

/*!
    \fn Texture::setSize(QSize size)
    \param size desired texture size. QSize(0,0) will take the full image
    \return true if size has changed, false otherwise
    set new texture size in order to reduce AGP bandwidth
 */
bool Texture::setSize(QSize size)
{
    //don't allow larger textures than the original image. the image will be upsampled by
    //OpenGL if necessary and not by QImage::scale
    size = size.boundedTo(qimage.size());

    if (glimage.width() == size.width())
    {
        return false;
    }

    int w = size.width();
    int h = size.height();

    if (w == 0)
    {
        glimage = QGLWidget::convertToGLFormat(qimage);
    }
    else
    {
        glimage = QGLWidget::convertToGLFormat(qimage.scaled(w, h, Qt::KeepAspectRatio, Qt::FastTransformation));
    }

    //recalculate half-texel offset
    calcVertex();

    return true;
}

/*!
    \fn Texture::rotate()
    \brief smart image rotation
    since the two most frequent usecases are a CW or CCW rotation of 90,
    perform these rotation with one (+90) or two (-90) calls of rotation()
 */
void Texture::rotate()
{
    QMatrix r;
    r.rotate(rotate_list[rotate_idx%4]);
    qimage = qimage.transformed(r);
    _load();

    //save new rotation in exif header
    KIPI::ImageInfo info = kipiInterface->info(filename);
    info.setAngle(rotate_list[rotate_idx%4]);

    reset();
    rotate_idx++;
}

/*!
    \fn Texture::setToOriginalSize()
    zoom image such that each pixel of the screen corresponds to a pixel in the jpg
    remember that OpenGL is not a pixel exact specification, and the image will still be filtered by OpenGL
 */
void Texture::zoomToOriginal()
{
    float zoomfactorToOriginal;
    reset();

    if (qimage.width()/qimage.height() > float(display_x)/float(display_y))
    {
        //image touches right and left edge of window
        zoomfactorToOriginal = float(display_x)/qimage.width();
    }
    else
    {
        //image touches upper and lower edge of window
        zoomfactorToOriginal = float(display_y)/qimage.height();
    }

    zoom(zoomfactorToOriginal,QPoint(display_x/2,display_y/2));
}

} // namespace KIPIviewer
