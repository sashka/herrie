/*
 * Copyright (c) 2006-2007 Ed Schouten <ed@fxq.nl>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/**
 * @file vfs_xspf.c
 * @brief XSPF file access.
 */

#include <spiff/spiff_c.h>

#include "vfs_modules.h"

int
vfs_xspf_open(struct vfsent *ve, int isdir)
{
	/* In order to speed up the process, we only match *.xspf */
	if (isdir || !g_str_has_suffix(ve->name, ".xspf"))
		return (-1);

	return (0);
}

int
vfs_xspf_populate(struct vfsent *ve)
{
	struct spiff_list *slist;
	struct spiff_track *strack;
	struct spiff_mvalue *sloc;
	char *dirname, *filename;
	struct vfsref *vr;

	slist = spiff_parse(ve->filename);
	if (slist == NULL)
		return (-1);

	dirname = g_path_get_dirname(ve->filename);

	SPIFF_LIST_FOREACH_TRACK(slist, strack) {
		SPIFF_TRACK_FOREACH_LOCATION(strack, sloc) {
			/* XXX: Strip HTTP escape */
			filename = sloc->value;
			/* Skip file:// part */
			if (strncmp(filename, "file://", 7) == 0)
				filename += 7;

			/* Add it to the list */
			vr = vfs_open(filename, strack->title, dirname);
			if (vr != NULL)
				vfs_list_insert_tail(&ve->population, vr);
		}
	}
	
	g_free(dirname);
	spiff_free(slist);
	return (0);
}