#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    unsigned       begin:5;
    unsigned       len:3;
} ngx_http_time_item_t;

static ngx_int_t ngx_http_time_string_variable(
    ngx_http_request_t *r,
    ngx_http_variable_value_t *v,
    uintptr_t data);
static ngx_int_t ngx_http_time_item_variable(
    ngx_http_request_t *r,
    ngx_http_variable_value_t *v,
    uintptr_t data);
static ngx_int_t ngx_http_time_tomsec_variable(
    ngx_http_request_t *r,
    ngx_http_variable_value_t *v,
    uintptr_t data);
static ngx_int_t ngx_http_time_tosec_variable(
    ngx_http_request_t *r,
    ngx_http_variable_value_t *v,
    uintptr_t data);

static ngx_int_t ngx_http_time_add_vars(ngx_conf_t *cf);

static ngx_command_t ngx_http_time_var_commands[] = { ngx_null_command, };

static ngx_http_module_t  ngx_http_time_var_module_ctx = {
    ngx_http_time_add_vars,        /* preconfiguration */
    NULL,                          /* postconfiguration */
    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */
    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */
    NULL,                          /* create location configuration */
    NULL,                          /* merge location configuration */
};

ngx_module_t  ngx_http_time_var_module = {
    NGX_MODULE_V1,
    &ngx_http_time_var_module_ctx,
    ngx_http_time_var_commands,
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

// acquire item value by using ngx_cached_err_log_time
static ngx_http_time_item_t ngx_http_time_item_info[] = {
    {0,4}, {5,2}, {8,2}, {11,2}, {14,2}, {17,2},
};

static ngx_http_variable_t  ngx_http_time_vars[] = {

    //format: 1970/09/28 12:00:00
    { ngx_string("tm_err_log_time"), NULL,
      ngx_http_time_string_variable,
      (uintptr_t)&ngx_cached_err_log_time,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    //format: Mon, 28 Sep 1970 06:00:00 GMT
    { ngx_string("tm_http_time"), NULL,
      ngx_http_time_string_variable,
      (uintptr_t)&ngx_cached_http_time,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    //format: 28/Sep/1970:12:00:00 +0600
    { ngx_string("tm_http_log_time"), NULL,
      ngx_http_time_string_variable,
      (uintptr_t)&ngx_cached_http_log_time,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    //format: 1970-09-28T12:00:00+06:00
    { ngx_string("tm_http_log_iso8601"), NULL,
      ngx_http_time_string_variable,
      (uintptr_t)&ngx_cached_http_log_iso8601,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_year"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[0],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_month"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[1],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_day"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[2],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_hour"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[3],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_minute"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[4],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_second"), NULL,
      ngx_http_time_item_variable,
      (uintptr_t)&ngx_http_time_item_info[5],
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_tosec"), NULL,
      ngx_http_time_tosec_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("tm_tomsec"), NULL,
      ngx_http_time_tomsec_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },


    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};

static ngx_int_t
ngx_http_time_add_vars(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;
    for (v = ngx_http_time_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

static ngx_int_t
ngx_http_time_string_variable(ngx_http_request_t *r,
                              ngx_http_variable_value_t *v,
                              uintptr_t data)
{
    ngx_str_t *str = (ngx_str_t *)data;
    v->len = str->len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = str->data;
    return NGX_OK;
}

static ngx_int_t
ngx_http_time_item_variable(ngx_http_request_t *r,
                            ngx_http_variable_value_t *v,
                            uintptr_t data)
{
    ngx_http_time_item_t *info = (ngx_http_time_item_t *)data;
    v->len = info->len;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = ngx_cached_err_log_time.data + info->begin;
    return NGX_OK;
}

static ngx_int_t
ngx_http_time_tomsec_variable(ngx_http_request_t *r,
                              ngx_http_variable_value_t *v,
                              uintptr_t data)
{
    ngx_time_t    *tp;
    u_char        *p;

    tp = ngx_timeofday();

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN + 4);
    if (p == NULL) {
        return NGX_ERROR;
    }
    v->len = ngx_sprintf(p, "%T.%03M", tp->sec, tp->msec) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;
    return NGX_OK;
}

static ngx_int_t
ngx_http_time_tosec_variable(ngx_http_request_t *r,
                              ngx_http_variable_value_t *v,
                              uintptr_t data)
{
    ngx_time_t    *tp;
    u_char        *p;

    tp = ngx_timeofday();

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }
    v->len = ngx_sprintf(p, "%T", tp->sec) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;
    return NGX_OK;
}
