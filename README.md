This module provides some time variables of nginx. The variables are all from the nginx cached content(no syscall and computing of time).
Usage
-----

Working example:

    default_type text/plain;
    echo 'year  :$tm_year';
    echo 'month :$tm_month';
    echo 'day   :$tm_day';
    echo 'hour  :$tm_hour';
    echo 'minute:$tm_minute';
    echo 'second:$tm_second';
    echo ''; 
    echo $tm_err_log_time;
    echo $tm_http_time;
    echo $tm_http_log_time;
    echo $tm_http_log_iso8601;
    echo ''; 
    echo $tm_tomsec;
    echo $tm_tosec;

output:

    year  :2012
    month :01
    day   :08
    hour  :03
    minute:54
    second:21

    2012/01/08 03:54:21
    Sat, 07 Jan 2012 19:54:21 GMT
    08/Jan/2012:03:54:21 +0800
    2012-01-08T03:54:21+08:00

    1325966061.521
    1325966061

Installation
------------

    ./configure --add-module=/path/to/ngx_http_time_var_module


