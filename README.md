# LDFL

[![Doxygen](https://github.com/kakwa/ldfl/actions/workflows/doxygen.yml/badge.svg)](https://github.com/kakwa/ldfl/actions/workflows/doxygen.yml)
[![Memleak Check](https://github.com/kakwa/ldfl/actions/workflows/valgrind.yml/badge.svg)](https://github.com/kakwa/ldfl/actions/workflows/valgrind.yml)
[![Unit Tests & Coverage](https://github.com/kakwa/ldfl/actions/workflows/coverage.yml/badge.svg)](https://github.com/kakwa/ldfl/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/kakwa/ldfl/graph/badge.svg?token=08AAHC625O)](https://codecov.io/gh/kakwa/ldfl)

## Status

This project is under Development, most functionalities are not implemented yet.

## Presentation

LDFL (LD File Liar) is a powerful `LD_PRELOAD` library that intercepts and modify `libc` file system operations. It allows you to:

- **Log Filesystem Interactions**: File & Directory manipulation can be logged to syslog or stderr
- **Remap File Paths**: Redirect file access to different locations
- **Control File Access**: Restrict or allow access to specific files/directories
- **Modify File Permissions**: Change ownership and permissions on-the-fly
- **Memory-based Files**: Serve files directly from memory
- **Static Content**: Serve predefined static content
- **Executable Redirection**: Redirect executable paths

This tool can be used on existing binaries or can be included with a static configuration header inside your projects.

## Documentation

You can also view the latest documentation online at [GitHub Pages](https://kakwa.github.io/ldfl/).

## Dependencies

The following dependencies are required to build ldfl:

- CMake (version 3.12 or higher)
- PCRE2 library
- Jansson library
- CUnit (optional, for tests)
- Doxygen (optional, for documentation)

On Ubuntu/Debian, you can install these dependencies with:
```bash
sudo apt update
sudo apt install -y cmake libpcre2-dev libjansson-dev libcunit1-dev doxygen
```

## Development

1. Clone the repository:
```bash
git clone https://github.com/kakwa/ldfl.git
cd ldfl
```

2. Create a build directory and configure CMake:
```bash
cmake .
```

3. Build the project:
```bash
make
```

Optional build options:
- `-DBUILD_TESTS=ON`: Enable building tests
- `-DBUILD_DOC=ON`: Enable building documentation
- `-DCOVERAGE=ON`: Enable code coverage (requires `-DBUILD_TESTS=ON`)
- `-DDEBUG=ON`: Build with debug symbols
- `-DSTATIC=ON`: Build static library

To run the tests & coverage
```bash
cmake . -DBUILD_TESTS=ON -DCOVERAGE=ON
make
make coverage

$BROWSER coverage/index.html
```

To build the documentation:
```bash
# Configure CMake
cmake -DBUILD_DOC=ON .
# Get Doxygen awesome CSS
./misc/setup_doxycss.sh
# Build Doc
make

# open the doc
$BROWSER ./docs/html/index.html
```

## Installation

After building, you can install the library system-wide:
```bash
sudo make install
```

This will install:
- The library (`libldfl.so`) to `/usr/local/lib/`
- The wrapper executable (`ldfl-cli`) to `/usr/local/bin/`

## Usage

### Wrapper Usage

1. Create a configuration file (e.g., `config.json`) with your mapping rules:
```bash
ldfl-cli -c config.json -- your-application [args...]
```

To enable debug output, use the `-d` flag:
```bash
ldfl-cli -d -c config.json -- your-application [args...]
```

If you need to specify a custom library path:
```bash
ldfl-cli -l /path/to/libldfl.so -c config.json -- your-application [args...]
```

### Direct `LD_PRELOAD` Usage

You can use LDFL directly with `LD_PRELOAD` without the wrapper:

```bash
# Set the configuration file
export LDFL_CONFIG=/path/to/config.json

# Preload the library
LD_PRELOAD=/path/to/libldfl.so your-application [args...]
```

## JSON Configuration

The configuration file is a JSON file with two main sections: `settings` and `mappings`.

### Settings

The `settings` section controls the logging behavior:

```json
{
  "settings": {
    "log_mask": [
      "mapping_rule_found",
      "fn_call",
      "init",
      "mapping_rule_apply",
      "mapping_rule_search",
      "fn_call_err"
    ],
    "log_level": "warning",
    "logger": "syslog"
  }
}
```

Available log masks:
- `mapping_rule_found`: Log when a mapping rule is found
- `fn_call`: Log LibC function calls
- `init`: Log initialization operations
- `mapping_rule_apply`: Log when a mapping rule is applied
- `mapping_rule_search`: Log mapping search operations
- `fn_call_err`: Log LibC function call errors

Log levels:
- `debug`
- `info`
- `warning`
- `error`

Loggers:
- `syslog`: System logger
- `stderr`: Standard error output
- `dummy`: No logging

### Mappings

The `mappings` section defines the file path remapping rules. Each mapping has the following properties:

```json
{
  "mappings": [
    {
      "name": "descriptive name",
      "search_pattern": "regex pattern",
      "operation": "operation type",
      "target": "target path or resource",
      "path_transform": "absolute|original",
      "extra_options": "operation specific options"
    }
  ]
}
```

#### Available Operations

1. **File Redirection** (`map`):
```json
{
  "name": "temp files redirect",
  "search_pattern": ".*/temp/([^/]*)$",
  "operation": "map",
  "target": "/tmp/$1",
  "path_transform": "absolute"
}
```

2. **Executable Redirection** (`exec_map`):
```json
{
  "name": "executable redirect",
  "search_pattern": ".*/.bin/\\([^/]*\\)$",
  "operation": "exec_map",
  "target": "/opt/ldfl/bin/\\1",
  "path_transform": "absolute"
}
```

3. **Memory File** (`mem_open`):
```json
{
  "name": "memory open",
  "search_pattern": ".*/file[0-9].txt",
  "operation": "mem_open",
  "target": null,
  "path_transform": "absolute"
}
```

4. **Static File** (`static`):
```json
{
  "name": "static file",
  "search_pattern": ".*/static.bin",
  "operation": "static",
  "target": "default_blob",
  "path_transform": "absolute"
}
```

5. **Permission Change** (`perm`):
```json
{
  "name": "change data perm",
  "search_pattern": ".*/data/.*",
  "operation": "perm",
  "target": null,
  "path_transform": "absolute",
  "extra_options": "user:group|dir_mode|file_mode"
}
```

6. **Access Control**:
   - **Allow** (`noop`):
```json
{
  "name": "allow /dev",
  "search_pattern": "^/dev/.*",
  "operation": "noop",
  "path_transform": "absolute"
}
```
   - **Deny** (`deny`):
```json
{
  "name": "default & deny",
  "search_pattern": ".*",
  "operation": "deny",
  "path_transform": "absolute"
}
```

7. **Read-Only** (`ro`):
```json
{
  "name": "read only files",
  "search_pattern": ".*/readonly/.*",
  "operation": "ro",
  "path_transform": "absolute"
}
```

## Embedding

For embedding LDFL in your project, copy over `lib/ldfl.c` in your project.

Create a header file (e.g., `ldfl-config.h`) with your configuration:

```c
static const unsigned char ldf_default_blob[] = "hello from ldfl";

ldfl_mapping_t ldfl_mapping[] = {
    /* name                   search_pattern          operation         target                path_transform, extra_options         */
    { "temp files redirect",  ".*/temp/([^/]*)$",     LDFL_OP_MAP,      "/tmp/$1",            LDFL_PATH_ABS,  NULL                   },
    { "inc redirect",         "(.*)/inc/(.*)",        LDFL_OP_MAP,      "$1/lib/$2",          LDFL_PATH_ABS,  NULL                   },
    { "executable redirect",  ".*/.bin/\\([^/]*\\)$", LDFL_OP_EXEC_MAP, "/opt/ldfl/bin/\\1",  LDFL_PATH_ABS,  NULL                   },
    { "memory open",          ".*/file[0-9].txt",     LDFL_OP_MEM_OPEN, NULL,                 LDFL_PATH_ABS,  NULL                   },
    { "static file",          ".*/static.bin",        LDFL_OP_STATIC,   ldf_default_blob,     LDFL_PATH_ABS,  NULL                   },
    { "change data perm",     ".*/data/.*",           LDFL_OP_PERM,     NULL,                 LDFL_PATH_ABS,  "kakwa:kakwa|0700|0600"},
    { "allow /dev",           "^/dev/.*",             LDFL_OP_NOOP,     NULL,                 LDFL_PATH_ABS,  NULL                   },
    { "default & deny",       ".*",                   LDFL_OP_DENY,     NULL,                 LDFL_PATH_ABS,  NULL                   },
    { NULL,                   NULL,                   LDFL_OP_END,      NULL,                 LDFL_PATH_ABS,  NULL                   }  // keep this last value
};

ldfl_setting_t ldfl_setting = {
    .log_mask    = LDFL_LOG_MAPPING_RULE_FOUND | LDFL_LOG_FN_CALL | LDFL_LOG_INIT | LDFL_LOG_MAPPING_RULE_APPLY | LDFL_LOG_FN_CALL_ERR,
    .log_level   = LOG_WARNING,
    .logger      = ldfl_syslog_logger,
};
```

In your code, add:
```c
#define LDFL_CONFIG "ldfl-config.h"
#include "ldfl.c"
```

Link against libpcre2 (`-lpcre2-8`) if necessary.

And finally, build your project.
