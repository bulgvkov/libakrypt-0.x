# Readme

## Сборка

См. readme библиотеки libakrypt

## Что сделано

Реализация SM4 из OpenSSL (https://github.com/openssl/openssl/blob/master/crypto..) встроена в libakrypt (по аналогии с Кузнечиком). Написаны тесты, они запускаются в рамках example-hello.

## Тесты

Функция для тестов: bool_t ak_bckey_test_sm4(void) \n
Лежит в файле: ak_sm4.c
Вызов теста находится в файле: ak_libakrypt.c (Сразу после вызова тестов "кузнечик" и "магма")

(При запуске example-hello они в том числе тоже запускаются)
