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
  | Author: longmon <1307995200@qq.com>                                  |
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
ZEND_BEGIN_MODULE_GLOBALS(ioc)
	HashTable *class_map;
	HashTable *object_map;
ZEND_END_MODULE_GLOBALS(ioc)
*/


#ifdef ZTS
#define IOC_G(v) TSRMG(ioc_globals_id, zend_ioc_globals *, v)
#else
#define IOC_G(v) (ioc_globals.v)
#endif

void ioc_init();
void ioc_load_class_map( HashTable *fileList );
void hashtable_foreach_print( HashTable *ht );
int ioc_get_object_instance( char *name, zval *return_value, zval ***argv, int argc TSRMLS_DC );
int ioc_add_object_to_hash( const char *name, zval **obj );
int ioc_get_object_from_hash( const char *name, zval *return_value TSRMLS_DC );
int ioc_include_and_compile_file( char *file TSRMLS_DC );
char* ioc_get_class_file_from_hash( const char *name );
int ioc_check_include_file( const char *file TSRMLS_DC );
#endif	/* PHP_IOC_H */

