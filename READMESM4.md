# Readme

## Сборка

См. readme библиотеки libakrypt.

## Что сделано

Реализация SM4 из OpenSSL (https://github.com/openssl/openssl/blob/master/crypto..) встроена в libakrypt (по аналогии с Кузнечиком). Написаны тесты, они запускаются в рамках example-hello.
```/* 1. Создаем контекст ключа алгоритма SM4 и устанавливаем значение ключа */
  if ((error = ak_bckey_context_create_sm4(&bkey)) != ak_error_ok) {
    ak_error_message(error, __func__,
                     "incorrect initialization of sm4 secret key context");
    return ak_false;
  }
  if ((error = ak_bckey_context_set_key(&bkey, key, sizeof(key))) !=
      ak_error_ok) {
    ak_error_message(error, __func__, "wrong creation of test key");
    result = ak_false;
    goto exit;
  }
```

## Тесты

Функция для тестов: bool_t ak_bckey_test_sm4(void).<br/>
Лежит в файле: ak_sm4.c.<br/>
Вызов теста находится в файле: ak_libakrypt.c (Сразу после вызова тестов "кузнечик" и "магма").<br/>

(При запуске example-hello они в том числе тоже запускаются).
