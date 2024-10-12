#include <assert.h>
#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

typedef enum
{
  CMD_STOP,
  CMD_START,
  CMD_RESTART,
  CMD_MAX,
} cmd_id_t;

char *dbus_method[CMD_MAX] = {
    [CMD_STOP] = "StopUnit",
    [CMD_START] = "StartUnit",
    [CMD_RESTART] = "RestartUnit",
};

char *get_service_arg(napi_env env, napi_callback_info info)
{
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  assert(status == napi_ok);
  if (argc < 1)
  {
    napi_throw_type_error(env, NULL, "Missing service");
    return NULL;
  }
  napi_valuetype valuetype0;
  status = napi_typeof(env, args[0], &valuetype0);
  assert(status == napi_ok);

  if (valuetype0 != napi_string)
  {
    napi_throw_type_error(env, NULL, "Argument must be a string");
    return NULL;
  }

  char srv_name[32] = {0};
  size_t len;
  status = napi_get_value_string_utf8(env, args[0], srv_name, sizeof(srv_name), &len);
  assert(status == napi_ok);

  if (len > sizeof(srv_name))
    status = napi_string_expected;
  assert(status == napi_ok);
  size_t mall_sz = len + strlen(".service") + 1;
  // printf("%d, %d, %d", (int)len, (int)mall_sz, (int)strlen(".service"));
  char *service_arg = malloc(mall_sz);
  snprintf(service_arg, mall_sz, "%s.service", srv_name);
  service_arg[mall_sz - 1] = '\0';
  return service_arg;
}

napi_status run_dbus(const char *service_arg, cmd_id_t cmd_id)
{
  napi_status status = napi_ok;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message *m = NULL;
  sd_bus *bus = NULL;
  const char *path;
  int r;
  /* Connect to the system bus */
  r = sd_bus_open_system(&bus);
  if (r < 0)
  {
    fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
    goto finish;
  }

  /* Issue the method call and store the respons message in m */
  r = sd_bus_call_method(bus,
                         "org.freedesktop.systemd1",         /* service to contact */
                         "/org/freedesktop/systemd1",        /* object path */
                         "org.freedesktop.systemd1.Manager", /* interface name */
                         //"StartUnit",                        /* method name */
                         dbus_method[cmd_id],
                         &error,      /* object to return error in */
                         &m,          /* return message on success */
                         "ss",        /* input signature */
                         service_arg, /* first argument */
                         "replace");  /* second argument */
  if (r < 0)
  {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    goto finish;
  }

  /* Parse the response message */
  r = sd_bus_message_read(m, "o", &path);
  if (r < 0)
  {
    fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
    goto finish;
  }

  printf("Queued service job as %s.\n", path);

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  sd_bus_unref(bus);
  return status;
}

static napi_value RestartUnit(napi_env env, napi_callback_info info)
{
  napi_status status;
  char *service_arg = get_service_arg(env, info);

  printf("restarting: %s\n", service_arg);
  status = run_dbus(service_arg, CMD_RESTART);
  if (service_arg != NULL)
    free(service_arg);

  assert(status == napi_ok);

  napi_value world;
  status = napi_create_string_utf8(env, "Hello, fuck!", 13, &world);

  return world;
}

static napi_value StartUnit(napi_env env, napi_callback_info info)
{
  napi_status status;
  char *service_arg = get_service_arg(env, info);

  printf("starting: %s\n", service_arg);
  status = run_dbus(service_arg, CMD_START);
  if (service_arg != NULL)
    free(service_arg);

  napi_value world;
  status = napi_create_string_utf8(env, "Hello, sys!", 13, &world);
  assert(status == napi_ok);
  return world;
}

static napi_value StopUnit(napi_env env, napi_callback_info info)
{
  napi_status status;
  char *service_arg = get_service_arg(env, info);

  printf("stop: %s\n", service_arg);
  status = run_dbus(service_arg, CMD_STOP);
  if (service_arg != NULL)
    free(service_arg);

  napi_value world;
  status = napi_create_string_utf8(env, "Hello, sys!", 13, &world);
  assert(status == napi_ok);
  return world;
}

#define DECLARE_NAPI_METHOD(name, func) \
  {name, 0, func, 0, 0, 0, napi_default, 0}

static napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;

  napi_value stop_unit;
  status = napi_create_function(env, NULL, 0, StopUnit, NULL, &stop_unit);
  if (status != napi_ok)
    return NULL;
  status = napi_set_named_property(env, exports, "stopUnit", stop_unit);
  if (status != napi_ok)
    return NULL;

  napi_value start_unit;
  status = napi_create_function(env, NULL, 0, StartUnit, NULL, &start_unit);
  if (status != napi_ok)
    return NULL;
  status = napi_set_named_property(env, exports, "startUnit", start_unit);
  if (status != napi_ok)
    return NULL;

  napi_value reset_unit;
  status = napi_create_function(env, NULL, 0, RestartUnit, NULL, &reset_unit);
  if (status != napi_ok)
    return NULL;
  status = napi_set_named_property(env, exports, "restartUnit", reset_unit);
  if (status != napi_ok)
    return NULL;

  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)