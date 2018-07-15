/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2014 - 2018 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Файл ak_random.с                                                                               */
/*  - содержит реализацию генераторов псевдо-случайных чисел                                       */
/* ----------------------------------------------------------------------------------------------- */
 #include <time.h>
 #include <ak_hash.h>
 #include <ak_mpzn.h>
 #include <ak_tools.h>

/* ----------------------------------------------------------------------------------------------- */
/*! Функция устанавливает значение полей структуры struct random в значения по-умолчанию.

    @param rnd указатель на структуру struct random
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create( ak_random rnd )
{
  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  rnd->data = NULL;
  rnd->oid = NULL;
  rnd->next = NULL;
  rnd->randomize_ptr = NULL;
  rnd->random = NULL;
  rnd->free = free;
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param rnd указатель на структуру struct random
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_destroy( ak_random rnd )
{
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                      "use a null pointer to a random generator" );
  if( rnd->data != NULL ) rnd->free( rnd->data );
  rnd->oid = NULL;
  rnd->next = NULL;
  rnd->randomize_ptr = NULL;
  rnd->random = NULL;
  rnd->free = NULL;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция очищает все внутренние поля, уничтожает генератор псевдо-случайных чисел
    (структуру struct random) и присваивает указателю значение NULL.

    @param rnd указатель на структуру struct random.
    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 ak_pointer ak_random_context_delete( ak_pointer rnd )
{
  if( rnd != NULL ) {
   ak_random_context_destroy(( ak_random ) rnd );
   free( rnd );
  } else ak_error_message( ak_error_null_pointer, __func__ ,
                                            "use a null pointer to a random generator" );
  return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Установка происходит путем вызова функции-члена класса random,
    которая и реализует механизм инициализации.

    @param rnd контекст генератора псевдо-случайных чисел.
    @param in указатель на данные, с помощью которых инициализируется генератор.
    @param size размер данных, в байтах.

    @return В случае успеха функция возвращает \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_randomize( ak_random rnd, const ak_pointer in, const size_t size )
{
 if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "use a null pointer to random generator" );
 if( in == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                           "use a null pointer to initializator" );
 if( rnd->randomize_ptr == NULL ) return ak_error_message( ak_error_undefined_function, __func__,
                                           "randomize() function not defined for this generator" );
 return rnd->randomize_ptr( rnd, in, size );
}

/* ----------------------------------------------------------------------------------------------- */
/*! Выработка последовательности псведо-случайных данных происходит путем
    вызова функции-члена класса random.

    @param rnd контекст генератора псевдо-случайных чисел.
    @param in указатель на область памяти, в которую помещаются псевдо-случайные данные.
    @param size размер помещаемых данных, в байтах.

    @return В случае успеха функция возвращает \ref ak_error_ok. В противном случае
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_random( ak_random rnd, const ak_pointer out, const size_t size )
{
 if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "use a null pointer to random generator" );
 if( out == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                             "use a null pointer to output data" );
 if( rnd->random == NULL ) return ak_error_message( ak_error_undefined_function, __func__,
                                                "this generator has undefined random() function" );
 return rnd->random( rnd, out, size );
}

/* ----------------------------------------------------------------------------------------------- */
  static ak_uint64 shift_value = 0; // Внутренняя статическая переменная (счетчик вызовов)

/* ----------------------------------------------------------------------------------------------- */
/*! Функция использует для генерации случайного значения текущее время, номер процесса и
    номер пользователя.
    Несмотря на кажущуюся случайность вырабатываемого значения, функция не должна использоваться для
    генерации значений, для которых требуется криптографическая случайность. Это связано с
    достаточно прогнозируемым изменением значений функции при многократных повторных вызовах.

    Основная задача данной функции - инициализаци программного генератора
    каким-либо знаением, в случае, когда пользователь не инициализирует программный генератор
    самостоятельно.

   \return Функция возвращает случайное число размером 8 байт (64 бита).                                                     */
