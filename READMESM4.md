# Readme

## Сборка

См. readme библиотеки libakrypt.

## Что сделано

Реализация SM4 из OpenSSL (https://github.com/openssl/openssl/blob/master/crypto..) встроена в libakrypt (по аналогии с Кузнечиком). Написаны тесты, они запускаются в рамках example-hello.

1. Создаем контекст ключа алгоритма SM4 и устанавливаем значение ключа<br/>

```
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

2. Проверяем независимую обработку блоков - режим простой замены согласно SM4<br/>

```
if ((error = ak_bckey_context_encrypt_ecb(&bkey, in, myout, sizeof(in))) !=
      ak_error_ok) {
    ak_error_message(error, __func__, "wrong ecb mode encryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, outecb, sizeof(outecb))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the ecb mode encryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }

  if ((error = ak_bckey_context_decrypt_ecb(&bkey, outecb, myout,
                                            sizeof(outecb))) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong ecb mode decryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, in, sizeof(in))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the ecb mode decryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }
  if (audit >= ak_log_maximum)
    ak_error_message(ak_error_ok, __func__,
                     "the ecb mode encryption/decryption test from SM4 is Ok");
```

3. Проверяем режим гаммирования согласно SM4

```
if ((error = ak_bckey_context_ctr(&bkey, in, myout, sizeof(in), ivctr,
                                    sizeof(ivctr))) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong counter mode encryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, outctr, sizeof(outctr))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the counter mode encryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }

  if ((error = ak_bckey_context_ctr(&bkey, myout, myout, sizeof(outctr), ivctr,
                                    sizeof(ivctr))) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong ecb mode decryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, in, sizeof(in))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the counter mode decryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }
  if (audit >= ak_log_maximum)
    ak_error_message(
        ak_error_ok, __func__,
        "the counter mode encryption/decryption test from SM4 is Ok");
```

4. Проверяем режим простой замены c зацеплением согласно SM4

```
if ((error = ak_bckey_context_encrypt_cbc(&bkey, in, myout, sizeof(in), ivcbc,
                                            sizeof(ivcbc))) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong cbc mode encryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, outcbc, sizeof(outcbc))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the cbc mode encryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }
  if ((error =
           ak_bckey_context_decrypt_cbc(&bkey, outcbc, myout, sizeof(outcbc),
                                        ivcbc, sizeof(ivcbc))) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong cbc mode encryption");
    result = ak_false;
    goto exit;
  }
  if (!ak_ptr_is_equal_with_log(myout, in, sizeof(in))) {
    ak_error_message(ak_error_not_equal_data, __func__,
                     "the cbc mode encryption test from SM4 is wrong");
    result = ak_false;
    goto exit;
  }
  if (audit >= ak_log_maximum)
    ak_error_message(ak_error_ok, __func__,
                     "the cbc mode encryption/decryption test from SM4 is Ok");
```

5. Освобождаем ключ и выходим

```
exit:
  if ((error = ak_bckey_context_destroy(&bkey)) != ak_error_ok) {
    ak_error_message(error, __func__, "wrong destroying of secret key");
    return ak_false;
  }

  return result;
```

## Тесты

Функция для тестов: bool_t ak_bckey_test_sm4(void).<br/>
Лежит в файле: ak_sm4.c.<br/>
Вызов теста находится в файле: ak_libakrypt.c (Сразу после вызова тестов "кузнечик" и "магма").<br/>

(При запуске example-hello они в том числе тоже запускаются).
