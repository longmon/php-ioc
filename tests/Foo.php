<?php

class Foo{
    public function __construct(){
        var_dump(__METHOD__);
    }

    public function get(){
        return "AAAAAAAAA";
    }
}
