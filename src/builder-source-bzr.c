/* builder-source-bzr.c
 *
 * Copyright (C) 2015 Red Hat, Inc
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Alexander Larsson <alexl@redhat.com>
 */

#include "config.h"

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/statfs.h>

#include "builder-utils.h"

#include "builder-source-bzr.h"
#include "builder-utils.h"
#include "builder-flatpak-utils.h"

struct BuilderSourceBzr
{
  BuilderSource parent;

  char         *url;
  char         *revision;
};

typedef struct
{
  BuilderSourceClass parent_class;
} BuilderSourceBzrClass;

G_DEFINE_TYPE (BuilderSourceBzr, builder_source_bzr, BUILDER_TYPE_SOURCE);

enum {
  PROP_0,
  PROP_URL,
  PROP_REVISION,
  LAST_PROP
};

static void
builder_source_bzr_finalize (GObject *object)
{
  BuilderSourceBzr *self = (BuilderSourceBzr *) object;

  g_free (self->url);
  g_free (self->revision);

  G_OBJECT_CLASS (builder_source_bzr_parent_class)->finalize (object);
}

static void
builder_source_bzr_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (object);

  switch (prop_id)
    {
    case PROP_URL:
      g_value_set_string (value, self->url);
      break;

    case PROP_REVISION:
      g_value_set_string (value, self->revision);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
builder_source_bzr_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (object);

  switch (prop_id)
    {
    case PROP_URL:
      g_free (self->url);
      self->url = g_value_dup_string (value);
      break;

    case PROP_REVISION:
      g_free (self->revision);
      self->revision = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
bzr (GFile   *dir,
     char   **output,
     GError **error,
     ...)
{
  g_autofree char *brz = NULL;
  gboolean res;
  va_list ap;

  brz = g_find_program_in_path ("brz");

  va_start (ap, error);
  res = flatpak_spawn (dir, output, 0, error, brz ? brz : "bzr", ap);
  va_end (ap);

  return res;
}

static GFile *
get_mirror_dir (BuilderSourceBzr *self, BuilderContext *context)
{
  g_autoptr(GFile) bzr_dir = NULL;
  g_autofree char *filename = NULL;
  g_autofree char *bzr_dir_path = NULL;

  bzr_dir = g_file_get_child (builder_context_get_state_dir (context),
                              "bzr");

  bzr_dir_path = g_file_get_path (bzr_dir);
  g_mkdir_with_parents (bzr_dir_path, 0755);

  filename = builder_uri_to_filename (self->url);
  return g_file_get_child (bzr_dir, filename);
}

static char *
get_current_commit (BuilderSourceBzr *self, BuilderContext *context, GError **error)
{
  g_autoptr(GFile) mirror_dir = NULL;
  char *output = NULL;

  mirror_dir = get_mirror_dir (self, context);

  if (!bzr (mirror_dir, &output, error,
            "revno", NULL))
    return NULL;

  return output;
}

static gboolean
builder_source_bzr_download (BuilderSource  *source,
                             gboolean        update_vcs,
                             BuilderContext *context,
                             GError        **error)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (source);

  g_autoptr(GFile) mirror_dir = NULL;

  if (self->url == NULL)
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED, "URL not specified");
      return FALSE;
    }

  mirror_dir = get_mirror_dir (self, context);

  if (!g_file_query_exists (mirror_dir, NULL))
    {
      g_autofree char *filename = g_file_get_basename (mirror_dir);
      g_autoptr(GFile) parent = g_file_get_parent (mirror_dir);
      g_autofree char *mirror_path = g_file_get_path (mirror_dir);
      g_autofree char *path_tmp = g_strconcat (mirror_path, ".clone_XXXXXX", NULL);
      g_autofree char *filename_tmp = NULL;
      g_autoptr(GFile) mirror_dir_tmp = NULL;
      g_autoptr(GFile) cached_bzr_dir = NULL;
      const char *branch_source;

      g_print ("Getting bzr repo %s\n", self->url);

      if (g_mkdtemp_full (path_tmp, 0755) == NULL)
        return flatpak_fail (error, "Can't create temporary directory");

      /* bzr can't check out to the empty dir, so remove it */
      rmdir (path_tmp);

      mirror_dir_tmp = g_file_new_for_path (path_tmp);
      filename_tmp = g_file_get_basename (mirror_dir_tmp);

      branch_source = self->url;

      cached_bzr_dir = builder_context_find_in_sources_dirs (context, "bzr", filename, NULL);
      if (cached_bzr_dir != NULL)
        branch_source = flatpak_file_get_path_cached (cached_bzr_dir);

      if (!bzr (parent, NULL, error,
                "branch", branch_source,  filename_tmp, NULL))
        return FALSE;

      /* Rewrite to real url if we used the cache */
      if (cached_bzr_dir != NULL)
        {
          g_autofree char *setting = g_strdup_printf ("parent_location=%s", self->url);
          if (!bzr (mirror_dir_tmp, NULL, error,
                    "config", setting, NULL))
            return FALSE;

          if (update_vcs &&
              !bzr (mirror_dir_tmp, NULL, error,
                    "pull", NULL))
            return FALSE;
        }

      if (!g_file_move (mirror_dir_tmp, mirror_dir, 0, NULL, NULL, NULL, error))
        return FALSE;
    }
  else if (update_vcs)
    {
      g_print ("Updating bzr repo %s\n", self->url);

      if (!bzr (mirror_dir, NULL, error,
                "pull", NULL))
        return FALSE;
    }

  return TRUE;
}

