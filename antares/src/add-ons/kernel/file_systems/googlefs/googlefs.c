/*
 * Copyright 2004-2008, François Revol, <revol@free.fr>.
 * Distributed under the terms of the MIT License.
 */

/*
 * googlefs - a bookmark-populated virtual filesystem using Google results.
 */

#define _BUILDING_fs 1

#include <sys/param.h>
#include <sys/stat.h>
#include <malloc.h>
#include <KernelExport.h>
//#include <NodeMonitor.h>
#include <stddef.h>
#include <signal.h>
#include <string.h>
#include <fs_query.h>
#include "query.h"
#include "googlefs.h"
#include "vnidpool.h"
#include "google_request.h"
#include "settings.h"

/* just publish fake entries; for debugging */
//#define NO_SEARCH

//#define DEBUG_GOOGLEFS 1

#if DEBUG_GOOGLEFS
#define PRINT(a) /*snooze(5000);*/ dprintf a
#else
#define PRINT(a) 
#endif
#define PFS "googlefs: "

/* needed to get /bin/df tell the mountpoint... */
#define ALLOW_DIR_OPEN

/* test:  */

vint32 refcount = 0;


extern struct attr_entry root_folder_attrs[];
extern struct attr_entry folders_attrs[];
extern struct attr_entry bookmark_attrs[];
extern struct attr_entry fake_bookmark_attrs[]; /* for debugging */
extern struct attr_entry template_1_attrs[];
extern struct attr_entry text_attrs[];
extern struct attr_entry mailto_me_bookmark_attrs[];

extern char *readmestr;

static int googlefs_create_gen(fs_nspace *ns, fs_node *dir, const char *name, int omode, int perms, ino_t *vnid, fs_node **node, struct attr_entry *iattrs, bool mkdir, bool uniq);

static void fill_default_stat(struct stat *st, nspace_id nsid, ino_t vnid, mode_t mode)
{
	time_t tm = time(NULL);
	st->st_dev = nsid;
	st->st_ino = vnid;
	st->st_mode = mode;
	st->st_nlink = 1;
	st->st_uid = 0;
	st->st_gid = 0;
	st->st_size = 0LL;
	st->st_blksize = 1024;
	st->st_atime = tm;
	st->st_mtime = tm;
	st->st_ctime = tm;
	st->st_crtime = tm;
}

/**	Publishes some entries in the root vnode: a query template, the readme file, and a People file of the author.
 */
int googlefs_publish_static_entries(fs_nspace *ns)
{
	status_t err = B_OK;
	fs_node *dir = ns->root;
	fs_node *n, *dummy;
	char ename[GOOGLEFS_NAME_LEN];
	char *p;
	int i;
	PRINT((PFS"googlefs_publish_static_entries(%ld)\n", ns->nsid));
	if (!ns || !dir)
		return EINVAL;

	err = googlefs_create_gen(ns, dir, "Search Google", 0, 0444, NULL, &n, template_1_attrs, false, true);
	if (err)
		return err;
	n->is_perm = 1;
	
	err = googlefs_create_gen(ns, dir, "README", 0, 0444, NULL, &n, text_attrs, false, true);
	if (err)
		return err;
	n->is_perm = 1;
	n->data = readmestr;
	n->data_size = strlen(n->data);// + 1;
	
	err = googlefs_create_gen(ns, dir, "Author", 0, 0444, NULL, &n, mailto_me_bookmark_attrs, false, true);
	if (err)
		return err;
	n->is_perm = 1;

	return B_OK;

err:
	PRINT((PFS"push_result_to_query: error 0x%08lx\n", err));
	return err;
}

#ifdef __ANTARES__
int googlefs_mount(nspace_id nsid, const char *devname, uint32 flags,
		const char *parms, fs_nspace **data, ino_t *vnid)
#else
int googlefs_mount(nspace_id nsid, const char *devname, ulong flags,
		const char *parms, size_t len, fs_nspace **data, ino_t *vnid)
#endif
{
	fs_nspace *ns;
	fs_node *root;
	int err;
	PRINT((PFS "mount(%ld, %s, 0x%08lx, %p, , )\n", nsid, devname, flags, parms));
#ifndef __ANTARES__
	(void) len;
#endif

	/* only allow a single mount */
	if (atomic_add(&refcount, 1))
		return EALREADY;
	
	err = load_settings();
	
	err = google_request_init();
	if (err)
		goto err_http;

	ns = malloc(sizeof(fs_nspace));
	if (!ns)
		return B_NO_MEMORY;
	memset(ns, 0, sizeof(fs_nspace));
	ns->nsid = nsid;
	err = vnidpool_alloc(&ns->vnids, MAX_VNIDS);
	if (err < 0)
		return err;
	err = vnidpool_get(ns->vnids, &ns->rootid);
	if (err < 0)
		return err;
	atomic_add(&ns->nodecount, 1);

	err = new_lock(&(ns->l), "googlefs main lock");
	if (err < 0)
		return err;
	ns->nodes = NULL;
	
	/* create root dir */
	err = B_NO_MEMORY;
	root = malloc(sizeof(fs_node));
	ns->root = root;
	if (root) {
		memset(root, 0, sizeof(fs_node));
		strcpy(root->name, ".");
		root->is_perm = 1;
		root->vnid = ns->rootid;
		fill_default_stat(&root->st, ns->nsid, ns->rootid, 0777 | S_IFDIR);
		root->attrs_indirect = root_folder_attrs;
		err = new_lock(&(root->l), "googlefs root dir");
		if (err >= 0) {
			*data = ns;
			*vnid = ns->rootid;
			ns->nodes = root; // sll_insert
			err = publish_vnode(nsid, *vnid, root);
			if (err == B_OK) {
				googlefs_publish_static_entries(ns);
				PRINT((PFS "mount() OK, nspace@ %p, id %ld, root@ %p, id %Ld\n", ns, ns->nsid, root, ns->rootid));
				return B_OK;
			}
			free_lock(&root->l);
		}
		free(root);
	}
	free_lock(&ns->l);
	free(ns);
err_http:
	atomic_add(&refcount, -1);
	return err;
}

int googlefs_unmount(fs_nspace *ns)
{
	status_t err;
	struct fs_node *node;
	PRINT((PFS "unmount(%ld)\n", ns->nsid));
	err = LOCK(&ns->l);
	if (err)
		return err;
	/* anything in still in use ? */
	for (node = ns->nodes; node; node = ns->nodes) {
		ns->nodes = node->nlnext; /* better cache that before we free node */
		googlefs_free_vnode(ns, node);
	}
	
	// Unlike in BeOS, we need to put the reference to our root node ourselves
#ifdef __ANTARES__
	put_vnode(ns->nsid, ns->rootid);
#endif
	
	free_lock(&ns->l);
	vnidpool_free(ns->vnids);
	free(ns);

	google_request_uninit();

	atomic_add(&refcount, -1);

	return B_OK;
}

static int compare_fs_node_by_vnid(fs_node *node, ino_t *id)
{
	return !(node->vnid == *id);
}

int googlefs_free_vnode(fs_nspace *ns, fs_node *node)
{
	free_lock(&node->l);
	atomic_add(&ns->nodecount, -1);
	vnidpool_put(ns->vnids, node->vnid);
	if (node->request)
		google_request_free(node->request);
	free(node->result);
	free(node);
	return 0;
}

#ifdef __ANTARES__
int googlefs_remove_vnode(fs_nspace *ns, char r, fs_node *node)
#else
int googlefs_remove_vnode(fs_nspace *ns, fs_node *node, char r)
#endif
{
	status_t err = B_OK;
	PRINT((PFS "remove_vnode(%ld, %Ld, %s)\n", ns->nsid, node->vnid, r?"r":"!r"));
	if (!r)
		err = LOCK(&ns->l);
	if (err)
		return err;
	if (node->vnid == ns->rootid) {
		PRINT((PFS "asked to remove the root node!!\n"));
	}
	//LOCK(&node->l);
	err = SLL_REMOVE(ns->nodes, nlnext, node);
	/* query dirs must be removed from the query list too */
	err = SLL_REMOVE(ns->queries, qnext, node);
	if (node->parent) {
		LOCK(&node->parent->l);
		SLL_REMOVE(node->parent->children, next, node);
		UNLOCK(&node->parent->l);
	}
	googlefs_free_vnode(ns, node);
	if (!r)
		UNLOCK(&ns->l);
	return err;
}

