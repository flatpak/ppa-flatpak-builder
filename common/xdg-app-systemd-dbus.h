/*
 * Generated by gdbus-codegen 2.47.0. DO NOT EDIT.
 *
 * The license of this code is the same as for the source it was derived from.
 */

#ifndef ____COMMON_XDG_APP_SYSTEMD_DBUS_H__
#define ____COMMON_XDG_APP_SYSTEMD_DBUS_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.freedesktop.systemd1.Manager */

#define SYSTEMD_TYPE_MANAGER (systemd_manager_get_type ())
#define SYSTEMD_MANAGER(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SYSTEMD_TYPE_MANAGER, SystemdManager))
#define SYSTEMD_IS_MANAGER(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SYSTEMD_TYPE_MANAGER))
#define SYSTEMD_MANAGER_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), SYSTEMD_TYPE_MANAGER, SystemdManagerIface))

struct _SystemdManager;
typedef struct _SystemdManager SystemdManager;
typedef struct _SystemdManagerIface SystemdManagerIface;

struct _SystemdManagerIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_start_transient_unit) (
    SystemdManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_name,
    const gchar *arg_mode,
    GVariant *arg_properties,
    GVariant *arg_aux);

  void (*job_removed) (
    SystemdManager *object,
    guint arg_id,
    const gchar *arg_job,
    const gchar *arg_unit,
    const gchar *arg_result);

};

GType systemd_manager_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *systemd_manager_interface_info (void);
guint systemd_manager_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void systemd_manager_complete_start_transient_unit (
    SystemdManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *job);



/* D-Bus signal emissions functions: */
void systemd_manager_emit_job_removed (
    SystemdManager *object,
    guint arg_id,
    const gchar *arg_job,
    const gchar *arg_unit,
    const gchar *arg_result);



/* D-Bus method calls: */
void systemd_manager_call_start_transient_unit (
    SystemdManager *proxy,
    const gchar *arg_name,
    const gchar *arg_mode,
    GVariant *arg_properties,
    GVariant *arg_aux,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean systemd_manager_call_start_transient_unit_finish (
    SystemdManager *proxy,
    gchar **out_job,
    GAsyncResult *res,
    GError **error);

gboolean systemd_manager_call_start_transient_unit_sync (
    SystemdManager *proxy,
    const gchar *arg_name,
    const gchar *arg_mode,
    GVariant *arg_properties,
    GVariant *arg_aux,
    gchar **out_job,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define SYSTEMD_TYPE_MANAGER_PROXY (systemd_manager_proxy_get_type ())
#define SYSTEMD_MANAGER_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SYSTEMD_TYPE_MANAGER_PROXY, SystemdManagerProxy))
#define SYSTEMD_MANAGER_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SYSTEMD_TYPE_MANAGER_PROXY, SystemdManagerProxyClass))
#define SYSTEMD_MANAGER_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SYSTEMD_TYPE_MANAGER_PROXY, SystemdManagerProxyClass))
#define SYSTEMD_IS_MANAGER_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SYSTEMD_TYPE_MANAGER_PROXY))
#define SYSTEMD_IS_MANAGER_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SYSTEMD_TYPE_MANAGER_PROXY))

typedef struct _SystemdManagerProxy SystemdManagerProxy;
typedef struct _SystemdManagerProxyClass SystemdManagerProxyClass;
typedef struct _SystemdManagerProxyPrivate SystemdManagerProxyPrivate;

struct _SystemdManagerProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  SystemdManagerProxyPrivate *priv;
};

struct _SystemdManagerProxyClass
{
  GDBusProxyClass parent_class;
};

GType systemd_manager_proxy_get_type (void) G_GNUC_CONST;

void systemd_manager_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
SystemdManager *systemd_manager_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
SystemdManager *systemd_manager_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void systemd_manager_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
SystemdManager *systemd_manager_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
SystemdManager *systemd_manager_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define SYSTEMD_TYPE_MANAGER_SKELETON (systemd_manager_skeleton_get_type ())
#define SYSTEMD_MANAGER_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SYSTEMD_TYPE_MANAGER_SKELETON, SystemdManagerSkeleton))
#define SYSTEMD_MANAGER_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SYSTEMD_TYPE_MANAGER_SKELETON, SystemdManagerSkeletonClass))
#define SYSTEMD_MANAGER_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SYSTEMD_TYPE_MANAGER_SKELETON, SystemdManagerSkeletonClass))
#define SYSTEMD_IS_MANAGER_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SYSTEMD_TYPE_MANAGER_SKELETON))
#define SYSTEMD_IS_MANAGER_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SYSTEMD_TYPE_MANAGER_SKELETON))

typedef struct _SystemdManagerSkeleton SystemdManagerSkeleton;
typedef struct _SystemdManagerSkeletonClass SystemdManagerSkeletonClass;
typedef struct _SystemdManagerSkeletonPrivate SystemdManagerSkeletonPrivate;

struct _SystemdManagerSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  SystemdManagerSkeletonPrivate *priv;
};

struct _SystemdManagerSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType systemd_manager_skeleton_get_type (void) G_GNUC_CONST;

SystemdManager *systemd_manager_skeleton_new (void);


G_END_DECLS

#endif /* ____COMMON_XDG_APP_SYSTEMD_DBUS_H__ */
