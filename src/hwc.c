/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Contact: Sangjin Lee <lsj119@samsung.com>
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
 *   Sangjin Lee (lsj119@samsung.com)
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/Xlibint.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/extutil.h>
#include <X11/extensions/hwcproto.h>
#include "hwc.h"

static char hwcExtensionName[] = HWC_NAME;
static XExtensionInfo *hwcInfo;

static int
HWCCloseDisplay(Display *dpy, XExtCodes *codes);
static int
HWCError(Display *display, xError *err, XExtCodes *codes, int *ret_code);
static Bool
HWCWireToCookie(Display	*dpy,XGenericEventCookie *cookie, xEvent	*event);
static Bool
HWCCopyCookie(Display *dpy, XGenericEventCookie *in, XGenericEventCookie *out);

static /* const */ XExtensionHooks hwcExtensionHooks = {
  NULL,                   /* create_gc */
  NULL,                   /* copy_gc */
  NULL,                   /* flush_gc */
  NULL,                   /* free_gc */
  NULL,                   /* create_font */
  NULL,                   /* free_font */
  HWCCloseDisplay,       /* close_display */
  NULL,        /* wire_to_event */
  NULL,        /* event_to_wire */
  HWCError,              /* error */
  NULL,                   /* error_string */
};

static XExtDisplayInfo *
HWCFindDisplay (Display *dpy)
{
    XExtDisplayInfo *dpyinfo;
    if (!hwcInfo)
    {
        if (!(hwcInfo = XextCreateExtension()))
            return NULL;
    }

    if (!(dpyinfo = XextFindDisplay (hwcInfo, dpy)))
    {
        dpyinfo = XextAddDisplay (hwcInfo,dpy,hwcExtensionName,&hwcExtensionHooks,0,NULL);
        if (!dpyinfo)
        {
            fprintf(stderr,"HWCFindDisplay: fail to AddDisplay\n");
            return NULL;
        }

        if (dpyinfo->codes) /* NULL if HWC doesn't exist on the server */
        {
            XESetWireToEventCookie(dpy, dpyinfo->codes->major_opcode, HWCWireToCookie);
            XESetCopyEventCookie(dpy, dpyinfo->codes->major_opcode, HWCCopyCookie);
        }
    }

    return dpyinfo;
}

static int
HWCCloseDisplay(Display *dpy, XExtCodes *codes)
{
   return XextRemoveDisplay (hwcInfo, dpy);
}

static int
HWCError(Display *display, xError *err, XExtCodes *codes, int *ret_code)
{
    return False;
}

static Bool
HWCWireToCookie(Display	*dpy, XGenericEventCookie *cookie, xEvent	*event)
{
    XExtDisplayInfo *info = HWCFindDisplay(dpy);
    xGenericEvent* ge = (xGenericEvent*)event;

    if (!info)
    {
        fprintf(stderr,"HWCWireToCookie: fail to find display info\n");
        return False;
    }

    if (ge->extension != info->codes->major_opcode)
    {
        fprintf(stderr, "HWCWireToCookie: wrong extension opcode %d\n",
                ge->extension);
        return False;
    }

    cookie->type = ge->type;
    cookie->serial = _XSetLastRequestRead(dpy, (xGenericReply *) event);
    cookie->send_event = ((event->u.u.type & 0x80) != 0);
    cookie->display = dpy;
    cookie->extension = ge->extension;
    cookie->evtype = ge->evtype;

    /*Fill cookie data*/
    switch(ge->evtype)
    {
        case HWCConfigureNotify:
        {
            xHWCConfigureNotify *in = (xHWCConfigureNotify*)event;
            HWCConfigureNotifyCookie* data;

            cookie->data = data = malloc(sizeof(HWCConfigureNotifyCookie));
            if (!data)
            {
                fprintf(stderr, "HWCWireToCookie: Fail to malloc ConfigureNotifyCookie\n");
                return False;
            }

            data->evtype = HWCConfigureNotify;
            data->maxLayer = in->maxLayer;
            return True;
        }
        default:
            printf("HWCWireToCookie: Unknown generic event. type %d\n", ge->evtype);
    }

    return False;
}