#ifdef __ANTARES__
int googlefs_read_vnode(fs_nspace *ns, ino_t vnid, fs_node **node, char r)
#else
int googlefs_read_vnode(fs_nspace *ns, ino_t vnid, char r, fs_node **node)
#endif
{
	fs_node *n;
	status_t err = B_OK;
	PRINT((PFS "read_vnode(%ld, %Ld, %s)\n", ns->nsid, vnid, r?"r":"!r"));
	if (!r)
		err = LOCK(&ns->l);
	if (err)
		return err;
	n = (fs_node *)SLL_FIND(ns->nodes, nlnext, (sll_compare_func)compare_fs_node_by_vnid, (void *)&vnid);
	if (n)
		*node = n;
	else
		err = ENOENT;
	if (!r)
		UNLOCK(&ns->l);
	return err;
}

#ifdef __ANTARES__
int googlefs_release_vnode(fs_nspace *ns, fs_node *node, char r)
#else
int googlefs_write_vnode(fs_nspace *ns, fs_node *node, char r)
#endif
{
	PRINT((PFS "write_vnode(%ld, %Ld, %s)\n", ns->nsid, node->vnid, r?"r":"!r"));
	return B_OK;
}

#ifndef __ANTARES__
int googlefs_secure_vnode(fs_nspace *ns, fs_node *node)
{
	PRINT((PFS "secure_vnode(%ld, %Ld)\n", ns->nsid, node->vnid));
	return B_OK;
}
#endif

static int compare_fs_node_by_name(fs_node *node, char *name)
{
	//return memcmp(node->name, name, GOOGLEFS_NAME_LEN);
	PRINT((PFS"find_by_name: '%s' <> '%s'\n", node->name, name));
	return strncmp(node->name, name, GOOGLEFS_NAME_LEN);
}

#ifdef __ANTARES__
int googlefs_get_vnode_name(fs_nspace *ns, fs_node *node, char *buffer, size_t len)
{
	fs_node *n, *dummy;
	status_t err = B_OK;
	PRINT((PFS "get_vnode_name(%ld, %Ld, )\n", ns->nsid, (int64)(node?node->vnid:-1)));
	strlcpy(buffer, node->name, MIN(GOOGLEFS_NAME_LEN, len));
	return B_OK;
}
#endif

#ifdef __ANTARES__
int googlefs_walk(fs_nspace *ns, fs_node *base, const char *file, ino_t *vnid, int *type)
#else
int googlefs_walk(fs_nspace *ns, fs_node *base, const char *file, char **newpath, ino_t *vnid)
#endif
{
	fs_node *n, *dummy;
	status_t err = B_OK;
	PRINT((PFS "walk(%ld, %Ld, %s)\n", ns->nsid, (int64)(base?base->vnid:-1), file));
	err = LOCK(&base->l);
	if (err)
		return err;
	if (!file) {
		err = EINVAL;
	} else if (!strcmp(file, "..")) {
		if (base && base->parent) {
			*vnid = base->parent->vnid; // XXX: LOCK(&base->l) ?
#ifdef __ANTARES__
			*type = S_IFDIR;
#endif
		} else
			err = EINVAL;
	} else if (!strcmp(file, ".")) { /* root dir */
		if (base) { // XXX: LOCK(&base->l) ?
			*vnid = base->vnid;
#ifdef __ANTARES__
			*type = S_IFDIR;
#endif
		} else
			err = EINVAL;
	} else if (base) { /* child of dir */
		n = (fs_node *)SLL_FIND(base->children, next, 
								(sll_compare_func)compare_fs_node_by_name, (void *)file);
		if (n) {
			*vnid = n->vnid;
#ifdef __ANTARES__
			*type = n->st.st_type & ~S_IUMSK; /*XXX: S_IFMT ?*/
#endif
		} else
			err = ENOENT;
	} else
		err = ENOENT;
	if (err == B_OK) {
		if (get_vnode(ns->nsid, *vnid, (void **)&dummy) != 0) /* inc ref count */
			err = EINVAL;
	}
	UNLOCK(&base->l);
	PRINT((PFS "walk() -> error 0x%08lx\n", err));
	return err;
}

int googlefs_opendir(fs_nspace *ns, fs_node *node, fs_dir_cookie **cookie)
{
	status_t err = B_OK;
	fs_dir_cookie *c;
	PRINT((PFS "opendir(%ld, %Ld)\n", ns->nsid, node->vnid));
	if (!node)
		return EINVAL;
	if (!S_ISDIR(node->st.st_mode))
		return B_NOT_A_DIRECTORY;
	err = LOCK(&node->l);
	if (err)
		return err;
	c = malloc(sizeof(fs_dir_cookie));
	if (c) {
		memset(c, 0, sizeof(fs_dir_cookie));
		c->omode = O_RDONLY;
		c->type = S_IFDIR;
		c->node = node;
		c->dir_current = 0;
		*cookie = c;
		SLL_INSERT(node->opened, next, c);
		UNLOCK(&node->l);
		return B_OK;
	} else
		err = B_NO_MEMORY;
	UNLOCK(&node->l);
	return err;
}

int googlefs_closedir(fs_nspace *ns, fs_node *node, fs_dir_cookie *cookie)
{
	status_t err = B_OK;
	PRINT((PFS "closedir(%ld, %Ld)\n", ns->nsid, node->vnid));
	err = LOCK(&node->l);
	if (err)
		return err;
	
	SLL_REMOVE(node->opened, next, cookie);
	UNLOCK(&node->l);
	return err;
}

int googlefs_rewinddir(fs_nspace *ns, fs_node *node, fs_dir_cookie *cookie)
{
	PRINT((PFS "rewinddir(%ld, %Ld)\n", ns->nsid, node->vnid));
	cookie->dir_current = 0;
	return B_OK;
}

#ifdef __ANTARES__
int googlefs_readdir(fs_nspace *ns, fs_node *node, fs_dir_cookie *cookie, struct dirent *buf, size_t bufsize, uint32 *num)
#else
int googlefs_readdir(fs_nspace *ns, fs_node *node, fs_dir_cookie *cookie, long *num, struct dirent *buf, size_t bufsize)
#endif
{
	status_t err = B_OK;
	fs_node *n = NULL;
	fs_node *parent = node->parent;
	int index;
	PRINT((PFS "readdir(%ld, %Ld) @ %d\n", ns->nsid, node->vnid, cookie->dir_current));
	if (!node || !cookie || !num || !*num || !buf || (bufsize < (sizeof(dirent_t)+GOOGLEFS_NAME_LEN)))
		return EINVAL;
	err = LOCK(&node->l);
	if (cookie->dir_current == 0) { /* .. */
		PRINT((PFS "readdir: giving ..\n"));
		/* the VFS will correct that anyway */
		buf->d_dev = ns->nsid;
		buf->d_pdev = ns->nsid;
		buf->d_ino = parent?parent->vnid:ns->rootid;
		buf->d_pino = (parent && parent->parent)?parent->parent->vnid:ns->rootid;
		strcpy(buf->d_name, "..");
		buf->d_reclen = 2*(sizeof(dev_t)+sizeof(ino_t))+sizeof(unsigned short)+strlen(buf->d_name)+1;
		cookie->dir_current++;
		*num = 1;
	} else if (cookie->dir_current == 1) { /* . */
		PRINT((PFS "readdir: giving .\n"));
		/* the VFS will correct that anyway */
		buf->d_dev = ns->nsid;
		buf->d_pdev = ns->nsid;
		buf->d_ino = node->vnid;
		buf->d_pino = parent?parent->vnid:ns->rootid;
		strcpy(buf->d_name, ".");
		buf->d_reclen = 2*(sizeof(dev_t)+sizeof(ino_t))+sizeof(unsigned short)+strlen(buf->d_name)+1;
		cookie->dir_current++;
		*num = 1;
	} else {
		index = cookie->dir_current-2;
		for (n = node->children; n && index; n = n->next, index--); //XXX: care about n->hidden || n->deleted
		if (n) {
			PRINT((PFS "readdir: giving ino %Ld, %s\n", n->vnid, n->name));
			buf->d_dev = ns->nsid;
			buf->d_pdev = ns->nsid;
			buf->d_ino = n->vnid;
			buf->d_pino = node->vnid;
			strcpy(buf->d_name, n->name);
			buf->d_reclen = 2*(sizeof(dev_t)+sizeof(ino_t))+sizeof(unsigned short)+strlen(buf->d_name)+1;
			cookie->dir_current++;
			*num = 1;
		} else {
			*num = 0;
		}
	}
	UNLOCK(&node->l);
	return B_OK;
}

