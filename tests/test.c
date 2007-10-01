/*
  Sample test application.
*/
#include <assert.h>
#include <memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_test(void)
{
  memcached_st memc;

  (void)memcached_create(&memc);
  memcached_free(&memc);
}

void allocation_test(void)
{
  memcached_st *memc;
  memc= memcached_create(NULL);
  assert(memc);
  memcached_free(memc);
}

void connection_test(void)
{
  memcached_return rc;
  memcached_st *memc;
  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  assert(memc);
  memcached_free(memc);
}

void error_test(void)
{
  memcached_st *memc;
  memc= memcached_create(NULL);
  memcached_return rc;

  for (rc= MEMCACHED_SUCCESS; rc < MEMCACHED_MAXIMUM_RETURN; rc++)
  {
    printf("Error %d -> %s\n", rc, memcached_strerror(memc, rc));
  }

  assert(memc);
  memcached_free(memc);
}

void set_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);
  
  memcached_free(memc);
}

void add_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_add(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_NOTSTORED);
  
  memcached_free(memc);
}

void replace_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_replace(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);
  
  memcached_free(memc);
}

void delete_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_delete(memc, key, strlen(key), (time_t)0);
  assert(rc == MEMCACHED_SUCCESS);
  
  memcached_free(memc);
}

void flush_test(void)
{
  memcached_st *memc;
  memcached_return rc;

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  memcached_free(memc);
}

void get_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *string;
  size_t string_length;
  uint16_t flags;

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
  
  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(rc == MEMCACHED_NOTFOUND);
  assert(string_length ==  0);
  assert(!string);

  memcached_free(memc);
}

void get_test2(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";
  char *string;
  size_t string_length;
  uint16_t flags;

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(string);
  assert(rc == MEMCACHED_SUCCESS);
  assert(string_length == strlen(value));
  assert(!memcmp(string, value, string_length));

  free(string);

  memcached_free(memc);
}

void get_test3(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value;
  size_t value_length= 8191;
  char *string;
  size_t string_length;
  uint16_t flags;
  int x;

  value = (char*)malloc(value_length);
  assert(value);

  for (x= 0; x < value_length; x++)
    value[x] = (char) (x % 127);

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, value_length,
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(rc == MEMCACHED_SUCCESS);
  assert(string);
  assert(string_length == value_length);
  assert(!memcmp(string, value, string_length));

  free(string);
  free(value);

  memcached_free(memc);
}

void get_test4(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "foo";
  char *value;
  size_t value_length= 8191;
  char *string;
  size_t string_length;
  uint16_t flags;
  int x;

  value = (char*)malloc(value_length);
  assert(value);

  for (x= 0; x < value_length; x++)
    value[x] = (char) (x % 127);

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, value_length,
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  for (x= 0; x < 10; x++)
  {
    string= memcached_get(memc, key, strlen(key),
                          &string_length, &flags, &rc);

    assert(rc == MEMCACHED_SUCCESS);
    assert(string);
    assert(string_length == value_length);
    assert(!memcmp(string, value, string_length));
    free(string);
  }

  free(value);

  memcached_free(memc);
}

void stats_servername_test(void)
{
  memcached_return rc;
  memcached_stat_st stat;
  rc= memcached_stat_servername(&stat, NULL,
                                 "localhost", 
                                 MEMCACHED_DEFAULT_PORT);
}

void increment_test(void)
{
  memcached_st *memc;
  unsigned int new_number;
  memcached_return rc;
  char *key= "number";
  char *value= "0";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_increment(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 1);

  rc= memcached_increment(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 2);

  memcached_free(memc);
}

void decrement_test(void)
{
  memcached_st *memc;
  unsigned int new_number;
  memcached_return rc;
  char *key= "number";
  char *value= "3";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_decrement(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 2);

  rc= memcached_decrement(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 1);

  memcached_free(memc);
}

void quit_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *key= "fudge";
  char *value= "sanford and sun";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)10, (uint16_t)3);
  assert(rc == MEMCACHED_SUCCESS);
  memcached_quit(memc);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)50, (uint16_t)9);
  assert(rc == MEMCACHED_SUCCESS);
  
  memcached_free(memc);
}

void mget_test(void)
{
  memcached_st *memc;
  memcached_return rc;
  char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};
  unsigned int x;
  uint16_t flags;

  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  /* We need to empty the server before continueing test */
  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                      &return_value_length, &flags, &rc)) != NULL)
  {
    assert(return_value);
  }
  assert(!return_value);
  assert(return_value_length == 0);
  assert(rc == MEMCACHED_NOTFOUND);

  for (x= 0; x < 3; x++)
  {
    rc= memcached_set(memc, keys[x], key_length[x], 
                      keys[x], key_length[x],
                      (time_t)50, (uint16_t)9);
    assert(rc == MEMCACHED_SUCCESS);
  }

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  x= 0;
  while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                                        &return_value_length, &flags, &rc)))
  {
    assert(return_value);
    assert(rc == MEMCACHED_SUCCESS);
    assert(key_length[x] == return_value_length);
    assert(!memcmp(return_value, keys[x], return_value_length));
    free(return_value);
    x++;
  }

  memcached_free(memc);
}

