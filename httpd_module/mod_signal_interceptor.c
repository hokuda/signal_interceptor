/* 
**  mod_signal_interceptor.c -- Apache sample test module
**  [Autogenerated via ``apxs -n signal_interceptor -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory 
**  by running:
**
**    $ apxs -c -i mod_signal_interceptor.c
**
**  Then activate it in Apache's httpd.conf file for instance
**  for the URL /signal_interceptor in as follows:
**
**    #   httpd.conf
**    LoadModule signal_interceptor_module modules/mod_signal_interceptor.so
**    <Location /signal_interceptor>
**    SetHandler signal_interceptor
**    </Location>
**
**  Then after restarting Apache via
**
**    $ apachectl restart
**
**  you immediately can request the URL /signal_interceptor and watch for the
**  output of this module. This can be achieved for instance via:
**
**    $ lynx -mime_header http://localhost/signal_interceptor 
**
**  The output should be similar to the following one:
**
**    HTTP/1.1 200 OK
**    Date: Tue, 31 Mar 1998 14:42:22 GMT
**    Server: Apache/1.3.4 (Unix)
**    Connection: close
**    Content-Type: text/html
**  
**    The sample page from mod_signal_interceptor.c
*/ 

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

#include "stdio.h"
#include "http_log.h"
#include "mpm_common.h"

#include "signal_interceptor.h"

static int signal_interceptor_hook_post_config(apr_pool_t *pconf,
                                          apr_pool_t *plog,
                                          apr_pool_t *ptemp,
                                          server_rec *s)
{
  ap_log_error(APLOG_MARK, APLOG_CRIT, 0, NULL, "[signal_interceptor (httpd module)] post_config for root process (pid: %d)__", getpid());
  set_custom_sigaction();
  return OK;
}

static int x_pre_mpm(apr_pool_t *p, ap_scoreboard_e sb_type)
{
  ap_log_error(APLOG_MARK, APLOG_CRIT, 0, NULL, "[signal_interceptor (httpd module)] pre_mpm for root process (pid: %d)__", getpid());
  set_custom_sigaction();
  return OK;
}

static int x_monitor(apr_pool_t *p, server_rec *s)
{
  ap_log_error(APLOG_MARK, APLOG_CRIT, 0, NULL, "[signal_interceptor (httpd module)] monitor for root process (pid: %d)__", getpid());
  set_custom_sigaction();
  return OK;
}

static void signal_interceptor_hook_child_init(apr_pool_t *p,
                                               server_rec *s)
{
  ap_log_error(APLOG_MARK, APLOG_CRIT, 0, NULL, "[signal_interceptor (httpd module)] child_init for child process (pid: %d)__", getpid());
  set_custom_sigaction();
}

static void signal_interceptor_register_hooks(apr_pool_t *p)
{
  ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "[signal_interceptor (httpd module)] register_hooks");
  // register hook to do something in the child processes
  ap_hook_child_init (signal_interceptor_hook_child_init,  NULL, NULL, APR_HOOK_MIDDLE);
  // register hook to do something in the root process
  ap_hook_post_config(signal_interceptor_hook_post_config, NULL, NULL, APR_HOOK_LAST);
  ap_hook_pre_mpm(x_pre_mpm, NULL, NULL, APR_HOOK_LAST);
  ap_hook_monitor(x_monitor, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA signal_interceptor_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    signal_interceptor_register_hooks  /* register hooks                      */
};