int googlefs_free_dircookie(fs_nspace *ns, fs_node *node, fs_dir_cookie *cookie)
{
	PRINT((PFS"freedircookie(%ld, %Ld)\n", ns->nsid, node?node->vnid:0LL));
	free(cookie);
	return B_OK;
}

int googlefs_rstat(fs_nspace *ns, fs_node *node, struct stat *st)
{
	status_t err = B_OK;
	if (!node || !st)
		return EINVAL;
	err = LOCK(&node->l);
	if (err)
		return err;
	memcpy(st, &node->st, sizeof(struct stat));
	st->st_dev = ns->nsid;
	st->st_ino = node->vnid;
	if (node->data_size)
		st->st_size = node->data_size;
	//st->st_size = 0LL;
	UNLOCK(&node->l);
	return err;
}

int googlefs_rfsstat(fs_nspace *ns, struct fs_info *info)
{
	info->block_size=1024;//googlefs_BUFF_SIZE;
	info->io_size=1024;//GOOGLEFS_BUFF_SIZE;
	info->total_blocks=0;
	info->free_blocks=0;
	info->total_nodes=MAX_VNIDS;
	info->free_nodes=ns->nodecount;
	info->dev=ns->nsid;
	info->root=ns->rootid;
	info->flags=/*B_FS_IS_SHARED|*/B_FS_IS_PERSISTENT|B_FS_HAS_MIME|B_FS_HAS_ATTR|B_FS_HAS_QUERY;
	strcpy (info->device_name, "");
	strcpy (info->volume_name, "Google");
	strcpy (info->fsh_name, GOOGLEFS_NAME);
	return B_OK;
}

int googlefs_open(fs_nspace *ns, fs_node *node, int omode, fs_file_cookie **cookie)
{
	status_t err = B_OK;
	fs_node *dummy;
	fs_file_cookie *fc;
	PRINT((PFS"open(%ld, %Ld, 0x%x)\n", ns->nsid, node->vnid, omode));
	if (!node || !cookie)
		return EINVAL;
	
//	err = LOCK(&ns->l);
//	if (err)
//		return err;
	err = LOCK(&node->l);
	if (err)
		goto err_n_l;
	err = EEXIST;
#ifndef ALLOW_DIR_OPEN
	err = EINVAL;//EISDIR;
	if (S_ISDIR(node->st.st_mode))
		goto err_malloc;
#endif
	err = B_NO_MEMORY;
	fc = malloc(sizeof(fs_file_cookie));
	if (!fc)
		goto err_malloc;
	memset(fc, 0, sizeof(fs_file_cookie));
	fc->node = node;
	fc->omode = omode;
	fc->type = S_IFREG;
	err = SLL_INSERT(node->opened, next, fc);
	if (err)
		goto err_linsert;
/*	err = get_vnode(ns->nsid, node->vnid, &dummy);
	if (err)
		goto err_getvn;*/
	//*vnid = node->vnid;
	*cookie = fc;
	err = B_OK;
	goto all_ok;
//err_:
//	put_vnode(ns->nsid, node->nsid);
err_getvn:
	SLL_REMOVE(node->opened, next, fc);
err_linsert:
	free(fc);
err_malloc:
all_ok:
	UNLOCK(&node->l);
err_n_l:
//	UNLOCK(&ns->l);
	return err;
}

int googlefs_close(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie)
{
	status_t err;
	PRINT((PFS"close(%ld, %Ld)\n", ns->nsid, node->vnid));
	if (!ns || !node || !cookie)
		return EINVAL;
	err = LOCK(&node->l);
	if (err)
		return err;
	SLL_REMOVE(node->opened, next, cookie);
	
all_ok:
err_n_l:
	UNLOCK(&node->l);
	return err;
}

int googlefs_free_cookie(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie)
{
	status_t err = B_OK;
	PRINT((PFS"freecookie(%ld, %Ld)\n", ns->nsid, node->vnid));
	err = LOCK(&node->l);
	if (err)
		return err;
	err = SLL_REMOVE(node->opened, next, cookie); /* just to amke sure */
//	if (err)
//		goto err_n_l;
	if (/*!node->is_perm &&*/ false) { /* not yet */
		err = remove_vnode(ns->nsid, node->vnid);
		ns->root->st.st_mtime = time(NULL);
		notify_listener(B_ENTRY_REMOVED, ns->nsid, ns->rootid, 0LL, node->vnid, NULL);
		notify_listener(B_STAT_CHANGED, ns->nsid, 0LL, 0LL, ns->rootid, NULL);
	}
	UNLOCK(&node->l);
	free(cookie);
//	err = B_OK;
err_n_l:
	return err;
}

int googlefs_read(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie, off_t pos, void *buf, size_t *len)
{
	status_t err = B_OK;
	PRINT((PFS"read(%ld, %Ld, %Ld, %ld)\n", ns->nsid, node->vnid, pos, *len));
	if (node->data_size == 0 || !node->data)
		err = ENOSYS;
	if (pos < 0 || pos > node->data_size)
		err = EFPOS;
	if (err) {
		*len = 0;
		return err;
	}
	*len = MIN(*len, node->data_size - (long)pos);
	memcpy(buf, ((char *)node->data) + pos, *len);
	return B_OK;
}

int googlefs_write(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie, off_t pos, const void *buf, size_t *len)
{
	PRINT((PFS"write(%ld, %Ld, %Ld, %ld)\n", ns->nsid, node->vnid, pos, *len));
	*len = 0;
	return ENOSYS;
}

int googlefs_wstat(fs_nspace *ns, fs_node *node, struct stat *st, long mask)
{
	PRINT((PFS"wstat(%ld, %Ld, , 0x%08lx)\n", ns->nsid, node->vnid, mask));
	return ENOSYS;
}

int googlefs_wfsstat(fs_nspace *ns, struct fs_info *info, long mask)
{
	PRINT((PFS"wfsstat(%ld, , 0x%08lx)\n", ns->nsid, mask));
	return ENOSYS;
}

/* this one returns the created fs_node to caller (for use by query engine) */
/**
 * @param dir the dir's fs_node we mkdir in
 * @param name name to mkdir (basename is uniq is set)
 * @param perms create with those permissions
 * @param node make this point to the fs_node if !NULL
 * @param iattr indirect attributes to set if desired (must be statically allocated)
 * @param mkdir create a directory instead of a file
 * @param uniq choose an unique name, appending a number if required
 */
