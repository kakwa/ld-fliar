ldfl_mapping_t ldfl_mapping[] = {
    /* name                   search_pattern          operation         target                extra_options         */
    { "temporary redirect",   ".*/temp/\\([^/]*\\)$", LDFL_OP_MAP,      "/tmp/\\1",           NULL},
    { "executable redirect",  ".*/.bin/\\([^/]*\\)$", LDFL_OP_EXEC_MAP, "/opt/fliar/bin/\\1", NULL},
    { "memory open",          ".*/file[0-9].txt",     LDFL_OP_MEM_OPEN, NULL,                 NULL},
    { "static file",          ".*/static.bin",        LDFL_OP_STATIC,   ldf_default_blob,     NULL},
    { "change perm/owner",    ".*/data/.*",           LDFL_OP_PERM,     NULL,                 "kakwa:kakwa|0700|0600"},
    { "allow & do nothing 1", "^/dev.*",              LDFL_OP_NOOP,     NULL,                 NULL},
    { "allow & do nothing 2", "^/proc.*",             LDFL_OP_NOOP,     NULL,                 NULL},
    { "allow & do nothing 3", "^/sys.*",              LDFL_OP_NOOP,     NULL,                 NULL},
    { "default & deny",       ".*",                   LDFL_OP_DENY,     NULL,                 NULL},
    { NULL,                   NULL,                   LDFL_OP_END,      NULL,                 NULL} // keep this last value
};


/*

.log_level values:
* LOG_EMERG
* LOG_ALERT
* LOG_CRIT
* LOG_ERR
* LOG_WARNING
* LOG_NOTICE
* LOG_INFO
* LOG_DEBUG

.logger values:
* ldfl_syslog_logger
* ldfl_stderr_logger
* ldfl_dummy_logger

or any logger implementing:
void cust_logger(int priority, const char *fmt, ...) {}

*/

ldfl_setting_t ldfl_setting = {
    .log_level   = LOG_DEBUG,
    .logger      = ldfl_syslog_logger,
};