void get_stats_keys(void)
{
 char **list;
 char **ptr;
 memcached_st *memc;
 memcached_stat_st stat;
 memcached_return rc;

 memc= memcached_create(NULL);
 assert(memc);

 list= memcached_stat_get_keys(memc, &stat, &rc);
 assert(rc == MEMCACHED_SUCCESS);
 for (ptr= list; *ptr; ptr++)
   printf("Found key %s\n", *ptr);

 free(list);
 memcached_free(memc);
}

void get_stats(void)
{
 unsigned int x;
 char **list;
 char **ptr;
 memcached_return rc;
 memcached_st *memc;
 memcached_stat_st *stat;

 memc= memcached_create(NULL);
 assert(memc);
 rc= memcached_server_add(memc, "localhost", 0);
 assert(rc == MEMCACHED_SUCCESS);

 stat= memcached_stat(memc, NULL, &rc);
 assert(rc == MEMCACHED_SUCCESS);

 assert(rc == MEMCACHED_SUCCESS);
 assert(stat);

 for (x= 0; x < memcached_server_count(memc); x++)
 {
   list= memcached_stat_get_keys(memc, &stat[x], &rc);
   assert(rc == MEMCACHED_SUCCESS);
   for (ptr= list; *ptr; ptr++)
     printf("Found key %s\n", *ptr);

   free(list);
 }

 free(stat);
 memcached_free(memc);
}

void get_stats_multiple(void)
{
 unsigned int x;
 char **list;
 char **ptr;
 memcached_return rc;
 memcached_st *memc;
 memcached_stat_st *stat;

 memc= memcached_create(NULL);
 assert(memc);
 rc= memcached_server_add(memc, "localhost", 0);
 assert(rc == MEMCACHED_SUCCESS);
 rc= memcached_server_add(memc, "localhost", 5555);
 assert(rc == MEMCACHED_SUCCESS);

 stat= memcached_stat(memc, NULL, &rc);
 assert(rc == MEMCACHED_SUCCESS);

 assert(rc == MEMCACHED_SUCCESS);
 assert(stat);

 for (x= 0; x < memcached_server_count(memc); x++)
 {
   list= memcached_stat_get_keys(memc, &stat[x], &rc);
   assert(rc == MEMCACHED_SUCCESS);
   for (ptr= list; *ptr; ptr++)
     printf("Found key %s\n", *ptr);

   free(list);
 }

 free(stat);
 memcached_free(memc);
}

void add_host_test(void)
{
  unsigned int x;
  memcached_st *memc;
  memcached_server_st *servers;
  memcached_return rc;
  char servername[]= "0.example.com";

  memc= memcached_create(NULL);
  assert(memc);
  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  servers= memcached_server_list_append(NULL, servername, 400, &rc);
  assert(servers);
  assert(1 == memcached_server_list_count(servers));

  for (x= 2; x < 20; x++)
  {
    char buffer[SMALL_STRING_LEN];

    snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
    servers= memcached_server_list_append(servers, buffer, 401, 
                                     &rc);
    assert(rc == MEMCACHED_SUCCESS);
    assert(x == memcached_server_list_count(servers));
  }

  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);

  memcached_server_list_free(servers);
  memcached_free(memc);
}

void add_host_test1(void)
{
  unsigned int x;
  memcached_st *memc;
  memcached_server_st *servers;
  memcached_return rc;
  char servername[]= "0.example.com";

  memc= memcached_create(NULL);
  assert(memc);

  servers= memcached_server_list_append(NULL, servername, 400, &rc);
  assert(servers);
  assert(1 == memcached_server_list_count(servers));

  for (x= 2; x < 20; x++)
  {
    char buffer[SMALL_STRING_LEN];

    snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
    servers= memcached_server_list_append(servers, buffer, 401, 
                                     &rc);
    assert(rc == MEMCACHED_SUCCESS);
    assert(x == memcached_server_list_count(servers));
  }

  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);

  memcached_server_list_free(servers);
  memcached_free(memc);
}


int main(int argc, char *argv[])
{
  /* Clean the server before beginning testing */
  flush_test();
  init_test();
  allocation_test();
  connection_test();
  error_test();
  set_test();
  add_test();
  replace_test();
  flush_test();
  delete_test();
  flush_test();
  get_test();
  get_test2();
  get_test3();
  get_test4();
  stats_servername_test();

  increment_test();
  decrement_test();
  quit_test();
  mget_test();
  get_stats();
  add_host_test();

  /* The multiple tests */
  if (argc == 2)
  {
    get_stats_multiple();
  }
  get_stats_keys();

  /* Clean up whatever we might have left */
  flush_test();
  return 0;
}
