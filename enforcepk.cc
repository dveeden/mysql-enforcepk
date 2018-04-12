#include <stdio.h>
#include <mysql/plugin.h>
#include <mysql/plugin_audit.h>
#include <mysql/service_my_plugin_log.h>
#include <mysqld_error.h>
#include "sql_error.h"

static my_bool is_super(MYSQL_THD thd)
{
  MYSQL_SECURITY_CONTEXT ctx;
  my_svc_bool is_super= FALSE;

  return (
    thd != NULL &&
    !thd_get_security_context(thd, &ctx) &&
    !security_context_get_option(ctx, "privilege_super", &is_super) &&
    is_super);
}

static int enforcepk_plugin_init(void *arg MY_ATTRIBUTE((unused)))
{
  return(0);
}

static int enforcepk_plugin_deinit(void *arg MY_ATTRIBUTE((unused)))
{
  return(0);
}

static int enforcepk_notify(MYSQL_THD thd,
                             mysql_event_class_t event_class,
                             const void *event)
{
  /* Don't apply restrictions on accounts with SUPER priv */
  /* if (is_super(thd))
    return 0; */

  if (event_class == MYSQL_AUDIT_QUERY_CLASS)
  {
    const struct mysql_event_query *event_query= (const struct mysql_event_query *)event;

    if ((strncasecmp(event_query->query.str, "CREATE TABLE", 12) == 0)
      && (strcasestr(event_query->query.str, "PRIMARY KEY") == NULL))
    {
      if (is_super(thd))
      {
          /* 1642 == ER_SIGNAL_WARN  Maybe we can find a better code? */
          push_warning(thd, Sql_condition::SL_WARNING, 1642,
            "Creating tables without primary key is not recommended");
      }
      else
      {
          my_message(ER_AUDIT_API_ABORT, "Creating tables without primary key requires SUPER privilege", MYF(0));
          return 1;
      }
    }
  }

  return 0;
}

static struct st_mysql_audit enforcepk_descriptor=
{
  MYSQL_AUDIT_INTERFACE_VERSION,                    /* interface version    */
  NULL,                                             /* release_thd function */
  enforcepk_notify,                        /* notify function      */
  { 0, /* MYSQL_AUDIT_GENERAL_ALL, */
    0, /* MYSQL_AUDIT_CONNECTION_CONNECT, */
    0, /* MYSQL_AUDIT_PARSE_ALL, */
    0, /* This event class is currently not supported. */
    0, /* MYSQL_AUDIT_TABLE_ACCESS_ALL, */
    0, /* MYSQL_AUDIT_GLOBAL_VARIABLE_ALL, */
    0, /* MYSQL_AUDIT_SERVER_STARTUP_ALL, */
    0, /* MYSQL_AUDIT_SERVER_SHUTDOWN_ALL, */
    0, /* MYSQL_AUDIT_COMMAND_START, */
    (unsigned long) MYSQL_AUDIT_QUERY_START,
    0  /* (unsigned long) MYSQL_AUDIT_STORED_PROGRAM_ALL */
  }
};

mysql_declare_plugin(enforcepk)
{
  MYSQL_AUDIT_PLUGIN,                    /* type                            */
  &enforcepk_descriptor,                 /* descriptor                      */
  "enforcepk",                           /* name                            */
  "Daniël van Eeden",                    /* author                          */
  "Require a primary key on new tables", /* description                     */
  PLUGIN_LICENSE_GPL,                    /* license                         */
  enforcepk_plugin_init,                 /* init function (when loaded)     */
  enforcepk_plugin_deinit,               /* deinit function (when unloaded) */
  0x0001,                                /* version                         */
  NULL,                                  /* status variables                */
  NULL,                                  /* system variables                */
  NULL,
  0,
}
mysql_declare_plugin_end;
