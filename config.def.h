/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 2048

/*
 * function            description                     argument (example)
 *
 * battery_perc        battery percentage              battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_state       battery charging state          battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_remaining   battery remaining HH:MM         battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * cpu_perc            cpu usage in percent            NULL
 * cpu_freq            cpu frequency in MHz            NULL
 * datetime            date and time                   format string (%F %T)
 * disk_free           free disk space in GB           mountpoint path (/)
 * disk_perc           disk usage in percent           mountpoint path (/)
 * disk_total          total disk space in GB          mountpoint path (/")
 * disk_used           used disk space in GB           mountpoint path (/)
 * entropy             available entropy               NULL
 * gid                 GID of current user             NULL
 * hostname            hostname                        NULL
 * ipv4                IPv4 address                    interface name (eth0)
 * ipv6                IPv6 address                    interface name (eth0)
 * kernel_release      `uname -r`                      NULL
 * keyboard_indicators caps/num lock indicators        format string (c?n?)
 *                                                     see keyboard_indicators.c
 * keymap              layout (variant) of current     NULL
 *                     keymap
 * load_avg            load average                    NULL
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * num_files           number of files in a directory  path
 *                                                     (/home/foo/Inbox/cur)
 * ram_free            free memory in GB               NULL
 * ram_perc            memory usage in percent         NULL
 * ram_total           total memory size in GB         NULL
 * ram_used            used memory in GB               NULL
 * run_command         custom shell command            command (echo foo)
 * swap_free           free swap in GB                 NULL
 * swap_perc           swap usage in percent           NULL
 * swap_total          total swap size in GB           NULL
 * swap_used           used swap in GB                 NULL
 * temp                temperature in degree celsius   sensor file
 *                                                     (/sys/class/thermal/...)
 *                                                     NULL on OpenBSD
 *                                                     thermal zone on FreeBSD
 *                                                     (tz0, tz1, etc.)
 * uid                 UID of current user             NULL
 * uptime              system uptime                   NULL
 * username            username of current user        NULL
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 * wifi_perc           WiFi signal in percent          interface name (wlan0)
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 */

void progressbar_fancy(
    unsigned int value,
    unsigned int num_tiles,
    char *output,
    size_t buf_size
) {
  unsigned int clamped = value > 100 ? 100 : value;
  unsigned int num_filled = num_tiles * (float)clamped / 100;
  unsigned int num_empty = num_tiles - num_filled;
  *output = 0;

  // uncomment to color-code the output
  // if (value >= 40)
  //   strncat(output, "", buf_size); // color

  while (num_filled--) strncat(output, "▰", buf_size);
  while (num_empty--) strncat(output, "▱", buf_size);
}

const char *
cpu_perc_fancy(void) {
  char output[1024] = "";
  const char *result = cpu_perc();
  if (result != NULL) {
    int percentage = atoi(result);
    progressbar_fancy(percentage, 5, output, LEN(output));
  }
  return bprintf("%s", output);
}

const char *
volume_fancy(void) {
  char output[1024] = "";
  const char *result = run_command("igoraudio-get-volume");
  if (result != NULL) {
    int percentage = atoi(result);
    progressbar_fancy(percentage, 5, output, LEN(output));
  }
  return bprintf("%s", output);
}

// alternative icons (requires font-awesome):
// (period in the end is needed so trailing whitespace is not autostripped
// and last characters are rendered well in terminal)
// hostname:          💻   .
// calendar:        .
// keyboard layout:          .
// audio:                       .
// battery:                .
// cpu: 
static const struct arg args[] = {
  /* function           format          argument */
  { hostname            , "%s", NULL },

  // Alternative option, without icons, pure text: <hostname> on <date>
  //{ hostname            , "%s", NULL },
  //{ datetime            , " on %s", "%a %b %d" },
  //{ keymap              , " | %s", NULL }
  { run_command         , "     %s", "~/.config/i3blocks/blocks/layout"},
  // igorg: try without volume in status; pa-applet should be enough
  //{ vol_perc            , "%s", "/dev/mixer" },
  //{ run_command       , " %s", "~/.config/i3blocks/blocks/volume-pulseaudio"},
  { datetime            , "     %s", "%Y-%m-%d" },
  { run_command         , "     %s:", "~/bin/igoraudio_status"},
  { volume_fancy        , "%s", NULL},
  { cpu_perc_fancy      , "      CPU:%s     ", NULL},
  // battery script prints a single space when no battery is installed
  // { run_command         , " /%s", "~/.config/i3blocks/blocks/battery"},
  // igorg: let's try without ssid/network strength
  //{ run_command         , " %s ", "~/.config/i3blocks/blocks/ssid"},
  //{ run_command         , "%s", "BLOCK_INSTANCE=wlp4s0 ~/.config/i3blocks/blocks/network"},
  //{ wifi_perc           , "%s","wlan0"},
  //{ wifi_essid          , "%s","wlan0"},
};