static int googlefs_create_gen(fs_nspace *ns, fs_node *dir, const char *name, int omode, int perms, ino_t *vnid, fs_node **node, struct attr_entry *iattrs, bool mkdir, bool uniq)
{
	char newname[GOOGLEFS_NAME_LEN];
	status_t err;
	fs_node *n;
	int i;
	PRINT((PFS"create_gen(%ld, %Ld, '%s', 0x%08lx, %c, %c)\n", ns->nsid, dir->vnid, name, omode, mkdir?'t':'f', uniq?'t':'f'));
	
	if (strlen(name) > GOOGLEFS_NAME_LEN-1)
		return ENAMETOOLONG;
	err = LOCK(&dir->l);
	if (err < 0)
		return err;
	err = ENOTDIR;
	if (!S_ISDIR(dir->st.st_mode))
		goto err_l;
	n = (fs_node *)SLL_FIND(dir->children, next, 
							(sll_compare_func)compare_fs_node_by_name, (void *)name);
	err = EEXIST;
	if (n && (omode & O_EXCL) && !uniq) /* already existing entry in there! */
		goto err_l;

	strncpy(newname, name, GOOGLEFS_NAME_LEN);
	newname[GOOGLEFS_NAME_LEN-1] = '\0';

	for (i = 1; uniq && n && i < 5000; i++) { /* uniquify the name */
		//sprintf("%"#(GOOGLEFS_NAME_LEN-8)"s %05d", name, i);
		strncpy(newname, name, 56);
		newname[56] = '\0';
		sprintf(newname+strlen(newname), " %05d", i);
		n = (fs_node *)SLL_FIND(dir->children, next, 
								(sll_compare_func)compare_fs_node_by_name, (void *)newname);
	}
	if (n && (uniq || mkdir)) /* still there! */
		goto err_l;
	name = newname;
	
	if (n) { /* already exists, so return it */
		if (node)
			*node = n;
		if (vnid)
			*vnid = n->vnid;
		err = B_OK;
		goto done;
	}
	err = ENOMEM;
	n = malloc(sizeof(fs_node));
	if (!n)
		goto err_l;
	memset(n, 0, sizeof(fs_node));
	err = vnidpool_get(ns->vnids, &n->vnid);
	if (err < B_OK)
		goto err_m;
	atomic_add(&ns->nodecount, 1);
	strcpy(n->name, name);
	//n->is_perm = 1;
	fill_default_stat(&n->st, ns->nsid, n->vnid, (perms & ~S_IFMT) | (mkdir?S_IFDIR:S_IFREG));
	err = new_lock(&(n->l), mkdir?"googlefs dir":"googlefs file");
	if (err)
		goto err_m;
	err = LOCK(&ns->l);
	if (err)
		goto err_nl;
	err = SLL_INSERT(ns->nodes, nlnext, n);
	if (err)
		goto err_lns;
	/* _TAIL so they are in order */
	err = SLL_INSERT(dir->children, next, n);
	if (err)
		goto err_insnl;
//	err = new_vnode(ns->nsid, n->vnid, n);
//	if (err)
//		goto err_ins;
	n->parent = dir;
	dir->st.st_nlink++;
	UNLOCK(&ns->l);
	n->attrs_indirect = iattrs;
	notify_listener(B_ENTRY_CREATED, ns->nsid, dir->vnid, 0, n->vnid, name);
	/* dosfs doesn't do that one but I believe it should */
	notify_listener(B_STAT_CHANGED, ns->nsid, 0LL, 0LL, dir->vnid, NULL);
	/* give node to caller if it wants it */
	if (node)
		*node = n;
	if (vnid)
		*vnid = n->vnid;
	goto done;
	
err_insnl:
	SLL_REMOVE(ns->nodes, nlnext, n);
err_lns:
	UNLOCK(&ns->l);
err_nl:
	free_lock(&n->l);
	atomic_add(&ns->nodecount, -1);
err_m:
	free(n);
err_l:
done:
	UNLOCK(&dir->l);
	return err;
}

int googlefs_create(fs_nspace *ns, fs_node *dir, const char *name, int omode, int perms, ino_t *vnid, fs_file_cookie **cookie)
{
	status_t err;
	fs_node *n;
	PRINT((PFS"create(%ld, %Ld, '%s', 0x%08lx)\n", ns->nsid, dir->vnid, name, omode));
	/* don't let ppl mess our fs up */
	return ENOSYS;
	
	err = googlefs_create_gen(ns, dir, name, omode, perms, vnid, &n, NULL, false, false);
	if (err)
		return err;
	err = googlefs_open(ns, n, omode, cookie);
	return err;
}

int googlefs_unlink(fs_nspace *ns, fs_node *dir, const char *name)
{
	status_t err;
	fs_node *n;
	PRINT((PFS"unlink(%ld, %Ld, %s)\n", ns->nsid, dir->vnid, name));
	//dprintf(PFS"unlink(%ld, %Ld, %s)\n", ns->nsid, dir->vnid, name);
	err = LOCK(&dir->l);
	if (err)
		return err;
	err = ENOENT;
	/* no need to check for S_ISDIR */
	n = (fs_node *)SLL_FIND(dir->children, next, 
							(sll_compare_func)compare_fs_node_by_name, (void *)name);
	if (n) {
		if (n->children)
			err = ENOTEMPTY;
		else if (n->is_perm)
			err = EROFS;
		//else if (S_ISDIR(n->st.st_mode))
		//	err = EISDIR;
		else if (n->vnid == ns->rootid)
			err = EACCES;
		else {
			SLL_REMOVE(dir->children, next, n);
			notify_listener(B_ENTRY_REMOVED, ns->nsid, dir->vnid, 0LL, n->vnid, NULL);
			notify_listener(B_STAT_CHANGED, ns->nsid, 0LL, 0LL, dir->vnid, NULL);
			remove_vnode(ns->nsid, n->vnid);
			err = B_OK;
		}
	}
	UNLOCK(&dir->l);
	return err;
}

int googlefs_rmdir(fs_nspace *ns, fs_node *dir, const char *name)
{
	PRINT((PFS"rmdir(%ld, %Ld, %s)\n", ns->nsid, dir->vnid, name));
	return googlefs_unlink(ns, dir, name);
}

static int googlefs_unlink_node_rec(fs_nspace *ns, fs_node *node)
{
	status_t err;
	fs_node *n;
	PRINT((PFS"googlefs_unlink_node_rec(%ld, %Ld:%s)\n", ns->nsid, node->vnid, node->name));
	if (!ns || !node)
		return EINVAL;
	// kill_request();
	LOCK(&node->l);
	while (1) {
		n = node->children;
		if (!n)
			break;
		UNLOCK(&node->l);
		err = googlefs_unlink_node_rec(ns, n);
		LOCK(&node->l);
	}
	UNLOCK(&node->l);
	err = googlefs_unlink(ns, node->parent, node->name);
	return err;
}

int googlefs_access(fs_nspace *ns, fs_node *node, int mode)
{
	PRINT((PFS"access(%ld, %Ld, 0x%x)\n", ns->nsid, node->vnid, mode));
	return B_OK;
}

int googlefs_setflags(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie, int flags)
{
	return EINVAL;
}

#if 0
static int googlefs_mkdir_gen(fs_nspace *ns, fs_node *dir, const char *name, int perms, fs_node **node, bool uniq)
{
	char newname[GOOGLEFS_NAME_LEN];
	status_t err;
	fs_node *n;
	int i;
	PRINT((PFS"mkdir_gen(%ld, %Ld, '%s', 0x%08lx, %c)\n", ns->nsid, dir->vnid, name, perms, uniq?'t':'f'));
	
	if (strlen(name) > GOOGLEFS_NAME_LEN-1)
		return ENAMETOOLONG;
	err = LOCK(&dir->l);
	if (err < 0)
		return err;
	err = ENOTDIR;
	if (!S_ISDIR(dir->st.st_mode))
		goto err_l;
	n = (fs_node *)SLL_FIND(dir->children, next, 
							(sll_compare_func)compare_fs_node_by_name, (void *)name);
	err = EEXIST;
	if (n && !uniq) /* already existing entry in there! */
		goto err_l;
	
	strncpy(newname, name, GOOGLEFS_NAME_LEN);
	newname[GOOGLEFS_NAME_LEN-1] = '\0';
	for (i = 1; n && i < 5000; i++) { /* uniquify the name */
		//sprintf("%"#(GOOGLEFS_NAME_LEN-8)"s %05d", name, i);
		sprintf("%56s %05d", name, i);
		n = (fs_node *)SLL_FIND(dir->children, next, 
								(sll_compare_func)compare_fs_node_by_name, (void *)newname);
	}
	if (n) /* still there! */
		goto err_l;
	name = newname;
	
	err = ENOMEM;
	n = malloc(sizeof(fs_node));
	if (!n)
		goto err_l;
	memset(n, 0, sizeof(fs_node));
	err = vnidpool_get(ns->vnids, &n->vnid);
	if (err < B_OK)
		goto err_m;
	atomic_add(&ns->nodecount, 1);
	strcpy(n->name, name);
	//n->is_perm = 1;
	fill_default_stat(&n->st, ns->nsid, n->vnid, (perms & ~S_IFMT) | S_IFDIR);
	err = new_lock(&(n->l), "googlefs dir");
	if (err)
		goto err_m;
	err = LOCK(&ns->l);
	if (err)
		goto err_nl;
	err = SLL_INSERT(ns->nodes, nlnext, n);
	if (err)
		goto err_lns;
	err = SLL_INSERT(dir->children, next, n);
	if (err)
		goto err_insnl;
//	err = new_vnode(ns->nsid, n->vnid, n);
//	if (err)
//		goto err_ins;
	n->parent = dir;
	dir->st.st_nlink++;
	UNLOCK(&ns->l);
	n->attrs_indirect = folders_attrs;
	notify_listener(B_ENTRY_CREATED, ns->nsid, dir->vnid, 0, n->vnid, name);
	/* dosfs doesn't do that one but I believe it should */
	notify_listener(B_STAT_CHANGED, ns->nsid, 0LL, 0LL, dir->vnid, NULL);
	/* give node to caller if it wants it */
	if (node)
		*node = n;
	goto done;
	
err_insnl:
	SLL_REMOVE(ns->nodes, nlnext, n);
err_lns:
	UNLOCK(&ns->l);
err_nl:
	free_lock(&n->l);
	atomic_add(&ns->nodecount, -1);
err_m:
	free(n);
err_l:
done:
	UNLOCK(&dir->l);
	return err;
}
#endif

