/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_IOC_H
#define PHP_IOC_H

extern zend_module_entry ioc_module_entry;
#define phpext_ioc_ptr &ioc_module_entry

#define PHP_IOC_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_IOC_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_IOC_API __attribute__ ((visibility("default")))
#else
#	define PHP_IOC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ioc);
PHP_MSHUTDOWN_FUNCTION(ioc);
PHP_RINIT_FUNCTION(ioc);
PHP_RSHUTDOWN_FUNCTION(ioc);
PHP_MINFO_FUNCTION(ioc);

ZEND_METHOD(ioc, init);
ZEND_METHOD(ioc, make);

ZEND_FUNCTION(ioc_version);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(ioc)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(ioc)
*/

/* In every utility function you add that needs to use variables 
   in php_ioc_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as IOC_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define IOC_G(v) TSRMG(ioc_globals_id, zend_ioc_globals *, v)
#else
#define IOC_G(v) (ioc_globals.v)
#endif

void ioc_init();
void ioc_load_class( HashTable *fileList );
void hashtable_foreach_print( HashTable *ht );
zval* ioc_get_object( char *name );

#endif	/* PHP_IOC_H */