static Bool
HWCCopyCookie(Display *dpy, XGenericEventCookie *in, XGenericEventCookie *out)
{
    XExtDisplayInfo *info = HWCFindDisplay(dpy);

    if (!info)
    {
        fprintf(stderr,"HWCCopyCookie: fail to find display info\n");
        return False;
    }

    if (in->extension != info->codes->major_opcode)
    {
        fprintf(stderr,"HWCCopyCookie: wrong extension opcode %d\n",
                in->extension);
        return False;
    }

    *out = *in;
    out->data = NULL;
    out->cookie = 0;

    switch(in->evtype) {
        case HWCConfigureNotify:
        {
            HWCConfigureNotifyCookie *_in, *_out;

            _in = (HWCConfigureNotifyCookie *)in->data;
            _out = malloc(sizeof(HWCConfigureNotifyCookie));
            if (!_out)
            {
                fprintf(stderr, "HWCCopyCookie: Failed to malloc Cookie out\n");
                return False;
            }

            *_out = *_in;
            out->data = (void*)_out;
            return True;
        }
        default:
            fprintf(stderr, "HWCCopyCookie: Failed to copy evtype %d", in->evtype);
            break;
    }

    return False;
}

Bool
HWCQueryVersion(Display * dpy, int *major, int *minor)
{
   XExtDisplayInfo *info = HWCFindDisplay(dpy);
   xHWCQueryVersionReply rep;
   xHWCQueryVersionReq *req;

   XextCheckExtension(dpy, info, hwcExtensionName, False);

   LockDisplay(dpy);
   GetReq(HWCQueryVersion, req);
   req->reqType = info->codes->major_opcode;
   req->hwcReqType = X_HWCQueryVersion;
   req->majorVersion = HWC_MAJOR;
   req->minorVersion = HWC_MINOR;
   if (!_XReply(dpy, (xReply *) & rep, 0, xFalse)) {
      UnlockDisplay(dpy);
      SyncHandle();
      return False;
   }
   *major = rep.majorVersion;
   *minor = rep.minorVersion;
   UnlockDisplay(dpy);
   SyncHandle();

   return True;
}

Bool
HWCOpen(Display * dpy, Window window, int *maxLayer)
{
   XExtDisplayInfo *info = HWCFindDisplay(dpy);
   xHWCOpenReply rep;
   xHWCOpenReq *req;

   XextCheckExtension(dpy, info, hwcExtensionName, False);

   LockDisplay(dpy);
   GetReq(HWCOpen, req);
   req->reqType = info->codes->major_opcode;
   req->hwcReqType = X_HWCOpen;
   req->window = window;

   if (!_XReply(dpy, (xReply *) & rep, 0, xFalse)) {
      UnlockDisplay(dpy);
      SyncHandle();
      return False;
   }

   *maxLayer = rep.maxLayer;

   UnlockDisplay(dpy);
   SyncHandle();

   return True;
}

void
HWCSetDrawables(Display * dpy, Window window, Drawable *drawables, XRectangle *srcRect, XRectangle *dstRect, int count)
{
    XExtDisplayInfo *info = HWCFindDisplay(dpy);
    xHWCSetDrawablesReq *req;
    xHWCDrawInfo *p;
    int i;

    XextSimpleCheckExtension(dpy, info, hwcExtensionName);
    LockDisplay(dpy);

    GetReqExtra(HWCSetDrawables, count * SIZEOF(xHWCDrawInfo), req);

    req->reqType = info->codes->major_opcode;
    req->hwcReqType = X_HWCSetDrawables;
    req->window = window;
    req->count = count;
    p = (xHWCDrawInfo *) & req[1];
    for (i = 0; i < count; i++)
    {
        p[i].drawable = drawables[i];
        p[i].srcX = srcRect[i].x;
        p[i].srcY = srcRect[i].y;
        p[i].srcWidth = srcRect[i].width;
        p[i].srcHeight = srcRect[i].height;
        p[i].dstX = dstRect[i].x;
        p[i].dstY = dstRect[i].y;
        p[i].dstWidth = dstRect[i].width;
        p[i].dstHeight = dstRect[i].height;
    }

    UnlockDisplay(dpy);
    SyncHandle();
}

void
HWCSelectInput(Display * dpy, Window window, int mask)
{
    XExtDisplayInfo *info = HWCFindDisplay(dpy);
    xHWCSelectInputReq *req;

    XextSimpleCheckExtension(dpy, info, hwcExtensionName);
    LockDisplay(dpy);

    GetReq(HWCSelectInput, req);
    req->reqType = info->codes->major_opcode;
    req->hwcReqType = X_HWCSelectInput;
    req->window = window;
    req->eventMask = mask;

    UnlockDisplay(dpy);
    SyncHandle();
}