int googlefs_mkdir(fs_nspace *ns, fs_node *dir, const char *name, int perms)
{
	PRINT((PFS"mkdir(%ld, %Ld, '%s', 0x%08lx)\n", ns->nsid, dir->vnid, name, perms));
	return googlefs_create_gen(ns, dir, name, O_EXCL, perms, NULL, NULL, folders_attrs, true, false);
}

/* attr stuff */

int googlefs_open_attrdir(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie **cookie)
{
	status_t err = B_OK;
	fs_attr_dir_cookie *c;
	PRINT((PFS "open_attrdir(%ld, %Ld)\n", ns->nsid, node->vnid));
	if (!node)
		return EINVAL;
	err = LOCK(&node->l);
	if (err)
		return err;
	c = malloc(sizeof(fs_attr_dir_cookie));
	if (c) {
		memset(c, 0, sizeof(fs_attr_dir_cookie));
		c->omode = O_RDONLY;
		c->type = S_ATTR_DIR;
		c->node = node;
		c->dir_current = 0;
		*cookie = c;
		SLL_INSERT(node->opened, next, c);
		UNLOCK(&node->l);
		return B_OK;
	} else
		err = B_NO_MEMORY;
	UNLOCK(&node->l);
	return err;
}

int googlefs_close_attrdir(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie *cookie)
{
	status_t err = B_OK;
	PRINT((PFS "close_attrdir(%ld, %Ld)\n", ns->nsid, node->vnid));
	err = LOCK(&node->l);
	if (err)
		return err;
	SLL_REMOVE(node->opened, next, cookie);
	UNLOCK(&node->l);
	return err;
}

int googlefs_free_attrdircookie(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie *cookie)
{
	PRINT((PFS"free_attrdircookie(%ld, %Ld)\n", ns->nsid, node->vnid));
	free(cookie);
	return B_OK;
}

int googlefs_rewind_attrdir(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie *cookie)
{
	PRINT((PFS "rewind_attrdir(%ld, %Ld)\n", ns->nsid, node->vnid));
	cookie->dir_current = 0;
	return B_OK;
}

#ifdef __ANTARES__
int googlefs_read_attrdir(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie *cookie, struct dirent *buf, size_t bufsize, uint32 *num)
#else
int googlefs_read_attrdir(fs_nspace *ns, fs_node *node, fs_attr_dir_cookie *cookie, long *num, struct dirent *buf, size_t bufsize)
#endif
{
	status_t err = B_OK;
	fs_node *n = NULL;
	fs_node *parent = node->parent;
	attr_entry *ae = NULL;
	int i;
	int count_indirect;
	PRINT((PFS "read_attrdir(%ld, %Ld) @ %d\n", ns->nsid, node->vnid, cookie->dir_current));
	if (!node || !cookie || !num || !*num || !buf || (bufsize < (sizeof(dirent_t)+GOOGLEFS_NAME_LEN)))
		return EINVAL;
	err = LOCK(&node->l);
	for (i = 0, count_indirect = 0; node->attrs_indirect && !ae && node->attrs_indirect[i].name; i++, count_indirect++)
		if (i == cookie->dir_current)
			ae = &node->attrs_indirect[i];
	for (i = 0; !ae && i < 10 && node->attrs[i].name; i++)
		if (i + count_indirect == cookie->dir_current)
			ae = &node->attrs[i];
	
	if (ae) {
		PRINT((PFS "read_attrdir: giving %s\n", ae->name));
		buf->d_dev = ns->nsid;
		buf->d_pdev = ns->nsid;
		buf->d_ino = node->vnid;
		buf->d_pino = node->parent?node->parent->vnid:ns->rootid;
		strcpy(buf->d_name, ae->name);
		buf->d_reclen = 2*(sizeof(dev_t)+sizeof(ino_t))+sizeof(unsigned short)+strlen(buf->d_name)+1;
		cookie->dir_current++;
		*num = 1;
	} else
		*num = 0;
	
	UNLOCK(&node->l);
	return B_OK;
}

/* Antares and BeOs differ in the way the handle attributes at the vfs layer. 
   BeOS uses atomic calls on the vnode,
   Antares retains the open/close/read/write semantics for attributes (loosing atomicity).
   Here we don't care much though, open is used for both to factorize attribute lookup. <- TODO
   _h suffixed funcs are for Antares API, _b are for BeOS.
 */

/* for Antares, but also used by BeOS calls to factorize code */
int googlefs_open_attr_h(fs_nspace *ns, fs_node *node, const char *name, int omode, fs_file_cookie **cookie)
{
	status_t err = B_OK;
	fs_node *dummy;
	fs_file_cookie *fc;
	attr_entry *ae = NULL;
	int i;
	PRINT((PFS"open_attr(%ld, %Ld, %s, 0x%x)\n", ns->nsid, node->vnid, name, omode));
	if (!node || !name || !cookie)
		return EINVAL;

	err = LOCK(&node->l);
	if (err)
		goto err_n_l;

	/* lookup attribute */
	for (i = 0; node->attrs_indirect && !ae && node->attrs_indirect[i].name; i++)
		if (!strcmp(name, node->attrs_indirect[i].name))
			ae = &node->attrs_indirect[i];
	for (i = 0; !ae && i < 10 && node->attrs[i].name; i++)
		if (!strcmp(name, node->attrs[i].name))
			ae = &node->attrs[i];

	/* should check omode */
	err = ENOENT;
	if (!ae)
		goto err_malloc;
	err = EEXIST;

	err = B_NO_MEMORY;
	fc = malloc(sizeof(fs_file_cookie));
	if (!fc)
		goto err_malloc;
	memset(fc, 0, sizeof(fs_file_cookie));
	fc->node = node;
	fc->omode = omode;
	fc->type = S_ATTR;
	fc->attr = ae;
	err = SLL_INSERT(node->opened, next, fc);
	if (err)
		goto err_linsert;
	
	*cookie = fc;
	err = B_OK;
	goto all_ok;
//err_:
//	put_vnode(ns->nsid, node->nsid);
err_getvn:
	SLL_REMOVE(node->opened, next, fc);
err_linsert:
	free(fc);
err_malloc:
all_ok:
	UNLOCK(&node->l);
err_n_l:
//	UNLOCK(&ns->l);
	return err;
}

int googlefs_close_attr_h(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie)
{
	status_t err;
	PRINT((PFS"close_attr(%ld, %Ld:%s)\n", ns->nsid, node->vnid, cookie->attr?cookie->attr->name:"?"));
	if (!ns || !node || !cookie)
		return EINVAL;
	err = LOCK(&node->l);
	if (err)
		return err;
	SLL_REMOVE(node->opened, next, cookie);
	
all_ok:
err_n_l:
	UNLOCK(&node->l);
	return err;
}

