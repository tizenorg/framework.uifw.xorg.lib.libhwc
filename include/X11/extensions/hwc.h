/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Soft-
 * ware"), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, provided that the above copyright
 * notice(s) and this permission notice appear in all copies of the Soft-
 * ware and that both the above copyright notice(s) and this permission
 * notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
 * ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN
 * THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSE-
 * QUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFOR-
 * MANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization of
 * the copyright holder.
 *
 * Origin Authors:
 *   Sangjin Lee <lsj119@samsung.com>
 */

#ifndef _HWC_H_
#define _HWC_H_

#include <X11/extensions/hwctokens.h>
#include <X11/extensions/Xrandr.h>

#include <X11/Xfuncproto.h>
#include <X11/Xlib.h>

_XFUNCPROTOBEGIN

extern Bool
HWCQueryVersion(Display * display, int *major, int *minor);

extern Bool
HWCOpen(Display * dpy, RRCrtc crtc, int *maxLayer);

extern void
HWCSetDrawables(Display * dpy, RRCrtc crtc, Window window, Drawable *drawables, XRectangle *srcRect, XRectangle *dstRect, HWCCompositeMethod *compMethods, int count);

extern void
HWCSelectInput(Display * dpy, Window window, int mask);

typedef struct
{
    int            evtype;
    int            maxLayer;
} HWCConfigureNotifyCookie;

_XFUNCPROTOEND

#endif
