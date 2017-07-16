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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ioc.h"

/* If you declare any globals in php_ioc.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ioc)
*/

/* True global resources - no need for thread safety here */
static int le_ioc;
static zend_class_entry ioc_class_entry;
static zend_class_entry *ioc_class_entry_ptr;
static HashTable *class_map 	= NULL;
static HashTable *object_map 	= NULL;
/* {{{ ioc_functions[]
 *
 * Every user visible function must have an entry in ioc_functions[].
 */
const zend_function_entry ioc_class_methods[] = {
	ZEND_ME(ioc, init, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)		/* For testing, remove later. */
	ZEND_ME(ioc, make, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	
	PHP_FE_END	/* Must be the last line in ioc_functions[] */
};
const zend_function_entry ioc_functions = {
	ZEND_FE( ioc_version, NULL )
	PHP_FE_END
};
/* }}} */

/* {{{ ioc_module_entry
 */
zend_module_entry ioc_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ioc",
	ioc_functions,
	PHP_MINIT(ioc),
	PHP_MSHUTDOWN(ioc),
	PHP_RINIT(ioc),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ioc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ioc),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_IOC_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IOC
ZEND_GET_MODULE(ioc)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ioc.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ioc_globals, ioc_globals)
    STD_PHP_INI_ENTRY("ioc.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ioc_globals, ioc_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_ioc_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ioc_init_globals(zend_ioc_globals *ioc_globals)
{
	ioc_globals->global_value = 0;
	ioc_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ioc)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ioc)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ioc)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ioc)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ioc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ioc support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

ZEND_METHOD(ioc, init)
{
    	zval *fileList = NULL;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"a", &fileList ) == FAILURE ){
		return;
	}
	ioc_init();
	ioc_load_class( Z_ARRVAL_P(fileList));
	hashtable_foreach_print( class_map );
	RETURN_TRUE;
}
ZEND_METHOD(ioc, make)
{
	char *name = NULL;
	int len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE ){
		return;
	}
	zval *object = ioc_get_object( name );
	return_value = object;
}

void ioc_init()
{
	if( !class_map ){
		ALLOC_HASHTABLE(class_map);
		zend_hash_init(class_map, 64, NULL, NULL, 1);
	}
	if( !object_map ) {
		ALLOC_HASHTABLE(object_map);
		zend_hash_init(object_map, 64, NULL, NULL, 1);
	}
}

void ioc_load_class( HashTable *fileList )
{
	if( !class_map ){
		return;
	}
	int count,i;
	zval **file;
	count = zend_hash_num_elements( fileList );
	zend_hash_internal_pointer_reset( fileList );
	for( i = 0; i < count; i++ ){
		char *key;
		int idx;
		if( SUCCESS == zend_hash_get_current_data( fileList, (void **)&file )){
			convert_to_string_ex(file);
			if( zend_hash_get_current_key( fileList, &key, &idx, 0) == HASH_KEY_IS_STRING ) {
				zend_hash_add( class_map, key, sizeof(key), (void**)file, sizeof(file), NULL );
			} else {
				zend_hash_index_update( clsss_map,(ulong)i, (void**)file, sizeof(file), NULL );
			}
		}
		zend_hash_move_forward(ht);
	}
}

void hashtable_foreach_print( HashTable *ht ){
	int count,i;
	zval **item;
	count = zend_hash_num_elements(ht);
	zend_hash_internal_pointer_reset(ht);
	printf("fileList:%d\n", count);
	for( i = 0; i < count; i++ ){
		char *key;
		int idx;
		zend_hash_get_current_data( ht, (void **)&item);
		if( zend_hash_get_current_key(ht,&key,&idx,0) == HASH_KEY_IS_STRING ){
			printf("    %s=>%s\n", key, Z_ARRVAL_PP(item));
		} else {
			printf("    %d=>%s\n", i,Z_ARRVAL_PP(item));
		}
		zend_hash_move_forward(ht);
	}
}

zval *ioc_get_object( char *name ){
	zval *obj = NULL;
	if( zend_hash_find( ) == SUCCESS){

	} 
}
