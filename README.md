# php-ioc

    php-ioc是一个C语言开发的php扩展程序，是实现PHP依赖反转和对象容器的工具。

简单来说，php-ioc管理着一个类名到类文件的映射列表。实例化对象时直接根据这个映射表来取得相应的类，并实例化，再后返回给用户空间。

## 应用

```php
$class_map = array(
    "Foo" => "/data/www/lib/Foo.class.php",
    "Bar" => "/data/www/lib/Bar.class.php"
);

ioc::init($class_map); //初始化，底层依次执行的是require操作，所以这时候，类中的静态方法已经可以直接使用。

$foo = ioc::make("Foo"); //实例化对象，对象被保存内存，下次可以直接使用

$foo1 = ioc::make("Foo"); //与foo是同一个对象