int googlefs_free_attr_cookie_h(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie)
{
	status_t err = B_OK;
	PRINT((PFS"free_attrcookie(%ld, %Ld:%s)\n", ns->nsid, node->vnid, cookie->attr?cookie->attr->name:"?"));
	err = LOCK(&node->l);
	if (err)
		return err;
	err = SLL_REMOVE(node->opened, next, cookie); /* just to amke sure */
//	if (err)
//		goto err_n_l;
	UNLOCK(&node->l);
	free(cookie);
//	err = B_OK;
err_n_l:
	return err;
}

#ifdef __ANTARES__

int	googlefs_read_attr_stat_h(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie, struct stat *st)
{
	status_t err = B_OK;
	attr_entry *ae = cookie->attr;
	PRINT((PFS"stat_attr(%ld, %Ld:%s)\n", ns->nsid, node->vnid, ae->name));
	if (!node || !st || !cookie || !cookie->attr)
		return EINVAL;
	memcpy(st, &node->st, sizeof(struct stat));
	st->st_type = ae->type;
	st->st_size = ae->size;
	err = B_OK;
	return err;
}

int	googlefs_read_attr_h(fs_nspace *ns, fs_node *node, fs_file_cookie *cookie, off_t pos, void *buf, size_t *len)
{
	status_t err = B_OK;
	attr_entry *ae = cookie->attr;
	PRINT((PFS"read_attr(%ld, %Ld:%s)\n", ns->nsid, node->vnid, ae->name));
	if (!node || !cookie || !len || !*len)
		return EINVAL;

	err = LOCK(&node->l);
	
	if (ae && pos < ae->size) {
		memcpy(buf, (char *)ae->value + pos, MIN(*len, ae->size-pos));
		*len = MIN(*len, ae->size-pos);
		err = B_OK;
	} else {
		*len = 0;
		err = ENOENT;
	}
	
	UNLOCK(&node->l);
	return err;
}


#else

int	googlefs_remove_attr_b(fs_nspace *ns, fs_node *node, const char *name)
{
	PRINT((PFS"%s()\n", __FUNCTION__));
	return ENOSYS;
}

int	googlefs_rename_attr_b(fs_nspace *ns, fs_node *node, const char *oldname, const char *newname)
{
	PRINT((PFS"%s()\n", __FUNCTION__));
	return ENOSYS;
}

int	googlefs_stat_attr_b(fs_nspace *ns, fs_node *node, const char *name, struct attr_info *buf)
{
	status_t err = B_OK;
	attr_entry *ae = NULL;
	int i;
	PRINT((PFS"stat_attr(%ld, %Ld, %s)\n", ns->nsid, node->vnid, name));
	if (!node || !name || !buf)
		return EINVAL;
	err = LOCK(&node->l);
	for (i = 0; node->attrs_indirect && !ae && node->attrs_indirect[i].name; i++)
		if (!strcmp(name, node->attrs_indirect[i].name))
			ae = &node->attrs_indirect[i];
	for (i = 0; !ae && i < 10 && node->attrs[i].name; i++)
		if (!strcmp(name, node->attrs[i].name))
			ae = &node->attrs[i];

	if (ae) {
		buf->type = ae->type;
		buf->size = ae->size;
		err = B_OK;
	} else
		err = ENOENT;
	
	UNLOCK(&node->l);
	return err;
}

int	googlefs_write_attr_b(fs_nspace *ns, fs_node *node, const char *name, int type,
					const void *buf, size_t *len, off_t pos)
{
	PRINT((PFS"%s()\n", __FUNCTION__));
	*len = 0;
	return ENOSYS;
}

int	googlefs_read_attr_b(fs_nspace *ns, fs_node *node, const char *name, int type,
					void *buf, size_t *len, off_t pos)
{
	status_t err = B_OK;
	attr_entry *ae = NULL;
	int i;
	PRINT((PFS"read_attr(%ld, %Ld, %s, 0x%08lx)\n", ns->nsid, node->vnid, name, type));
	if (!node || !name || !len || !*len)
		return EINVAL;
	err = LOCK(&node->l);
	for (i = 0; node->attrs_indirect && !ae && node->attrs_indirect[i].name; i++)
		if (!strcmp(name, node->attrs_indirect[i].name))
			ae = &node->attrs_indirect[i];
	for (i = 0; !ae && i < 10 && node->attrs[i].name; i++)
		if (!strcmp(name, node->attrs[i].name))
			ae = &node->attrs[i];
	
	if (ae && ae->type == type && pos < ae->size) {
		memcpy(buf, (char *)ae->value + pos, MIN(*len, ae->size-pos));
		*len = MIN(*len, ae->size-pos);
		err = B_OK;
	} else {
		*len = 0;
		err = ENOENT;
	}
	
	UNLOCK(&node->l);
	return err;
}

#endif

/* query stuff */

static int compare_fs_node_by_recent_query_string(fs_node *node, char *query)
{
	time_t tm = time(NULL);
	//return memcmp(node->name, name, GOOGLEFS_NAME_LEN);
	PRINT((PFS"find_by_recent_query_string: '%s' <> '%s'\n", \
			node->request?node->request->query_string:NULL, query));
	if (!node->request || !node->request->query_string)
		return -1;
	/* reject if older than 5 min */
	if (node->st.st_crtime + 60 * 5 < tm)
		return -1;
	return strcmp(node->request->query_string, query);
}

int	googlefs_open_query(fs_nspace *ns, const char *query, ulong flags,
					port_id port, long token, fs_query_cookie **cookie)
{
	status_t err = B_OK;
	fs_query_cookie *c;
	fs_node *qn, *n, *dummy;
	const char *p;
	char *qstring = NULL;
	char qname[GOOGLEFS_NAME_LEN];
	bool accepted = true;
	bool reused = false;
	int i;
	PRINT((PFS"open_query(%ld, '%s', 0x%08lx, %ld, %ld)\n", ns->nsid, query, flags, port, token));
//	if (flags & B_LIVE_QUERY)
//		return ENOSYS; /* no live query yet, they are live enough anyway */
	//return ENOSYS;
	if (!query || !cookie)
		return EINVAL;

	// filter out queries that aren't for us, we don't want to trigger google searches when apps check for mails, ... :)
	
	err = B_NO_MEMORY;
	c = malloc(sizeof(fs_query_cookie));
	if (!c)
		return err;
	memset(c, 0, sizeof(fs_query_cookie));
	c->omode = O_RDONLY;
	c->type = S_IFQUERY;
	c->dir_current = 0;

	err = ENOSYS;
	if (strncmp(query, "((name==\"*", 10))
		accepted = false;
	else {
		qstring = query_unescape_string(query + 10, &p, '"');
		if (!qstring)
			accepted = false;
		else if (!p)
			accepted = false;
		else if (strcmp(p, "\")&&(BEOS:TYPE==\"application/x-vnd.Be-bookmark\"))"))
			accepted = false;
		else {
			if (qstring[0] == '*')
				strcpy(qstring+1, qstring);
			if (qstring[strlen(qstring)-1] == '*')
				qstring[strlen(qstring)-1] = '\0';
			if (!query_strip_bracketed_Cc(qstring))
				goto err_qs;
		}
	}
	
	if (!accepted) {
		free(qstring);
		/* return an empty cookie */
		*cookie = c;
		return B_OK;
	}
	PRINT((PFS"open_query: QUERY: '%s'\n", qstring));
	/* reuse query if it's not too old */
	LOCK(&ns->l);
	qn = SLL_FIND(ns->queries, qnext, 
				(sll_compare_func)compare_fs_node_by_recent_query_string, (void *)qstring);
	UNLOCK(&ns->l);
	reused = (qn != NULL);
	if (reused) {
		PRINT((PFS"open_query: reusing %ld:%Ld\n", ns->nsid, qn->vnid));
		err = get_vnode(ns->nsid, qn->vnid, (void **)&dummy); /* inc ref count */
		if (err)
			goto err_mkdir;
		/* wait for the query to complete */
		while (!qn->qcompleted)
			snooze(10000);
		goto reuse;
	}

	/* stripped name for folder */
	strncpy(qname, qstring, GOOGLEFS_NAME_LEN);
	qname[GOOGLEFS_NAME_LEN-1] = '\0';

	/* strip out slashes */
	p = qname;
	while ((p = strchr(p, '/')))
		strcpy(p, p + 1);

	/* should get/put_vnode(ns->root); around that I think... */
	err = googlefs_create_gen(ns, ns->root, qname, 0, 0755, NULL, &qn, folders_attrs, true, true);
	if (err)
		goto err_qs;

	err = get_vnode(ns->nsid, qn->vnid, (void **)&dummy); /* inc ref count */
	if (err)
		goto err_mkdir;

//#ifndef NO_SEARCH

	/* let's ask google */
	err = google_request_open(qstring, ns, qn, &qn->request);
	if (err)
		goto err_gn;
	
	PRINT((PFS"open_query: request_open done\n"));
#ifndef NO_SEARCH
	err = google_request_process(qn->request);
	if (err)
		goto err_gro;
	PRINT((PFS"open_query: request_process done\n"));
	
#else
	/* fake entries */
	for (i = 0; i < 10; i++) {
		err = googlefs_create_gen(ns, qn, "B", 0, 0644, NULL, &n, fake_bookmark_attrs, false, true);
		/* fake that to test sorting */
		*(int32 *)&n->attrs[1].value = i + 1; // hack
		n->attrs[0].type = 'LONG';
		n->attrs[0].value = &n->attrs[1].value;
		n->attrs[0].size = sizeof(int32);
		n->attrs[0].name = "GOOGLE:order";
		notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[0].name);
		if (err)
			goto err_gn;
	}