static gboolean
builder_source_bzr_extract (BuilderSource  *source,
                            GFile          *dest,
                            GFile          *source_dir,
                            BuilderOptions *build_options,
                            BuilderContext *context,
                            GError        **error)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (source);

  g_autoptr(GFile) mirror_dir = NULL;
  g_autofree char *mirror_dir_path = NULL;
  g_autofree char *dest_path = NULL;

  mirror_dir = get_mirror_dir (self, context);

  mirror_dir_path = g_file_get_path (mirror_dir);
  dest_path = g_file_get_path (dest);

  if (!bzr (NULL, NULL, error,
            "branch", "--stacked", mirror_dir_path, dest_path, "--use-existing-dir", NULL))
    return FALSE;

  if (self->revision)
    {
      g_autofree char *revarg = g_strdup_printf ("-r%s", self->revision);
      if (!bzr (dest, NULL, error,
                "revert", revarg, NULL))
        return FALSE;
    }

  return TRUE;
}

static gboolean
builder_source_bzr_bundle (BuilderSource  *source,
                           BuilderContext *context,
                           GError        **error)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (source);

  g_autoptr(GFile) sources_dir = NULL;
  g_autoptr(GFile) dest_dir = NULL;
  g_autoptr(GFile) dest_dir_tmp = NULL;
  g_autoptr(GFile) bzr_sources_dir = NULL;

  g_autofree char *sources_dir_path = NULL;

  g_autofree char *base_name = NULL;
  g_autofree char *base_name_tmp = NULL;

  sources_dir = get_mirror_dir (self, context);

  if (sources_dir == NULL) {
    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED, "Can't locate repo with URL '%s'", self->url);
    return FALSE;
  }

  base_name = g_file_get_basename (sources_dir);

  bzr_sources_dir = flatpak_build_file (builder_context_get_app_dir (context),
                                        "sources/bzr",
                                        NULL);
  if (!flatpak_mkdir_p (bzr_sources_dir, NULL, error))
    return FALSE;

  base_name_tmp = g_strconcat (base_name, ".clone_tmp", NULL);
  dest_dir_tmp = flatpak_build_file (bzr_sources_dir,
                                          base_name_tmp,
                                          NULL);
  dest_dir = flatpak_build_file (bzr_sources_dir,
                                 base_name,
                                 NULL);

  sources_dir_path = g_file_get_path (sources_dir);
  if (!bzr (bzr_sources_dir, NULL, error,
            "branch", sources_dir_path, base_name_tmp, NULL) ||
      !g_file_move (dest_dir_tmp, dest_dir, 0, NULL, NULL, NULL, error))
    return FALSE;

  return TRUE;
}

static void
builder_source_bzr_checksum (BuilderSource  *source,
                             BuilderCache   *cache,
                             BuilderContext *context)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (source);
  g_autofree char *current_commit = NULL;

  g_autoptr(GError) error = NULL;

  builder_cache_checksum_str (cache, self->url);
  builder_cache_checksum_str (cache, self->revision);

  current_commit = get_current_commit (self, context, &error);
  if (current_commit)
    builder_cache_checksum_str (cache, current_commit);
  else if (error)
    g_warning ("Failed to get current bzr checksum: %s", error->message);
}

static gboolean
builder_source_bzr_update (BuilderSource  *source,
                           BuilderContext *context,
                           GError        **error)
{
  BuilderSourceBzr *self = BUILDER_SOURCE_BZR (source);
  char *current_commit;

  current_commit = get_current_commit (self, context, NULL);
  if (current_commit)
    {
      g_free (self->revision);
      self->revision = current_commit;
    }

  return TRUE;
}

static void
builder_source_bzr_class_init (BuilderSourceBzrClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  BuilderSourceClass *source_class = BUILDER_SOURCE_CLASS (klass);

  object_class->finalize = builder_source_bzr_finalize;
  object_class->get_property = builder_source_bzr_get_property;
  object_class->set_property = builder_source_bzr_set_property;

  source_class->download = builder_source_bzr_download;
  source_class->extract = builder_source_bzr_extract;
  source_class->bundle = builder_source_bzr_bundle;
  source_class->update = builder_source_bzr_update;
  source_class->checksum = builder_source_bzr_checksum;

  g_object_class_install_property (object_class,
                                   PROP_URL,
                                   g_param_spec_string ("url",
                                                        "",
                                                        "",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_REVISION,
                                   g_param_spec_string ("revision",
                                                        "",
                                                        "",
                                                        NULL,
                                                        G_PARAM_READWRITE));
}

static void
builder_source_bzr_init (BuilderSourceBzr *self)
{
}
