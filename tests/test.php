<?php

class foo{
    public function test(){
        echo __METHOD__,"\n";
    }
}

$filelist = array(
    "Foo" => realpath(__DIR__)."/Foo.php",
    "Bar" => realpath(__DIR__)."/Bar.php",
);

ioc::init( $filelist );