#endif /*NO_SEARCH*/	
	//
	//err = google_request_close(q->request);

	LOCK(&ns->l);
	SLL_INSERT(ns->queries, qnext, qn);
	UNLOCK(&ns->l);
reuse:
	/* put the chocolate on the cookie */
	LOCK(&qn->l);
	SLL_INSERT(qn->opened, next, c);
	UNLOCK(&qn->l);
	qn->qcompleted = 1; /* tell other cookies we're done */
	c->node = qn;
	*cookie = c;
	free(qstring);
	return B_OK;

err_grp:
err_gro:
	if (qn->request)
		google_request_close(qn->request);
err_gn:
	put_vnode(ns->nsid, qn->vnid);
err_mkdir:
	if (!reused)
		googlefs_unlink(ns, ns->root, qn->name);
err_qs:
	free(qstring);
err_m:
	free(c);
	PRINT((PFS"open_query: error 0x%08lx\n", err));
	return err;
}

int googlefs_close_query(fs_nspace *ns, fs_query_cookie *cookie)
{
	status_t err;
	fs_node *q, *n;
	PRINT((PFS"close_query(%ld, %Ld)\n", ns->nsid, cookie->node?cookie->node->vnid:0LL));
	//return ENOSYS;
	q = cookie->node;
	if (!q)
		return B_OK;
	// kill_request();
	LOCK(&q->l);
	SLL_REMOVE(q->opened, next, cookie);
	if (q->request /*&& !q->opened*/) {
		err = google_request_close(q->request);
	}
	UNLOCK(&q->l);
	/* if last cookie on the query and sync_unlink, trash all */
	if (sync_unlink_queries && !q->opened)
		err = googlefs_unlink_node_rec(ns, q);
	err = put_vnode(ns->nsid, q->vnid);
	return err;
}

#ifdef __ANTARES__
/* protos are different... */
int googlefs_free_query_cookie(fs_nspace *ns, fs_dir_cookie *cookie)
{
	PRINT((PFS"free_query_cookie(%ld)\n", ns->nsid));
	free(cookie);
	return B_OK;
}
#endif

#ifdef __ANTARES__
int	googlefs_read_query(fs_nspace *ns, fs_query_cookie *cookie, struct dirent *buf, size_t bufsize, uint32 *num)
#else
int	googlefs_read_query(fs_nspace *ns, fs_query_cookie *cookie, long *num, struct dirent *buf, size_t bufsize)
#endif
{
	status_t err = B_OK;
	fs_node *n = NULL;
	fs_node *node = cookie->node;
	int index;
	PRINT((PFS"read_query(%ld, %Ld) @ %d\n", ns->nsid, node?node->vnid:0LL, cookie->dir_current));
	if (!cookie || !num || !*num || !buf || (bufsize < (sizeof(dirent_t)+GOOGLEFS_NAME_LEN)))
		return EINVAL;
	if (!node) {
		/* a query we don't care about, just return no entries to please apps */
		*num = 0;
		return B_OK;
	}
	//return ENOSYS;
	err = LOCK(&node->l);
	index = cookie->dir_current;
	for (n = node->children; n && index; n = n->next, index--);
	if (n) {
		PRINT((PFS "read_query: giving ino %Ld, %s\n", n->vnid, n->name));
		buf->d_dev = ns->nsid;
		buf->d_pdev = ns->nsid;
		buf->d_ino = n->vnid;
		buf->d_pino = node->vnid;
		strcpy(buf->d_name, n->name);
		buf->d_reclen = 2*(sizeof(dev_t)+sizeof(ino_t))+sizeof(unsigned short)+strlen(buf->d_name)+1;
		cookie->dir_current++;
		*num = 1;
	} else {
		*num = 0;
	}
	UNLOCK(&node->l);
	return B_OK;
}

int googlefs_push_result_to_query(struct google_request *request, struct google_result *result)
{
	status_t err = B_OK;
	fs_nspace *ns = request->ns;
	fs_node *qn = request->query_node;
	fs_node *n, *dummy;
	char ename[GOOGLEFS_NAME_LEN];
	char *p;
	int i;
	PRINT((PFS"push_result_to_query(%ld, %Ld, %d:'%s')\n", ns->nsid, qn->vnid, result->id, result->name));
	//dprintf(PFS"push_result_to_query(%ld, %Ld, %d:'%s')\n", ns->nsid, qn->vnid, result->id, result->name);
	//return ENOSYS;
	if (!ns || !qn)
		return EINVAL;

	// filter out queries that aren't for us, we don't want to trigger google searches when apps check for mails, ... :)
	
	/* stripped name for folder */
	strncpy(ename, result->name, GOOGLEFS_NAME_LEN);
	ename[GOOGLEFS_NAME_LEN-1] = '\0';
	/* strip out slashes */
	p = ename;
	while ((p = strchr(p, '/')))
		*p++ = '_';
	
	err = googlefs_create_gen(ns, qn, ename, 0, 0644, NULL, &n, bookmark_attrs, false, true);
	if (err)
		return err;
	LOCK(&n->l);
	n->result = result;
	i = 0;
	n->attrs[i].type = 'CSTR';
	n->attrs[i].value = result->name;
	n->attrs[i].size = strlen(result->name)+1;
	n->attrs[i].name = "META:title";
	notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
	i++;
	n->attrs[i].type = 'CSTR';
	n->attrs[i].value = result->url;
	n->attrs[i].size = strlen(result->url)+1;
	n->attrs[i].name = "META:url";
	notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
	i++;
	n->attrs[i].type = 'CSTR';
	n->attrs[i].value = request->query_string;
	n->attrs[i].size = strlen(request->query_string)+1;
	n->attrs[i].name = "META:keyw";
	notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
	i++;
	n->attrs[i].type = 'LONG';
	n->attrs[i].value = &result->id;
	n->attrs[i].size = sizeof(int32);
	n->attrs[i].name = "GOOGLE:order";
	notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
	i++;
	if (result->snipset[0]) {
		n->attrs[i].type = 'CSTR';
		n->attrs[i].value = result->snipset;
		n->attrs[i].size = strlen(result->snipset)+1;
		n->attrs[i].name = "GOOGLE:excerpt";
		notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
		i++;
	}
	if (result->cache_url[0]) {
		n->attrs[i].type = 'CSTR';
		n->attrs[i].value = result->cache_url;
		n->attrs[i].size = strlen(result->cache_url)+1;
		n->attrs[i].name = "GOOGLE:cache_url";
		notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
		i++;
	}
	if (result->similar_url[0]) {
		n->attrs[i].type = 'CSTR';
		n->attrs[i].value = result->similar_url;
		n->attrs[i].size = strlen(result->similar_url)+1;
		n->attrs[i].name = "GOOGLE:similar_url";
		notify_listener(B_ATTR_CHANGED, ns->nsid, 0, 0, n->vnid, n->attrs[i].name);
		i++;
	}
	UNLOCK(&n->l);
	return B_OK;

err:
	PRINT((PFS"push_result_to_query: error 0x%08lx\n", err));
	return err;
}

