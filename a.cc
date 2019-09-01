
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include "duktape.h"

static void push_file_as_string(duk_context *ctx, const char *filename) {
    FILE *f;
    size_t len;
    char buf[16384];

    f = fopen(filename, "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
        fclose(f);
        duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
    } else {
        duk_push_undefined(ctx);
    }
}
static duk_ret_t final(duk_context *ctx) {
  duk_get_prop_string(ctx, 0, "ptr");
  void *p = duk_to_pointer(ctx, -1);
  printf("finalizer %p\n", p);
  return 0;
}

static duk_ret_t prt(duk_context *ctx) {
  printf("%s\n", duk_to_string(ctx, 0));
  return 0;
}

static duk_ret_t testfunc(duk_context *ctx) {
  printf("testfunc\n");

  duk_push_this(ctx);
  //printf("%s\n", duk_to_string(ctx, -1));


  duk_get_prop_string(ctx, 0, "ptr");
  void *p = duk_to_pointer(ctx, -1);
  printf("%p\n", p);
  duk_pop(ctx);
  //duk_pop(ctx);
  return 0;
}


static duk_ret_t cbk(duk_context *ctx) {
  //printf("%s\n", duk_to_string(ctx, 0));



  duk_dup(ctx, 0);
  duk_put_global_string(ctx, "_callback1");

  //std::thread tid([ctx]() {
  //  sleep(1);
    printf("threaded\n");

    duk_get_global_string(ctx, "_callback1");
    duk_push_null(ctx);
    duk_put_global_string(ctx, "_callback1");
    int rc = duk_pcall(ctx, 0);
    if (rc != 0) {
        printf("Callback failed: '%s'\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);
  //});
  //tid.detach();
  return 0;
}

const duk_function_list_entry funclist[] = {
    { "test", testfunc, 0 /* no args */ },
    { NULL, NULL, 0 }
};

int main(int argc, char *argv[]) {
  duk_context *ctx = duk_create_heap_default();

  duk_push_c_function(ctx, prt, 1 /*nargs*/);
  duk_put_global_string(ctx, "prt");

  duk_push_c_function(ctx, cbk, 1 /*nargs*/);
  duk_put_global_string(ctx, "callback");

//  duk_eval_string(ctx, "prt('aaa'); 1+2");
  push_file_as_string(ctx, "a.js");
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
        return 1;
    }

  printf("1+2=%d\n", (int) duk_get_int(ctx, -1));


  duk_get_global_string(ctx, "zz");
  duk_push_string(ctx, "some text");
  int rc = duk_pcall(ctx, 1);
    if (rc != 0) {
        printf("Callback failed: '%s'\n", duk_safe_to_string(ctx, -1));
    }

  for (int i=0; i<100000; i++) {
    printf("stack %d %d\n", duk_get_top(ctx), duk_get_top_index(ctx));
    duk_get_global_string(ctx, "otest");
    duk_push_string(ctx, "s1");
    duk_push_object(ctx);

    duk_push_c_function(ctx, final, 1);
    duk_set_finalizer(ctx, -2);

    duk_put_function_list(ctx, -1, funclist);

    duk_push_pointer(ctx, (void*)0xabc);
    duk_put_prop_string(ctx, -2, "ptr");
    duk_push_string(ctx, "s2");
    rc = duk_pcall(ctx, 3);
      if (rc != 0) {
          printf("otest: '%s'\n", duk_safe_to_string(ctx, -1));
      }
      duk_pop(ctx);
  }

  //sleep(10);
  duk_destroy_heap(ctx);
  return 0;
}
