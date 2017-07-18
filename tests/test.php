<?php


$filelist = array(
    "Foo" => realpath(__DIR__)."/Foo.php",
    "Bar" => realpath(__DIR__)."/Bar.php",
);

ioc::init( $filelist );


var_dump(get_included_files());

Bar::halo();

$foo = ioc::make("Foo");

echo $foo->get();