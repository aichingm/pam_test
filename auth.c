#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCC(x) x == PAM_SUCCESS

int authenticate(char* service, char* user, char* pass);

struct pam_response* reply;

int init_conversation(int num_msg,
                      const struct pam_message** msg,
                      struct pam_response** resp,
                      void* appdata_ptr) {
  *resp = reply;
  return PAM_SUCCESS;
}

static struct pam_conv conv = {init_conversation, NULL};

int main(int argc, char* argv[]) {
  char *service, *user, *pass;
  int retval;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s [service] [username] [password]\n", argv[0]);
    return 1;
  }

  return authenticate(argv[1], argv[2], strdup(argv[3]));
}

int authenticate(char* service, char* user, char* pass) {
  pam_handle_t* pamh = NULL;
  int retval = pam_start(service, user, &conv, &pamh);

  if (SUCC(retval)) {
    reply = (struct pam_response*)malloc(sizeof(struct pam_response));
    reply[0].resp = pass;
    reply[0].resp_retcode = 0;

    retval = pam_authenticate(pamh, 0);
    printf(SUCC(retval) ? "Authenticated\n" : "Not Authenticated\n");
    pam_end(pamh, PAM_SUCCESS);
  }
  return SUCC(retval) ? 0 : 1;
}