/* ----------------------------------------------------------------------------------------------- */
 ak_uint64 ak_random_value( void )
{
  ak_uint64 vtme = time( NULL );
  ak_uint64 clk = clock();
#ifndef _WIN32
  ak_uint64 pval = getpid();
  ak_uint64 uval = getuid();
#else
  ak_uint64 pval = _getpid();
  ak_uint64 uval = 67;
#endif
  ak_uint64 value = ( shift_value += 11 )*125643267795740073LL + pval;
            value = ( value * 506098983240188723LL ) + 71331*uval + vtme;
  return value ^ clk;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                 реализация класса rng_lcg                                       */
/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс для хранения внутренних состояний линейного конгруэнтного генератора.             */
 typedef struct random_lcg {
  /*! \brief текущее значение внутреннего состояния генератора */
  ak_uint64 val;
 } *ak_random_lcg;

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_next( ak_random rnd )
{
  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  (( ak_random_lcg ) ( rnd->data ))->val *= 125643267795740073LL;
  (( ak_random_lcg ) ( rnd->data ))->val += 506098983240188723LL;
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_randomize_ptr( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  size_t idx = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to initial vector" );
    return ak_error_null_pointer;
  }
  if( !size ) {
    ak_error_message( ak_error_zero_length, __func__ , "use initial vector with zero length" );
    return ak_error_null_pointer;
  }
  /* сначала начальное значение, потом цикл по всем элементам массива */
  (( ak_random_lcg ) ( rnd->data ))->val = value[idx];
  do {
        rnd->next( rnd );
        (( ak_random_lcg ) ( rnd->data ))->val += value[idx];
  } while( ++idx < size );
 return rnd->next( rnd );
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_lcg_random( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  size_t i = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to data" );
    return ak_error_null_pointer;
  }
  if( !size ) {
    ak_error_message( ak_error_zero_length, __func__ , "use a data with zero length" );
    return ak_error_zero_length;
  }

  for( i = 0; i < size; i++ ) {
    value[i] = (ak_uint8) ((( ak_random_lcg ) ( rnd->data ))->val >> 16 );
    rnd->next( rnd );
  }
  return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Генератор вырабатывает последовательность внутренних состояний, удовлетворяющую
    линейному сравнению \f$ x_{n+1} \equiv a\cdot x_n + c \pmod{2^{64}}, \f$
    в котором константы a и c удовлетворяют равенствам
    \f$ a = 125643267795740073 \f$ и \f$ b = 506098983240188723. \f$

    Далее, последовательность внутренних состояний преобразуется в последовательность
    байт по следующему правилу
    \f$ \gamma_n = \displaystyle\frac{x_n - \hat x_n}{2^{24}} \pmod{256}, \f$
    где \f$\hat x_n \equiv x_n \pmod{2^{24}}. \f$

    @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_lcg( ak_random generator )
{
  int error = ak_error_ok;
  ak_uint64 qword = ak_random_value(); /* вырабатываем случайное число */

  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  if(( generator->data = malloc( sizeof( struct random_lcg ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
              "incorrect memory allocation for an internal variables of random generator" );

  generator->oid = ak_oid_context_find_by_name("lcg");
  generator->next = ak_random_lcg_next;
  generator->randomize_ptr = ak_random_lcg_randomize_ptr;
  generator->random = ak_random_lcg_random;
 /* функция generator->free уже установлена при вызове ak_random_create */

 /* для корректной работы присваиваем какое-то случайное начальное значение */
  ak_random_lcg_randomize_ptr( generator, &qword, sizeof( ak_uint64 ));
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                              реализация класса rng_xorshift64                                   */
/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс для хранения внутренних состояний генератора xorshift64.                          */
 typedef struct random_xorshift64 {
  /*! \brief текущее значение внутреннего состояния генератора */
   ak_uint64 val;
   ak_uint64 res;
 } *ak_random_xorshift64;

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift64_next( ak_random rnd )
{
  ak_uint64 x = 0;
  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  x = (( ak_random_xorshift64 ) ( rnd->data ))->val;
  x ^= ( x >> 12 );
  x ^= ( x << 25 );
  x ^= ( x >> 27 );
  (( ak_random_xorshift64 ) ( rnd->data ))->val = x;
  (( ak_random_xorshift64 ) ( rnd->data ))->res = x*0x2545F4914F6CDD1DLL;
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift64_randomize_ptr( ak_random rnd,
                                                           const ak_pointer ptr, const size_t size )
{
  size_t idx = 0;
  ak_uint8 *value = ptr;

  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to initial vector" );
    return ak_error_null_pointer;
  }
  if( !size ) {
    ak_error_message( ak_error_zero_length, __func__ , "use initial vector with zero length" );
    return ak_error_null_pointer;
  }
  /* сначала начальное значение, потом цикл по всем элементам массива */
  (( ak_random_xorshift64 ) ( rnd->data ))->val = ak_max( value[idx], 1 );
  do {
        rnd->next( rnd );
        (( ak_random_xorshift64 ) ( rnd->data ))->val += value[idx];
  } while( ++idx < size );
  /* генератор не может быть инициализирован нулем */
  if( (( ak_random_xorshift64 ) ( rnd->data ))->val == 0 )
    (( ak_random_xorshift64 ) ( rnd->data ))->val = 0x42134ea1;

 return rnd->next( rnd );
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_xorshift64_random( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  ak_uint64 *arr = (ak_uint64 *) ptr;
  size_t i = 0, blocks = ( size >>3 ), tail = size - ( blocks << 3 );

  if( rnd == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to a random generator" );
    return ak_error_null_pointer;
  }
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "use a null pointer to data" );
    return ak_error_null_pointer;
  }
  if( !size ) {
    ak_error_message( ak_error_zero_length, __func__ , "use a data with zero length" );
    return ak_error_zero_length;
  }

 /* сначала заполняем блоками по 8 байт */
  for( i = 0; i < blocks; i++ ) {
     arr[i] = (( ak_random_xorshift64 ) ( rnd->data ))->res;
     rnd->next( rnd );
  }
 /* потом остаток из младших разрядов */
  for( i = 0; i < tail; i++ )
     ((ak_uint8 *)( arr+blocks ))[i] =
                               ((ak_uint8 *)( &(( ak_random_xorshift64 ) ( rnd->data ))->res ))[i];
  rnd->next( rnd );
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Генератор вырабатывает последовательность внутренних состояний, удовлетворяющую ...

    @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_xorshift64( ak_random generator )
{
  int error = ak_error_ok;
  ak_uint64 qword = ak_random_value(); /* вырабатываем случайное число */

  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  if(( generator->data = malloc( sizeof( struct random_xorshift64 ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
              "incorrect memory allocation for an internal variables of random generator" );

  generator->oid = ak_oid_context_find_by_name("xorshift64");
  generator->next = ak_random_xorshift64_next;
  generator->randomize_ptr = ak_random_xorshift64_randomize_ptr;
  generator->random = ak_random_xorshift64_random;
 /* функция generator->free уже установлена при вызове ak_random_create */

 /* для корректной работы присваиваем какое-то случайное начальное значение */
  ak_random_xorshift64_randomize_ptr( generator, &qword, sizeof( ak_uint64 ));
 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                 реализация класса rng_file                                      */
/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс для хранения внутренних состояний генератора-файла                                */
 typedef struct random_file {
  /*! \brief файловый дескриптор */
  int fd;
 } *ak_random_file;

/* ----------------------------------------------------------------------------------------------- */
 static void ak_random_file_free( ak_pointer ptr )
{
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "freeing a null pointer to data" );
    return;
  }
  if( close( (( ak_random_file ) ptr )->fd ) == -1 )
    ak_error_message( ak_error_close_file, __func__ , "wrong closing a file with random data" );
  free(ptr);
}

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_file_random( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  ak_uint8 *value = ptr;
  size_t result = 0, count = size;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                     "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                   "use a null pointer to data" );
  if( !size ) return ak_error_message( ak_error_zero_length, __func__ ,
                                                                  "use a data with zero length" );
  /* считываем несколько байт */
  slabel:
  result = read( (( ak_random_file ) ( rnd->data ))->fd, value,
  #ifdef _MSC_VER
    (unsigned int)
  #endif
    count );

  /* если конец файла, то переходим в начало */
  if( result == 0 ) {
    lseek( (( ak_random_file ) ( rnd->data ))->fd, 0, SEEK_SET );
    goto slabel;
  }
  /* если мы считали меньше, чем надо */
  if( result < count ) {
    value += result;
    count -= result;
    goto slabel;
  }
  /* если ошибка чтения, то возбуждаем ошибку */
  if( result == -1 ) {
    ak_error_message( ak_error_read_data, __func__ , "wrong reading data from file" );
    return ak_error_read_data;
  }
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Данный генератор связывается с заданным файлом и возвращает содержащиеся в нем значения
    в качестве случайных чисел. Если данные в файле заканчиваются, то считывание начинается
    с начала файла.

    Основное назначение данного генератора - считывание данных из файловых устройств,
    таких как /dev/randon или /dev/urandom.

    @param generator Контекст создаваемого генератора.
    @param filename Имя файла.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_file( ak_random generator, const char *filename )
{
  int error = ak_error_ok;
  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  if(( generator->data = malloc( sizeof( struct random_file ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
           "incorrect memory allocation for an internal variables of random generator" );

 /* теперь мы открываем заданный пользователем файл */
  if( ((( ak_random_file ) ( generator->data ))->fd = open( filename, O_RDONLY | O_BINARY )) == -1 ) {
    ak_error_message_fmt( ak_error_open_file, __func__ ,
                                  "wrong opening a file \"%s\" with random data", filename );
    ak_random_context_destroy( generator );
    return ak_error_open_file;
  }

  // для данного генератора oid не определен
  generator->next = NULL;
  generator->randomize_ptr = NULL;
  generator->random = ak_random_file_random;
  generator->free = ak_random_file_free; /* эта функция должна закрыть открытый ранее файл */

 return error;
}

#if defined(__unix__) || defined(__APPLE__)
/* ----------------------------------------------------------------------------------------------- */
/*! @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_random( ak_random generator )
{
 int result = ak_random_context_create_file( generator, "/dev/random" );
  if( result == ak_error_ok ) generator->oid = ak_oid_context_find_by_name("dev-random");
 return result;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param generator Контекст создаваемого генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_urandom( ak_random generator )
{
 int result = ak_random_context_create_file( generator, "/dev/urandom" );
  if( result == ak_error_ok ) generator->oid = ak_oid_context_find_by_name("dev-urandom");
 return result;
}
#endif

/* ----------------------------------------------------------------------------------------------- */
/*                                         реализация класса winrtl                                */
/* ----------------------------------------------------------------------------------------------- */
#ifdef _WIN32
 #include <windows.h>
 #include <wincrypt.h>

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс для хранения контекста криптопровайдера. */
 typedef struct random_winrtl {
  /*! \brief контекст криптопровайдера */
  HCRYPTPROV handle;
} *ak_random_winrtl;

/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_winrtl_random( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  if( !CryptGenRandom( (( ak_random_winrtl )rnd->data)->handle, (DWORD) size, ptr ))
    return ak_error_message( ak_error_undefined_value, __func__,
                                                    "wrong generation of pseudo random sequence" );
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
 static void ak_random_winrtl_free( ak_pointer ptr )
{
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "freeing a null pointer to data" );
    return;
  }
  if( !CryptReleaseContext( (( ak_random_winrtl )ptr )->handle, 0 )) {
    ak_error_message_fmt( ak_error_close_file,
            __func__ , "wrong closing a system crypto provider with error: %x", GetLastError( ));
  }
  free( ptr );
}

/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_winrtl( ak_random generator )
{
  HCRYPTPROV handle = 0;

  int error = ak_error_ok;
  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  if(( generator->data = malloc( sizeof( struct random_winrtl ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
           "incorrect memory allocation for an internal variables of random generator" );

  /* теперь мы открываем криптопровайдер для доступа к генерации случайных значений
     в начале мы пытаемся создать новый ключ */
  if( !CryptAcquireContext( &handle, NULL, NULL,
                                         PROV_RSA_FULL, CRYPT_NEWKEYSET )) {
    /* здесь нам не удалось создать ключ, поэтому мы пытаемся его открыть */
    if( GetLastError() == NTE_EXISTS ) {
      if( !CryptAcquireContext( &handle, NULL, NULL,
                                         PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT )) {
        ak_error_message_fmt( error = ak_error_open_file, __func__,
                      "wrong open default key for system crypto provider with error: %x", GetLastError( ));
       ak_random_context_destroy( generator );
       return error;
      }
    } else {
       ak_error_message_fmt( error = ak_error_access_file, __func__,
                      "wrong creation of default key for system crypto provider with error: %x", GetLastError( ));
       ak_random_context_destroy( generator );
       return error;
     }
  }
  (( ak_random_winrtl )generator->data)->handle = handle;

  generator->oid = ak_oid_context_find_by_name("winrtl");
  generator->next = NULL;
  generator->randomize_ptr = NULL;
  generator->random = ak_random_winrtl_random;
  generator->free = ak_random_winrtl_free; /* эта функция должна закрыть открытый ранее криптопровайдер */

 return error;
}

#endif

/* ----------------------------------------------------------------------------------------------- */
/*                                   реализация класса hashrnd                                     */
/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс для хранения внутренних состояний генератора hashrnd. */
 typedef struct random_hashrnd {
  /*! \brief структура используемой бесключевой функции хеширования */
   struct hash ctx;
  /*! \brief текущее значение счетчика обработанных блоков */
   ak_mpzn512 counter;
  /*! \brief массив выработанных значений */
   ak_uint8 buffer[64];
  /*! \brief текущее количество доступных для выдачи октетов */
   size_t len;
 } *ak_random_hashrnd;

/* ----------------------------------------------------------------------------------------------- */
/*! @param rnd Контекст генератора.
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_hashrnd_next( ak_random rnd )
{
  ak_random_hashrnd hrnd = NULL;
  ak_mpzn512 one = ak_mpzn512_one;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                    "use a null pointer to a random generator" );
 /* получаем указатель и вырабатываем новый вектор значений */
  hrnd = ( ak_random_hashrnd )rnd->data;
  if( hrnd->len != 0 ) return ak_error_message( ak_error_wrong_length, __func__,
                                                             "unexpected use of next function" );
 /* увеличиваем счетчик */
  ak_mpzn_add( hrnd->counter, hrnd->counter, one, ak_mpzn512_size );
 /* вычисляем новое хеш-значение */
  ak_hash_context_ptr( &hrnd->ctx, hrnd->counter, 64, hrnd->buffer );
 /* определяем доступный объем данных для считывания */
  hrnd->len = hrnd->ctx.hsize;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param ptr Указатель на область внутренних данных генератора.                                  */
/* ----------------------------------------------------------------------------------------------- */
 static void ak_random_hashrnd_free( ak_pointer ptr )
{
  int error = ak_error_ok;
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__ , "freeing a null pointer to data" );
    return;
  }
 /* уничтожаем контекст функции хеширования */
  if(( error = ak_hash_context_destroy( &(( ak_random_hashrnd )ptr)->ctx )) != ak_error_ok )
     ak_error_message( error, __func__ , "wrong destroying internal hash function context" );
 /* теперь уничтожаем собственно структуру hashrnd */
  free(ptr);
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param rnd Контекст генератора.
    \param ptr Указатель на область данных, которыми инициалиируется генератор
    \param size Размер области в байтах
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_hashrnd_randomize_ptr( ak_random rnd,
                                                           const ak_pointer ptr, const size_t size )
{
  ak_random_hashrnd hrnd = NULL;
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                     "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                   "use a null pointer to data" );
  if( !size ) return ak_error_message( ak_error_zero_length, __func__ ,
                                                                  "use a data with zero length" );
 /* восстанавливаем начальное значение */
  hrnd = ( ak_random_hashrnd )rnd->data;
  hrnd->len = 0;
  memset( hrnd->counter, 0, 64 );
  memset( hrnd->buffer, 0, 64 );

 /* теперь вырабатываем 47 октетов начального заполнения */
  if(( size <= 47 ) || ( hrnd->ctx.hsize > 64 )) memcpy( hrnd->counter+2, ptr, ak_min( size, 47 ));
   else {
          ak_uint8 buffer[64];  /* промежуточный буффер */
          memset( buffer,  0x11, 64 );
          ak_hash_context_ptr( &hrnd->ctx, ptr, size, buffer );
          memcpy( hrnd->counter+2, buffer, 47 );
   }
 /* вычисляем псевдо-случайные данные */
  ak_random_hashrnd_next( rnd );
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param rnd Контекст генератора.
    \param ptr Указатель на область памяти, в которую помещаются вырабатываемые значения
    \param size Размер области в байтах
    \return В случае успеха, функция возвращает \ref ak_error_ok. В противном случае
            возвращается код ошибки.                                                               */
/* ----------------------------------------------------------------------------------------------- */
 static int ak_random_hashrnd_random( ak_random rnd, const ak_pointer ptr, const size_t size )
{
  ak_uint8 *inptr = ptr;
  size_t realsize = size;
  ak_random_hashrnd hrnd = NULL;

  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                     "use a null pointer to a random generator" );
  if( ptr == NULL ) return ak_error_message( ak_error_null_pointer, __func__ ,
                                                                   "use a null pointer to data" );
  if( !size ) return ak_error_message( ak_error_zero_length, __func__ ,
                                                                  "use a data with zero length" );
  hrnd = ( ak_random_hashrnd )rnd->data;

  while( realsize > 0 ) {
    size_t offset = ak_min( realsize, hrnd->len );
    memcpy( inptr, hrnd->buffer + (hrnd->ctx.hsize - hrnd->len), offset );
    inptr += offset;
    realsize -= offset;
    if(( hrnd->len -= offset ) <= 0 ) /* вычисляем следующий массив данных */
      ak_random_hashrnd_next( rnd );
  }
 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функия создает генератор, вырабатывающий последовательность псевдо-случайных данных с
    использованием бесключевой функции хеширования согласно рекомендациям по
    стандартизации Р 1323565.1.006-2017.
    Параметр oid задает используемый алгоритм хеширования.

    \param generator контекст инициализируемого генератора псевдо-случайных чисел.
    \param oid идентификатор бесключевой функции хеширования.
    \return Функция возвращает код ошибки.                                                         */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_hashrnd_oid( ak_random generator, ak_oid oid )
{
  struct random rnd;
  int error = ak_error_ok;
  ak_random_hashrnd hrnd = NULL;
  char oidname[32]; /* имя для oid генератора псевдо-случайных чисел */

  if( oid == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                 "using null pointer to hash function OID" );
 /* проверяем, что OID от бесключевой функции хеширования */
  if( oid->engine != hash_function )
    return ak_error_message( ak_error_oid_engine, __func__ , "using oid with wrong engine" );
 /* проверяем, что OID от алгоритма, а не от параметров */
  if( oid->mode != algorithm )
    return ak_error_message( ak_error_oid_mode, __func__ , "using oid with wrong mode" );

 /* создаем генератор */
  if(( error = ak_random_context_create( generator )) != ak_error_ok )
    return ak_error_message( error, __func__ , "wrong initialization of random generator" );

  if(( hrnd = generator->data = malloc( sizeof( struct random_hashrnd ))) == NULL )
    return ak_error_message( ak_error_out_of_memory, __func__ ,
              "incorrect memory allocation for an internal variables of random generator" );

  /* инициализируем поля и данные внутренней структуры данных */
   if(( error = ak_hash_context_create_oid( &hrnd->ctx, oid )) != ak_error_ok ) {
     ak_random_context_destroy( generator );
     return ak_error_message( error, __func__ ,
                                   "incorrect creation of internal hash function context" );
   }
   hrnd->len = 0;
   memset( hrnd->counter, 0, 64 );
   memset( hrnd->buffer, 0, 64 );
   ak_snprintf( oidname, 30, "hashrnd-%s", oid->name );

   generator->oid = ak_oid_context_find_by_name( oidname );
   generator->next = ak_random_hashrnd_next;
   generator->randomize_ptr = ak_random_hashrnd_randomize_ptr;
   generator->random = ak_random_hashrnd_random;
   generator->free = ak_random_hashrnd_free;

 /* для корректной работы присваиваем какое-то случайное начальное значение,
    используя для этого другой генератор псевдо-случайных чисел */

#if defined(__unix__) || defined(__APPLE__)
   error = ak_random_context_create_random( &rnd );
#else
   error = ak_random_context_create_xorshift64( &rnd );
#endif
   if( error != ak_error_ok ) {
     ak_random_context_destroy( generator );
     return ak_error_message( error, __func__ ,
                                "incorrect creation of internal random generator context" );
   }
  /* константа 47 означает, что младшие 16 октетов вектора (область изменения счетчика)
     останутся нулевыми, также как и один старший октет (согласно рекомендациям Р 1323565.1.006-2017) */
   ak_random_context_random( &rnd, hrnd->counter+2, 47 );
   ak_random_context_destroy( &rnd );
  /* вычисляем псевдо-случайные данные */
   ak_random_hashrnd_next( generator );

 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param generator контекст инициализируемого генератора псевдо-случайных чисел.                 */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_hashrnd_gosthash94( ak_random generator )
{
  return ak_random_context_create_hashrnd_oid( generator,
                                                      ak_oid_context_find_by_name( "gosthash94" ));
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param generator контекст инициализируемого генератора псевдо-случайных чисел.                 */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_hashrnd_streebog256( ak_random generator )
{
  return ak_random_context_create_hashrnd_oid( generator,
                                                     ak_oid_context_find_by_name( "streebog256" ));
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param generator контекст инициализируемого генератора псевдо-случайных чисел.                 */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_hashrnd_streebog512( ak_random generator )
{
  return ak_random_context_create_hashrnd_oid( generator,
                                                     ak_oid_context_find_by_name( "streebog512" ));
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param ctx указатель на контекст генератора псевдо-случайных чисел
    @param oid OID генератора.

    @return В случае успеха возвращается ak_error_ok (ноль). В случае возникновения ошибки
    возвращается ее код.                                                                           */
/* ----------------------------------------------------------------------------------------------- */
 int ak_random_context_create_oid( ak_random rnd, ak_oid oid )
{
  int error = ak_error_ok;

 /* выполняем проверку */
  if( rnd == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                             "using null pointer to random generator context" );
  if( oid == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                 "using null pointer to random generator OID" );
 /* проверяем, что OID от того, что нужно */
  if( oid->engine != random_generator )
    return ak_error_message( ak_error_oid_engine, __func__ , "using oid with wrong engine" );
 /* проверяем, что OID от алгоритма, а не от каких-то там параметров  */
  if( oid->mode != algorithm )
    return ak_error_message( ak_error_oid_mode, __func__ , "using oid with wrong mode" );
 /* проверяем, что производящая функция определена */
  if( oid->func.create == NULL )
    return ak_error_message( ak_error_undefined_function, __func__ ,
                                                       "using oid with undefined constructor" );
 /* инициализируем контекст */
  if(( error = (( ak_function_random * )oid->func.create )( rnd )) != ak_error_ok )
      return ak_error_message( error, __func__, "invalid creation of random generator context");

 return error;
}

/* ----------------------------------------------------------------------------------------------- */
/*!  \example test-internal-random01.c                                                             */
/*!  \example test-internal-random02.c                                                             */
/* ----------------------------------------------------------------------------------------------- */
/*                                                                                    ak_random.c  */
/* ----------------------------------------------------------------------------------------------- */