//	#pragma mark -

#ifdef __ANTARES__

static status_t
googlefs_std_ops(int32 op, ...)
{
	switch (op) {
		case B_MODULE_INIT:
			PRINT((PFS"std_ops(INIT)\n"));
			return B_OK;
		case B_MODULE_UNINIT:
			PRINT((PFS"std_ops(UNINIT)\n"));
			return B_OK;
		default:
			return B_ERROR;
	}
}


static file_system_module_info sGoogleFSModule = {
	{
		"file_systems/googlefs" B_CURRENT_FS_API_VERSION,
		0,
		googlefs_std_ops,
	},

	"googlefs",					// short_name
	GOOGLEFS_PRETTY_NAME,		// pretty_name
	0,	// DDM flags

	// scanning
	NULL,	// fs_identify_partition,
	NULL,	// fs_scan_partition,
	NULL,	// fs_free_identify_partition_cookie,
	NULL,	// free_partition_content_cookie()

	&googlefs_mount,
	&googlefs_unmount,
	&googlefs_rfsstat,
	NULL,
	NULL,

	/* vnode operations */
	&googlefs_walk,
	&googlefs_get_vnode_name,
	&googlefs_read_vnode,
	&googlefs_release_vnode,
	&googlefs_remove_vnode,

	/* VM file access */
	NULL, 	// &googlefs_can_page
	NULL,	// &googlefs_read_pages
	NULL, 	// &googlefs_write_pages

	NULL,	// &googlefs_get_file_map,

	NULL, 	// &googlefs_ioctl
	&googlefs_setflags,
	NULL,	// &googlefs_select
	NULL,	// &googlefs_deselect
	NULL, 	// &googlefs_fsync

	NULL,	// &googlefs_read_link,
	NULL, 	// &googlefs_create_symlink,

	NULL, 	// &googlefs_link,
	NULL,	// &googlefs_unlink,
	NULL, 	// &googlefs_rename,

	&googlefs_access,
	&googlefs_rstat,
	&googlefs_wstat,

	/* file operations */
	&googlefs_create,
	&googlefs_open,
	&googlefs_close,
	&googlefs_free_cookie,
	&googlefs_read,
	NULL, 	// &googlefs_write,

	/* directory operations */
	&googlefs_mkdir,
	&googlefs_rmdir,
	&googlefs_opendir,
	&googlefs_closedir,
	&googlefs_free_dircookie,
	&googlefs_readdir,
	&googlefs_rewinddir,
	
	/* attribute directory operations */
	&googlefs_open_attrdir,
	&googlefs_close_attrdir,
	&googlefs_free_attrdircookie,
	&googlefs_read_attrdir,
	&googlefs_rewind_attrdir,

	/* attribute operations */
	NULL,	// &googlefs_create_attr
	&googlefs_open_attr_h,
	&googlefs_close_attr_h,
	&googlefs_free_attr_cookie_h,
	&googlefs_read_attr_h,
	NULL,	// &googlefs_write_attr_h,

	&googlefs_read_attr_stat_h,
	NULL,	// &googlefs_write_attr_stat
	NULL,	// &googlefs_rename_attr
	NULL,	// &googlefs_remove_attr

	/* index directory & index operations */
	NULL,	// &googlefs_open_index_dir
	NULL,	// &googlefs_close_index_dir
	NULL,	// &googlefs_free_index_dir_cookie
	NULL,	// &googlefs_read_index_dir
	NULL,	// &googlefs_rewind_index_dir

	NULL,	// &googlefs_create_index
	NULL,	// &googlefs_remove_index
	NULL,	// &googlefs_stat_index

	/* query operations */
	&googlefs_open_query,
	&googlefs_close_query,
	&googlefs_free_query_cookie,
	&googlefs_read_query,
	NULL,	// &googlefs_rewind_query,
};

module_info *modules[] = {
	(module_info *)&sGoogleFSModule,
	NULL,
};

#else /* BeOS fsil api */

_EXPORT vnode_ops fs_entry =
{
	(op_read_vnode *)&googlefs_read_vnode,
	(op_write_vnode *)&googlefs_write_vnode,
	(op_remove_vnode *)&googlefs_remove_vnode,
	(op_secure_vnode *)&googlefs_secure_vnode,
	(op_walk *)&googlefs_walk,
	(op_access *)&googlefs_access,
	(op_create *)&googlefs_create,
	(op_mkdir *)&googlefs_mkdir,
	NULL, //(op_symlink *)&googlefs_symlink,
	NULL, //	&googlefs_link,
	NULL, //(op_rename *)&googlefs_rename,
	(op_unlink *)&googlefs_unlink,
	(op_rmdir *)&googlefs_rmdir,
	NULL, //(op_readlink *)&googlefs_readlink,
	(op_opendir *)&googlefs_opendir,
	(op_closedir *)&googlefs_closedir,
	(op_free_cookie *)&googlefs_free_dircookie,
	(op_rewinddir *)&googlefs_rewinddir,
	(op_readdir *)&googlefs_readdir,
	(op_open *)&googlefs_open,
	(op_close *)&googlefs_close,
	(op_free_cookie *)&googlefs_free_cookie,
	(op_read *)&googlefs_read,
	(op_write *)&googlefs_write,
	NULL, //	&googlefs_readv,
	NULL, //	&googlefs_writev,
	NULL, //	&googlefs_ioctl,
	(op_setflags *)&googlefs_setflags,
	(op_rstat *)&googlefs_rstat,
	(op_wstat *)&googlefs_wstat,
	NULL, //	&googlefs_fsync,
	NULL, //	&googlefs_initialize,
	(op_mount *)&googlefs_mount,
	(op_unmount *)&googlefs_unmount,
	NULL, //	&googlefs_sync,
	(op_rfsstat *)&googlefs_rfsstat,
	(op_wfsstat *)&googlefs_wfsstat,
	NULL, //	&googlefs_select,
	NULL, //	&googlefs_deselect,
	NULL, //	&googlefs_open_indexdir,
	NULL, //	&googlefs_close_indexdir,
	NULL, //	&googlefs_free_indexdircookie,
	NULL, //	&googlefs_rewind_indexdir,
	NULL, //	&googlefs_read_indexdir,
	NULL, //	&googlefs_create_index,
	NULL, //	&googlefs_remove_index,
	NULL, //	&googlefs_rename_index,
	NULL, //	&googlefs_stat_index,
	(op_open_attrdir *)&googlefs_open_attrdir,
	(op_close_attrdir *)&googlefs_close_attrdir,
	(op_free_cookie *)&googlefs_free_attrdircookie,
	(op_rewind_attrdir *)&googlefs_rewind_attrdir,
	(op_read_attrdir *)&googlefs_read_attrdir,
	(op_write_attr *)&googlefs_write_attr_b,
	(op_read_attr *)&googlefs_read_attr_b,
	(op_remove_attr *)&googlefs_remove_attr_b,
	(op_rename_attr *)&googlefs_rename_attr_b,
	(op_stat_attr *)&googlefs_stat_attr_b,
	(op_open_query *)&googlefs_open_query,
	(op_close_query *)&googlefs_close_query,
	(op_free_cookie *)&googlefs_free_dircookie,//&googlefs_free_querycookie,
	(op_read_query *)&googlefs_read_query	
};

_EXPORT int32 api_version=B_CUR_FS_API_VERSION;

#endif

