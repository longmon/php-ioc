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
HashTable *class_map 	= NULL;
HashTable *object_map 	= NULL;

/* {{{ ioc_functions[]
 *
 * Every user visible function must have an entry in ioc_functions[].
 */
const zend_function_entry ioc_class_methods[] = {
	ZEND_ME(ioc, init, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(ioc, make, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	
	PHP_FE_END	/* Must be the last line in ioc_functions[] */
};
const zend_function_entry ioc_functions[] = {
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

	INIT_CLASS_ENTRY( ioc_class_entry, "ioc", ioc_class_methods );
	ioc_class_entry_ptr = zend_register_internal_class( &ioc_class_entry TSRMLS_CC);

	//定义属性
    zend_declare_property_string(ioc_class_entry_ptr, "name", strlen("name"), "", ZEND_ACC_PUBLIC TSRMLS_CC);
    //定义常量
    zend_declare_class_constant_string(ioc_class_entry_ptr, "VERSION", strlen("VERSION"), "0.1 alpha" TSRMLS_CC);

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
	ioc_init();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ioc)
{
	if( class_map ){
		zend_hash_destroy(class_map);
		pefree(class_map, 1);
	}
	if( object_map ){
		zend_hash_destroy(object_map);
		pefree(object_map, 1 );
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ioc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, 	"ioc support", "enabled");
	php_info_print_table_row(2, 	"author",      "longmon");
	php_info_print_table_row(2, 	"version",     "0.1 alpha");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

ZEND_METHOD(ioc, init)
{
    zval *fileList = NULL;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &fileList ) == FAILURE ){
		return;
	}
	ioc_load_class_map( Z_ARRVAL_P(fileList) );
	RETURN_TRUE;
}
ZEND_METHOD(ioc, make)
{
	char *name = NULL;
	int len;
	zval ***argv = NULL;
	int argc = 0;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s*", &name, &len, &argv, &argc) == FAILURE ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Method ioc::make( class_name string [, parameter1, parameter2 ...]) required at least one paramter!");
		RETURN_FALSE;
	}
	if( ioc_get_object_instance( name, return_value, argv, argc TSRMLS_CC ) == FAILURE ){
		RETURN_NULL();
	}
}

ZEND_FUNCTION(ioc_version)
{
	RETURN_STRING("0.1 alpha", 1);
}

void ioc_init()
{
	
	if( !class_map ){
		class_map = pemalloc(sizeof(HashTable), 1);
		zend_hash_init(class_map, 64, NULL, NULL, 1);
	}
	if( !object_map ) {
		object_map = pemalloc(sizeof(HashTable), 1);
		zend_hash_init(object_map, 64, NULL, NULL, 1);
	}
}

void ioc_load_class_map( HashTable *fileList )
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
		ulong idx;
		if( SUCCESS == zend_hash_get_current_data( fileList, (void **)&file )){
			convert_to_string_ex(file);
			if( zend_hash_get_current_key( fileList, &key, &idx, 0) == HASH_KEY_IS_STRING ) {
				zend_hash_add( class_map, key, sizeof(key), (void**)file, sizeof(file), NULL );
			} else {
				zend_hash_index_update( class_map,(ulong)i, (void**)file, sizeof(file), NULL );
			}
			ioc_include_and_compile_file( Z_STRVAL_PP(file) TSRMLS_CC );
		}
		zend_hash_move_forward( fileList );
	}
}

void hashtable_foreach_print( HashTable *ht ){
	int count,i;
	zval **item;
	count = zend_hash_num_elements(ht);
	zend_hash_internal_pointer_reset(ht);
	php_printf("fileList:%d\n", count);
	for( i = 0; i < count; i++ ){
		char *key;
		ulong idx;
		if(zend_hash_get_current_data( ht, (void **)&item) != SUCCESS ){
			continue;
		}
		if( Z_TYPE_PP(item) == IS_ARRAY ){
			return hashtable_foreach_print( Z_ARRVAL_PP(item));
		}
		convert_to_string_ex(item);
		if( zend_hash_get_current_key(ht,&key,&idx,0) == HASH_KEY_IS_STRING ){
			php_printf("    %s=>%s\n", key, Z_STRVAL_PP(item));
		} else {
			php_printf("    %d=>%s\n", i, Z_STRVAL_PP(item));
		}
		zend_hash_move_forward(ht);
	}
	return;
}

