// getipinfo - cli utility to get ip info using ip-api.com api, ifconfig.me,
// grep, sed and curl DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char ip[32];
  if (argc < 2) {
    char *getipcmd = "curl -sS ifconfig.me";

    FILE *getippipe = popen(getipcmd, "r");
    if (getippipe == NULL) {
      perror("failed to run curl command");
      return EXIT_FAILURE;
    }

    if (fgets(ip, sizeof(ip), getippipe) == NULL) {
      perror("failed to read ip");
      pclose(getippipe);
      return EXIT_FAILURE;
    }
    pclose(getippipe);

    ip[strcspn(ip, "\n")] = '\0';
  } else {
    strncpy(ip, argv[1], sizeof(ip) - 1);
    ip[sizeof(ip) - 1] = '\0';
  }

  char getipdatacmd[64];
  snprintf(getipdatacmd, sizeof(getipdatacmd), "curl -sS ip-api.com/json/%s",
           ip);

  FILE *getipdatapipe = popen(getipdatacmd, "r");
  if (getipdatapipe == NULL) {
    perror("failed to run curl command");
    pclose(getipdatapipe);
    return EXIT_FAILURE;
  }

  char ipdata[2014];
  fgets(ipdata, sizeof(ipdata), getipdatapipe);
  pclose(getipdatapipe);

  char parsecmd[4096];
  snprintf(parsecmd, sizeof(parsecmd),
           "echo '%s' | grep -oP '\"query\":\"\\K[^\"]+' | tr -d '\\n'; "
           "echo; "
           "echo '%s' | grep -oP '\"country\":\"\\K[^\"]+' | tr -d '\\n'; "
           "echo ' ' | tr -d '\\n'; "
           "echo '%s' | grep -oP '\"countryCode\":\"\\K[^\"]+' | sed -E "
           "'s/^/[/; s/$/]/' | tr -d '\\n'; "
           "echo ', ' | tr -d '\\n'; "
           "echo '%s' | grep -oP '\"regionName\":\"\\K[^\"]+' | tr -d '\\n'; "
           "echo ', ' | tr -d '\\n'; "
           "echo '%s' | grep -oP '\"city\":\"\\K[^\"]+'; "
           "echo '%s' | grep -oP '\"lat\":\\K[^,]+' | tr -d '\\n'; "
           "echo ' | ' | tr -d '\\n'; "
           "echo '%s' | grep -oP '\"lon\":\\K[^,]+'; "
           "echo '%s' | grep -oP '\"timezone\":\"\\K[^\"]+'; "
           "echo '%s' | grep -oP '\"isp\":\"\\K[^\"]+' | tr -d '\\n'; "
           "echo ' | ' | tr -d '\\n'; "
           "echo '%s' | grep -oP '\"as\":\"\\K[^\"]+' ",
           ipdata, ipdata, ipdata, ipdata, ipdata, ipdata, ipdata, ipdata,
           ipdata, ipdata);

  system(parsecmd);

  return EXIT_SUCCESS;
}
