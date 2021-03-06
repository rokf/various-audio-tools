#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <soundpipe.h>
#include <sporth.h>
#include <ao/ao.h>

#include <sys/stat.h>
#include <sys/types.h>

#define CODEBUF_SIZE 8192

int main(int argc, char **argv)
{
  ao_device *device;
  ao_sample_format format;
  int default_driver;
  char *buffer;
  int buf_size;
  int i;
  int sample;
  float seconds;

  char *codebuf;
  codebuf = calloc(CODEBUF_SIZE, sizeof(char));

  ao_initialize();

  default_driver = ao_default_driver_id();

  memset(&format, 0, sizeof(format));
  format.bits = 16;
  format.channels = 2;
  format.rate = 44100;

  seconds = 0.05;

  format.byte_format = AO_FMT_LITTLE;
  if (ao_is_big_endian()) format.byte_format = AO_FMT_BIG;

  device = ao_open_live(default_driver, &format, NULL);

  buf_size = format.bits/8 * format.channels * format.rate * seconds;
  buffer = calloc(buf_size, sizeof(char));

  sp_data *sp;
  sp_create(&sp);
  sp->len = seconds * format.rate;
  plumber_data pd;
  plumber_register(&pd);
  plumber_init(&pd);
  pd.sp = sp;


  if (argv[1]) {
    FILE* file = fopen(argv[1],"r");
    fread(codebuf, sizeof(char), CODEBUF_SIZE, file);
    fclose(file);
  } else {
    printf("Usage: sporth_ao_stream [path to file]\n");
    return 0;
  }

  long int modified_time;

  struct stat attr;
  stat(argv[1], &attr);

  modified_time = attr.st_mtime;

  if (plumber_parse_string(&pd, codebuf) != PLUMBER_OK) return 0;
  plumber_compute(&pd, PLUMBER_INIT);

  int running = 1;


  while (running) {
    stat(argv[1], &attr);
    if (modified_time < attr.st_mtime) {
      modified_time = attr.st_mtime;

      FILE* file = fopen(argv[1],"r");
      fread(codebuf, sizeof(char), CODEBUF_SIZE, file);
      fclose(file);

      plumber_recompile_string(&pd, codebuf);

    }

    i = 0;
    sp->len = seconds * format.rate;
    while (sp->len > 0) {

      plumber_compute(&pd, PLUMBER_COMPUTE);
      sp->out[0] = sporth_stack_pop_float(&(pd.sporth.stack));

      sample = (int)((float)(0.75 * 32768.0) * sp->out[0]);

      buffer[4*i] = buffer[4*i+2] = sample & 0xff;
      buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;

      sp->len--;
      sp->pos++;
      i++;
    }
    ao_play(device, buffer, buf_size);
  }



  plumber_clean(&pd);
  sp_destroy(&sp);

  ao_close(device);
  ao_shutdown();

  free(buffer); free(codebuf);
  return 0;
}