int ioc_get_object_instance( char *name, zval *return_value, zval ***argv, int argc TSRMLS_DC ){

	if( ioc_get_object_from_hash( name, return_value TSRMLS_CC ) == SUCCESS ){
		return 0;
	}
	
	char *class_file = NULL;
	class_file = ioc_get_class_file_from_hash( name )  ;
	if( !class_file )
	{
		return -1;
	}

	if( ioc_include_and_compile_file( class_file TSRMLS_CC ) == FAILURE ) {
		php_error_docref( NULL TSRMLS_CC, E_WARNING, "compile file %s failed", class_file );
		return -1;
	}

	if( ioc_instance_object( name, return_value, argv, argc TSRMLS_CC ) == SUCCESS ){
		return 0;
	}
	return -1;
}

int ioc_add_object_to_hash( const char *name, zval **obj )
{
	if( !obj ){
		return -1;
	}
	if( !object_map ){
		return -1;
	}
	if( zend_hash_update( object_map, name, sizeof(name), obj, sizeof(**obj), NULL) == SUCCESS ){
		return 0;
	}
	return -1;
}

int ioc_get_object_from_hash( const char *name, zval *return_value TSRMLS_DC )
{
	if( !object_map ){
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "object_map had not been alloced");
		return -1;
	}
	zval **obj;
	if( zend_hash_find( object_map, name, sizeof(name), (void **)&obj ) == SUCCESS ){
		Z_TYPE_P(return_value)   = IS_OBJECT;
		Z_OBJVAL_P(return_value) = Z_OBJVAL_PP(obj);
		//zval_ptr_dtor(obj);
		return 0;
	}
	return -1;
}

char* ioc_get_class_file_from_hash( const char *name  ){

	char *fileName = NULL;
	if( !class_map ){
		return NULL;
	}
	zval **zfile;
	if( zend_hash_find( class_map, name, sizeof(name), (void **)&zfile ) == SUCCESS ) {
		//convert_to_string_ex( zfile );
		return Z_STRVAL_PP(zfile);
	}
	return NULL;
}

/**
 * 包含文件，
 * 参考：http://foio.github.io/php-extension-autoload/
 */
int ioc_include_and_compile_file( char *file TSRMLS_DC )
{
	if( ioc_check_include_file( file ) == SUCCESS ){
		return SUCCESS;
	}
	if( access( file, F_OK|R_OK ) == FAILURE ){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "File %s dose not exists!", file );
		return FAILURE;
	}

	zend_file_handle handle;
	zend_op_array *op;
	char realpath[MAXPATHLEN];
	if( !VCWD_REALPATH( file, realpath ) ){
		return -1;
	}
	handle.filename 		= file;
	handle.free_filename 	= 0;
	handle.type 			= ZEND_HANDLE_FILENAME;
	handle.opened_path  	= NULL;
	handle.handle.fp 		= NULL;

	op = zend_compile_file( &handle, ZEND_INCLUDE TSRMLS_CC );
	if( op && handle.handle.stream.handle ){
		int dummy = 1;
		if( !handle.opened_path ){
			handle.opened_path = file;
		}
		//将源文件注册到执行期间的全局变量(EG)的include_files列表中，这样就标记了源文件已经包含过了
        zend_hash_update(&EG(included_files), handle.opened_path, strlen(handle.opened_path)+1, (void *)&dummy, sizeof(int), NULL);
		zend_destroy_file_handle(&handle TSRMLS_CC);
	} 
	if( op )
	{
		zval *retval;
		//保存原来的执行环境，包括active_op_array,opline_ptr等
        zval ** __old_return_value_pp   = EG(return_value_ptr_ptr);
        zend_op ** __old_opline_ptr     = EG(opline_ptr); 
        zend_op_array * __old_op_array  = EG(active_op_array);
        //保存环境完成后，初始化本次执行环境，替换op_array
        EG(return_value_ptr_ptr) = &retval;
        EG(active_op_array)      = op;

		#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
        if (!EG(active_symbol_table)) {
            zend_rebuild_symbol_table(TSRMLS_C);
        }
		#endif	

		//调用zend API执行源文件的op_array
        zend_execute(op TSRMLS_CC);
        //op_array执行完成后销毁，要不然就要内存泄露了，哈哈
        destroy_op_array(op TSRMLS_CC);
        efree(op);
		//通过检查执行期间的全局变量(EG)的exception是否被标记来确定是否有异常
        if (!EG(exception)) {
            if (EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
                zval_ptr_dtor(EG(return_value_ptr_ptr));
            }
        }

		//ok,执行到这里说明源文件的op_array已经执行完成了，我们要恢复原来的执行环境了
        EG(return_value_ptr_ptr) = __old_return_value_pp;
        EG(opline_ptr)           = __old_opline_ptr; 
        EG(active_op_array)      = __old_op_array; 
		return 0;
	}
	return -1;
}

int ioc_check_include_file( const char *file TSRMLS_DC )
{
	int *dumy;
	zend_hash_internal_pointer_reset(&EG(included_files));
	if( zend_hash_find( &EG(included_files), file, strlen(file)+1, (void **)&dumy ) == SUCCESS ){
		return SUCCESS;
	}
	return FAILURE;
}

int ioc_instance_object( char *name, zval *return_value, zval ***argv, int argc TSRMLS_DC )
{
	zend_class_entry **class_ce, *old_scope;
	zval *retval_ptr;

	if( zend_lookup_class( name, strlen(name), &class_ce TSRMLS_CC ) == SUCCESS ){
		object_init_ex(return_value, *class_ce);	
		ioc_add_object_to_hash(name, &return_value );
		old_scope = EG(scope);
		EG(scope) = *class_ce;
		zend_function *func = Z_OBJ_HT_P(return_value)->get_constructor(return_value TSRMLS_CC);
		EG(scope) = old_scope;
		if( func ){
			zend_fcall_info fci;
			zend_fcall_info_cache fcc;

			if( !(func->common.fn_flags & ZEND_ACC_PUBLIC )){ //判断是否是public, 如果不是就停止执行，并抛出错误
				php_error_docref( NULL TSRMLS_CC, E_WARNING, "Access non-public constructor of class %s", name);
				zval_dtor(return_value);
				RETURN_FALSE;
			}
			
			fci.size = sizeof(fci);
			fci.function_table = EG(function_table);
			fci.function_name = NULL;
			fci.symbol_table = NULL;
			fci.object_ptr = return_value;
			fci.retval_ptr_ptr = &retval_ptr;
			fci.param_count = 0;
			fci.params = NULL;
			fci.no_separation = 1;

			fcc.initialized = 1;
			fcc.function_handler = func;
			fcc.calling_scope = EG(scope);
			fcc.called_scope = Z_OBJCE_P(return_value);
			fcc.object_ptr = return_value;

			if( zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE ) {
				if( argv ){
					efree(argv);
				}
				if( retval_ptr ){
					zval_ptr_dtor(&retval_ptr);
				}
				php_error_docref( NULL TSRMLS_CC, E_WARNING, "Invocation of %s's constructor failed", name);
				zval_dtor(return_value);
				RETURN_NULL();
			}
			if (retval_ptr) {
				zval_ptr_dtor(&retval_ptr);
			}
			if( argv ){
				efree(argv);
			}
			
		} else {
			//no contructor 
		}
		return 0;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s dose not exists!", name );
		return -1;
	}
}
